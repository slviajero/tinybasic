/*

  $Id: hardware-arduino.h,v 1.2 2022/04/10 06:25:05 stefan Exp stefan $

  Stefan's basic interpreter 

  Playing around with frugal programming. See the licence file on 
  https://github.com/slviajero/tinybasic for copyright/left.
    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)

  Author: Stefan Lenz, sl001@serverfabrik.de

  Hardware definition file coming with TinybasicArduino.ino aka basic.c

  - ARDUINOLCD, ARDUINOTFT and LCDSHIELD active the LCD code, 
    LCDSHIELD automatically defines the right settings for 
    the classical shield modules
  - ARDUINOPS2 activates the PS2 code. Default pins are 2 and 3.
    If you use other pins the respective changes have to be made 
      below. 
  - _if_  and PS2 are both activated STANDALONE cause the Arduino
      to start with keyboard and lcd as standard devices.
  - ARDUINOEEPROM includes the EEPROM access code
  - ARDUINOEFS, ARDUINOSD, ESPSPIFFS, RP2040LITTLEFS activate filesystem code 
  - activating Picoserial, Picoserial doesn't work on MEGA

  Architectures and the definitions from the Arduino IDE

    ARDUINO_ARCH_SAM: no tone command, dtostrf
    ARDUINO_ARCH_RP2040: dtostrf (for ARDUINO_NANO_RP2040_CONNECT)
    ARDUINO_ARCH_SAMD: dtostrf (for ARDUINO_SAMD_MKRWIFI1010, ARDUINO_SEEED_XIAO_M0)
    ARDUINO_ARCH_ESP8266: SPIFFS, dtostrf (ESP8266)
    ARDUINO_AVR_MEGA2560, ARDUARDUINO_SAM_DUE: second serial port is Serial1 - no software serial
    ARDUARDUINO_SAM_DUE: hardware heuristics
    ARDUINO_ARCH_AVR: nothing
    ARDUINO_AVR_LARDU_328E: odd EEPROM code, seems to work, somehow
    ARDUINO_ARCH_EXP32 and ARDUINO_TTGO_T7_V14_Mini32, no tone, no analogWrite, avr/xyz obsolete

  The code still contains hardware heuristics from my own projects, 
  will be removed in the future

*/

#if defined(ARDUINO) && ! defined(__HARDWAREH__)
#define __HARDWAREH__ 

/* 
	Arduino hardware settings , set here what you need or
	use one of the predefined configurations below

	input/output methods USERPICOSERIAL, ARDUINOPS2
		ARDUINOPRT, DISPLAYCANSCROLL, ARDUINOLCDI2C,
		ARDUINOTFT
	storage ARDUINOEEPROM, ARDUINOSD, ESPSPIFFS
	sensors ARDUINORTC, ARDUINOWIRE, ARDUINOSENSORS
	network ARDUINORF24, ARDUNIOMQTT

	leave this unset if you use the definitions below
*/
#undef USESPICOSERIAL 
#undef ARDUINOPS2
#undef ARDUINOPRT
#undef DISPLAYCANSCROLL
#undef ARDUINOLCDI2C
#undef ARDUINONOKIA51
#undef ARDUINOILI9488
#undef LCDSHIELD
#undef ARDUINOTFT
#undef ARDUINOVGA
#undef ARDUINOEEPROM
#undef ARDUINOEFS
#undef ARDUINOSD
#undef ESPSPIFFS
#undef RP2040LITTLEFS
#undef ARDUINORTC
#undef ARDUINOWIRE
#undef ARDUINORF24
#undef ARDUINOETH
#undef ARDUINOMQTT
#undef ARDUINOSENSORS
#undef STANDALONE

/* 
	Predefined hardware configurations, this assumes that all of the 
	above are undef

	UNOPLAIN: 
		a plain UNO with no peripherals
	AVRLCD: 
		a AVR system with an LCD shield
	WEMOSSHIELD: 
		a Wemos D1 with a modified simple datalogger shield
		optional keyboard and i2c display
	MEGASHIELD: 
		an Arduino Mega with Ethernet Shield
		optional keyboard and i2c display
	TTGOVGA: 
		TTGO VGA1.4 system with PS2 keyboard, standalone
  MEGATFT, DUETFT
    TFT 7inch screen systems, standalone
  NANOBOARD
    Arduino Nano Every board with PS2 keyboard and sensor 
    kit
  ESP01BOARD
    ESP01 based board as a sensor / MQTT interface
*/

#undef UNOPLAIN
#undef AVRLCD
#undef WEMOSSHIELD
#undef ESP01BOARD
#undef MEGASHIELD
#undef TTGOVGA
#undef DUETFT
#undef MEGATFT
#undef NANOBOARD

/* 
	PIN settings and I2C addresses for various hardware configurations
	used a few heuristics and then the hardware definitions above 

	#define SDPIN sets the SD CS pin - can be left as a default for most HW configs
    	TTGO needs it as default definitions in the board file are broken
	#define PS2DATAPIN, PS2IRQPIN sets PS2 pin
*/

/* PS2 Keyboard pins for AVR - use one interrupt pin 2 and one date pin 
    5 not 4 because 4 conflicts with SDPIN of the standard SD shield */
#define PS2DATAPIN 5
#define PS2IRQPIN  2

/* Ethernet - 10 is the default */
//#define ETHPIN 10

/* The Pretzelboard definitions for Software Serial, conflicts with SPI */
#define SOFTSERIALRX 11
#define SOFTSERIALTX 12

/* near field pin settings for CE and CSN*/
#define RF24CEPIN 8
#define RF24CSNPIN 9

/* 
 *  list of default i2c addresses
 *
 * some clock modules do this: #define EEPROMI2CADDR 0x057
 * 0x050 this is the default lowest adress of standard EEPROMs
 * default for the size is 4096, define your EFS EEPROM size here 
 */
#define EEPROMI2CADDR 0x057
#define RTCI2CADDR 0x068
#undef EFSEEPROMSIZE

/*
 * Sensor library code - experimental
 */
#ifdef ARDUINOSENSORS
#define ARDUINODHT
#define DHTTYPE DHT22
#define DHTPIN 2
#define ARDUINOSHT
#ifdef ARDUINOSHT
#define ARDUINOWIRE
#endif
#undef  ARDUINOMQ2
#define MQ2PIN A0
#undef ARDUINOLMS6
#endif

/*
 * the hardware models
 *	These are predefined hardware configutations 
 */

/* an AVR based Arduino with nothing else */
#if defined(UNOPLAIN)
#define ARDUINOEEPROM
#endif

/* an AVR ARDUINO (UNO or MEGA) with the classical LCD shield */
#if defined(AVRLCD)
#define ARDUINOEEPROM
#define DISPLAYCANSCROLL
#define LCDSHIELD
#endif

/*
 * a Wemos ESP8266 with a mdified datalogger shield 
 * standalone capable, with Wire and MQTT.
 */
#if defined(WEMOSSHIELD)
#define ARDUINOEEPROM
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOLCDI2C
#define ARDUINOSD
#define ARDUINORTC
#define ARDUINOWIRE
#define SDPIN 			D8
#define PS2DATAPIN	D2
#define PS2IRQPIN		D9
#define ARDUINOMQTT
#endif

/* an ESP01 board, using the internal flash */
#if defined(ESP01BOARD)
#define ARDUINOEEPROM
#define ESPSPIFFS
#define ARDUINOMQTT
#endif

/*
 * mega with a Ethernet shield 
 * standalone capable, Ethernet is not enabled by default
 */
#if defined(MEGASHIELD)
#define ARDUINOEEPROM
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOLCDI2C
#define ARDUINOSD
#define ARDUINOWIRE
#define ARDUINOPRT
#define SDPIN	4
#endif

/* 
 * VGA system with SD card, based on the TTGO VGA 1.4 
 * ESP32 
 * standalone by default, with MQTT
 */
#if defined(TTGOVGA)
#define ARDUINOEEPROM
#define ARDUINOVGA
#define ARDUINOSD
#define ARDUINOMQTT
#define SDPIN   13
#define STANDALONE 
#endif

/*
 * MEGA with a TFT shield, standalone by default
 */
#if defined(MEGATFT)
#define ARDUINOEEPROM
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOTFT
#define ARDUINOSD
#define ARDUINOWIRE
#define ARDUINOPRT
#define PS2DATAPIN 18
#define PS2IRQPIN  19
#define SDPIN 53
#define STANDALONE
#endif

/*
 * DUE with a TFT shield, standalone by default
 */
