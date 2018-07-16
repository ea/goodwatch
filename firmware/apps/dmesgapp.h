/*! \file dmesgapp.h
  \brief Send full dmesg log over radio 
  
*/

//! Enter the dmesg application.
void dmesgapp_init();

//! Exit the dmesg application.
int dmesgapp_exit();

//! Draw the dmesg screen.
void dmesgapp_draw();

//! Keypress handler for the dmesg applet.
int dmesgapp_keypress(char ch);

//! Callback after one packet has been sent, keeps sending the rest
void dmesgapp_packettx();
