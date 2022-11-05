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

#include "display.h"
#include "Arduino.h"

/*
 * Some settings, defaults, and dependencies
 * 
 * Handling Wire and SPI is tricky as some of the libraries 
 * also include and start SPI and Wire code. 
 */

/* these displays needs wire */
#if defined(ARDUINOLCDI2C) || defined(ARDUINOSSD1306) 
#define ARDUINOWIRE
#endif

/* the NOKIA and ILI9488 display needs SPI */
#if defined(ARDUINONOKIA51) || defined(ARDUINOILI9488)
#define ARDUINOSPI
#endif

/* 
 * The PICO serial library saves memory for small systems
 * https://github.com/slviajero/PicoSerial
 */
#ifdef USESPICOSERIAL
#include <PicoSerial.h>
#endif

/* Standard SPI */
#ifdef ARDUINOSPI
#include <SPI.h>
#endif

/* Standard wire */
#ifdef ARDUINOWIRE
#include <Wire.h>
#endif

/* 
 * the display library includes for LCD
 */
#ifdef LCDSHIELD 
#include <LiquidCrystal.h>
#endif

#ifdef ARDUINOLCDI2C
#include <LiquidCrystal_I2C.h>
#endif

/*
 * This is the monochrome library of Oli Kraus
 * https://github.com/olikraus/u8g2/wiki/u8g2reference
 * It can harware scroll.
 */
#if defined(ARDUINONOKIA51) || defined(ARDUINOSSD1306)
#include <U8g2lib.h>
#endif

/*
 * This is the (old) ILI9488 library originally created by Jarett Burket
 * https://github.com/slviajero/ILI9488
 * It can hardware scroll (not yet used)
 */
#ifdef ARDUINOILI9488
#include <Adafruit_GFX.h>
#include <ILI9488.h>
#endif

/*
 * This is the MCUFRIED library originally for parallel TFTs
 * https://github.com/prenticedavid/MCUFRIEND_kbv
 * 
 */
#ifdef ARDUINOMCUFRIEND
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
#endif


/*
 * For TFT we use the UTFT library
 * http://www.rinkydinkelectronics.com/library.php?id=51
 * please note the License, it is not GPL but NON COMMERCIAL 
 * Creative Commons. 
 */
#ifdef ARDUINOTFT
#include <memorysaver.h>
#include <UTFT.h>
#endif


/* 
 *	Arduino default serial baudrate and serial flags for the 
 * two supported serial interfaces. Serial is always active and 
 * connected to channel &1 with 9600 baud. 
 * 
 * channel 4 (ARDUINOPRT) can be either in character or block 
 * mode. Blockmode is set as default here. This means that all 
 * available characters are always loaded to a string -> inb()
 */
const int serial_baudrate = 9600;
char sendcr = 0;
int serial1_baudrate = 9600;
short blockmode = 0;


/* 
 * start the SPI bus - this is a little mean as some libraries also 
 * try to start the SPI which may lead to on override of the PIN settings
 * if the library code is not clean - currenty no conflict known
 */
void spibegin() {
#ifdef ARDUINOSPI
  SPI.begin();
#endif
}

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
 * global variables for a standard LCD shield. 
 * Includes the standard Arduino LiquidCrystal library
 */
#ifdef LCDSHIELD 
#define DISPLAYDRIVER
/* LCD shield pins to Arduino
 *  RS, EN, d4, d5, d6, d7; 
 * backlight on pin 10;
 */
const int dsp_rows=2;
const int dsp_columns=16;
LiquidCrystal lcd( 8,  9,  4,  5,  6,  7);
void dspbegin() { 	lcd.begin(dsp_columns, dsp_rows); dspsetscrollmode(1, 1);  }
void dspprintchar(char c, short col, short row) { lcd.setCursor(col, row); lcd.write(c);}
void dspclear() { lcd.clear(); }
void dspupdate() {}
#endif

/* 
 * A LCD display connnected via I2C, uses the standard 
 * Arduino I2C display library.
 */ 