#if defined(DUETFT)
#undef  ARDUINOEEPROM
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOTFT
#define ARDUINOSD
#define ARDUINOWIRE
#define ARDUINOPRT
#define PS2DATAPIN 9
#define PS2IRQPIN  8
#define SDPIN 53
#define STANDALONE
#endif

#if defined(NANOBOARD)
#undef USESPICOSERIAL 
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOLCDI2C
#define ARDUINOEEPROM
#define ARDUINOPRT
#define ARDUINOEFS
#define ARDUINORTC
#define ARDUINOWIRE
#define EEPROMI2CADDR 0x050 /* use clock EEPROM 0x057, set to 0x050 for external EEPROM */
#define STANDALONE
#endif

/* 
 * the non AVR arcitectures - this is somehow raw
 * the ARDUINO 100 definition is probably not needed anymore
 */

#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040)
#include <avr/dtostrf.h>
#define ARDUINO 100
#endif


/*
 * Some settings, defaults, and dependencies
 * 
 * Handling Wire and SPI is tricky as some of the libraries 
 * also include and start SPI and Wire code. 
 */

/* a clock needs wire */
#ifdef ARDUINORTC
#define ARDUINOWIRE
#endif

/* a display needs wire */
#ifdef ARDUINOLCDI2C
#define ARDUINOWIRE
#endif

/* EEPROM storage needs wire */
#if defined(ARDUINOEFS)
#define ARDUINOWIRE
#endif

/* radio needs SPI */
#ifdef ARDUINORF24
#define ARDUINOSPI
#endif

/* a filesystem needs SPI */
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
#define ARDUINOSPI
#endif

/* networking may need SPI */
#ifdef ARDUINOMQTT
#define ARDUINOSPI
#endif

/* the NOKIA and ILI9488 display needs SPI */
#if defined(ARDUINONOKIA51) || defined(ARDUINOILI9488)
#define ARDUINOSPI
#endif

/* Networking needs the background task capability */
#if defined(ARDUINOMQTT) || defined(ARDUINOETH)
#define ARDUINOBGTASK
#endif

/* 
 * graphics adapter only when graphics hardware, overriding the 
 * language setting 
 * this is odd and can be removed later on
 */
#if !defined(ARDUINOTFT) && !defined(ARDUINOVGA) && !defined(ARDUINOILI9488) && !defined(ARDUINONOKIA51)
#undef HASGRAPH
#endif

/* 
 * Keyboard library, on AVR systems Paul Stoffregens original 
 * PS2 library works. For ESP use my patched version from 
 * https://github.com/slviajero/PS2Keyboard
 */
#ifdef ARDUINOPS2
#include <PS2Keyboard.h>
#endif

/*
 * ESPy stuff, pgmspace has changed location 
 */
#ifdef ARDUINOPROGMEM
#ifdef ARDUINO_ARCH_ESP32
#include <pgmspace.h>
#else
#include <avr/pgmspace.h>
#endif
#endif

/*
 * This works for AVR and ESP EEPROM dummy. Throws a 
 * compiler error for other platforms.
 */
#ifdef ARDUINOEEPROM
#include <EEPROM.h>
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
#ifdef ARDUINONOKIA51
#include <U8g2lib.h>
#endif

/*
 * This is the (old) ILI9488 library originally created by Jarett Burket
 * https://github.com/slviajero/ILI9488
 * It can harware scroll.
 */
#ifdef ARDUINOILI9488
#include <Adafruit_GFX.h>
#include <ILI9488.h>
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
 * experimental networking code 
 * currently the standard Ethernet shield, ESP Wifi 
 * and RP2040 Wifi is supported. All of them with 
 * the standard library.
 * In addition to this Pubsub is used
 * https://github.com/slviajero/pubsubclient
 * for MQTT
 */
#ifdef ARDUINOMQTT
#ifdef ARDUINOETH
#include <Ethernet.h>
#else
#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266WiFi.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
#include <WiFi.h>
#endif
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD)
#include <WiFiNINA.h>
#endif
#endif
#include <PubSubClient.h>
#endif

/*
 * VGA is only implemented on one platform - TTGO VGA 1.4
 * This does currently not compile with the newest ESP32 
 * Tested with 
 * https://github.com/slviajero/FabGL
 * Newer versions not yet tested
 */
#if defined(ARDUINOVGA) && defined(ARDUINO_TTGO_T7_V14_Mini32)
#include <WiFi.h> 
#include <fabgl.h> 
#endif

/*
 * real time clock support, with the very tiny 
 * uRTC library 
 * https://github.com/slviajero/uRTCLib
 */
#ifdef ARDUINORTC
#include <uRTCLib.h>
#endif

/*
 * SD filesystems with the standard SD driver, tested 
 * on AVR and ESP
 */
#ifdef ARDUINOSD
#define FILESYSTEMDRIVER
#include <SD.h>
#endif

/*
 * ESPSPIFFS tested on ESP8266 and ESP32
 * supports formating in BASIC
 */ 
#ifdef ESPSPIFFS
#define FILESYSTEMDRIVER
#ifdef ARDUINO_ARCH_ESP8266
#include <FS.h>
#endif
#ifdef ARDUINO_ARCH_ESP32
#include <FS.h>
#include <SPIFFS.h>
#endif
#endif

/*
 * RP2040 internal filesystem 
 * This is test code from https://github.com/slviajero/littlefs
 * and the main branch is actively developed
 */
#ifdef RP2040LITTLEFS
#define FILESYSTEMDRIVER
#define LFS_MBED_RP2040_VERSION_MIN_TARGET      "LittleFS_Mbed_RP2040 v1.1.0"
#define LFS_MBED_RP2040_VERSION_MIN             1001000
#define _LFS_LOGLEVEL_          1
#define RP2040_FS_SIZE_KB       1024
#define FORCE_REFORMAT          false
#include <LittleFS_Mbed_RP2040.h>
#endif

/*
 * external flash file systems override internal filesystems
 * currently BASIC can only have one filesystem
 */ 
#if defined(ARDUINOSD)
#undef ESPSPIFFS
#undef RP2040LITTLEFS
#endif

/*
 * support for external EEPROMs as filesystem
 * overriding all other filessystems. This is a minimalistic
 * filesystem meant for very small systems with not enough 
 * memory for real filesystems
 * https://github.com/slviajero/EepromFS
 */ 
#ifdef ARDUINOEFS
#undef ESPSPIFFS
#undef RP2040LITTLEFS
#undef ARDUINOSD
#define FILESYSTEMDRIVER
#include <EepromFS.h>
#endif

/*
 * incompatibilities
 * Picoserial only tested on the small boards
 */
#if ! defined(ARDUINO_AVR_UNO) && ! defined(ARDUINO_AVR_DUEMILANOVE) 
#undef USESPICOSERIAL
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

#ifdef ARDUINOPRT
const int serial1_baudrate = 9600;
short blockmode = 1;
#else 
const int serial1_baudrate = 0;
short blockmode = 0;
#endif

/*  handling time - part of the Arduino core - only needed on POSIX OSes */
void timeinit() {}

/* starting wiring is only needed on raspberry */
void wiringbegin() {}

/*
 * helper functions OS, heuristic on how much memory is 
 * available in BASIC
 * Arduino information from
 * data from https://docs.arduino.cc/learn/programming/memory-guide
 */

#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM)
extern "C" char* sbrk(int incr);
int freeRam() {
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
}
#elif defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
int freeRam() {
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0  
    ? (int)&__heap_start : (int) __brkval);  
}
#elif defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
int freeRam() {
  return ESP.getFreeHeap();
}
#else
long freeRam() {
  return 0; 
}
#endif

/*
 * Heuristic Wifi systems reserve 4k by default, small 8 bit AVR try to guess sizes conservatively
 * RP2040 cannot measure, we set to 16 bit full address space
 */
long freememorysize() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAMD)
  return freeRam() - 4000;
#endif
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_SAM)
  int overhead=192;
#ifdef ARDUINOSD
  overhead+=512;
#endif
#ifdef ARDUINOETH
  overhead+=256;
#endif
  return freeRam() - overhead;
#endif
#ifdef ARDUINO_NANO_RP2040_CONNECT
  return 65536;
#endif
  return 0;
}

/* 
 * the sleep and restart functions - only implemented for some controllers
 */
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) 
void(* callzero)() = 0;
#endif

void restartsystem() {
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
	ESP.restart();
#endif
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) 
	callzero();
#endif
}

void activatesleep(long t) {
#if defined(ARDUINO_ARCH_ESP8266)
  ESP.deepSleep(t*1000);
#endif
}

/* 
 * start the SPI bus - this is a little mean as some libraries also 
 * try to start the SPI which may lead to on override of the PIN settings
 * if the library code is not clean - currenty no conflict known
 */
