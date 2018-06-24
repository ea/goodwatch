/*! \file apps.c
  \brief Application manager.

  This module manages the different applications, and the switching
  between them.
 */

#include <msp430.h>
#include <stdio.h>

#include "api.h"
#include "applist.h"

//! We begin on the clock.
static int appindex=DEFAULTAPP, idlecount=0;

//! The currently selected application.
const struct app *applet=&apps[0];

//! Every 3 minutes we return to the clock unless this is called.
void app_cleartimer(){
  idlecount=0;
}

//! Renders the current app to the screen.
void app_draw(){
  static int lastmin=0;;
  
  //If we go three minutes without action, return to main screen.
  if(lastmin!=RTCMIN){
    lastmin=RTCMIN;
    idlecount++;
  }
  if(idlecount>3){
    app_cleartimer();
    app_forcehome();
  }
  
  //Call the cap if it exists, or switch to the clock if we're at the
  //end of the list.
  if(applet->draw)
    applet->draw();
  else
    app_forcehome();
  return;
}

//! Force return to the home app.
void app_forcehome(){
  //First we try to exit politely.
  if(applet->exit)
    applet->exit();

  //And force it if that doesn't work.
  ucs_slow();  //Gotta drop the clock rate, in case it was high.
  appindex=0;
  applet = &apps[appindex];
  applet->init();
}

//! Initializes the set of applications.
void app_init(){
  if(applet->init)
    applet->init();
  else if(!applet->name){
    appindex=0;
    applet = &apps[appindex];
  }

  return;
}

//! Sets an app by a pointer to its structure.  Used for submenus.
void app_set(const struct app *newapplet){
  applet=newapplet;
  app_init();
}

//! Move to the next application if the current allows it.
void app_next(){
  //Clear the 3-minute timer when we switch apps.  This is also
  //cleared by keypresses.
  app_cleartimer();

  /* First we ask the current app if it will allow the transaction by
     calling its exit() routine.  Zero or a null function pointer allow
     for the transition, but non-zero indicates that the transition has
     been cancelled.  For example, this is done by the RPN calculator
     when the item on the stack is not zero.
  */
  
  //Return if there is an exit function and it returns non-zero.
  if(applet->exit && applet->exit())
    return;
  
  applet = &apps[++appindex];
  if(!applet->draw){
    appindex=0;
    applet = &apps[appindex];
  }

  //Initialize the new application.
  if(applet->init)
    applet->init();
  
  return;
}

//! Provide an incoming packet.
void app_packetrx(uint8_t *packet, int len){
  /* In monitor mode, we forward the packet to the monitor, rather
     than to the application.
   */
  if(uartactive){
    monitor_packetrx(packet,len);
    return;
  }

  /* Otherwise, we send it to the active application, but only if that
     application has a handler.
   */
  if(!applet->packetrx){
    printf("No packet RX handler for %s.",
	   applet->name);
    return;
  }

  applet->packetrx(packet,len);
}

//! Callback after sending a packet.
void app_packettx(uint8_t *packet, int len){
  /* We send it to the active application, but only if that
     application has a handler.
   */
  if(!applet->packettx){
    printf("No packet TX handler for %s.",
	   applet->name);
    return;
  }

  applet->packettx();
}

//! Handles a keypress, if a handler is registered.
void app_keypress(char ch){
  //We only pass it to applications that have a handler.
  if(applet->keypress){
    if(applet->keypress(ch)){
      /* Some applets need to visually respond at the moment of their
	 keypress, while others need to be drawn at a predictable
	 framerate.  So if--any only if--the keypress() function tells
	 us to do we redraw it.
       */
      lcd_predraw();
      app_draw();
      lcd_postdraw();
    }
  }
}