#ifdef ARDUINOLCDI2C
#define DISPLAYDRIVER
const int dsp_rows=4;
const int dsp_columns=20;
LiquidCrystal_I2C lcd(0x27, dsp_columns, dsp_rows);
void dspbegin() {   lcd.init(); lcd.backlight(); dspsetscrollmode(1, 1); }
void dspprintchar(char c, short col, short row) { lcd.setCursor(col, row); lcd.write(c);}
void dspclear() { lcd.clear(); }
void dspupdate() {}
#endif


/* 
 * A Nokia 5110 with ug8lib2 - can scroll quite well
 * https://github.com/olikraus/u8g2/wiki/u8g2reference
 * This is a buffered display it has a dspupdate() function 
 * it also needs to call dspgraphupdate() after each graphic 
 * operation
 *
 * default PIN settings here are for ESP8266, using the standard 
 * SPI SS for 15 for CS/CE, and 0 for DC, 2 for reset
 *
 */ 
#ifdef ARDUINONOKIA51
#define DISPLAYDRIVER
#ifndef NOKIA_CS
#define NOKIA_CS 15
#endif
#ifndef NOKIA_DC
#define NOKIA_DC 0
#endif
#ifndef NOKIA_RST
#define NOKIA_RST 2
#endif
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, NOKIA_CS, NOKIA_DC, NOKIA_RST); 
const int dsp_rows=6;
const int dsp_columns=10;
uint8_t dspfgcolor = 1;
uint8_t dspbgcolor = 0;
char dspfontsize = 8;
void dspbegin() { u8g2.begin(); u8g2.setFont(u8g2_font_amstrad_cpc_extended_8r); }
void dspprintchar(char c, short col, short row) { char b[] = { 0, 0 }; b[0]=c; u8g2.drawStr(col*dspfontsize+2, (row+1)*dspfontsize, b); }
void dspclear() { u8g2.clearBuffer(); u8g2.sendBuffer(); }
void dspupdate() { u8g2.sendBuffer(); }
void rgbcolor(int r, int g, int b) {}
void vgacolor(short c) { dspfgcolor=c%3; u8g2.setDrawColor(dspfgcolor); }
void plot(int x, int y) { u8g2.setDrawColor(dspfgcolor); u8g2.drawPixel(x, y); dspgraphupdate(); }
void line(int x0, int y0, int x1, int y1)   { u8g2.drawLine(x0, y0, x1, y1);  dspgraphupdate(); }
void rect(int x0, int y0, int x1, int y1)   { u8g2.drawFrame(x0, y0, x1-x0, y1-y0);  dspgraphupdate(); }
void frect(int x0, int y0, int x1, int y1)  { u8g2.drawBox(x0, y0, x1-x0, y1-y0);  dspgraphupdate(); }
void circle(int x0, int y0, int r) { u8g2.drawCircle(x0, y0, r); dspgraphupdate(); }
void fcircle(int x0, int y0, int r) { u8g2.drawDisc(x0, y0, r); dspgraphupdate(); }
#endif

/* 
 * Small SSD1306 OLED displays with I2C interface
 * This is a buffered display it has a dspupdate() function 
 * it also needs to call dspgraphupdate() after each graphic 
 * operation
 */ 
#ifdef ARDUINOSSD1306
#define DISPLAYDRIVER
#define SSD1306WIDTH 32
#define SSD1306HEIGHT 128
/* constructors may look like this, last argument is the reset pin
 * //U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);
 * //U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0,  SCL, SDA, U8X8_PIN_NONE);  
 */
#if SSD1306WIDTH == 32
/* U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, SCL, SDA, U8X8_PIN_NONE); 
 * use hardware I2C instead of software. Tested (only) on ESP32C3 and ESP8266 so far.
 */
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
#endif
#if SSD1306WIDTH == 64
/* the Heltec board has an internal software I2C on pins 4=SDA and 15=SCL */
#ifdef ARDUINO_heltec_wifi_lora_32_V2
U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, 15, 4, 16); 
#else 
/* use hardware I2C instead of software. Tested (only) on ESP32C3 and ESP8266 so far.
 */
