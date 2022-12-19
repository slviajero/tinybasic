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

/* the only language feature of BASIC that we need is the VT52 component */
#define HASVT52

/* dummy memsize parameter */
#define MEMSIZE 128

#include "basic.h"
#include "hardware-arduino.h"

/* XON and XOFF characters */
#define XON 0x11
#define XOFF 0x13

void setup() {

/* start the primary serial interface */
  Serial.begin(9600);

/* start the terminal interface */
  Serial1.begin(9600);

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
  if (Serial1.available()) {
    Serial1.write(XOFF);
    while (Serial1.available()) dspwrite(Serial1.read());
    Serial1.write(XON);
  }
  
/* send all characters from the display to the serial stream */
  while (kbdavailable()) {
    ch=kbdread();
    Serial1.write(ch);
  } 
}
