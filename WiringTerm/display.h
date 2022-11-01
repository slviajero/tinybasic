/*
 *
 * $Id: hardware-arduino.h,v 1.4 2022/08/15 18:08:56 stefan Exp stefan $
 *
 * Stefan's basic interpreter - display driver component - isolated
 * 
 *  See the licence file on 
 * https://github.com/slviajero/tinybasic for copyright/left.
 *   (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 * Author: Stefan Lenz, sl001@serverfabrik.de
 *
 * Hardware definition file coming with IoTBasic.ino aka basic.c
 *
 * - ARDUINOLCD, ARDUINOTFT and LCDSHIELD active the LCD code, 
 *   LCDSHIELD automatically defines the right settings for 
 *   the classical shield modules
 *  - ARDUINOPS2 activates the PS2 code. Default pins are 2 and 3.
 *    If you use other pins the respective changes have to be made 
 *    below. 
 *  - _if_  and PS2 are both activated STANDALONE cause the Arduino
 *    to start with keyboard and lcd as standard devices.
 *  - ARDUINOEEPROM includes the EEPROM access code
 *  - ARDUINOEFS, ARDUINOSD, ESPSPIFFS, RP2040LITTLEFS activate filesystem code 
 * - activating Picoserial, Picoserial doesn't work on MEGA
 *
 * Architectures and the definitions from the Arduino IDE
 *
 *  ARDUINO_ARCH_SAM: no tone command, dtostrf
 *  ARDUINO_ARCH_RP2040: dtostrf (for ARDUINO_NANO_RP2040_CONNECT)
 *  ARDUINO_ARCH_SAMD: dtostrf (for ARDUINO_SAMD_MKRWIFI1010, ARDUINO_SEEED_XIAO_M0)
 *  ARDUINO_ARCH_ESP8266: SPIFFS, dtostrf (ESP8266)
 *  ARDUINO_AVR_MEGA2560, ARDUARDUINO_SAM_DUE: second serial port is Serial1 - no software serial
 *  ARDUINO_SAM_DUE: hardware heuristics
 *  ARDUINO_ARCH_AVR: nothing
 *  ARDUINO_ARCH_LGT8F: EEPROM code for flash EEPROM - platform fully supported now, yet no call 0
 *  ARDUINO_ARCH_ESP32 and ARDUINO_TTGO_T7_V14_Mini32, no tone, no analogWrite, avr/xyz obsolete
 *
 * The code still contains hardware heuristics from my own projects, 
 * will be removed in the future
 *
 */

#if defined(ARDUINO) && ! defined(__DISPLAYH__)
#define __DISPLAYH__ 

/* 
 * Arduino hardware settings , set here what you need or
 * use one of the predefined configurations below
 *
 * input/output methods USERPICOSERIAL, ARDUINOPS2
 *	ARDUINOPRT, DISPLAYCANSCROLL, ARDUINOLCDI2C,
 *	ARDUINOTFT, ARDUINONOKIA51, ARDUINOILI9488,
 *  ARDUINOSSD1306, ARDUINOMCUFRIEND
 *
 *	leave this unset if you use the definitions below
 */

#define DISPLAYCANSCROLL
#undef HASVT52
#undef ARDUINOLCDI2C
#undef ARDUINONOKIA51
#undef ARDUINOILI9488
#undef ARDUINOSSD1306
#define ARDUINOMCUFRIEND
#undef ARDUINOGRAPHDUMMY
#undef LCDSHIELD
#undef ARDUINOTFT

/* 
 * PIN settings and I2C addresses for various hardware configurations
 *	used a few heuristics and then the hardware definitions above 
 *
 *	#define SDPIN sets the SD CS pin - can be left as a default for most HW configs
 *   	TTGO needs it as default definitions in the board file are broken
 *	#define PS2DATAPIN, PS2IRQPIN sets PS2 pin
 */

/* use standard I2C pins almost always */
#undef SDA_PIN
#undef SCL_PIN

/*
 * DISPLAY driver code section, the hardware models define a set of 
 * of functions and definitions needed for the display driver. These are 
 *
 * dsp_rows, dsp_columns: size of the display
 * dspbegin(), dspprintchar(c, col, row), dspclear(), dspupdate()
 * 
 * All displays which have this functions can be used with the 
 * generic display driver below.
 * 
 * Graphics displays need to implement the functions 
 * 
 * rgbcolor(), vgacolor()
 * plot(), line(), rect(), frect(), circle(), fcircle() 
 * 
 * Color is currently either 24 bit or 4 bit 16 color vga.
 */

/* 
 *  these four function need to be implemented depending in the hardware
 *  in C this would be weak functions, in C++ virtual functions of the 
 */
void dspbegin();
void dspprintchar(char, short, short);
void dspclear();
void dspupdate();

/* the graphics functions */
void rgbcolor(int, int, int);
void vgacolor(short);
void plot(int, int);
void line(int, int, int, int);
void rect(int, int, int, int);
void frect(int, int, int, int);
void circle(int, int, int);
void fcircle(int, int, int);

/* the state of the display currently unused */
int dspstat(char);

/* set the cursor to one location */
void dspsetcursor(short, short);

/* returns true if display is active */
char dspactive();

/* update methods for displays with page mode like epaper */
void dspsetupdatemode(char);
char dspgetupdatemode();
void dspgraphupdate();

/* vt52 state engine */
void dspvt52(char*);

/* 0 normal scroll, 1 enable waitonscroll function */
void dspsetscrollmode(char, short);

/* clear the buffer */
void dspbufferclear();

/* do the scroll */
void dspscroll();

/* write one character */
void dspwrite(char);

/* wait function on scroll */
char dspwaitonscroll();

// defined _DISPLAYH_
#endif