U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0); 
#endif
#endif
const char dspfontsize = 8;
const int dsp_rows=SSD1306WIDTH/dspfontsize;
const int dsp_columns=SSD1306HEIGHT/dspfontsize;
uint16_t dspfgcolor = 1;
uint16_t dspbgcolor = 0;
void dspbegin() { u8g2.begin(); u8g2.setFont(u8g2_font_amstrad_cpc_extended_8r); }
void dspprintchar(char c, short col, short row) { char b[] = { 0, 0 }; b[0]=c; u8g2.drawStr(col*dspfontsize+2, (row+1)*dspfontsize, b); }
void dspclear() { u8g2.clearBuffer(); u8g2.sendBuffer(); }
void dspupdate() { u8g2.sendBuffer(); }
void rgbcolor(int r, int g, int b) {}
void vgacolor(short c) { dspfgcolor=c%3; u8g2.setDrawColor(dspfgcolor); }
void plot(int x, int y) { u8g2.setDrawColor(dspfgcolor); u8g2.drawPixel(x, y); dspgraphupdate(); }
void line(int x0, int y0, int x1, int y1)   { u8g2.drawLine(x0, y0, x1, y1); dspgraphupdate(); }
void rect(int x0, int y0, int x1, int y1)   { u8g2.drawFrame(x0, y0, x1-x0, y1-y0);  dspgraphupdate(); }
void frect(int x0, int y0, int x1, int y1)  { u8g2.drawBox(x0, y0, x1-x0, y1-y0);  dspgraphupdate(); }
void circle(int x0, int y0, int r) { u8g2.drawCircle(x0, y0, r); dspgraphupdate(); }
void fcircle(int x0, int y0, int r) { u8g2.drawDisc(x0, y0, r); dspgraphupdate(); }
#endif

/* 
 * A ILI9488 with Jarett Burkets version of Adafruit GFX and patches
 * by Stefan Lenz
 * currently only slow software scrolling implemented in BASIC
 * 
 * https://github.com/slviajero/ILI9488
 * 
 * we use 9, 8, 7 as CS, CE, RST by default and A7 for the led brightness control
 */ 
 
#ifdef ARDUINOILI9488
#define DISPLAYDRIVER
#ifndef ILI_CS
#define ILI_CS  9
#endif
#ifndef ILI_DC
#define ILI_DC  8
#endif
#ifndef ILI_RST
#define ILI_RST 7
#endif
#ifndef ILI_LED
#define ILI_LED A3
#endif
ILI9488 tft = ILI9488(ILI_CS, ILI_DC, ILI_RST);
/* ILI in landscape */
const int dsp_rows=20;
const int dsp_columns=30;
char dspfontsize = 16;
uint16_t dspfgcolor = ILI9488_WHITE;
uint16_t dspbgcolor = ILI9488_BLACK;
void dspbegin() { 
  tft.begin(); 
  tft.setRotation(3); /* ILI in landscape, SD slot up */
  tft.setTextColor(dspfgcolor);
  tft.setTextSize(2);
  tft.fillScreen(dspbgcolor); 
  pinMode(ILI_LED, OUTPUT);
  analogWrite(ILI_LED, 255);
  dspsetscrollmode(1, 4);
 }
void dspprintchar(char c, short col, short row) { tft.drawChar(col*dspfontsize, row*dspfontsize, c, dspfgcolor, dspbgcolor, 2); }
void dspclear() { tft.fillScreen(dspbgcolor); }
void dspupdate() {}
void rgbcolor(int r, int g, int b) { dspfgcolor=tft.color565(r, g, b);}
void vgacolor(short c) {  
  short base=128;
  if (c==8) { rgbcolor(64, 64, 64); return; }
  if (c>8) base=255;
  rgbcolor(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
}
void plot(int x, int y) { tft.drawPixel(x, y, dspfgcolor); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1, dspfgcolor); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, x0, x1, y1, dspfgcolor);}
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, x0, x1, y1, dspfgcolor); }
void circle(int x0, int y0, int r) { tft.drawCircle(x0, y0, r, dspfgcolor); }
void fcircle(int x0, int y0, int r) { tft.fillCircle(x0, y0, r, dspfgcolor); }
#endif