void spibegin() {
#ifdef ARDUINOSPI
#ifdef ARDUINO_TTGO_T7_V14_Mini32
/* this fixes the wrong board definition in the ESP32 core for this board */
 	SPI.begin(14, 2, 12, 13);
#else 
 	SPI.begin();
#endif
#endif
}

/*
 * DISPLAY driver code section, the hardware models define a set of 
 * of functions and definitions needed for the display driver. These are 
 *
 * dsp_rows, dsp_columns: size of the display
 * dspbegin(), dspprintchar(c, col, row), dspclear()
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
#define HASKEYPAD
/* elementary keypad reader left=1, right=2, up=3, down=4, select=<lf> */
short keypadread(){
	short a=analogRead(A0);
	if (a > 850) return 0;
	else if (a>600 && a<800) return 10;
	else if (a>400 && a<600) return '1'; 
	else if (a>200 && a<400) return '3';
	else if (a>60  && a<200) return '4';
	else if (a<60)           return '2';
	return 0;
}
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
#endif


/* 
 * A Nokia 5110 with ug8lib2 - can scroll quite well
 * https://github.com/olikraus/u8g2/wiki/u8g2reference
 */ 
#ifdef ARDUINONOKIA51
#define DISPLAYDRIVER
#define NOKIA_CS 4
#define NOKIA_DC 0
#define NOKIA_RST 5
U8G2_PCD8544_84X48_F_4W_HW_SPI u8g2(U8G2_R0, NOKIA_CS, NOKIA_DC, NOKIA_RST); 
const int dsp_rows=6;
const int dsp_columns=10;
uint8_t dspfgcolor = 1;
uint8_t dspbgcolor = 0;
void dspbegin() { u8g2.begin(); u8g2.setFont(u8g2_font_amstrad_cpc_extended_8r); }
void dspprintchar(char c, short col, short row) { char b[] = { 0, 0 }; b[0]=c; u8g2.drawStr(col*8+2, (row+1)*8, b); u8g2.sendBuffer(); }
void dspclear() { u8g2.clearBuffer(); u8g2.sendBuffer(); }
void rgbcolor(int r, int g, int b) {}
void vgacolor(short c) { dspfgcolor=c%3; u8g2.setDrawColor(dspfgcolor); }
void plot(int x, int y) { u8g2.setDrawColor(dspfgcolor); u8g2.drawPixel(x, y); u8g2.sendBuffer(); }
void line(int x0, int y0, int x1, int y1)   { u8g2.drawLine(x0, y0, x1, y1); u8g2.sendBuffer(); }
void rect(int x0, int y0, int x1, int y1)   { u8g2.drawFrame(x0, y0, x1-x0, y1-y0); u8g2.sendBuffer(); }
void frect(int x0, int y0, int x1, int y1)  { u8g2.drawBox(x0, y0, x1-x0, y1-y0); u8g2.sendBuffer(); }
void circle(int x0, int y0, int r) { u8g2.drawCircle(x0, y0, r); u8g2.sendBuffer(); }
void fcircle(int x0, int y0, int r) { u8g2.drawDisc(x0, y0, r); u8g2.sendBuffer(); }
#endif

/* 
 * A ILI9488 with Jarett Burkets version of Adafruit GFX
 * currently only slow software scrolling implemented in BASIC
 * although the library can do more
 * https://github.com/jaretburkett/ILI9488
 */ 
#ifdef ARDUINOILI9488
#define DISPLAYDRIVER
#define ILI_CS  2
#define ILI_DC  3  
#define ILI_LED A0  
#define ILI_RST 4 
ILI9488 tft = ILI9488(ILI_CS, ILI_DC, ILI_RST);
const int dsp_rows=30;
const int dsp_columns=20;
char dspfontsize = 16;
uint16_t dspfgcolor = ILI9488_WHITE;
uint16_t dspbgcolor = ILI9488_BLACK;
void dspbegin() { tft.begin(); tft.setTextColor(dspfgcolor); tft.setTextSize(2); tft.fillScreen(dspbgcolor); }
void dspprintchar(char c, short col, short row) { tft.drawChar(col*dspfontsize, row*dspfontsize, c, dspfgcolor, dspbgcolor, 2); }
void dspclear() { tft.fillScreen(dspbgcolor); }
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
 * this is the VGA code for fablib - experimental
 * not all modes and possibilities explored, with networking on an ESP
 * VGA16 is advisable. It leaves enough memory for the interpreter and network.
 * this code overrides the display driver logic as fabgl brings an own 
 * terminal emulation
 */
#if defined(ARDUINOVGA) && defined(ARDUINO_TTGO_T7_V14_Mini32) 
//static fabgl::VGAController VGAController;
fabgl::VGA16Controller VGAController; // 16 color object with less memory 
static fabgl::Terminal      Terminal;
static Canvas cv(&VGAController);
TerminalController tc(&Terminal);
Color vga_graph_pen = Color::BrightWhite;
Color vga_graph_brush = Color::Black;
Color vga_txt_pen = Color::BrightGreen;
Color vga_txt_background = Color::Black;
/* this starts the vga controller and the terminal right now */
void vgabegin() {
	VGAController.begin(GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_23, GPIO_NUM_15);
	VGAController.setResolution(VGA_640x200_70Hz);
	Terminal.begin(&VGAController);
	Terminal.setBackgroundColor(vga_txt_background);
	Terminal.setForegroundColor(vga_txt_pen);
  Terminal.connectLocally();
  Terminal.clear();
  Terminal.enableCursor(true); 
  Terminal.setTerminalType(TermType::VT52);
}
void vgascale(int* x, int* y) {
	*y=*y*10/24;
}
void rgbcolor(int r, int g, int b) {
	short vga;
	if (r>191 || g>191 || b>191) vga=8; else vga=0;
	vga=vga+r/128+g/128*2+b/128*4;
	vga_graph_pen=fabgl::Color(vga);
}
void vgacolor(short c) { vga_graph_pen = fabgl::Color(c%16); }
void plot(int x, int y) { 
	vgascale(&x, &y);
	cv.setPenColor(vga_graph_pen);
	cv.setPixel(x,y);
	cv.setPenColor(vga_txt_pen);
}
void line(int x0, int y0, int x1, int y1) {
	vgascale(&x0, &y0);
	vgascale(&x1, &y1);
	cv.setPenColor(vga_graph_pen);
	cv.setPenWidth(1);
 	cv.drawLine(x0, y0, x1, y1);
 	cv.setPenColor(vga_txt_pen);
}
void rect(int x0, int y0, int x1, int y1) { 
	vgascale(&x0, &y0);
	vgascale(&x1, &y1);
	cv.setPenColor(vga_graph_pen);
	cv.setPenWidth(1);
	cv.drawRectangle(x0, y0, x1, y1);
	cv.setPenColor(vga_txt_pen);
}
void frect(int x0, int y0, int x1, int y1) {  
	vgascale(&x0, &y0);
	vgascale(&x1, &y1);
	cv.setBrushColor(vga_graph_pen);
	cv.fillRectangle(x0, y0, x1, y1);
	cv.setBrushColor(vga_txt_background);
}
void circle(int x0, int y0, int r) {  
	int rx = r;
	int ry = r;
	vgascale(&x0, &y0);
	vgascale(&rx, &ry);
	cv.setPenColor(vga_graph_pen);
	cv.setPenWidth(1);
	cv.drawEllipse(x0, y0, rx, ry);
	cv.setPenColor(vga_txt_pen);
}
void fcircle(int x0, int y0, int r) {  
	int rx = r;
	int ry = r;
	vgascale(&x0, &y0);
	vgascale(&rx, &ry);
	cv.setBrushColor(vga_graph_pen);
	cv.fillEllipse(x0, y0, rx, ry);
	cv.setBrushColor(vga_txt_background);	
}
void vgawrite(char c){
	switch(c) {
  		case 12: // form feed is clear screen
  			Terminal.write(27); Terminal.write('H');
  			Terminal.write(27); Terminal.write('J');
    		return;
  		case 10: // this is LF Unix style doing also a CR
  			Terminal.write(10); Terminal.write(13);
    		return;
  	}
  	Terminal.write(c);
}
#else 
void vgabegin(){}
void vgawrite(char c){}
#endif

/* 
 * Keyboard code for either the Fablib Terminal class or 
 * PS2Keyboard - please note that you need the ESP patched 
 * version here as mentioned above
 * 
 * sets HASKEYBOARD to inform basic about this capability
 * 
 * keyboards can implement 
 * 	kbdbegin()
 * they need to provide
 * 	kbdavailable(), kbdread(), kbdcheckch()
 * the later is for interrupting running BASIC code
 */
