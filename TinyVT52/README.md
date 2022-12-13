# Stefan's Tiny VT52

## The idea

This is the spin off of my BASIC retro project. It emulates a VT52 terminal on a microcontroller 
with a display and optionally a keyboard. The terminal runs at 9600 baud by default but can be 
changed to any other baud rate.

Most VT52 control characters and GEMDOS extensions are supported.

The code of the display and device drivers of BASIC is used here. Currenty supported display are 
parallel and serial LCD, parallel TFT with MCUFRIEND, SD1306 and SD1963, SPI ILIs like 9488. 

## Files in this archive 

TinyVT52.ino is the program source of the VT52 sketch. 

basic.h is the header file of BASIC.

hardware-arduino.h contains the platform specific definitions and a thin OS like layer for hardware abstraction. Actually this is not only a header but really a complete library. I will rewrite this code in the future and make it a library.

This file needs to be edited before compile. The displays and the keyboard model has to be set there. 

## VT52 and ANSI control codes of the display 

ESC A	Cursor up	Move cursor one line upwards.

ESC B	Cursor down	Move cursor one line downwards.

ESC C	Cursor right	Move cursor one column to the right.

ESC D	Cursor left	Move cursor one column to the left.

ESC F	Enter graphics mode	Use special graphics character set, VT52 and later.

ESCG	Exit graphics mode	Use normal US/UK character set
ESCH	Cursor home	Move cursor to the upper left corner.
ESCI	Reverse line feed	Insert a line above the cursor, then move the cursor into it.
May cause a reverse scroll if the cursor was on the first line.
ESCJ	Clear to end of screen	Clear screen from cursor onwards.
ESCK	Clear to end of line	Clear line from cursor onwards.
ESCL	Insert line	Insert a line.
ESCM	Delete line	Remove line.
ESCYrc	Set cursor position	Move cursor to position c,r, encoded as single characters.
The VT50H also added the "SO" command that worked identically,
providing backward compatibility with the VT05.
ESCZ	ident	Identify what the terminal is, see notes below.
ESC=	Alternate keypad	Changes the character codes returned by the keypad.
ESC>	Exit alternate keypad	Changes the character codes returned by the keypad.