/* 
 * A MCUFRIEND parallel port display for the various tft shields 
 * This implementation is mainly for Arduino MEGA
 * 
 * currently only slow software scrolling implemented in BASIC
 * 
 */ 

#ifdef ARDUINOMCUFRIEND
#define DISPLAYDRIVER
#ifndef LCD_CS
#define LCD_CS  A3
#endif
#ifndef LCD_CD
#define LCD_CD  A2
#endif
#ifndef LCD_WR
#define LCD_WR  A1
#endif
#ifndef LCD_RD
#define LCD_RD  A0
#endif
#ifndef LCD_RESET
#define LCD_RESET A4
#endif
MCUFRIEND_kbv tft;
/* ILI in landscape */
const int dsp_rows=20;
const int dsp_columns=30;
char dspfontsize = 16;
uint16_t dspfgcolor = 0xFFFF;
uint16_t dspbgcolor = 0x0000;
void dspbegin() { 
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9481; /* write-only shield - taken from the MCDFRIEND demo */
  tft.begin(ID); 
  tft.setRotation(1); /* ILI in landscape, 3: SD slot on right the side */
  tft.setTextColor(dspfgcolor);
  tft.setTextSize(2);
  tft.fillScreen(dspbgcolor); 
  dspsetscrollmode(1, 4);
 }
void dspprintchar(char c, short col, short row) { tft.drawChar(col*dspfontsize, row*dspfontsize, c, dspfgcolor, dspbgcolor, 2); }
void dspclear() { tft.fillScreen(dspbgcolor); }
void dspupdate() {}
void rgbcolor(int r, int g, int b) { dspfgcolor=tft.color565(r, g, b);}
void vgacolor(short c) {  
  short base=128;
  if (c==8) { rgbcolor(64, 64, 64); return; }
  if (c>8) base=255;
  rgbcolor(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
}
void plot(int x, int y) { tft.drawPixel(x, y, dspfgcolor); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1, dspfgcolor); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, x0, x1, y1, dspfgcolor);}
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, x0, x1, y1, dspfgcolor); }
void circle(int x0, int y0, int r) { tft.drawCircle(x0, y0, r, dspfgcolor); }
void fcircle(int x0, int y0, int r) { tft.fillCircle(x0, y0, r, dspfgcolor); }
#endif

/* 
 * A no operations graphics dummy  
 * Tests the BASIC side of the graphics code without triggering 
 * any output
 */ 