#ifdef ARDUINO_TTGO_T7_V14_Mini32
#define PS2FABLIB
#define HASKEYBOARD
fabgl::PS2Controller PS2Controller;
#else
#if defined(ARDUINO) && defined(ARDUINOPS2)
#define PS2KEYBOARD
#define HASKEYBOARD
PS2Keyboard keyboard;
#endif
#endif

void kbdbegin() {
#ifdef PS2KEYBOARD
	keyboard.begin(PS2DATAPIN, PS2IRQPIN, PS2Keymap_German);
#else
#ifdef PS2FABLIB
	PS2Controller.begin(PS2Preset::KeyboardPort0);
	PS2Controller.keyboard()->setLayout(&fabgl::GermanLayout);
#endif
#endif
}

char kbdavailable(){
#ifdef PS2KEYBOARD
	return keyboard.available();
#else
#ifdef PS2FABLIB
	return Terminal.available();
#endif
#endif
#ifdef HASKEYPAD
	return keypadread()!=0;
#endif	
	return 0;
}

char kbdread() {
	char c;
	while(!kbdavailable()) byield();
#ifdef PS2KEYBOARD	
	c=keyboard.read();
#endif
#ifdef PS2FABLIB
	c=Terminal.read();
#endif
#ifdef HASKEYPAD
	c=keypadread();
/* on a keypad wait for key release and then some more */
	while(kbdavailable()) byield();
  delay(100);
#endif	
	if (c == 13) c=10;
	return c;
}

char kbdcheckch() {
#ifdef PS2KEYBOARD
/*
 * only works with the patched library https://github.com/slviajero/PS2Keyboard
 * return keyboard.peek();
 * for the original library  https://github.com/PaulStoffregen/PS2Keyboard use this code 
 */
	if (kbdavailable()) return kbdread();
#else
#ifdef PS2FABLIB
	if (kbdavailable()) return kbdread();
#endif
#endif
#ifdef HASKEYPAD
		return keypadread();
#endif	
	return 0;
}

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
 */

#ifdef DISPLAYDRIVER
short dspmycol = 0;
short dspmyrow = 0;
char esc = 0;

void dspsetcursor(short c, short r) {
	dspmyrow=r;
	dspmycol=c;
}

char dspactive() {
	return od & ODSP;
}

/* 
 * This is the minimalistic VT52 state engine. It is an interface to 
 * process single byte control sequences of the form <ESC> char 
 *
 * It is planned to implement an interface to the wiring, graphics,
 * and print library here.
 */

#ifdef HASVT52
/* the state variables, and modes */
char vt52s = 0;
enum vt52modes {vt52text, vt52graph, vt52print, vt52wiring} vt52mode = vt52text;

/* vt52 state engine */
void dspvt52(char* c){

/* reading and processing multi byte commands */
	switch (vt52s) {
		case 'Y':
			if (esc == 2) { dspmyrow=(*c-31)%dsp_rows; esc=1; return;}
			if (esc == 1) { dspmycol=(*c-31)%dsp_columns; *c=0; }
      vt52s=0; 
			break;
		case 'x':
			switch(*c) {
				case 'p':
					vt52mode=vt52print;
					break;
				case 'w':
					vt52mode=vt52wiring;
					break;
				case 'g':
					vt52mode=vt52graph;
				case 't':
				default:
					vt52mode=vt52text;
					break;
			}
			*c=0;
			vt52s=0;
			break;
	}
 
/* commands of the terminal in text mode */
	switch (*c) {
		case 'A': // cursor up
			if (dspmyrow>0) dspmyrow--;
			break;
		case 'B': // cursor down
			dspmyrow=(dspmyrow++) % dsp_rows;
			break;
		case 'C': // cursor right
			dspmycol=(dspmycol++) % dsp_columns;
			break; 
		case 'D': // cursor left
			if (dspmycol>0) dspmycol--;
			break;
		case 'E': // GEMDOS / TOS extension clear screen
			dspbufferclear();
			dspclear();
			break;
		case 'H': // cursor home
			dspmyrow=0;
   		dspmycol=0;
			break;	
		case 'Y': // Set cursor position
			vt52s='Y';
			esc=2;
  		*c=0;
			return;

/* these standard VT52 function and their GEMDOS extensions are
		not implemented. */ 
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
		case ']': // Printer extension - print screen 
			break;
/* the Arduino interface extensions defined in IoT BASIC
		access to some functions of BASIC through escape sequences */
		case 'x':
			vt52s='x';
			*c=0; 
			break;
	}
	esc=0;
	*c=0;
}
#endif


/* scrollable displays need a character buffer */
#ifdef DISPLAYCANSCROLL
char dspbuffer[dsp_rows][dsp_columns];
char  dspscrollmode = 0;
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
	if (esc) dspvt52(&c);
#endif

/* the minimal cursor control functions of BASIC */
  switch(c) {
  	case 10: // this is LF Unix style doing also a CR
    	dspmyrow=(dspmyrow + 1);
    	if (dspmyrow >= dsp_rows) dspscroll(); 
    	dspmycol=0;
    	return;
    case 12: // form feed is clear screen 
    	dspbufferclear();
    	dspclear();
    	return;
    case 13: // classical carriage return 
    	dspmycol=0;
    	return;
  	case 27: // escape - initiate vtxxx mode
			esc=1;
			return;
    case 127: // delete
    	if (dspmycol > 0) {
      	dspmycol--;
      	dspbuffer[dspmyrow][dspmycol]=0;
      	dspprintchar(' ', dspmycol, dspmyrow);
      	return;
    	}
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
}

char dspwaitonscroll() {
  char c;

	if ( dspscrollmode == 1 ) {
		if (dspmyrow == dsp_rows-1) {
      c=inch();
      if (c == ' ') outch(12);
		  return c;
		}
	}
	return 0;
}

/* code for low memory simple display access */
#else 

void dspbufferclear() {}
char dspwaitonscroll() { return 0; }
void dspwrite(char c){

/* on escape call the vt52 state engine */
#ifdef HASVT52
	if (esc) { dspvt52(&c); }
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
    	return;
    case 11: // one char down 
    	dspmyrow=(dspmyrow+1) % dsp_rows;
    	return;
    case 13: // classical carriage return 
    	dspmycol=0;
    	return;
		case 27: // escape - initiate vtxxx mode
			esc=1;
			return;
   	case 127: // delete
    	if (dspmycol > 0) {
      	dspmycol--;
      	dspprintchar(' ', dspmycol, dspmyrow);
    	}
    	return;
  }

/* all other non printables ignored */ 
	if (c < 32 ) return; 

	dspprintchar(c, dspmycol++, dspmyrow);
	dspmycol=dspmycol%dsp_columns;
}

void dspsetscrollmode(char c, short l) {}
#endif
#else
const int dsp_rows=0;
const int dsp_columns=0;
void dspwrite(char c){};
void dspbegin() {};
char dspwaitonscroll() { return 0; };
char dspactive() {return FALSE; }
void dspsetscrollmode(char c, short l) {}
void dspsetcursor(short c, short r) {}
#endif

/* 
 * Arduino Real Time clock code based on uRTC 
 * this is a minimalistic library. The interface here 
 * offers the values as number_t plus a string, 
 * combining all values. 
 */
#ifdef ARDUINORTC
uRTCLib rtc(RTCI2CADDR);
char rtcstring[18] = { 0 }; 

char* rtcmkstr() {
	int cc = 1;
	short t;
	char ch;
	t=rtcget(2);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]=':';
	t=rtcread(1);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]=':';
	t=rtcread(0);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='-';
	t=rtcread(3);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='/';
	t=rtcread(4);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='/';
	t=rtcread(5);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc]=0;
	rtcstring[0]=cc-1;

	return rtcstring;
}

short rtcread(char i) {
	switch (i) {
		case 0: 
			return rtc.second();
		case 1:
			return rtc.minute();
		case 2:
			return rtc.hour();
		case 3:
			return rtc.day();
		case 4:
			return rtc.month();
		case 5:
			return rtc.year();
		case 6:
			return rtc.dayOfWeek();
		case 7:
			return rtc.temp();
		default:
			return 0;
	}
}

short rtcget(char i) {
	rtc.refresh();
	return rtcread(i);
}

void rtcset(char i, short v) {
	uint8_t tv[7];
	char j;
	rtc.refresh();
	for (j=0; j<7; j++) tv[j]=rtcread(j);
	tv[i]=v;
	rtc.set(tv[0], tv[1], tv[2], tv[6], tv[3], tv[4], tv[5]);
}
#endif

