/*
 *
 *  $Id: TinyVT52.ino,v 1.1 2022/12/13 19:00:16 stefan Exp stefan $ 
 *
 *  Stefan's TinyVT52 derived from the IoT BASIC interpreter 
 *
 *  See the licence file on 
 *  https://github.com/slviajero/tinybasic for copyright/left.
 *    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 *  Author: Stefan Lenz, sl001@serverfabrik.de
 *
 * Device drivers and low level I/O routines are used from the BASIC code
 * These definitions are in basic.h and hardware-arduino.h. These two files 
 * are exactly identical to the BASIC language files to make sure that further
 * work on the device drivers can be used here without changes.
 * 
 * To use the terminal, the hardware definition in hardware-arduino.h need to be set
 * 
 * Any one of the following macros need to be defined 
 * 
 * Always set #define DISPLAYCANSCROLL
 * 
 * Displays: define one(!) of the following displays in hardware
 *  ARDUINOLCDI2C, ARDUINONOKIA51, ARDUINOILI9488, ARDUINOSSD1306
 *  ARDUINOMCUFRIEND, ARDUINOGRAPHDUMMY, LCDSHIELD, ARDUINOTFT
 * 
 * Keyboards:
 *  ARDUINOPS2, ARDUINOUSBKBD (alpha!!), ARDUINOZX81KBD
 * 
 * Printers:
 *  ARDUINOPRT 
 *
 */

/* which serial port is used for terminal I/O */
#define SERIALPORT Serial

/* which serial port is used for printer IO */
#define PRTSERIAL Serial3

/* the only language feature of BASIC that we need is the VT52 component */
#define HASVT52

/* should the VT52 also handle Wiring */
#define VT52WIRING

/* the device driver code from BASIC */
#include "basic.h"
#include "hardware-arduino.h"

/* XON and XOFF characters */
#define XON 0x11
#define XOFF 0x13

/* a shallow buffer */
#define VT52BUFSIZE 64
char vt52sbuf[VT52BUFSIZE];
int vt52sbi = 0;

void setup() {

/* start the terminal interface */
  SERIALPORT.begin(9600);

/* start the display stream */
  dspbegin();  

/* if a printer port is defined, start the printer, this is Serial1 of Software Serial */
#if defined(ARDUINOPRT)
  prtbegin();
#endif

/* if any wire subsystem is requested, start it as well */
#if defined(NEEDSWIRE)
  wirebegin();
#endif

/* and then there is SPI */
#if defined(ARDUINOSPI)
  spibegin();
#endif

}

void loop() {
  char ch;

/* first test, just use the serial port as in */

/* read a chunk of characters */
/* display output is slow, it is an operation at the timescale of 10 ms per character, XOFF while we do it*/
  if (SERIALPORT.available()) {
/* send XOFF to hold the output */
    SERIALPORT.write(XOFF);
/* free the serial buffer immediately to be ready for characters sent before XOFF could be processes*/
    while (SERIALPORT.available() && vt52bi < VT52BUFSIZE) vt52sbuf[vt52bi++]=SERIALPORT.read();
/* now empty the buffer completely */
    for (int i=0; i<vt52bi; i++) dspwrite(vt52sbuf[i]);
    vt52bi=0;
/* send XON to continue output */
    SERIALPORT.write(XON);
  }
  
/* send all characters from the display to the serial stream */
  while (kbdavailable()) {
    ch=kbdread();
    SERIALPORT.write(ch);
  } 
}