#ifdef ARDUINOGRAPHDUMMY
#define DISPLAYDRIVER
const int dsp_rows=20;
const int dsp_columns=30;
char dspfontsize = 16;
uint16_t dspfgcolor = 0xFFFF;
uint16_t dspbgcolor = 0x0000;
void dspbegin() { dspsetscrollmode(1, 4); }
void dspprintchar(char c, short col, short row) { }
void dspclear() { }
void dspupdate() {}
void rgbcolor(int r, int g, int b) { dspfgcolor=0; }
void vgacolor(short c) {  
  short base=128;
  if (c==8) { rgbcolor(64, 64, 64); return; }
  if (c>8) base=255;
  rgbcolor(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
}
void plot(int x, int y) { }
void line(int x0, int y0, int x1, int y1)   { }
void rect(int x0, int y0, int x1, int y1)   { }
void frect(int x0, int y0, int x1, int y1)  {  }
void circle(int x0, int y0, int r) {  }
void fcircle(int x0, int y0, int r) {  }
#endif


/*
 * SD1963 TFT display code with UTFT.
 * Tested witth SD1963 800*480 board. 
 *	it is mainly intended for a MEGA or DUE as a all in one system
 *	this is for a MEGA shield and the CTE DUE shield, for the due 
 *	you need to read the comment in Arduino/libraries/UTFT/hardware/arm
 *	HW_ARM_defines.h -> uncomment the DUE shield
 * See also 
  * https://github.com/slviajero/tinybasic/wiki/Projects:-4.-A-standalone-computer-with-a-TFT-screen-based-on-a-DUE
 */
#ifdef ARDUINOTFT
#define DISPLAYDRIVER
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
#ifdef ARDUINO_SAM_DUE
UTFT tft(CTE70,25,26,27,28);
#else 
UTFT tft(CTE70,38,39,40,41);
#endif
const int dsp_rows=30;
const int dsp_columns=50;
char dspfontsize = 16;
void dspbegin() { tft.InitLCD(); tft.setFont(BigFont); tft.clrScr(); dspsetscrollmode(1, 4); }
void dspprintchar(char c, short col, short row) { tft.printChar(c, col*dspfontsize, row*dspfontsize); }
void dspclear() { tft.clrScr(); }
void dspupdate() {}
/*
 * Graphics code - not part of the display driver library
 */
#ifdef HASGRAPH
void rgbcolor(int r, int g, int b) { tft.setColor(r,g,b); }
void vgacolor(short c) {
	short base=128;
	if (c==8) { rgbcolor(64, 64, 64); return; }
	if (c>8) base=255;
	rgbcolor(base*(c&1), base*((c&2)/2), base*((c&4)/4));  
}
void plot(int x, int y) { tft.drawPixel(x, y); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, y0, x1, y1); }
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, y0, x1, y1); }
void circle(int x0, int y0, int r) { tft.drawCircle(x0, y0, r); }
void fcircle(int x0, int y0, int r) { tft.fillCircle(x0, y0, r); }
#endif
#endif


/*
 * this is a generic display code 
 * it combines the functions of LCD and TFT drivers
 * if this code is active 
 *
 * dspprintchar(char c, short col, short row)
 * dspclear()
 * spbegin()
 *
 * have to be defined before in a hardware dependent section.
 *
 * VGA systems don't use the display driver for text based output.
 *
 * The display driver exists as a buffered version that can scroll
 * or an unbuffered version that cannot scroll. Interfaces to hardware
 * scrolling are not yet implemented.
 * 
 * dspupdatemode controls the page update behaviour 
 *    0: character mode, display each character separately
 *    1: line mode, update the display after each line
 *    2: page mode, update the display after a ETX
 * ignored if the display has no update function
 *
 */

#ifdef DISPLAYDRIVER
short dspmycol = 0;
short dspmyrow = 0;
char vt52esc = 0;
char dspupdatemode = 0;

int dspstat(char c) {return 0; }

void dspsetcursor(short c, short r) {
	dspmyrow=r;
	dspmycol=c;
}

char dspactive() {
	return 1;
}

/*
 * control the update modes for page update displays
 * like Epaper, Nokia 5110 and SSD1306
 * dspgraphupdate is the helper for the graphics functions
 */

void dspsetupdatemode(char c) {
  dspupdatemode=c;
}

char dspgetupdatemode() {
  return dspupdatemode;
}

void dspgraphupdate() {
  if (dspupdatemode == 0) dspupdate();
}

/* 
 * This is the minimalistic VT52 state engine. It is an interface to 
 * process single byte control sequences of the form <ESC> char 
 *
 * It is planned to implement an interface to the wiring, graphics,
 * and print library here.
 */
#define VT52DEBUG 1

#ifdef HASVT52
/* the state variables, and modes */
char vt52state = 0;

/* the stack */
const char vt52stacksize = 16;
char vt52stack[vt52stacksize];
char vt52sp = 0;

/* the output buffer */
const char vt52buffersize = 16;
char vt52buffer[vt52buffersize];
char vt52bindex = 0;

void vt52push(char c) {
  if (vt52sp < vt52stacksize) vt52stack[vt52sp++]=c-32;
}

char vt52pop() {
  if (vt52sp > 0) return vt52stack[--vt52sp];
}

void vt52error(char e) {
  vt52state=0;
  vt52esc=0;
  vt52sp=0;
  /* handle error messages here */
  dspprintchar(e, 10, 0);
  if (VT52DEBUG) vt52debug(' ');
}

