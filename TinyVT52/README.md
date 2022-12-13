# Stefan's Tiny VT52

## The idea

This is the spin off of my BASIC retro project. It emulates a VT52 terminal on a microcontroller 
with a display and optionally a keyboard. 

Most VT52 control characters and GEMDOS extensions are supported.

The code of the display and device drivers of BASIC is used here.

## Files in this archive 

TinyVT52.ino is the program source of the VT52 sketch. 

basic.h is the header file of BASIC.

hardware-arduino.h contains the platform specific definitions and a thin OS like layer for hardware abstraction. Actually this is not only a header but really a complete library. I will rewrite this code in the future and make it a library.