/* 
 * External EEPROM is handled through an EFS filesystem object  
 * see https://github.com/slviajero/EepromFS 
 * for details. Here the most common parameters are set as a default.
*/
#ifdef ARDUINOEFS
#ifndef EFSEEPROMSIZE
#define EFSEEPROMSIZE 4096
#endif
#ifndef EEPROMI2CADDR
#define EEPROMI2CADDR 0x50
#endif
EepromFS EFS(EEPROMI2CADDR, EFSEEPROMSIZE);
#endif


/*
 * definitions for ESP Wifi and MQTT, super experimental.
 * As networking is only used for MQTT at the moment, 
 * mqtt, Wifi and Ethernet comes all in one. 
 *
 * No encryption/authetication is implemented in MQTT. 
 * Only public, open servers can be used.
 *
 * MQTT topics can only be 32 bytes long.
 * Buffered incoming and outgoing messages can be 128 bytes
 * per default.
 *
 * wifisettings.h is the generic network definition file
 * all network settings are compiled into the code 
 * BASIC cannot change them at runtime.
 */
#ifdef ARDUINOMQTT
#include "wifisettings.h"
#ifdef ARDUINOETH
EthernetClient bethclient;
PubSubClient bmqtt(bethclient);
#else
WiFiClient bwifi;
PubSubClient bmqtt(bwifi);
#endif

/* the buffers of the outgoing and incoming MQTT topic */ 
#define MQTTLENGTH 32
static char mqtt_otopic[MQTTLENGTH];
static char mqtt_itopic[MQTTLENGTH];

/* 
 * the buffers for MQTT messages, input and output goes 
 * through these static buffers. 
 */
#define MQTTBLENGTH 128
volatile char mqtt_buffer[MQTTBLENGTH];
volatile short mqtt_messagelength;
volatile char mqtt_obuffer[MQTTBLENGTH];
volatile short mqtt_charsforsend;

/* the name of the client, generated pseudo randomly to avoind 
		naming conflicts */
static char mqttname[12] = "iotbasicxxx";
void mqttsetname() {
	long m = millis();
	mqttname[8]=(char)(65+m%26);
	m=m/26;
	mqttname[9]=(char)(65+m%26);
	m=m/26;
	mqttname[10]=(char)(65+m%26);
}

/* 
 *	network begin method 
 *	needs the settings from wifisettings.h
 * 
 * Ethernet begin has to be reviewed to avoid DHCP 
 * start timeout if network is not connected
 */
void netbegin() {
#ifdef ARDUINOETH
#ifdef ETHPIN
Ethernet.init(ETHPIN);
#endif
Ethernet.begin(mac);
#else  
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
	WiFi.mode(WIFI_STA);
	WiFi.begin(ssid, password);
	WiFi.setAutoReconnect(true);
#endif
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD)
	WiFi.begin(ssid, password);
#endif
#endif
}

/*
 * network connected method
 * on ESP Wifi try to reconnect, the delay is odd 
 * This is a partial reconnect, BASIC needs to handle 
 * repeated reconnects
 */
char netconnected() {
#ifdef ARDUINOETH
  return bethclient.connected();
#else
	if (WiFi.status() != WL_CONNECTED) {  
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
	  WiFi.reconnect(); 
	  delay(1000); 
#elif defined(ARDUINO_ARCH_SAMD)
    WiFi.begin(ssid, password);
    delay(1000);
#endif 	
	};
	return(WiFi.status() == WL_CONNECTED);
#endif
}

/*
 * mqtt callback function, using the byte type here
 * because payload can be binary - interface to BASIC 
 * strings need to be done
 *
 * mqtt event handling in BASIC can be triggered here
 */
void mqttcallback(char* t, byte* p, unsigned int l) {
	short i;
	mqtt_messagelength=l;
	for(i=0; i<l; i++) mqtt_buffer[i]=(char)p[i];
}

/*
 * starting mqtt 
 * set the server, register the callback and purge all topics
 */
void mqttbegin() {
	bmqtt.setServer(mqtt_server, mqtt_port);
	bmqtt.setCallback(mqttcallback);
	*mqtt_itopic=0;
	*mqtt_otopic=0;
	mqtt_charsforsend=0;
}

/*
 * reconnecting mqtt - exponential backoff here 
 * exponental backoff reconnect in 10 ms * 2^n intervals
 * no randomization 
 */
char mqttreconnect() {
	short timer=10;
	char reconnect=0;

/* all good and nothing to be done, we are connected */
	if (bmqtt.connected()) return true;

/* try to reconnect the network */
  if (!netconnected()) delay(5000);
  if (!netconnected()) return false;
	
/* create a new random name right now */
	mqttsetname();

/* try to reconnect assuming that the network is connected */
	while (!bmqtt.connected() && timer < 400) {
		bmqtt.connect(mqttname);
		delay(timer);
		timer=timer*2;
    reconnect=1;
	}

/* after reconnect resubscribe if there is a valid topic */
	if (*mqtt_itopic && bmqtt.connected() && reconnect) bmqtt.subscribe(mqtt_itopic);
 
	return bmqtt.connected();
}

/* mqtt state information */
int mqttstate() {
	return bmqtt.state();
}

/* subscribing to a topic  */
void mqttsubscribe(char *t) {
	short i;

	for (i=0; i<MQTTLENGTH; i++) {
		if ((mqtt_itopic[i]=t[i]) == 0 ) break;
	}
	if (!mqttreconnect()) {ert=1; return;};
	if (!bmqtt.subscribe(mqtt_itopic)) ert=1;
}

void mqttunsubscribe() {
	if (!mqttreconnect()) {ert=1; return;};
	if (!bmqtt.unsubscribe(mqtt_itopic)) ert=1;
	*mqtt_itopic=0;
}

/*
 * set the topic we pushlish, coming from OPEN
 * BASIC can do only one topic.
 * 
 */
void mqttsettopic(char *t) {
	int i;

	for (i=0; i<MQTTLENGTH; i++) {
		if ((mqtt_otopic[i]=t[i]) == 0 ) break;
	}
}

/* 
 *	print a mqtt message 
 *  we buffer until we reach either cr or until the buffer is full
 *	this is needed to do things like PRINT A,B,C as one message
 */
void mqttouts(char *m, short l) {
	int i=0;

	while(mqtt_charsforsend < MQTTBLENGTH && i<l) mqtt_obuffer[mqtt_charsforsend++]=m[i++];
	if (mqtt_obuffer[mqtt_charsforsend-1] == '\n' || mqtt_charsforsend > MQTTBLENGTH) {
		if (!mqttreconnect()) {ert=1; return;};
		if (!bmqtt.publish(mqtt_otopic, (uint8_t*) mqtt_obuffer, (unsigned int) mqtt_charsforsend-1, false)) ert=1;
		mqtt_charsforsend=0;
	}
} 

/* ins copies the buffer into a basic string 
	- behold the jabberwock - length gynmastics */
void mqttins(char *b, short nb) {
	for (z.a=0; z.a<nb && z.a<mqtt_messagelength; z.a++) b[z.a+1]=mqtt_buffer[z.a];
 	b[0]=z.a;
 	mqtt_messagelength=0;
 	*mqtt_buffer=0;
}

/* just one character to emulate basic get */
char mqttinch() {
	char ch=0;
	short i;

	if (mqtt_messagelength>0) {
		ch=mqtt_buffer[0];
		for (i=0; i<mqtt_messagelength-1; i++) mqtt_buffer[i]=mqtt_buffer[i+1];
	}
	mqtt_messagelength--;
	return ch;
}

#else 
void netbegin() {}
char netconnected() { return 0; }
void mqttbegin() {}
int  mqttstate() {return 0;}
void mqttsubscribe(char *t) {}
void mqttsettopic(char *t) {}
void mqttouts(char *m, short l) {}
void mqttins(char *b, short nb) { z.a=0; };
char mqttinch() {return 0;};
#endif

/* 
 *	EEPROM handling, these function enable the @E array and 
 *	loading and saving to EEPROM with the "!" mechanism
 */ 
void ebegin(){
#if ( defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) ) && defined(ARDUINOEEPROM)
  EEPROM.begin(EEPROMSIZE);
#endif
}

void eflush(){
#if ( defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) ) && defined(ARDUINOEEPROM) 
  EEPROM.commit();
#endif 
}


#if defined(ARDUINOEEPROM)
address_t elength() { 
#if defined(ARDUINO_ARCH_ESP8266) ||defined(ARDUINO_ARCH_ESP32)
  return EEPROMSIZE;
#endif
#ifdef ARDUINO_ARCH_AVR
#ifndef ARDUINO_AVR_LARDU_328E
  return EEPROM.length(); 
#else 
  return 512;
#endif
#endif
  return 0;
}