void vt52debug(char c) {
  if (vt52state>0) dspprintchar(vt52state,0, dsp_rows-1); else dspprintchar('0' ,0, dsp_rows-1);
  dspprintchar(' ', 1, dsp_rows-1); 
  dspprintchar(vt52esc+48, 2, dsp_rows-1);
  dspprintchar(vt52sp+48, 3, dsp_rows-1);
  dspprintchar(c, 4, dsp_rows-1);
}

void vt52out(char c) {
  if (vt52bindex < vt52buffersize) vt52buffer[vt52bindex++]=c;
}

int vt52avail() {
  return vt52bindex;
}

int vt52read() {
  if (vt52bindex > 0) return vt52buffer[--vt52bindex]; else return 0;
}

/* vt52 state engine */
void dspvt52(char* c){

/* 
 * reading and processing multi byte commands
 * vt52state tells us which multibyte command we deal with
 * vt52esc tells us how many arguments we still expect
 */
  int ic=*c;
  int mode, pin, val;

  /* vt52debug(ic); */
 
	switch (vt52state) {
		case 'Y': /* cursor address mode */
      switch(vt52esc) {
        case 1: 
          dspmycol=(*c-32)%dsp_columns;
          vt52state=0; /* last argument processed - we are done */
          break;
        case 2: 
          dspmyrow=(*c-32)%dsp_rows;
          break;
      }
      if (vt52esc > 0) vt52esc--; /* received one character */
      *c=0;
			return;
    case 'w': /* process wiring commands */
      /* 
       * stuff happens here until no more bytes are expected 
       * we use numbers base 64 here. One argument addresses numbers from 0-63 
       * two arguments numbers from 0 to 4095, least significant byte first
       * numbers between 32 and 96 are arguments base 64, 96 and above are commands, a 
       * commands used the arguments in the stack and exits 
       */
      if (ic < 96) {
        if (vt52sp == vt52stacksize) { vt52error('s'); return; }
        vt52push(ic); 
      } else {
        switch(ic) {
          case 97: /* 'a' pinMode */
            if (vt52sp != 2) { vt52error('a'); return; }
            mode=vt52pop();
            pin=vt52pop();
            pinMode(pin, mode);
            break;   
          case 98: /* 'b' digitalWrite */
            if (vt52sp != 2) { vt52error('a'); return; }
            val=vt52pop();
            pin=vt52pop();
            digitalWrite(pin, val);
            break;
          case 99: /* 'c' digitalRead */
            if (vt52sp != 1) { vt52error('a'); return; }
            pin=vt52pop();
            val=digitalRead(pin);
            vt52out(32+val);
            break;
          case 100: /* 'd' analogWrite */
            if (vt52sp != 3) { vt52error('a'); return; }
            val=64*vt52pop()+vt52pop();
            pin=vt52pop();
            analogWrite(pin, val);
            break;
          case 101: /* 'e' analogRead */
            if (vt52sp != 1) { vt52error('a'); return; }
            pin=vt52pop();
            val=analogRead(pin);
            vt52out(val%64+32);
            vt52out(val/64+32); 
            break;
          case 'x': /* exit wiring mode */
          default:
            vt52sp=0;
            vt52state=0;
            vt52esc=0;
        }    
      }
      if (VT52DEBUG) vt52debug(*c);
      *c=0;
      return;
	}
 
/* 
 * commands of the terminal in text mode, single byte escape sequences 
 * do their job here and break
 * multi byte and state changes need to set c accordingly and return (!)
 */
	switch (*c) {
		case 'A': /* cursor up */
			if (dspmyrow > 0) dspmyrow--;
			break;
		case 'B': /* cursor down */
			if (dspmyrow < dsp_rows) dspmyrow++;
			break;
		case 'C': /* cursor right */
			if (dspmycol < dsp_columns) dspmycol++;
			break; 
		case 'D': /* cursor left */
			if (dspmycol>0) dspmycol--;
			break;
		case 'E': /* GEMDOS / TOS extension clear screen */
			dspbufferclear();
			dspclear();
			break;
		case 'H': /* cursor home */
			dspmyrow=0;
   		dspmycol=0;
			break;	
		case 'Y': /* Set cursor position - expect two arguments, delete the char*/
			vt52state='Y';
			vt52esc=2;
  		*c=0;
			return;

/* 
 *  these are some (standard) VT52 functions and GEMDOS extensions reinterpreted 
 *  
 */ 
    case 'F': // enter graphics mode
    case 'G': // exit graphics mode
		case 'I': // reverse line feed
		case 'J': // clear to end of screen
		case 'K': // clear to end of line
		case 'L': // Insert line
		case 'M': // Delete line
		case 'Z': // Ident
		case '=': // alternate keypad on
		case '>': // alternate keypad off
		case 'b': // GEMDOS / TOS extension text color
		case 'c': // GEMDOS / TOS extension background color
		case 'd': // GEMDOS / TOS extension clear to start of screen
		case 'e': // GEMDOS / TOS extension enable cursor
		case 'f': // GEMDOS / TOS extension disable cursor
		case 'j': // GEMDOS / TOS extension restore cursor
		case 'k': // GEMDOS / TOS extension save cursor
		case 'l': // GEMDOS / TOS extension clear line
		case 'o': // GEMDOS / TOS extension clear to start of line		
		case 'p': // GEMDOS / TOS extension reverse video
		case 'q': // GEMDOS / TOS extension normal video
		case 'v': // GEMDOS / TOS extension enable wrap
		case 'w': // GEMDOS / TOS extension disable wrap
		case '^': // Printer extensions - print on
		case '_': // Printer extensions - print off
		case 'W': // Printer extensions - print without display on
		case 'X': // Printer extensions - print without display off
		case 'V': // Printer extensions - print cursor line
		case ']': // Printer extension  - print screen 
			break;
/* 
 *  the Arduino interface extensions defined in IoT BASIC
 *  access to some functions of BASIC through escape sequences 
 *  x is graphics mode, y is the wiring mode
 */
    case 'x':
      vt52state='g';
      *c=0;
      return;
    case 'y':
      vt52state='w';
      if (VT52DEBUG) vt52debug(*c); 
      *c=0;
      return;
	}
/* 
 *  forget the character and leave escape mode, this is the fate of all single 
 *  character escape sequences
 */
  vt52esc=0;
  *c=0;
}
#endif

