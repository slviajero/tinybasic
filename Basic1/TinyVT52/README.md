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

hardware-arduino.h contains the device driver code of BASIC. It is mostly dspwrite() which is used for the terminal plus the prt code if a printer is connected. 

This file needs to be edited before compile. The displays and the keyboard model has to be set there. In the hardware definition section of hardware-arduino.h all definitions have to be undef except the display one uses, the scroll directive and optionally the printer one uses. For a parallel display it would look like this:

	#undef USESPICOSERIAL 
	#undef ARDUINOPS2
	#undef ARDUINOUSBKBD
	#undef ARDUINOZX81KBD
	#undef ARDUINOPRT
	#define DISPLAYCANSCROLL
	#undef ARDUINOLCDI2C
	#undef ARDUINONOKIA51
	#undef ARDUINOILI9488
	#undef ARDUINOSSD1306
	#define ARDUINOMCUFRIEND
	#undef ARDUINOGRAPHDUMMY
	#undef LCDSHIELD
	#undef ARDUINOTFT
	#undef ARDUINOVGA
	#undef ARDUINOEEPROM
	#undef ARDUINOI2CEEPROM
	#undef ARDUINOEFS
	#undef ARDUINOSD
	#undef ESPSPIFFS
	#undef RP2040LITTLEFS
	#undef ARDUINORTC
	#undef ARDUINOWIRE
	#undef ARDUINOWIRESLAVE
	#undef ARDUINORF24
	#undef ARDUINOETH
	#undef ARDUINOMQTT
	#undef ARDUINOSENSORS
	#undef ARDUINOSPIRAM 
	#undef STANDALONE
	#undef STANDALONESECONDSERIAL


Only ARDUINOMCUFRIEND and DISPLAYCANSCROLL are defined and the code is compiled accordingly.

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

ASCII 127 is delete and moved the cursor back.

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

ESC b#	Foreground color - Set text colour to the selected value (only implemented on color displays).

ESC c#	Background color - Set background colour (only implemented on color displays).

ESC d	Clear to start of screen - Clear screen from the cursor up to the home position.

ESC e	Enable cursor - Makes the cursor visible on the screen (only implemented on LCDs).

ESC f	Disable cursor - Makes the cursor invisible (only implemented on LCDs).

ESC j	Save cursor	- Saves the current position of the cursor in memory.

ESC k	Restore cursor - Return the cursor to the settings previously saved with j.

ESC l	Clear line - Erase the entire line and positions the cursor on the left.

ESC o	Clear to start of line - Clear current line from the start to the left side to the cursor.

ESC p	Reverse video - Switch on inverse video text (implemented but with no function).

ESC q	Normal video - Switch off inverse video text (implemented but with no function).

ESC v	Wrap on	- Enable line wrap, removing the need for CR/LF at line endings.

ESC w	Wrap off - Disable line wrap.

## Notes on color

the original VT52 was of course monochrome. Color code was later added in the GEMDOS extensions. The code here maps color to the 16 principal VGA colors from 0 (black) to 15 (white). Many displays support deeper color, typically 16 or even 24 bit. Using the VGA colors for text is a compromise. It keeps the display buffer small enough for Arduino UNOs and other small MCUs.

## Notes on baud rates

VT52 could receive date with up to 9600 baud. The could handle scrolling even at that buadrate. The transmit speed of a VT52 was 4800 baud maximum.

One would expect that modern microcontrollers can do this as well. However, this is not true. 

Sending a character to a TFT with the parallel interface from an Arduino MEGA2560 takes 6ms. This means that at peak performance the terminal can process 160 characters i.e. 1200 baud. In practice usable baud rates are lower. Expect 800 baud. The reason for this is that for a TFT the CPU of the MCU has to render every changed character and create a bitmap which then has to be send to the display. In the olden times of the VT52, the rendering was done in the video hardware itself. 

The VT52 code tries to circumvent the problem by double buffering and by sending XON/XOFF messages. 