void eupdate(address_t a, short c) { 
#if defined(ARDUINO_ARCH_ESP8266) ||defined(ARDUINO_ARCH_ESP32)|| defined(ARDUINO_AVR_LARDU_328E)
  EEPROM.write(a, c);
#else
  EEPROM.update(a, c); 
#endif
}

short eread(address_t a) { return (signed char) EEPROM.read(a); }
#endif

/* no EEPROM present */
#if ! defined(ARDUINOEEPROM)
address_t elength() { return 0; }
void eupdate(address_t a, short c) { return; }
short eread(address_t a) { return 0; }
#endif

/* 
 *	the wrappers of the arduino io functions, to avoid 
 */	
/* not needed in ESP32 2.0.2 core any more 
#ifdef ARDUINO_ARCH_ESP32
void analogWrite(int a, int b){}
#endif
*/

void aread(){ push(analogRead(pop())); }

void dread(){ push(digitalRead(pop())); }

void awrite(number_t p, number_t v){
	if (v >= 0 && v<256) analogWrite(p, v);
	else error(EORANGE);
}

void dwrite(number_t p, number_t v){
	if (v == 0) digitalWrite(p, LOW);
	else if (v == 1) digitalWrite(p, HIGH);
	else error(EORANGE);
}

void pinm(number_t p, number_t m){
	if (m>=0 && m<=2) pinMode(p, m);
	else error(EORANGE); 
}

void bmillis() {
	number_t m;
/* millis is processed as integer and is cyclic mod maxnumber and not cast to float!! */
	m=(number_t) (millis()/(unsigned long)pop() % (unsigned long)maxnum);
	push(m); 
};

void bpulsein() { 
  unsigned long t, pt;
  t=((unsigned long) pop())*1000;
  y=pop(); 
  x=pop(); 
  pt=pulseIn(x, y, t)/10; 
  push(pt);
}

void btone(short a) {
	x=pop();
	y=pop();
#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_ESP32)
	if (a == 3) pop();
	return;
#else 
	if (a == 2) {
		tone(y,x);
	} else {
		tone(pop(), y, x);
	}
#endif	
}

/* 
 *	the byield function is called after every statement
 *	it allows two levels of background tasks. 
 *
 *	ARDUINOBGTASK controls if time for background tasks is 
 * 	needed, usually set by hardware features
 *
 * 	YIELDINTERVAL by default is 32, generating a 32 ms call
 *		to the network loop function. YIELDTIME is 2 generating
 *		a 2 ms wait after the network loop to allow for buffer 
 * 		clearing after - this is needed on ESP8266
 *
 * 	LONGYIELDINTERVAL by default is 1000, generating a one second
 *		call to maintenance functions.
 *
 *	On an ESP8266 byield() is called every 100 microseconds
 *	(after each statement) in RUN mode. BASIC DELAY calls 
 * 	this every YIELDTIME ms. 
 */
void byield() {	
#if defined(ARDUINOBGTASK)
	if (millis()-lastyield > YIELDINTERVAL-1) {
		yieldfunction();
		lastyield=millis();
		delay(YIELDTIME);
  }
  if (millis()-lastlongyield > LONGYIELDINTERVAL-1) {
  	longyieldfunction();
  	lastlongyield=millis();
  }
 #endif
  delay(0);
}

/* everything that needs to be done often - 32 ms */
void yieldfunction() {
#ifdef ARDUINOMQTT
	bmqtt.loop();
#endif
}

/* everything that needs to be done not so often - 1 second */
void longyieldfunction() {
#ifdef ARDUINOETH
  	Ethernet.maintain();
#endif 
}

/* 
 *	The file system driver - all methods needed to support BASIC fs access
 * 	Different filesystems need different prefixes and fs objects, these 
 * 	filesystems use the stream API
 */
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
File ifile;
File ofile;
char tempname[SBUFSIZE];
#ifdef ARDUINOSD
File root;
File file;
#ifdef ARDUINO_ARCH_ESP32
const char rootfsprefix[2] = "/";
#else
const char rootfsprefix[1] = "";
#endif
#endif
#ifdef ESPSPIFFS
const char rootfsprefix[2] = "/";
#ifdef ARDUINO_ARCH_ESP8266
Dir root;
File file;
#endif
#ifdef ARDUINO_ARCH_ESP32
File root;
File file;
#endif
#endif
#ifdef ARDUINO_ARCH_ESP8266
#define FILE_OWRITE (sdfat::O_READ | sdfat::O_WRITE | sdfat::O_CREAT | sdfat::O_TRUNC)
#else 
#ifdef ARDUINO_ARCH_ESP32
#define FILE_OWRITE FILE_WRITE
#else 
#define FILE_OWRITE (O_READ | O_WRITE | O_CREAT | O_TRUNC)
#endif
#endif
#endif

/* using the POSIX API in LittleFS */
#ifdef RP2040LITTLEFS
#define FILESYSTEMDRIVER
FILE* ifile;
FILE* ofile;
DIR* root;
struct dirent* file;
LittleFS_MBED *myFS;
const char rootfsprefix[10] = MBED_LITTLEFS_FILE_PREFIX;
#endif

/* use EEPROM as filesystem */
#ifdef ARDUINOEFS
byte ifile;
byte ofile;
byte file;
#endif

/* these filesystems have a path prefix */
#if defined(RP2040LITTLEFS) || defined(ESPSPIFFS) || defined(ARDUINOSD) 
char tmpfilename[10+SBUFSIZE];

/* add the prefix to the filename */
char* mkfilename(const char* filename) {
	short i,j;
	for(i=0; i<10 && rootfsprefix[i]!=0; i++) tmpfilename[i]=rootfsprefix[i];
	tmpfilename[i++]='/';
	for(j=0; j<SBUFSIZE && filename[j]!=0; j++) tmpfilename[i++]=filename[j];
	tmpfilename[i]=0;
	return tmpfilename;
}

/* remove the prefix from the filename */
const char* rmrootfsprefix(const char* filename) {
	short i=0;
	while (filename[i] == rootfsprefix[i] && rootfsprefix[i] !=0 && filename[i] !=0 ) i++;
	if (filename[i]=='/') i++;
	return &filename[i];
}
#endif

/* 
 *	filesystem starter for SPIFFS and SD on ESP, ESP32 and Arduino plus LittleFS
 *	the verbose option needs to be checked 
 */
void fsbegin(char v) {
#ifdef ARDUINOSD 
#ifndef SDPIN
#define SDPIN
#endif
 	if (SD.begin(SDPIN) && v) { outsc("SDcard ok \n"); }	
#endif
#if defined(ESPSPIFFS) && defined(ARDUINO_ARCH_ESP8266) 
 	if (SPIFFS.begin() && v) {
		outsc("SPIFFS ok \n");
		FSInfo fs_info;
		SPIFFS.info(fs_info);
		outsc("File system size "); outnumber(fs_info.totalBytes); outcr();
		outsc("File system used "); outnumber(fs_info.usedBytes); outcr();
 	}
#endif
#if defined(ESPSPIFFS) && defined(ARDUINO_ARCH_ESP32) 
 	if (SPIFFS.begin() && v) {
		outsc("SPIFFS ok \n"); outcr();
 	}
#endif
#ifdef RP2040LITTLEFS
	myFS = new LittleFS_MBED();
	if (myFS->init() && v) outsc("LittleFS ok \n");
#endif
#ifdef ARDUINOEFS
	int s=EFS.begin();
	if (s>0 && v) {
		outsc("Mounted EFS with "); outnumber(s); outsc(" slots.\n"); 
	} else {
		if (EFS.format(32) && v) outsc("EFS: formating 32 slots.\n");
	}
#endif
}

/*
 *	File I/O function on an Arduino
 * 
 * filewrite(), fileread(), fileavailable() as byte access
 * open and close is handled separately by (i/o)file(open/close)
 * only one file can be open for write and read at the same time
 */
void filewrite(char c) {
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	if (ofile) ofile.write(c); else ert=1;
#endif
#if defined(RP2040LITTLEFS)
	if (ofile) fputc(c, ofile); else ert=1;
#endif
#if defined(ARDUINOEFS)
	if (ofile) EFS.fputc(c, ofile); else ert=1;
#endif
}