/* VT52 display can give stuff back if the advanced functions are implemented */
#ifdef HASVT52
int dspavail() {return vt52avail(); }
char dspread() {return vt52read(); }
#else 
int dspavail() {return 0; }
char dspread() {return 0; }
#endif

/* scrollable displays need a character buffer */
#ifdef DISPLAYCANSCROLL
char dspbuffer[dsp_rows][dsp_columns];
char dspscrollmode = 0;
short dsp_scroll_rows = 1; 

/* 0 normal scroll, 1 enable waitonscroll function */
void dspsetscrollmode(char c, short l) {
	dspscrollmode = c;
	dsp_scroll_rows = l;
}

/* clear the buffer */
void dspbufferclear() {
	short r,c;
	for (r=0; r<dsp_rows; r++)
		for (c=0; c<dsp_columns; c++)
      dspbuffer[r][c]=0;
  dspmyrow=0;
  dspmycol=0;
}

/* do the scroll */
void dspscroll(){
	short r,c;
	int i;
  char a,b;
  	
/* scroll data up and redraw the display */
  for (r=0; r<dsp_rows-dsp_scroll_rows; r++) { 
    for (c=0; c<dsp_columns; c++) {
			a=dspbuffer[r][c];
			b=dspbuffer[r+dsp_scroll_rows][c];
			if ( a != b ) {
  			if (b >= 32) dspprintchar(b, c, r); else dspprintchar(' ', c, r);
      }      
			dspbuffer[r][c]=b;
		} 
	}

/* delete the characters in the remaining lines */
	for (r=dsp_rows-dsp_scroll_rows; r<dsp_rows; r++) {
		for (c=0; c<dsp_columns; c++) {
			if (dspbuffer[r][c] > 32) dspprintchar(' ', c, r); 
			dspbuffer[r][c]=0;     
    }
	}
  
/* set the cursor to the fist free line	*/ 
  dspmycol=0;
	dspmyrow=dsp_rows-dsp_scroll_rows;
}

