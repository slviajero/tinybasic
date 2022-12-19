# Stefan's Tiny VT52

## The idea

This is the spin off of my BASIC retro project. It emulates a VT52 terminal on a microcontroller 
with a display and optionally a keyboard. The terminal runs at 9600 baud by default but can be 
changed to any other baud rate.

Most VT52 control characters and GEMDOS extensions are supported.

The code of the display and device drivers of BASIC is used here. Currenty supported display are:

- Text LCD based LCD shields
- Text LCDs from 2x16 and 4x20 with parallel and I2C interfaces
- ILI9486 480x320 parallel TFTs with the MCUFRIEND graph library
- ILI9488 480x320 SPI based displays with the ug2 library
- SD1963 TFTs with up to 800x600 pixels. This display uses utft.
- Nokia 5110 displays with SPI.
- OLEDS like 1306.

## Files in this archive 

TinyVT52.ino is the program source of the VT52 sketch. 

basic.h is the header file of BASIC.

hardware-arduino.h contains the platform specific definitions and a thin OS like layer for hardware abstraction. Actually this is not only a header but really a complete library. I will rewrite this code in the future and make it a library.

This file needs to be edited before compile. The displays and the keyboard model has to be set there. 

## Supported ANSI codes

The terminal implements a small set of ANSI control characters.

^B 	2 	STX Start of Text - Moves the cursor home.

^C 	3 	ETX End of Text - Updates the display in page mode.

^G	7	BEL	Bell - Makes an audible noise.

^H	8	BS	Backspace - Moves the cursor left.

^I	9	HT	Tab	- Moves the cursor right to next multiple of 8.

^J	10	LF	Line Feed - Moves to next line, scrolls the display up if at bottom of the screen. Goes to column 0 (i.e. UNIX style line feed)

^K 	11	VT 	Vertical tab - Move to the next line without change in horizontal position.

^L	12	FF	Form Feed - Clear the screen. 

^M	13	CR	Carriage Return	Moves the cursor to column zero.


## VT52 escape sequences

The supported escape sequences of the VT52 terminal are

ESC A	Cursor up - Move cursor one line upwards.

ESC B	Cursor down - Move cursor one line downwards.

ESC C	Cursor right - Move cursor one column to the right.

ESC D	Cursor left - Move cursor one column to the left.

ESC H	Cursor home	- Move cursor to the upper left corner.

ESC I	Reverse line feed - Insert a line above the cursor, then move the cursor into it. 

ESC J	Clear to end of screen - Clear screen from cursor onwards.

ESC K	Clear to end of line - Clear line from cursor onwards.

ESC L	Insert line - Insert a line.

ESC M	Delete line	- Remove line.

ESC Yrc	Set cursor position - Move cursor to position c,r, encoded as single characters.

ESC Z	ident	Identify what the terminal is, see notes below.

ESC E	Clear screen - Clear screen and place cursor at top left corner.

ESC b#	Foreground color - Set text colour to the selected value

ESC c#	Background color - Set background colour

ESC d	Clear to start of screen - Clear screen from the cursor up to the home position.

ESC e	Enable cursor - Makes the cursor visible on the screen.

ESC f	Disable cursor - Makes the cursor invisible.

ESC j	Save cursor	- Saves the current position of the cursor in memory, TOS 1.02 and later.

ESC k	Restore cursor - Return the cursor to the settings previously saved with j.

ESC l	Clear line - Erase the entire line and positions the cursor on the left.

ESC o	Clear to start of line - Clear current line from the start to the left side to the cursor.

ESC p	Reverse video - Switch on inverse video text (implemented but with no function).

ESC q	Normal video - Switch off inverse video text (implemented but with no function).

ESC v	Wrap on	- Enable line wrap, removing the need for CR/LF at line endings.

ESC w	Wrap off - Disable line wrap.