char fileread(){
	char c;
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	if (ifile) c=ifile.read(); else { ert=1; return 0; }
	if (c == -1 ) ert=-1;
	return c;
#endif
#ifdef RP2040LITTLEFS
	if (ifile) c=fgetc(ifile); else { ert=1; return 0; }
	if (c == -1 ) ert=-1;
	return c;
#endif
#ifdef ARDUINOEFS
	if (ifile) c=EFS.fgetc(ifile); else { ert=1; return 0; }
	if (c == -1 ) ert=-1;
	return c;
#endif
	return 0;
}

int fileavailable(){
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	return ifile.available();
#endif
#ifdef RP2040LITTLEFS
	return !feof(ifile);
#endif
#ifdef ARDUINOEFS
	return EFS.available(ifile);
#endif
	return 0;
}

char ifileopen(const char* filename){
#ifdef ARDUINOSD
	ifile=SD.open(mkfilename(filename), FILE_READ);
	return (int) ifile;
#endif
#ifdef ESPSPIFFS
	ifile=SPIFFS.open(mkfilename(filename), "r");
	return (int) ifile;
#endif
#ifdef RP2040LITTLEFS
	ifile=fopen(mkfilename(filename), "r");
	return (int) ifile;
#endif
#ifdef ARDUINOEFS
	ifile=EFS.fopen(filename, "r");
	return (int) ifile;
#endif
	return 0;
}

void ifileclose(){
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	ifile.close();
#endif	
#ifdef RP2040LITTLEFS
	if (ifile) fclose(ifile);
	ifile=NULL;	
#endif
#ifdef ARDUINOEFS
	if (ifile) EFS.fclose(ifile);
	ifile=0;	
#endif
}

char ofileopen(char* filename, const char* m){
#ifdef ARDUINOSD
	if (*m == 'w') ofile=SD.open(mkfilename(filename), FILE_OWRITE);
	if (*m == 'a') ofile=SD.open(mkfilename(filename), FILE_WRITE);
	return (int) ofile;
#endif
#ifdef ESPSPIFFS
	ofile=SPIFFS.open(mkfilename(filename), m);
	return (int) ofile;
#endif
#ifdef RP2040LITTLEFS
	ofile=fopen(mkfilename(filename), m);
	return (int) ofile; 
#endif
#ifdef ARDUINOEFS
	ofile=EFS.fopen(filename, m);
	return (int) ofile; 
#endif
	return 0;
}

void ofileclose(){
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
    ofile.close();
#endif
#ifdef RP2040LITTLEFS
	if (ofile) fclose(ofile); 
#endif
#ifdef ARDUINOEFS
	if (ofile) EFS.fclose(ofile); 
#endif	
}

/*
 * directory handling for the catalog function
 * these methods are needed for a walkthtrough of 
 * one directory
 *
 * rootopen()
 * while rootnextfile()
 * 	if rootisfile() print rootfilename() rootfilesize()
 *	rootfileclose()
 * rootclose()
 */
void rootopen() {
#ifdef ARDUINOSD
	root=SD.open("/");
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
	root=SPIFFS.openDir("/");
#endif
#ifdef ARDUINO_ARCH_ESP32
	root=SPIFFS.open("/");
#endif
#endif
#ifdef RP2040LITTLEFS
	root=opendir(rootfsprefix);
#endif
#ifdef ARDUINOEFS
	EFS.dirp=0;
#endif	
}

int rootnextfile() {
#ifdef ARDUINOSD
	file=root.openNextFile();
	return (file != 0);
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
  	if (root.next()) {
    	file=root.openFile("r");
    	return 1;
  	} else {
    	return 0;
	}
#endif
#ifdef ARDUINO_ARCH_ESP32
	file=root.openNextFile();
	return (file != 0);
#endif
#endif
#ifdef RP2040LITTLEFS
	file = readdir(root);
	return (file != 0);
#endif
#ifdef ARDUINOEFS
	file = EFS.readdir();
	return (file != 0);
#endif
  return FALSE;
}

int rootisfile() {
#ifdef ARDUINOSD
	return (! file.isDirectory());
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
	return TRUE;
#endif
#ifdef ARDUINO_ARCH_ESP32
	return (! file.isDirectory());
#endif
#endif
#ifdef RP2040LITTLEFS
	return (file->d_type == DT_REG);
#endif
#ifdef ARDUINOEFS
	return true;
#endif	
	return FALSE;
}

const char* rootfilename() {
#ifdef ARDUINOSD
	//return (char*) file.name();
 return rmrootfsprefix(file.name());
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
	// c_str() and copy - real ugly
  int i=0;
	String s=root.fileName();
 	for (i=0; i<s.length(); i++) { tempname[i]=s[i]; }
 	tempname[i]=0;
	return rmrootfsprefix(tempname);
#endif
#ifdef ARDUINO_ARCH_ESP32
	return rmrootfsprefix(file.name());
#endif
#endif
#ifdef RP2040LITTLEFS
	return (file->d_name);
#endif
#ifdef ARDUINOEFS
	return EFS.filename(file);
#endif
	return FALSE; 
}

int rootfilesize() {
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
  return file.size();
#endif  
#ifdef RP2040LITTLEFS
#endif
#ifdef ARDUINOEFS
	return EFS.filesize(file);
#endif
  return 0;
}

void rootfileclose() {
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
  file.close();
#endif 
#ifdef RP2040LITTLEFS
#endif
#ifdef ARDUINOEFS
#endif
}

void rootclose(){
#ifdef ARDUINOSD
  root.close();
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
  return;
#endif
#ifdef ARDUINO_ARCH_ESP32
  root.close();
#endif
#endif
#ifdef RP2040LITTLEFS
#endif
#ifdef ARDUINOEFS
#endif
}

/*
 * remove method for files
 */
void removefile(char *filename) {
#ifdef ARDUINOSD	
	SD.remove(filename);
	return;
#endif
#ifdef ESPSPIFFS
	SPIFFS.remove(mkfilename(filename));
	return;
#endif
#ifdef RP2040LITTLEFS
	remove(mkfilename(filename));
	return;
#endif
#ifdef ARDUINOEFS
	EFS.remove(filename);
	return;
#endif
}

/*
 * formatting for fdisk of the internal filesystems
 */
void formatdisk(short i) {
#ifdef ARDUINOSD	
	return;
#endif
#ifdef ESPSPIFFS
	if (SPIFFS.format()) { SPIFFS.begin(); outsc("ok\n"); } else { outsc("fail\n"); }
#endif
#ifdef RP2040LITTLEFS
  fs.reformat(&bd);
	return;
#endif
#ifdef ARDUINOEFS
	if (i>0 && i<256) {
		if (EFS.format(i)) { EFS.begin(); outsc("ok\n"); } else { outsc("fail\n"); }
		outcr();
	} else error(EORANGE);
	return;
#endif
}

/*
 *	Primary serial code uses the Serial object or Picoserial
 *
 *	Picoserial has an own input buffer and an 
 *	interrupt function. This is used to fill the input buffer 
 *	directly on read. Write is standard like in the Serial code.
 */
#ifdef USESPICOSERIAL
volatile static char picochar;
volatile static char picoa = FALSE;
volatile static char* picob = NULL;
static short picobsize = 0;
volatile static short picoi = 1;

/* getchar */
void picogetchar(int c){
	if (picob && (! picoa) ) {
    	picochar=c;
		if (picochar != '\n' && picochar != '\r' && picoi<picobsize-1) {
			picob[picoi++]=picochar;
			outch(picochar);
		} else {
			picoa = TRUE;
			picob[picoi]=0;
			picob[0]=picoi;
			z.a=picoi;
			picoi=1;
		}
		picochar=0; /* every buffered byte is deleted */
	} else {
    	if (c != 10) picochar=c;
	}
}
#endif

/* 
 * blocking serial single char read for Serial
 * unblocking for Picoserial because it is not 
 * character oriented -> consins.
 */
char serialread() {
#ifdef USESPICOSERIAL
	char c;
	c=picochar;
	picochar=0;
	return c;	
#else
	while (!Serial.available()) byield();
	return Serial.read();
#endif
}

/*
 *  serial begin code with a one second delay after start
 *	this is not needed any more 
 */
void serialbegin() {
#ifdef USESPICOSERIAL
	(void) PicoSerial.begin(serial_baudrate, picogetchar); 
#else
	Serial.begin(serial_baudrate);
  //while(!Serial) byield();
#endif
	delay(1000);
}

/* write to a serial stream */
void serialwrite(char c) {
#ifdef USESPICOSERIAL
	PicoSerial.print(c);
#else
/* write never blocks. discard any bytes we can't get rid of */
  Serial.write(c);  
  // if (Serial.availableForWrite()>0) Serial.write(c);	
#endif
}