void dspwrite(char c){

/* on escape call the vt52 state engine */
#ifdef HASVT52
	if (vt52esc) dspvt52(&c);
#endif

/* the minimal cursor control functions of BASIC */
  switch(c) {
  	case 10: // this is LF Unix style doing also a CR
    	dspmyrow=(dspmyrow + 1);
    	if (dspmyrow >= dsp_rows) dspscroll(); 
    	dspmycol=0;
      if (dspupdatemode == 1) dspupdate();
    	return;
    case 12: // form feed is clear screen 
    	dspbufferclear();
    	dspclear();
    	return;
    case 13: // classical carriage return 
      dspmycol=0;
      return;
  	case 27: // escape - initiate vtxxx mode
			vt52esc=1;
			return;
    case 127: // delete
    	if (dspmycol > 0) {
      	dspmycol--;
      	dspbuffer[dspmyrow][dspmycol]=0;
      	dspprintchar(' ', dspmycol, dspmyrow);
      	return;
    	}
    case 3: // ETX = Update display for buffered display like Epaper
      dspupdate();
      return;
  }

/* all other non printables ignored */ 
	if (c < 32 ) return; 

	dspprintchar(c, dspmycol, dspmyrow);
	dspbuffer[dspmyrow][dspmycol++]=c;
	if (dspmycol == dsp_columns) {
		dspmycol=0;
		dspmyrow=(dspmyrow + 1);
    if (dspmyrow >= dsp_rows) dspscroll(); 
	}
  if (dspupdatemode == 0) dspupdate();
}

/* again a break in API, using inch here */
char dspwaitonscroll() {
	return 0;
}

/* code for low memory simple display access */
#else 

void dspbufferclear() {}
char dspwaitonscroll() { return 0; }
void dspwrite(char c){

/* on escape call the vt52 state engine */
#ifdef HASVT52
	if (vt52esc) { dspvt52(&c); }
#endif

	switch(c) {
  	case 12: // form feed is clear screen plus home
    	dspclear();
    	dspmyrow=0;
      dspmycol=0;
    	return;
  	case 10: // this is LF Unix style doing also a CR
    	dspmyrow=(dspmyrow + 1)%dsp_rows;
    	dspmycol=0;
      if (dspupdatemode == 1) dspupdate();
    	return;
    case 11: // one char down 
    	dspmyrow=(dspmyrow+1) % dsp_rows;
    	return;
    case 13: // classical carriage return 
    	dspmycol=0;
    	return;
		case 27: // escape - initiate vtxxx mode
			vt52esc=1;
			return;
   	case 127: // delete
    	if (dspmycol > 0) {
      	dspmycol--;
      	dspprintchar(' ', dspmycol, dspmyrow);
    	}
    	return;
    case 3: // ETX = Update display for buffered display like Epaper
      dspupdate();
      return;
  }

/* all other non printables ignored */ 
	if (c < 32 ) return; 

	dspprintchar(c, dspmycol++, dspmyrow);
	dspmycol=dspmycol%dsp_columns;
  if (dspupdatemode == 0) dspupdate();
}

void dspsetscrollmode(char c, short l) {}
#endif
#else
const int dsp_rows=0;
const int dsp_columns=0;
void dspsetupdatemode(char c) {}
void dspwrite(char c){};
void dspbegin() {};
int dspstat(char c) {return 0; }
char dspwaitonscroll() { return 0; };
char dspactive() {return 0; }
void dspsetscrollmode(char c, short l) {}
void dspsetcursor(short c, short r) {}
int dspavail() {return 0;}
char dspread() {return 0;}
#endif
