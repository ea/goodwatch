/*! \file dmesgapp.c
  \brief Send full dmesg log over radio 
  
*/

#include<stdio.h>
#include<string.h>
#include<msp430.h>
#include "api.h"

//keeping all the same settings as beacon app
#define LEN 32

/* Settings for the GoodWatch, similar to 1.2kbaud SimpliciTI.
 */
static const uint8_t goodwatch_settings[]={
  IOCFG0,0x06,  //GDO0 Output Configuration
  FIFOTHR,0x47, //RX FIFO and TX FIFO Thresholds
  PKTCTRL1, 0x04, //No address check.
  //PKTCTRL0, 0x05,//Packet Automation Control, variable length.
  PKTCTRL0, 0x04, //Packet automation control, fixed length.
  FSCTRL1,0x06, //Frequency Synthesizer Control
  FREND0,    0x11,   // FREND0    Front end TX configuration, use PA_TABLE[1]
  FREQ2,0x21,   //Frequency Control Word, High Byte
  FREQ1,0x62,   //Frequency Control Word, Middle Byte
  FREQ0,0x76,   //Frequency Control Word, Low Byte
  MDMCFG4,0xF5, //Modem Configuration
  MDMCFG3,0x83, //Modem Configuration
  MDMCFG2,0x13, //Modem Configuration
  DEVIATN,0x15, //Modem Deviation Setting
  MCSM0,0x10,   //Main Radio Control State Machine Configuration
  FOCCFG,0x16,  //Frequency Offset Compensation Configuration
  WORCTRL,0xFB, //Wake On Radio Control
  FSCAL3,0xE9,  //Frequency Synthesizer Calibration
  FSCAL2,0x2A,  //Frequency Synthesizer Calibration
  FSCAL1,0x00,  //Frequency Synthesizer Calibration
  FSCAL0,0x1F,  //Frequency Synthesizer Calibration
  TEST2,0x81,   //Various Test Settings
  TEST1,0x35,   //Various Test Settings
  TEST0,0x09,   //Various Test Settings
  ADDR,  0x00,   // ADDR      Device address.
  MCSM1, 0x30,   //MCSM1, return to IDLE after packet.  Or with 2 for TX carrier test.
  MCSM0,  0x18,   // MCSM0     Main Radio Control State Machine configuration.
  IOCFG2,  0x29,   // IOCFG2    GDO2 output pin configuration.
  IOCFG0,  0x06,   // IOCFG0    GDO0 output pin configuration.
  PKTLEN,  LEN,    // PKTLEN    Packet length.
  0,0
};


static uint16_t state = 0;
static char *dmesg_buffer=(char*)0x2400;
static uint16_t dmesg_idx = 0;

//! Check if the current dmesg chunk is all empty
int all_zeros(){
  uint16_t i;
  for(i=0;i<LEN;i++){
    if(dmesg_buffer[dmesg_idx+i] != '\0') return 0;
  }
  return 1;
}

//! Transmit one dmesg packet
void dmesgapp_packettx(){
  __delay_cycles(20000); //waste a bit of time so recieving side can keep up
  if(state){
    while(all_zeros())dmesg_idx += LEN; // skip all-null packet
    if(dmesg_idx < DMESGLEN){
      packet_tx((uint8_t*)dmesg_buffer+dmesg_idx,LEN);
      dmesg_idx+=LEN;
    }else{
      dmesg_idx=0;
      state=0; // we are done
    }
  }
}
int dmesgapp_keypress(char ch);

//! Enter the dmesg application.
void dmesgapp_init(){
  /* This enters the application.  We use the codeplug frequency.
   */
  if(has_radio){
    dmesg_idx = 0;
    radio_on();
    radio_writesettings(goodwatch_settings);
    radio_writepower(0x25);
    codeplug_setfreq();
  }else{
    app_next();
  }
}

//! Exit the dmesg application.
int dmesgapp_exit(){
  //Cut the radio off.
  state = 0;
  radio_off();
  //Allow the exit.
  return 0;
}

//! Draw the dmesg screen.
void dmesgapp_draw(){
  switch(state){
  case 0:
  lcd_string("press 0");
  break;
  case 1:
    lcd_string("        ");
    lcd_number(dmesg_idx);
    break;
  }
}

//! Keypress handler for the dmesg app.
int dmesgapp_keypress(char ch){
  switch(ch){
  case '0':
  case '1':
  case '4':
  case '7':
    if(radio_getstate()==1){
      state=1;
      dmesgapp_packettx();
    }
    break;
  }

  return 0;
}