/* check on a character, needed for breaking */
short serialcheckch() {
#ifdef USESPICOSERIAL
	return picochar;
#else
	if (Serial.available()) return Serial.peek(); else return 0;
#endif	
}

/* avail method, needed for AVAIL() */ 
short serialavailable() {
#ifdef USESPICOSERIAL
	return picoi;
#else
	return Serial.available();
#endif	
}

/*
 * reading from the console with inch or the picoserial callback
 * this mixes interpreter levels as inch/outch are used here 
 * this code needs to go to the main interpreter section after 
 * thorough rewrite
 */
void consins(char *b, short nb) {
	char c;
 
	z.a = 1;
#ifdef USESPICOSERIAL
	if (id == ISERIAL) {
		picob=b;
		picobsize=nb;
		picoa=FALSE;
		while (! picoa);
		//outsc(b+1); 
		outcr();
		return;
	}
#endif
	while(z.a < nb) {
  		c=inch();
  		if (id == ISERIAL || id == IKEYBOARD) outch(c);
  		if (c == '\r') c=inch(); 			/* skip carriage return */
  		if (c == '\n' || c == -1) { 	/* terminal character is either newline or EOF */
    		break;
  		} else if ( (c == 127 || c == 8) && z.a>1) {
   			z.a--;
  		} else {
   			b[z.a++]=c;
  		} 
	}
	b[z.a]=0;
	z.a--;
  b[0]=(unsigned char)z.a; 
}

/* 
 * Start a secondary serial port for printing and/or networking 
 * This is either Serial1 on a MEGA or DUE or Nano Every or a SoftwareSerial 
 * instance
 */
#ifdef ARDUINOPRT
#if !defined(ARDUINO_AVR_MEGA2560) && !defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_AVR_NANO_EVERY)
#include <SoftwareSerial.h>
/* definition of the serial port pins from "pretzel board"
for UNO 11 is not good for rx */
const int software_serial_rx = SOFTSERIALRX;
const int software_serial_tx = SOFTSERIALTX;
SoftwareSerial Serial1(software_serial_rx, software_serial_tx);
#endif

// second serial port
void prtbegin() {
	Serial1.begin(serial1_baudrate);
}

void prtwrite(char c) {
	Serial1.write(c);
}

char prtread() {
	while (!Serial1.available()) byield();
	return Serial1.read();
}

short prtcheckch() {
	if (Serial1.available()) return Serial1.peek(); else return 0;
}

short prtavailable() {
	return Serial1.available();
}
#endif


/* 
 * The wire code, direct access to wire communication
 */ 
#ifdef ARDUINOWIRE
uint8_t wire_slaveid = 0;
uint8_t wire_myid = 0;
#endif

/* default begin is as a master, no PIN definitions yet */
void wirebegin() {
#ifdef ARDUINOWIRE
	Wire.begin();
#endif
}

/*
 *	as a master open sets the slave id for the communication
 *	no extra begin while we stay master
 */
void wireopen(char* s) {
#ifdef ARDUINOWIRE
	if (s[0] == 'm') {
		wire_slaveid=s[1];
		wire_myid=0;
	} else if ( s[0] == 's' ) { 
		wire_myid=s[1];
		wire_slaveid=0;
		Wire.begin(wire_myid);
		// here the slave code if this Arduino is a slave
		// to be done
	} else 
		error(EORANGE);
#endif
}

/* input an entire string */
void wireins(char *b, uint8_t l) {
#ifdef ARDUINOWIRE
	z.a=0;
	Wire.requestFrom(wire_slaveid, l);
	while (Wire.available() && z.a<l) b[++z.a]=Wire.read();
	b[0]=z.a;
#else 
	b[0]=0;
	z.a=0;
#endif
}

/* send an entire string */
void wireouts(char *b, uint8_t l) {
#ifdef ARDUINOWIRE
	  Wire.beginTransmission(wire_slaveid); 
	  for(z.a=0; z.a<l; z.a++) Wire.write(b[z.a]);     
	  ert=Wire.endTransmission(); 
#endif
}

/* 
 *	Read from the radio interface, radio is always block 
 *	oriented. This function is called from ins for an entire 
 *	line.
 *
 *	In blockmode the entire message is returned in the 
 *	receiving string while in line mode the length of the 
 *	string is adapted. Blockmode can be used to transfer
 *	binary data.
 */

#ifdef ARDUINORF24
/*
 * definitions for the nearfield module, still very experimental
 * We use the standard RF24 nearfield library 
 */
const char rf24_ce = RF24CEPIN; 
const char rf24_csn = RF24CSNPIN; 
#include <nRF24L01.h>
#include <RF24.h>
rf24_pa_dbm_e rf24_pa = RF24_PA_MAX;
RF24 radio(rf24_ce, rf24_csn);

/* generate a uint64_t pipe address from the filename string for RF64 */
uint64_t pipeaddr(char * f){
	uint64_t t = 0;
	t=(uint8_t)f[0];
	for(short i=1; i<=4; i++) t=t*256+(uint8_t)f[i];
	return t;
} 
#endif

/* read an entire string */
void radioins(char *b, short nb) {
#ifdef ARDUINORF24
    if (radio.available()) {
    	radio.read(b+1, nb);
    	if (!blockmode) {
        	for (z.a=0; z.a<nb; z.a++) if (b[z.a+1]==0) break;		
    	} else {
    		z.a=radio.getPayloadSize();
      		if (z.a > nb) z.a=nb;
    	}
      	b[0]=z.a;
	} else {
      	b[0]=0; 
      	b[1]=0;
      	z.a=0;
	}
#else 
      b[0]=0; 
      b[1]=0;
      z.a=0;
#endif
}

/* write to radio, no character mode here */
void radioouts(char *b, short l) {
#ifdef ARDUINORF24
	radio.stopListening();
	if (radio.write(b, l)) ert=0; else ert=1;
	radio.startListening();
#endif
}

/* 
 *	we always read from pipe 1 and use pipe 0 for writing, 
 *	the filename is the pipe address, by default the radio 
 *	goes to reading mode after open and is only stopped for 
 *	write
 */
void iradioopen(char *filename) {
#ifdef ARDUINORF24
	radio.begin();
	radio.openReadingPipe(1, pipeaddr(filename));
	radio.startListening();
#endif
}

void oradioopen(char *filename) {
#ifdef ARDUINORF24
	radio.begin();
	radio.openWritingPipe(pipeaddr(filename));
#endif
}

/* 
 *	Arduino Sensor library code 
 *		The sensorread() is a generic function called by 
 *		SENSOR basic function and command. The first argument
 *		is the sensor and the second argument the value.
 *		sensorread(n, 0) checks if the sensorstatus.
 */
#ifdef ARDUINOSENSORS
#ifdef ARDUINODHT
#include "DHT.h"
DHT dht(DHTPIN, DHTTYPE);
#endif
#ifdef ARDUINOSHT
#include <SHT3x.h>
SHT3x SHT;
#endif
#ifdef ARDUINOMQ2
#include <MQ2.h>
MQ2 mq2(MQ2PIN);
#endif
#ifdef ARDUINOLMS6
#include <Arduino_LSM6DSOX.h>
/* https://www.arduino.cc/en/Reference/Arduino_LSM6DSOX */
#endif

void sensorbegin(){
#ifdef ARDUINODHT
  dht.begin();
#endif
#ifdef ARDUINOSHT
  SHT.Begin();
#endif
#ifdef ARDUINOMQ2
  mq2.begin();
#endif
}

number_t sensorread(short s, short v) {
  switch (s) {
    case 0:
      return analogRead(A0+v);
    case 1:
#ifdef ARDUINODHT
			switch (v) {
				case 0:
					return 1;
				case 1:
					return dht.readHumidity();
				case 2:
					return dht.readTemperature();
			}     	
#endif
      return 0;
    case 2:
#ifdef ARDUINOSHT
      switch (v) {
        case 0:
          return 1;
        case 1:
          SHT.UpdateData();
          return SHT.GetRelHumidity();
        case 2:
          SHT.UpdateData();
          return SHT.GetTemperature();
      }       
#endif
      return 0;
    case 3:
#ifdef ARDUINOMQ2
      switch (v) {
        case 0:
          return 1;
        case 1:
          (void) mq2.read(false);
          return mq2.readLPG();;
        case 2:
          (void) mq2.read(false);
          return mq2.readCO();
        case 3:
          (void) mq2.read(false);
          return mq2.readSmoke();
      }       
#endif
      return 0;
    default:
      return 0;
  }
}

#else
void sensorbegin() {}
number_t sensorread(short s, short v) {return 0;};
#endif

// defined HARDWARE_H
#endif
