/*
 *
 * $Id: hardware-arduino.h,v 1.8 2023/02/18 20:16:59 stefan Exp stefan $
 *
 * Stefan's basic interpreter 
 *
 * Playing around with frugal programming. See the licence file on 
 * https://github.com/slviajero/tinybasic for copyright/left.
 *   (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 * Author: Stefan Lenz, sl001@serverfabrik.de
 * 
 * Credits:
 *  - XMC contributed by Florian
 *
 * Hardware definition file coming with TinybasicArduino.ino aka basic.c
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
 *  ARDUINO_AVR_MEGA2560, ARDUARDUINO_SAM_DUE: second serial port is Serial1-3 - no software serial
 *  ARDUINO_SAM_DUE: hardware heuristics
 *  ARDUINO_ARCH_AVR: nothing
 *  ARDUINO_ARCH_LGT8F: EEPROM code for flash EEPROM - platform fully supported now, yet no call 0
 *  ARDUINO_ARCH_ESP32 and ARDUINO_TTGO_T7_V14_Mini32, no tone, no analogWrite, avr/xyz obsolete
 *
 * The code still contains hardware heuristics from my own projects, 
 * will be removed in the future
 *
 */

#if defined(ARDUINO) && ! defined(__HARDWAREH__)
#define __HARDWAREH__ 

/* 
 * Arduino hardware settings , set here what you need or
 * use one of the predefined configurations below
 *
 * input/output methods USERPICOSERIAL, ARDUINOPS2
 *	ARDUINOPRT, DISPLAYCANSCROLL, ARDUINOLCDI2C,
 *	ARDUINOTFT, ARDUINONOKIA51, ARDUINOILI9488,
 *  ARDUINOSSD1306, ARDUINOMCUFRIEND
 * storage ARDUINOEEPROM, ARDUINOSD, ESPSPIFFS, RP2040LITTLEFS
 * storage ARDUINOEFS, SM32SDIO
 * sensors ARDUINOWIRE, ARDUINOSENSORS
 * network ARDUINORF24, ARDUNIOMQTT 
 * memory ARDUINOSPIRAM
 * real time clocks ARDUINORTC, ARDUINORTCEMULATION
 *
 *	leave this unset if you use the definitions below
 */

#undef USESPICOSERIAL 
#undef ARDUINOPS2
#undef ARDUINOUSBKBD
#undef ARDUINOZX81KBD
#undef ARDUINOPRT
#undef DISPLAYCANSCROLL
#undef ARDUINOLCDI2C
#undef ARDUINONOKIA51
#undef ARDUINOILI9488
#undef ARDUINOSSD1306
#undef ARDUINOMCUFRIEND
#undef ARDUINOEDP47
#undef ARDUINOGRAPHDUMMY
#undef LCDSHIELD
#undef ARDUINOTFT
#undef ARDUINOVGA
#define ARDUINOEEPROM
#undef ARDUINOI2CEEPROM
#undef ARDUINOEFS
#undef ARDUINOSD
#undef ESPSPIFFS
#undef RP2040LITTLEFS
#undef STM32SDIO
#undef ARDUINORTC
#undef ARDUINORTCEMULATION
#undef ARDUINOTONEEMULATION
#undef ARDUINOWIRE
#undef ARDUINOWIRESLAVE
#undef ARDUINORF24
#undef ARDUINOETH
#undef ARDUINOMQTT
#undef ARDUINOSENSORS
#undef ARDUINOSPIRAM 
#undef STANDALONE
#undef STANDALONESECONDSERIAL

/* experimental features, don't use unless you know the code */
/* 
 * this setting uses the EEPROM as program storage 
 * The idea is to create a virtual memory layout starting from 0 with the EEPROM
 * from elength() and then adding the BASIC RAM to it. himem and top need to be 
 * handled carefully. 
 */
#undef ARDUINOPGMEEPROM

/* 
 * Predefined hardware configurations, this assumes that all of the 
 *	above are undef
 *
 *	UNOPLAIN: 
 *		a plain UNO with no peripherals
 *	AVRLCD: 
 *		a AVR system with an LCD shield
 *	WEMOSSHIELD: 
 *		a Wemos D1 with a modified simple datalogger shield
 *		optional keyboard and i2c display
 *	MEGASHIELD: 
 *		an Arduino Mega with Ethernet Shield
 *		optional keyboard and i2c display
 *	TTGOVGA: 
 *		TTGO VGA1.4 system with PS2 keyboard, standalone
 * MEGATFT, DUETFT
 *    TFT 7inch screen systems, standalone
 * NANOBOARD:
 *   Arduino Nano Every board with PS2 keyboard and sensor 
 *    kit
 * MEGABOARD:
 *  A board for the MEGA with 64 kB RAM, SD Card, and real time
 *    clock
 * UNOBOARD:
 *  A board for an UNO with 64kB memory and EEPROM disk
 *    fits into an UNO flash only with integer
 * ESP01BOARD:
 *    ESP01 based board as a sensor / MQTT interface
 * RP2040BOARD:
 *    A ILI9488 hardware design based on an Arduino connect RP2040.
 * RP2040BOARD2:
 *  like the one above but based on the Pi Pico core
 * ESP32BOARD:
 *  same like above with an ESP32 core
 * MKRBOARD:
 *   a digital signage and low energy board
 */

#undef UNOPLAIN
#undef AVRLCD
#undef WEMOSSHIELD
#undef MEGASHIELD
#undef TTGOVGA
#undef DUETFT
#undef MEGATFT
#undef NANOBOARD
#undef MEGABOARD
#undef UNOBOARD
#undef ESP01BOARD
#undef RP2040BOARD
#undef RP2040BOARD2
#undef ESP32BOARD
#undef MKR1010BOARD

/* 
 * PIN settings and I2C addresses for various hardware configurations
 *	used a few heuristics and then the hardware definitions above 
 *
 *	#define SDPIN sets the SD CS pin - can be left as a default for most HW configs
 *   	TTGO needs it as default definitions in the board file are broken
 *	#define PS2DATAPIN, PS2IRQPIN sets PS2 pin
 */

/* PS2 Keyboard pins for AVR - use one interrupt pin 2 and one date pin 
    5 not 4 because 4 conflicts with SDPIN of the standard SD shield */
#define PS2DATAPIN 5
#define PS2IRQPIN  2

/* Ethernet - 10 is the default */
/* #define ETHPIN 10 */

/* The Pretzelboard definitions for Software Serial, conflicts with SPI */
#define SOFTSERIALRX 11
#define SOFTSERIALTX 12

/* near field pin settings for CE and CSN*/
#define RF24CEPIN 8
#define RF24CSNPIN 9

/* use standard I2C pins almost always */
#undef SDA_PIN
#undef SCL_PIN

/* set this is you want pin 4 on low interrupting the interpreter */
/* #define BREAKPIN 4 */
#undef BREAKPIN

/* the primary serial stream aka serial aka sream 1 */
#ifndef ALTSERIAL
#define SERIALPORT Serial
#endif

/* the secondary serial port aka prt aka stream 4 */
#ifndef PRTSERIAL
#define PRTSERIAL Serial1
#endif

/* 
 *  Pin settings for the ZX81 Keyboard
 *  first the 8 rows, then the 5 columns or the keyboard
 * 
 * MEGAs have many pins and default is to use the odd pins on the side
 * UNOs, NANOs, and others use the lower pins by default avoiding the 
 * pin 13 which is LED and doesn't work with standard schematics 
 */
#ifdef ARDUINOZX81KBD
#ifdef ARDUINO_AVR_MEGA2560
const byte zx81pins[] = {37, 35, 33, 31, 29, 27, 25, 23, 47, 45, 43, 41, 39};
#else
const char zx81pins[] = {7, 8, 9, 10, 11, 12, A0, A1, 2, 3, 4, 5, 6 };
#endif
#endif

/* 
 *  this is soft SPI for SD cards on MEGAs using 
 *  pins 10-13, a patched SD library is needed 
 *  for this: https://github.com/slviajero/SoftSD
 *  only needed for MEGA boards with an UNO shield
 */
#undef SOFTWARE_SPI_FOR_SD

/* 
 *  list of default i2c addresses
 *
 * some clock modules do have their EEPROM at 0x57. 
 * 0x050 this is the default lowest adress of standard EEPROMs
 * Configurable range is between 0x50 and 0x57 for modules with jumpers. 
 * Some clock modules do have their EEPROM at 0x57. 
 * 
 * Clock default for the size is 4096. Define your EFS EEPROM and I2C EEPROM
 * size here. One parameter set is for EFS and one parameter set is for 
 * plain serial EEPROMs.
 * 
 * RTCs are often at 0x68 
 */
#define EFSEEPROMADDR 0x050
/* #define EFSEEPROMSIZE 32768 */

#define RTCI2CADDR 0x068

/* the size of the plain I2C EEPROM, typically a clock */
#define I2CEEPROMADDR 0x057
/* #define I2CEEPROMSIZE 4096 */

/* is the I2C EEPROM buffered */
#define ARDUINOI2CEEPROM_BUFFERED

/*
 * Sensor library code - configure your sensors here
 */
#ifdef ARDUINOSENSORS
#undef ARDUINODHT
#define DHTTYPE DHT22
#define DHTPIN 2
#define  ARDUINOSHT
#define  ARDUINOMQ2
#define MQ2PIN A0
#undef ARDUINOLMS6
#undef ARDUINOAHT
#undef ARDUINOBMP280
#undef ARDUINOBME280
#endif


#if defined(ARDUINOSHT) || defined(ARDUINOLMS6) || defined(ARDUINOAHT) || defined(ARDUINOBMP280) || defined(RDUINOBME280)
#define ARDUINOWIRE
#endif

/*
 * The hardware models.
 * These are predefined hardware configurations.
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
/* #define ARDUINOMQTT */
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
#undef ARDUINOUSBKBD
#define DISPLAYCANSCROLL
#define ARDUINOTFT
#define ARDUINOSD
#define ARDUINOWIRE
#define ARDUINOPRT
#define ARDUINORTC
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
#define EFSEEPROMADDR 0x050 /* use clock EEPROM 0x057, set to 0x050 for external EEPROM */
#define STANDALONE
#endif

/* a UNO shield with memory and EFS EEPROM */
#if defined(UNOBOARD)
#define ARDUINOEEPROM
#define ARDUINOSPIRAM
#define ARDUINOEFS
#define ARDUINOWIRE
#define EFSEEPROMADDR 0x050
#define EFSEEPROMSIZE 65534
#endif

/* a MEGA shield with memory and SD card */
#if defined(MEGABOARD)
#undef USESPICOSERIAL
#define DISPLAYCANSCROLL
#define ARDUINOLCDI2C
#define ARDUINOEEPROM
#define ARDUINOPRT
#define ARDUINOSD
#define ARDUINOWIRE
#define ARDUINORTC 
#define ARDUINOSPIRAM
#define RAMPIN 53
#define SDPIN  49
#endif

/* an ESP01 board, using the internal flash 
 *  with the ESP01-8266 only pins 0 and 2 are usable freely
 *  on ESP01-ESP32C3 this is 9 and 2 while 2 is an analog pin
 *  9 cannot be pulled on low by any peripheral on boot because this 
 *  brings the board to flash mode
 */
#if defined(ESP01BOARD)
#undef ARDUINOEEPROM
#define ESPSPIFFS
#define ARDUINOMQTT
#define ARDUINOWIRE
#if defined(ARDUINOWIRE) && defined(ARDUINO_ARCH_ESP8266)
#define SDA_PIN 0
#define SCL_PIN 2
#endif
/* see:  https://github.com/espressif/arduino-esp32/issues/6376 
 *  nothing should block the port, e.g. DHT or anything
 */
#if defined(ARDUINOWIRE) && defined(ARDUINO_ARCH_ESP32)
#define SDA_PIN 9
#define SCL_PIN 2
#endif
/*
 *
 * Currently only 8=SDA and 9=SCL works / tested with AHT10
 */
#endif

/* an RP2040 based board with an ILI9488 display */
#if defined(RP2040BOARD)
#undef USESPICOSERIAL
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#undef  ARDUINOEEPROM
#define ARDUINOI2CEEPROM
#define ARDUINOPRT
#define ARDUINOSD
#undef RP2040LITTLEFS
#define ARDUINOWIRE
#define ARDUINORTC 
#define ARDUINOPS2
#define ARDUINOMQTT
#undef STANDALONE
#endif

/* an RP2040 Raspberry Pi Pico based board with an ILI9488 display */
#if defined(RP2040BOARD2)
#undef USESPICOSERIAL
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#undef  ARDUINOEEPROM
#undef ARDUINOPRT
#undef ARDUINOSD
#define RP2040LITTLEFS
#undef ARDUINOWIRE
#undef ARDUINORTC 
#undef ARDUINOPS2
#undef ARDUINOMQTT
#undef STANDALONE
#define ILI_LED A2
#define ILI_CS  15
#define ILI_RST 14
#define ILI_DC  13
#endif


/* an ESP32 board with an ILI9488 display, 
  some SD problems here with some hardware */
#if defined(ESP32BOARD)
#define ILI_CS 12
#define ILI_DC 27
#define ILI_RST 14
#define ILI_LED 26
#undef USESPICOSERIAL
#define ESPSPIFFS
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#define ARDUINOEEPROM
#define ARDUINOMQTT
#define ARDUINOWIRE
#endif

/* a board based on the Arduino MKR 1010 Wifi 
 *  made for low energy games 
 */
#if defined(MKR1010BOARD)
#define ILI_CS 7
#define ILI_DC 4
#define ILI_RST 6
#define ILI_LED A3
#undef  USESPICOSERIAL
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#define ARDUINOEFS
#define ARDUINOMQTT
#define ARDUINOWIRE
/* careful with the setting, lockout possible easily */
#undef ARDUINOUSBKBD
#undef STANDALONE
#endif

/*
 * defining the systype variable which informs BASIC about the platform at runtime
 */

#if defined(ARDUINO_ARCH_AVR)
const mem_t bsystype = SYSTYPE_AVR;
#elif defined(ARDUINO_ARCH_ESP8266)
const mem_t bsystype = SYSTYPE_ESP8266;
#elif defined(ARDUINO_ARCH_ESP32)
const mem_t bsystype = SYSTYPE_ESP32;
#elif defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED_RP2040)
const mem_t bsystype = SYSTYPE_RP2040;
#elif defined(ARDUINO_ARCH_SAM) && defined(ARDUINO_ARCH_SAMD)
const mem_t bsystype = SYSTYPE_SAM;
#elif defined(ARDUINO_ARCH_XMC)
const mem_t bsystype = SYSTYPE_XMC;
#elif defined(ARDUINO_ARCH_SMT32)
const mem_t bsystype = SYSTYPE_SMT32;
#else
const mem_t bsystype = SYSTYPE_UNKNOWN;
#endif 

/* 
 * the non AVR arcitectures - this is somehow raw
 * the ARDUINO 100 definition is probably not needed anymore
 */

#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED_RP2040) 
/* removed, unneeded, takes more space than it should */
/* #include <avr/dtostrf.h> */
#define ARDUINO 100
#endif

/* 
 *  DUE has no tone, we switch to emulation mode automatically
 */
#ifdef ARDUINO_SAM_DUE
#define ARDUINOTONEEMULATION
#endif

/*
 * Some settings, defaults, and dependencies
 * 
 * HASWIRE is set to start wire. Some libraries do this again.
 * 
 * Handling Wire and SPI is tricky as some of the libraries 
 * also include and start SPI and Wire code. 
 */

/* a clock needs wire */
#ifdef ARDUINORTC
#define ARDUINOWIRE
#endif

/* a display needs wire */
#if defined(ARDUINOLCDI2C) || defined(ARDUINOSSD1306) 
#define ARDUINOWIRE
#endif

/* EEPROM storage needs wire */
#if defined(ARDUINOEFS)
#define ARDUINOWIRE
#endif

/* external EEPROMs also need wire */
#if defined(ARDUINOI2CEEPROM)
#define ARDUINOWIRE
#endif

/* plain Wire support also needs wire ;-) */
#if defined(ARDUINOWIRE)
#define HASWIRE
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

/* the RAM chips */
#if defined(ARDUINOSPIRAM)
#define ARDUINOSPI
#endif


/* Networking and keyboards need the background task capability */
#if defined(ARDUINOMQTT) || defined(ARDUINOETH) || defined(ARDUINOUSBKBD) || defined(ARDUINOZX81KBD)
#define BASICBGTASK
#endif

/* picoserial is not a available on many platforms */
#ifdef USESPICOSERIAL
#ifndef UCSR0A
#undef USESPICOSERIAL
#endif
#endif

/* 
 * graphics adapter only when graphics hardware, overriding the 
 * language setting 
 * this is odd and can be removed later on
 */
#if !defined(ARDUINOTFT) && !defined(ARDUINOVGA) && !defined(ARDUINOILI9488) && !defined(ARDUINONOKIA51) && !defined(ARDUINOSSD1306) && !defined(ARDUINOMCUFRIEND) && !defined(ARDUINOGRAPHDUMMY) && !defined(ARDUINOEDP47)
#undef HASGRAPH
#endif

/* 
 *  incompatibilities and library stuff
 */
/* these platforms have no EEPROM and no emulation built-in */
#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED_RP2040) 
#undef ARDUINOEEPROM
#endif

/* 
 * Keyboard library, on AVR systems Paul Stoffregens original 
 * PS2 library works.
 * I recommend to use my patched version from 
 * https://github.com/slviajero/PS2Keyboard
 * works with ESP, has keyboard.peek()
 */
#ifdef ARDUINOPS2
#include <PS2Keyboard.h>
#endif

/*
 * The USB keyboard code - tested only on DUE and the like
 * not really good 
 */
#ifdef ARDUINOUSBKBD
#include <KeyboardController.h>
#endif

/*
 * The ZX81 keyboard code - tested on AVR MEGA256
 */
#ifdef ARDUINOZX81KBD
#include <ZX81Keyboard.h>
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
 *  Fix a few things around XMC, contributed by Florian
 */
#if defined(ARDUINO_ARCH_XMC)
#undef USESPICOSERIAL
#undef ARDUINOPROGMEM
#endif 

/*
 * This works for AVR and ESP EEPROM dummy. 
 * On XMC you need https://github.com/slviajero/XMCEEPROMLib
 * Throws a compiler error for other platforms.
 */
#ifdef ARDUINOEEPROM
#ifdef ARDUINO_ARCH_XMC
#include <XMCEEPROMLib.h>
#else
#ifdef ARDUINO_ARCH_SAMD
//#include <FlashStorage_SAMD.h>
#else
#include <EEPROM.h>
#endif
#endif
#endif

/* Standard SPI */
#ifdef ARDUINOSPI
#include <SPI.h>
#endif

/* Standard wire - triggered by the HASWIRE macro now */
#ifdef HASWIRE
#include <Wire.h>
#endif

/* 
 * the display library includes for LCD
 */
#ifdef LCDSHIELD 
#include <LiquidCrystal.h>
#endif

/*
 * I2C displays 
 */

#ifdef ARDUINOLCDI2C
#include <LiquidCrystal_I2C.h>
#endif

/*
 * This is the monochrome library of Oli Kraus
 * https://github.com/olikraus/u8g2/wiki/u8g2reference
 * It can harware scroll, but this is not yet implemented 
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
 * Lilygo EDP47 displays, 4.7 inch epapers using the respective library 
 * from Lilygo
 * https://github.com/Xinyuan-LilyGO/LilyGo-EPD47
 * 
 */
#ifdef ARDUINOEDP47
#include "epd_driver.h"
#include "font/firasans.h"
#endif


/* 
 * experimental networking code 
 * currently the standard Ethernet shield, ESP Wifi 
 * MKW Wifi, and RP2040 Wifi is supported. All of them 
 * with the standard library.
 *
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
 * Needs https://github.com/slviajero/FabGL
 */
#if defined(ARDUINOVGA) && defined(ARDUINO_TTGO_T7_V14_Mini32)
#include <WiFi.h> 
#include <fabgl.h> 
#endif

/*
 * SD filesystems with the standard SD driver
 * for MEGA 256 a soft SPI solution is needed 
 * if standard shields are used, this is a patched
 * SD library https://github.com/slviajero/SoftSD
 */
#ifdef ARDUINOSD
#define FILESYSTEMDRIVER
#if defined(SOFTWARE_SPI_FOR_SD)
#include <SoftSD.h>
#else
#include <SD.h>
#endif
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
 * STM32 SDIO driver for he SD card slot of the STM32F4 boards (and others)
 */
#ifdef STM32SDIO 
#define FILESYSTEMDRIVER
#include <STM32SD.h> 
#ifndef SD_DETECT_PIN
#define SD_DETECT_PIN SD_DETECT_NONE
#endif
#endif
/*
 * external flash file systems override internal filesystems
 * currently BASIC can only have one filesystem
 */ 
#ifdef ARDUINOSD
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
#undef STM32SDIO
#define FILESYSTEMDRIVER
#endif

/* the EFS object is used for filesystems and raw EEPROM access */
#if (defined(ARDUINOI2CEEPROM) && defined(ARDUINOI2CEEPROM_BUFFERED)) || defined(ARDUINOEFS)
#include <EepromFS.h>
#endif

/* if there is an unbuffered I2C EEPROM, use an autodetect mechanism */
#if defined(ARDUINOI2CEEPROM) 
unsigned int i2ceepromsize = 0;
#endif

/*
 * Software SPI only on Mega2560
 */

#ifndef ARDUINO_AVR_MEGA2560
#undef SOFTWARE_SPI_FOR_SD
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
mem_t sendcr = 0;

#ifdef ARDUINOPRT
int serial1_baudrate = 9600;
mem_t blockmode = 1;
#else 
const int serial1_baudrate = 0;
mem_t blockmode = 0;
#endif

/*  handling time - part of the Arduino core - only needed on POSIX OSes */
void timeinit() {}

/* starting wiring is only needed on raspberry */
void wiringbegin() {}

/* POSIX signals - not needed here */
void signalon() {}

/*
 * helper functions OS, heuristic on how much memory is 
 * available in BASIC
 * Arduino information from
 * data from https://docs.arduino.cc/learn/programming/memory-guide
 */
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_XMC) || defined(ARDUINO_ARCH_STM32)
extern "C" char* sbrk(int incr);
long freeRam() {
  char top;
  return &top - reinterpret_cast<char*>(sbrk(0));
}
#elif defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_LGT8F)
long freeRam() {
  extern int __heap_start,*__brkval;
  int v;
  return (int)&v - (__brkval == 0  
    ? (int)&__heap_start : (int) __brkval);  
}
#elif defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
long freeRam() {
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
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_STM32)
  return freeRam() - 4000;
#endif
#if defined(ARDUINO_ARCH_XMC)
  return freeRam() - 2000;
#endif
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_LGT8F) 
  int overhead=192;
#ifdef HASFLOAT 
  overhead+=96;
#endif
#ifdef ARDUINO_AVR_MEGA2560
  overhead+=96;
#endif
#ifdef ARDUINOWIRE
  overhead+=128;
#endif
#ifdef ARDUINORF24
  overhead+=128;
#endif
#if defined(ARDUINOSD)
  overhead+=512;
#endif
#ifdef ARDUINOZX81KBD
  overhead+=64;
#endif
#ifdef ARDUINOETH
  overhead+=256;
#endif
#ifdef HASGRAPH
  overhead+=256; /* a bit on the safe side */
#endif
  return freeRam() - overhead;
#endif
#if defined(ARDUINO_NANO_RP2040_CONNECT) || defined(ARDUINO_RASPBERRY_PI_PICO)
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
    eflush(); /* if there is a I2C eeprom dummy, flush the buffer */
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
  ESP.restart();
#endif
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
  callzero();
#endif
#if defined(ARDUINO_ARCH_LGT8F)
#endif
}

/*
 * I used these two articles 
 * https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
 * https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
 * for this very simple implementation - needs to be improved (pass data from sleep
 * state to sleep state via EEPROM)
 */
#if defined(ARDUINO_ARCH_SAMD) 
#define HASBUILTINRTC
#include "RTCZero.h"
#include "ArduinoLowPower.h"
RTCZero rtc;
#endif

/* STM32duino have the same structure */
#if defined(ARDUINO_ARCH_STM32)
#define HASBUILTINRTC
#include "STM32RTC.h"
#include "STM32LowPower.h"
STM32RTC& rtc = STM32RTC::getInstance();
#endif

/* for ESP32 we also include the time stuctures and offer a POSIX style clock*/
#if defined(ARDUINO_ARCH_ESP32)
#include "time.h"
#include <sys/time.h>
#endif


/* this is unfinished, don't use */ 
void rtcsqw();

#define LOWPOWERINTPIN 2
void aftersleepinterrupt(void) { }

void activatesleep(long t) {
  eflush(); /* if there is a I2C eeprom dummy, flush the buffer */
#if defined(ARDUINO_ARCH_ESP8266)
  ESP.deepSleep(t*1000);
#endif
#if defined(ARDUINO_ARCH_ESP32)
  esp_sleep_enable_timer_wakeup(t*1000);
  esp_deep_sleep_start();
#endif
#if defined(ARDUINO_ARCH_SAMD)
  LowPower.sleep((int) t);
#endif
#if defined(ARDUINO_AVR_ATmega644)
/* unfinished, don't use, just test code
  rtcsqw();
  pinMode(LOWPOWERINTPIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(LOWPOWERINTPIN), aftersleepinterrupt, CHANGE);
  sleepMode(SLEEP_POWER_SAVE);
  sleep();
  detachInterrupt(digitalPinToInterrupt(LOWPOWERINTPIN));
  noSleep();
*/
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
 * 
 * Non rgb ready displays on rgbcolor translate to their native color
 * when BASIC requests an rgb color, in this case the nearest 4 bit 
 * color of the display is also stored for use in the text DISLAY driver
 * code
 */

 /* generate a 4 bit vga color from a given rgb color */
uint8_t rgbtovga(int r, int g, int b) {
  short vga;
  if (r>191 || g>191 || b>191) vga=8; else vga=0;
  vga=vga+r/128+g/128*2+b/128*4;
  return vga;
}

/* 
 * global variables for a standard LCD shield. 
 * Includes the standard Arduino LiquidCrystal library
 */
#ifdef LCDSHIELD 
#define DISPLAYDRIVER
#undef DISPLAYHASCOLOR
#undef DISPLAYHASGRAPH
/* LCD shield pins to Arduino
 *  RS, EN, d4, d5, d6, d7; 
 * backlight on pin 10;
 */
const int dsp_rows=2;
const int dsp_columns=16;
LiquidCrystal lcd( 8,  9,  4,  5,  6,  7);
void dspbegin() { lcd.begin(dsp_columns, dsp_rows); dspsetscrollmode(1, 1);  }
void dspprintchar(char c, mem_t col, mem_t row) { lcd.setCursor(col, row); if (c) lcd.write(c);}
void dspclear() { lcd.clear(); }
void dspupdate() {}
void dspsetcursor(mem_t c) { if (c) lcd.blink(); else lcd.noBlink(); }
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0; }
#define HASKEYPAD
/* elementary keypad reader left=1, right=2, up=3, down=4, select=<lf> */
short keypadread(){
	int a=analogRead(A0);
	if (a >= 850) return 0;
	else if (a>=600 && a<850) return 10;
	else if (a>=400 && a<600) return '1'; 
	else if (a>=200 && a<400) return '3';
	else if (a>=60  && a<200) return '4';
	else return '2';
}
/* repeat mode of the keypad - off means block, on means return immediately */
mem_t kbdrepeat=0;
#endif

/* 
 * A LCD display connnected via I2C, uses the standard 
 * Arduino I2C display library.
 */ 
#ifdef ARDUINOLCDI2C
#define DISPLAYDRIVER
#undef DISPLAYHASCOLOR
#undef DISPLAYHASGRAPH
const int dsp_rows=4;
const int dsp_columns=20;
LiquidCrystal_I2C lcd(0x27, dsp_columns, dsp_rows);
void dspbegin() { lcd.init(); lcd.backlight(); dspsetscrollmode(1, 1); }
void dspprintchar(char c, mem_t col, mem_t row) { lcd.setCursor(col, row); if (c) lcd.write(c); }
void dspclear() { lcd.clear(); }
void dspupdate() {}
void dspsetcursor(mem_t c) { if (c) lcd.blink(); else lcd.noBlink(); }
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0; }
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
#define DISPLAYPAGEMODE
#undef DISPLAYHASCOLOR /* display driver not color aware for this display */
#define DISPLAYHASGRAPH
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
typedef uint8_t dspcolor_t;
dspcolor_t dspfgcolor = 1;
dspcolor_t dspbgcolor = 0;
char dspfontsize = 8;
void dspbegin() { u8g2.begin(); u8g2.setFont(u8g2_font_amstrad_cpc_extended_8r); }
void dspprintchar(char c, mem_t col, mem_t row) { char b[] = { 0, 0 }; b[0]=c; if (c) u8g2.drawStr(col*dspfontsize+2, (row+1)*dspfontsize, b); }
void dspclear() { u8g2.clearBuffer(); u8g2.sendBuffer(); dspfgcolor=1; }
void dspupdate() { u8g2.sendBuffer(); }
void dspsetcursor(mem_t c) {}
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0;}
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
 * 4.7 inch epaper displays are derived from the NOKIA51 code, no grayscales 
 * at the moment. Forcing the font into rectangles and hoping this works.
 * 
 * Epapers bypass the display driver here and use a graphics based display 
 * mode instead
 */
#ifdef ARDUINOEDP47
#define GRAPHDISPLAYDRIVER
#define DISPLAYPAGEMODE
#undef DISPLAYHASCOLOR /* display driver not color aware for this display */
#define DISPLAYHASGRAPH
const int dsp_width=960;
const int dsp_height=540;
const int dsp_rows=0;
const int dsp_columns=0;
typedef uint8_t dspcolor_t;
dspcolor_t dspfgcolor = 1;
dspcolor_t dspbgcolor = 0;
char dspfontsize = 24;
int dspgraphcursor_x = 0;
int dspgraphcursor_y = dspfontsize;
void dspbegin() { epd_init(); dspclear(); }
void dspprintstring(char* s) { 
  epd_poweron();
  writeln((GFXfont *)&FiraSans, s, &dspgraphcursor_x, &dspgraphcursor_y, NULL);
  epd_poweroff();
}
void dspclear() { epd_poweron();  epd_clear(); epd_poweroff(); dspfgcolor=1; }
void dspupdate() { }
void dspsetcursor(mem_t c) {}
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0;}
void rgbcolor(int r, int g, int b) {}
void vgacolor(short c) { dspfgcolor=c%3; }
void plot(int x, int y) {  }
void line(int x0, int y0, int x1, int y1)   {  }
void rect(int x0, int y0, int x1, int y1)   {   }
void frect(int x0, int y0, int x1, int y1)  { }
void circle(int x0, int y0, int r) {  }
void fcircle(int x0, int y0, int r) {  }
#endif
 

/* 
 * Small SSD1306 OLED displays with I2C interface
 * This is a buffered display it has a dspupdate() function 
 * it also needs to call dspgraphupdate() after each graphic 
 * operation
 */ 
#ifdef ARDUINOSSD1306
#define DISPLAYDRIVER
#define DISPLAYPAGEMODE
#undef DISLAYHASCOLOR /* display driver not color aware for this display */
#define DISPLAYHASGRAPH
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
typedef uint8_t dspcolor_t;
dspcolor_t dspfgcolor = 1;
dspcolor_t dspbgcolor = 0;
void dspbegin() { u8g2.begin(); u8g2.setFont(u8g2_font_amstrad_cpc_extended_8r); }
void dspprintchar(char c, mem_t col, mem_t row) { char b[] = { 0, 0 }; b[0]=c; if (c) u8g2.drawStr(col*dspfontsize+2, (row+1)*dspfontsize, b); }
void dspclear() { u8g2.clearBuffer(); u8g2.sendBuffer(); dspfgcolor=1; }
void dspupdate() { u8g2.sendBuffer(); }
void dspsetcursor(mem_t c) {}
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0;}
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
#define DISPLAYHASCOLOR
#define DISPLAYHASGRAPH
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
typedef uint16_t dspcolor_t;
const uint16_t dspdefaultfgcolor = 0xFFFF;
const uint8_t dspdefaultfgvgacolor = 0x0F;
dspcolor_t dspfgcolor = dspdefaultfgcolor;
dspcolor_t dspbgcolor = 0;
dspcolor_t dsptmpcolor = 0;
uint8_t dspfgvgacolor = dspdefaultfgvgacolor;
uint8_t dsptmpvgacolor = 0;
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
void dspprintchar(char c, mem_t col, mem_t row) {  if (c) tft.drawChar(col*dspfontsize, row*dspfontsize, c, dspfgcolor, dspbgcolor, 2); }
void dspclear() { 
  tft.fillScreen(dspbgcolor); 
  dspfgcolor = dspdefaultfgcolor; 
  dspfgvgacolor = dspdefaultfgvgacolor; 
}
void dspupdate() {}
void dspsetcursor(mem_t c) {}
void dspsavepen() { dsptmpcolor=dspfgcolor; dsptmpvgacolor=dspfgvgacolor; }
void dsprestorepen() { dspfgcolor=dsptmpcolor; dspfgvgacolor=dsptmpvgacolor; }
void dspsetfgcolor(uint8_t c) { vgacolor(c); }
void dspsetbgcolor(uint8_t c) { }
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0; }
void rgbcolor(int r, int g, int b) { dspfgvgacolor=rgbtovga(r, g, b); dspfgcolor=tft.color565(r, g, b);}
void vgacolor(short c) {  
  short base=128;
  dspfgvgacolor=c;
  if (c==8) { dspfgcolor=tft.color565(64, 64, 64); return; }
  if (c>8) base=255;
  dspfgcolor=tft.color565(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
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
#define DISPLAYHASCOLOR
#define DISPLAYHASGRAPH
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
typedef uint16_t dspcolor_t;
const uint16_t dspdefaultfgcolor = 0xFFFF;
const uint8_t dspdefaultfgvgacolor = 0x0F;
dspcolor_t dspfgcolor = dspdefaultfgcolor;
dspcolor_t dspbgcolor = 0;
dspcolor_t dsptmpcolor = 0;
uint8_t dspfgvgacolor = dspdefaultfgvgacolor;
uint8_t dsptmpvgacolor = 0;
void dspbegin() { 
  uint16_t ID = tft.readID();
  if (ID == 0xD3D3) ID = 0x9481; /* write-only shield - taken from the MCDFRIEND demo */
  tft.begin(ID); 
  tft.setRotation(1); /* ILI in landscape, 3: SD slot on right the side */
  tft.setTextColor(dspfgcolor);
  tft.setTextSize(2);
  tft.fillScreen(dspbgcolor); 
  dspsetscrollmode(1, 4); /* scrolling is on, scroll 4 lines at once */
 }
void dspprintchar(char c, mem_t col, mem_t row) {  if (c) tft.drawChar(col*dspfontsize, row*dspfontsize, c, dspfgcolor, dspbgcolor, 2); }
void dspclear() { 
  tft.fillScreen(dspbgcolor); 
  dspfgcolor = dspdefaultfgcolor; 
  dspfgvgacolor = dspdefaultfgvgacolor; 
}
void dspupdate() {}
void dspsetcursor(mem_t c) {}
void dspsavepen() { dsptmpcolor=dspfgcolor; dsptmpvgacolor=dspfgvgacolor; }
void dsprestorepen() { dspfgcolor=dsptmpcolor; dspfgvgacolor=dsptmpvgacolor; }
void dspsetfgcolor(uint8_t c) { vgacolor(c); }
void dspsetbgcolor(uint8_t c) { }
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0; }
void rgbcolor(int r, int g, int b) { dspfgvgacolor=rgbtovga(r, g, b); dspfgcolor=tft.color565(r, g, b);}
void vgacolor(short c) {  
  short base=128;
  dspfgvgacolor=c;
  if (c==8) { dspfgcolor=tft.color565(64, 64, 64); return; }
  if (c>8) base=255;
  dspfgcolor=tft.color565(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
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
#undef DISPLAYHASCOLOR
#define DISPLAYHASGRAPH
const int dsp_rows=20;
const int dsp_columns=30;
const uint16_t dspdefaultfgcolor = 1;
char dspfontsize = 16;
typedef uint16_t dspcolor_t;
dspcolor_t dspfgcolor = 0xFFFF;
dspcolor_t dspbgcolor = 0x0000;
void dspbegin() { dspsetscrollmode(1, 4); }
void dspprintchar(char c, mem_t col, mem_t row) {}
void dspclear() {}
void dspupdate() {}
void dspsetcursor(mem_t c) {}
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0; }
void rgbcolor(int r, int g, int b) { dspfgcolor=0; }
void vgacolor(short c) {  
  short base=128;
  if (c==8) { rgbcolor(64, 64, 64); return; }
  if (c>8) base=255;
  rgbcolor(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
}
void plot(int x, int y) {}
void line(int x0, int y0, int x1, int y1) {}
void rect(int x0, int y0, int x1, int y1) {}
void frect(int x0, int y0, int x1, int y1) {}
void circle(int x0, int y0, int r) {}
void fcircle(int x0, int y0, int r) {}
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
#define DISPLAYHASCOLOR 
#define DISPLAYHASGRAPH
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
const uint32_t dspdefaultfgcolor = 0x00FFFFFF;
const uint8_t dspdefaultfgvgacolor = 0x0F;
typedef uint32_t dspcolor_t;
dspcolor_t dspfgcolor = dspdefaultfgcolor;
dspcolor_t dspbgcolor = 0;
dspcolor_t dsptmpcolor = 0;
uint8_t dspfgvgacolor = dspdefaultfgvgacolor;
uint8_t dsptmpvgacolor = 0;
void dspbegin() { tft.InitLCD(); tft.setFont(BigFont); tft.clrScr(); dspsetscrollmode(1, 4); }
void dspprintchar(char c, mem_t col, mem_t row) { if (c) tft.printChar(c, col*dspfontsize, row*dspfontsize); }
void dspclear() { 
  tft.clrScr();  
  dspfgcolor = dspdefaultfgcolor; 
  dspfgvgacolor = dspdefaultfgvgacolor; 
  vgacolor(dspfgvgacolor);
}
void rgbcolor(int r, int g, int b) { 
  tft.setColor(r,g,b); 
  dspfgcolor=((uint8_t)r << 16) + ((uint8_t)g << 8) + b;
  dspfgvgacolor=rgbtovga(r, g, b);
}
void vgacolor(short c) {
  short base=128;
  dspfgvgacolor=c;
  if (c==8) { tft.setColor(64, 64, 64); return; }
  if (c>8) base=255;
  tft.setColor(base*(c&1), base*((c&2)/2), base*((c&4)/4));  
}
void dspupdate() {}
void dspsetcursor(mem_t c) {}
void dspsavepen() { dsptmpcolor=dspfgcolor; dsptmpvgacolor=dspfgvgacolor; }
void dsprestorepen() { dspfgcolor=dsptmpcolor; dspfgvgacolor=dsptmpvgacolor; }
void dspsetfgcolor(uint8_t c) { vgacolor(c); }
void dspsetbgcolor(uint8_t c) { }
void dspsetreverse(mem_t c) {}
mem_t dspident() {return 0;}
void plot(int x, int y) { tft.drawPixel(x, y); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, y0, x1, y1); }
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, y0, x1, y1); }
void circle(int x0, int y0, int r) { tft.drawCircle(x0, y0, r); }
void fcircle(int x0, int y0, int r) { tft.fillCircle(x0, y0, r); }
#endif

/* 
 * this is the VGA code for fablib - experimental
 * not all modes and possibilities explored, with networking on an ESP
 * VGA16 is advisable. It leaves enough memory for the interpreter and network.
 * this code overrides the display driver logic as fabgl brings an own 
 * terminal emulation
 */
#if defined(ARDUINOVGA) && defined(ARDUINO_TTGO_T7_V14_Mini32) 
/* static fabgl::VGAController VGAController; */
fabgl::VGA16Controller VGAController; /* 16 color object with less memory */
static fabgl::Terminal Terminal;
static Canvas cv(&VGAController);
TerminalController tc(&Terminal);
Color vga_graph_pen = Color::BrightWhite;
Color vga_graph_brush = Color::Black;
Color vga_txt_pen = Color::BrightGreen;
Color vga_txt_background = Color::Black;
#ifdef HASTONE
fabgl::SoundGenerator soundGenerator;
#endif


/* this starts the vga controller and the terminal right now */
void vgabegin() {
	VGAController.begin(GPIO_NUM_22, GPIO_NUM_21, GPIO_NUM_19, GPIO_NUM_18, GPIO_NUM_5, GPIO_NUM_4, GPIO_NUM_23, GPIO_NUM_15);
	VGAController.setResolution(VGA_640x200_70Hz);
	Terminal.begin(&VGAController);
	Terminal.setBackgroundColor(vga_txt_background);
	Terminal.setForegroundColor(vga_txt_pen);
  Terminal.connectLocally();
  Terminal.clear();
  Terminal.enableCursor(1); 
  Terminal.setTerminalType(TermType::VT52);
}

int vgastat(char c) {return 0; }

/* scale the screen size */
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
    case 12: /* form feed is clear screen */
  		Terminal.write(27); Terminal.write('H');
    	Terminal.write(27); Terminal.write('J');
      return;
    case 10: /* this is LF Unix style doing also a CR */
      Terminal.write(10); Terminal.write(13);
    	return;
  	}
  	Terminal.write(c);
}
#else 
void vgabegin(){}
int vgastat(char c) {return 0; }
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
char fabgllastchar = 0;
#else
#if defined(ARDUINO) && defined(ARDUINOPS2)
#define PS2KEYBOARD
#define HASKEYBOARD
PS2Keyboard keyboard;
#else 
#if defined(ARDUINO) && defined(ARDUINOUSBKBD)
#define HASKEYBOARD
#define USBKEYBOARD
USBHost usb;
KeyboardController keyboard(usb);
char usbkey=0;
#else 
#if defined(ARDUINOZX81KBD)
#define HASKEYBOARD
#define ZX81KEYBOARD
ZX81Keyboard keyboard;
#endif
#endif
#endif
#endif

/*
 * Experimental, unfinished, rudimentary
 */
#if defined(ARDUINOUSBKBD)
/* not really needed, only here for reference */
char usbkeymapUS[] = 
{' ', '"', '!', '#', '$', '%', '&', '\'', '(', ')', '*', '+', 
 ',', '-', '.', '/', '0', '1', '2', '3', '4', '5', '6', '7', 
 '8', '9', ':', ';', '<', '=', '>', '?', '@', 'A', 'B', 'C', 
 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z', '[', 
 '\\', ']', '^', '_', '`', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
 't', 'u', 'v', 'w', 'x', 'y', 'z', '{', '|', '}', 0, 0};
/* map the ASCII codes of the essential keys for BASIC of a
 *  German keyboard, most notable is < and > which is ö/a
 */
char usbkeymapGerman[] = 
{' ', '!',  '!', '#', '$', '%', '/', '>', ')', '=', '(', '+', 
 ',', '-', '.', '-', '0', '1', '2', '3', '4', '5', '6', '7', 
 '8', '9', ':', '<', ';', '=', ':', '_', '"', 'A', 'B', 'C', 
 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 
 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Z', 'Y', '[', 
 '#', '+', '&', '?', '@', 'a', 'b', 'c', 'd', 'e', 'f', 'g', 
 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's',
 't', 'u', 'v', 'w', 'x', 'z', 'y', '{', '\'', '*', 0, 0};

/*
 * he weak functions from the keyboard controller class implemented
 */
void keyPressed() {}
void keyReleased() {
  switch (keyboard.getOemKey()) {
    case 40:
      usbkey=10;
      break;
    case 42:
    case 76:
      usbkey=127;
      break;
    case 41:
      usbkey=27;
      break;
    default:
      usbkey=keyboard.getKey(); 
      if (usbkey>31 && usbkey<128) usbkey=usbkeymapGerman[usbkey-32]; 
  }
}
#endif

/* 
 * keyboard controller code 
 */

void kbdbegin() {
#ifdef PS2KEYBOARD
	keyboard.begin(PS2DATAPIN, PS2IRQPIN, PS2Keymap_German);
#else
#ifdef PS2FABLIB
	PS2Controller.begin(PS2Preset::KeyboardPort0);
	PS2Controller.keyboard()->setLayout(&fabgl::GermanLayout);
#else 
#ifdef USBKEYBOARD
/* nothing to be done here */
#else
#ifdef ZX81KEYBOARD
  keyboard.begin(zx81pins);
#endif
#endif
#endif
#endif
}

int kbdstat(char c) {return 0; }

char kbdavailable(){
#ifdef PS2KEYBOARD
	return keyboard.available();
#else
#ifdef PS2FABLIB
  if (fabgllastchar) return Terminal.available()+1; else return Terminal.available();
#else 
#ifdef USBKEYBOARD
/* if we already have a key, tell the caller we have one */
  if (usbkey) return 1; 
/* if not, look it up */
  if (usbkey) return 1; else return 0;
#else
#ifdef ZX81KEYBOARD
  return keyboard.available();
#endif
#endif
#endif
#endif
#ifdef HASKEYPAD
/* a poor man's debouncer, unstable state returns 0 */
  char c=keypadread();
  if (c) bdelay(2); else return 0;
  if (c == keypadread()) return 1; else return 0;
	/* return keypadread()!=0; */
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
  if (fabgllastchar) { c=fabgllastchar; fabgllastchar=0; }
  else c=Terminal.read();
#else 
#ifdef USBKEYBOARD
/* if we have read a key before, return it else look it up */
  c=usbkey; 
  usbkey=0; 
#else
#ifdef ZX81KEYBOARD
  c=keyboard.read();
#endif
#endif
#endif
#ifdef HASKEYPAD
	c=keypadread();
/* if the keypad is in non repeat mode, block */
	if (!kbdrepeat) while(kbdavailable()) byield();
#endif	
	if (c == 13) c=10;
	return c;
}

char kbdcheckch() {
char c;
#ifdef PS2KEYBOARD
/*
 * only works with the patched library https://github.com/slviajero/PS2Keyboard
 */
#ifdef PS2KEYBOARD_HASPEEK
 return keyboard.peek();
#else 
/*
 * for the original library  https://github.com/PaulStoffregen/PS2Keyboard 
 * GET does not work properly with it as there is no peek functionality which is needed
 * for non blocking IO and the ability to stop programs
 */
  if (kbdavailable()) return kbdread();
#endif
#else
#ifdef PS2FABLIB
  if (fabgllastchar) return fabgllastchar;
	if (kbdavailable()) { fabgllastchar=Terminal.read(); return fabgllastchar; }
#else 
#ifdef USBKEYBOARD
  return usbkey;
#else 
#ifdef ZX81KEYBOARD
  return keyboard.lastKey; /* dont peek here as checkch called in a fast loop in statement(), peek done in byield*/
#endif
#endif
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
 * dspbegin()
 * dspupdate()
 * dspsetcursor(mem_t c) 
 * dspsetfgcolor(address_t c)  
 * void dspsetbgcolor(address_t c)  
 * void dspsetreverse(mem_t c)  
 * mem_t dspident()  
 *
 * have to be defined before in a hardware dependent section.
 * Only dspprintchar and dspclear are needed, all other can be stubs
 *
 * VGA systems don't use the display driver for text based output.
 *
 * The display driver exists as a buffered version that can scroll
 * or an unbuffered version that cannot scroll. Interfaces to hardware
 * scrolling are not yet implemented.
 * 
 * A VT52 state engine is implemented and works for buffered and 
 * unbuffered displays. Only buffered displays have the full VT52
 * feature set including most of the GEMDOS extensions described here:
 * https://en.wikipedia.org/wiki/VT52
 * 
 * dspupdatemode controls the page update behaviour 
 *    0: character mode, display each character separately
 *    1: line mode, update the display after each line
 *    2: page mode, update the display after an ETX
 * ignored if the display has no update function
 *
 */

#ifdef DISPLAYDRIVER

/* the cursor position */
mem_t dspmycol = 0;
mem_t dspmyrow = 0;

/* the escape state of the vt52 terminal */
mem_t dspesc = 0;

/* which update mode do we have */
mem_t dspupdatemode = 0;

/* how do we handle wrap 0 is wrap, 1 is no wrap */
mem_t dspwrap = 0; 

/* the print mode */
mem_t dspprintmode = 0;

/* the scroll control variables */
mem_t dspscrollmode = 0;
mem_t dsp_scroll_rows = 1;

int dspstat(char c) { return 0; }

void dspsetcursorx(mem_t c) {
  if (c>=0 && c<dsp_columns) dspmycol=c;
  dspprintchar(0, dspmycol, dspmyrow); /* needed to really position the cursor if there is a hardware cursor */
}

void dspsetcursory(mem_t r) {
  if (r>=0 && r<dsp_rows) dspmyrow=r;
    dspprintchar(0, dspmycol, dspmyrow); /* needed to really position the cursor if there is a hardware cursor */
}

mem_t dspgetcursorx() { return dspmycol; }

mem_t dspgetcursory() { return dspmyrow; }

char dspactive() {
	return od == ODSP;
}

/* to whom the bell tolls - implement this to you own liking */
void dspbell() {}

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

/* scrollable displays need a character buffer */
#ifdef DISPLAYCANSCROLL

/* 
 * text color code for the scrolling display 
 * 
 * non scrolling displays simply use the pen color of the display
 * stored in dspfgcolor to paint the information on the screen
 * 
 * for scrolling displays we store the color information of every
 * character in the display buffer to enable scrolling, to limit the 
 * storage requirements, this code translates the color to a 4 bit VGA
 * color. This means that if BASIC uses 24 bit colors, the color may
 * change at scroll
 *
 */

#ifdef DISPLAYHASCOLOR
/* 
 * the display buffer is int here, lower 8 bits plus the sign
 * are the character, higher 7 the color and font 
 */
typedef short dspbuffer_t;
#else

/* plain monochrome display buffer */
typedef char dspbuffer_t;
const uint16_t dspfgvgacolor = 0;
#endif

dspbuffer_t dspbuffer[dsp_rows][dsp_columns];

/* buffer access functions */
dspbuffer_t dspget(address_t i) {
  if (i>=0 && i<=dsp_columns*dsp_rows-1) return dspbuffer[i/dsp_columns][i%dsp_columns]; else return 0;
}

dspbuffer_t dspgetrc(mem_t r, mem_t c) { return dspbuffer[r][c]; }

dspbuffer_t dspgetc(mem_t c) { return dspbuffer[dspmyrow][c]; }

/* this functions prints a character and updates the display buffer */
void dspsetxy(dspbuffer_t ch, mem_t c, mem_t r) {
  if (r>=0 && c>=0 && r<dsp_rows && c<dsp_columns) {
    dspbuffer[r][c]=(dspbuffer_t)ch | (dspfgvgacolor << 8);
    if (ch != 0) dspprintchar(ch, c, r); else dspprintchar(' ', c, r);
  }
}

void dspset(address_t i, dspbuffer_t ch) {
  mem_t c=i%dsp_columns;
  mem_t r=i/dsp_columns;
  dspsetxy(ch, c, r);
}

/* 0 normal scroll, 1 enable waitonscroll function */
void dspsetscrollmode(char c, short l) {
	dspscrollmode = c;
	dsp_scroll_rows = l;
}

/* clear the buffer */
void dspbufferclear() {
	mem_t r,c;
	for (r=0; r<dsp_rows; r++)
		for (c=0; c<dsp_columns; c++)
      dspbuffer[r][c]=0;
  dspmyrow=0;
  dspmycol=0;
}

#ifdef DISPLAYHASCOLOR
const uint16_t charmask = 0x80FF;
#endif

/* do the scroll */
void dspscroll(mem_t scroll_rows, mem_t scroll_top=0){
	mem_t r,c;
  dspbuffer_t a,b;
  
/* scroll data up and redraw the display */
  for (r=scroll_top; r<dsp_rows-scroll_rows; r++) { 
    for (c=0; c<dsp_columns; c++) {
			a=dspbuffer[r][c];
			b=dspbuffer[r+scroll_rows][c];
			if ( a != b ) {
  			if (b != 0) { 
#ifdef DISPLAYHASCOLOR
            dspsavepen();
            dspsetfgcolor((b >> 8) & 15);
            dspprintchar(b & charmask, c, r); 
            dsprestorepen();
#else
  			    dspprintchar(b, c, r); 
#endif     
  			} else {
  			  dspprintchar(' ', c, r);
  			}
      }      
			dspbuffer[r][c]=b;
		} 
	}

/* delete the characters in the remaining lines */
	for (r=dsp_rows-scroll_rows; r<dsp_rows; r++) {
		for (c=0; c<dsp_columns; c++) {
			if (dspbuffer[r][c] != 0 && ( dspbuffer[r][c]) != 32) dspprintchar(' ', c, r); 
			dspbuffer[r][c]=0;     
    }
	}
  
/* set the cursor to the first free line	*/ 
  dspmycol=0;
	dspmyrow=dsp_rows-scroll_rows;
}

/* do the reverse scroll only one line implemented */
void dspreversescroll(mem_t line){
  mem_t r,c;
  dspbuffer_t a,b;
  
/* scroll data up and redraw the display */
  for (r=dsp_rows; r>line; r--) { 
    for (c=0; c<dsp_columns; c++) {
      a=dspbuffer[r][c];
      b=dspbuffer[r-1][c];
      if ( a != b ) {
        if (b != 0) { 
#ifdef DISPLAYHASCOLOR
            dspsavepen();
            dspsetfgcolor((b >> 8) & 15);
            dspprintchar(b & charmask, c, r); 
            dsprestorepen();
#else
            dspprintchar(b, c, r); 
#endif 
        } else {
          dspprintchar(' ', c, r);
        }
      }      
      dspbuffer[r][c]=b;
    } 
  }

/* delete the characters in the remaining line */
  for (c=0; c<dsp_columns; c++) {
    if (dspbuffer[line][c] != 0 && dspbuffer[line][c] != 32) dspprintchar(' ', c, r); 
    dspbuffer[line][c]=0;     
  }
 
/* set the cursor to the free line  */ 
  dspmyrow=line;
}

/* again a break in API, using inch here */
char dspwaitonscroll() {
  char c;

	if ( dspscrollmode == 1 ) {
		if (dspmyrow == dsp_rows-1) {
      c=inch();
      if (c == ' ') dspwrite(12);
		  return c;
		}
	}
	return 0;
}

/* code for low memory simple display access */
#else 
/* buffer access functions */
char dspget(address_t i) { return 0; }

char dspgetrc(mem_t r, mem_t c) { return 0; }

char dspgetc(mem_t c) { return 0; }

void dspsetxy(char ch, mem_t c, mem_t r) {
  if (ch != 0) dspprintchar(ch, c, r); else dspprintchar(' ', c, r);
}

void dspset(address_t i, char ch) {
  mem_t c=i%dsp_columns;
  mem_t r=i/dsp_columns;
  dspsetxy(ch, c, r);
}

/* unbuffered display, the only thing we can do is go home */
void dspbufferclear() {
  dspmyrow=0;
  dspmycol=0;
}

char dspwaitonscroll() { return 0; }

/* a stub for dspwrite */
void dspscroll(mem_t scroll_rows, mem_t scroll_top=0){
  dspmyrow=dsp_rows-1;
}

void dspsetscrollmode(char c, short l) {}
void dspreversescroll(mem_t a){}
#endif

/* the generic write function for displays with and without buffers */

void dspwrite(char c){
mem_t dspmycolt;

/* on escape call the vt52 state engine */
#ifdef HASVT52
  if (dspesc) { 
    dspvt52(&c); 
  }
#endif

/* do we print ? */
#ifdef ARDUINOPRT
  if (dspprintmode) {
    prtwrite(c);
    if (sendcr && c == 10) prtwrite(13); /* some printers want cr */
    if (dspprintmode == 2) return; /* do not print in mode 2 */
  }
#endif 
  
  switch(c) {
    case 0:
      return;
    case 7: // bell just a stub
      dspbell();
      return;
    case 9: // tab 
      dspmycolt = dspmycol/8;
      if ((dspmycolt+1)*8<dsp_columns-1) dspmycol=(dspmycolt+1)*8;
      return;
    case 10: // this is LF Unix style doing also a CR
      dspmyrow++;
      if (dspmyrow >= dsp_rows) dspscroll(dsp_scroll_rows);  
      dspmycol=0;
      if (dspupdatemode == 1) dspupdate();
      return;
    case 11: // vertical tab - converted to line feed without carriage return
      if (dspmyrow < dsp_rows-1) dspmyrow++;
      return; 
    case 12: // form feed is clear screen plus home
      dspbufferclear();
      dspclear();
      return;
    case 13: // classical carriage return, no form feed 
      dspmycol=0;
      return;
    case 27: // escape - initiate vtxxx mode
      dspesc=1;
      return;
    case 28: // cursor back - this is what terminal applications send for cursor back
      if (dspmycol > 0) dspmycol--;
      return;
    case 29: // cursor forward - this is what terminal applications send for cursor back
      if (dspmycol < dsp_columns-1) dspmycol++;
      return;
    case 8:   // back space is delete the moment
    case 127: // delete
      if (dspmycol > 0) {
        dspmycol--;
        dspsetxy(0, dspmycol, dspmyrow);
      }
      return;
    case 2: // we abuse start of text as a home sequence, may also be needed for Epaper later
      dspmycol=dspmyrow=0;
      return;
    case 3: //  ETX = Update display for buffered display like Epaper 
      dspupdate();
      return;
    default: // eliminate all non printables - problematic for LCD special character
      if (c<32) return;
      break;
  }
  
  dspsetxy(c, dspmycol, dspmyrow);
  dspmycol++;
  if (dspmycol == dsp_columns) {
    if (!dspwrap) { /* we simply ignore the cursor */
      dspmycol=0;
      dspmyrow=(dspmyrow + 1);
    }
    if (dspmyrow >= dsp_rows) dspscroll(dsp_scroll_rows); 
  }
  if (dspupdatemode == 0) dspupdate();
}

/* 
 * This is the minimalistic VT52 state engine. It is an interface to 
 * process single byte control sequences of the form <ESC> char 
 */
 
#ifdef HASVT52
/* the state variable */
char vt52s = 0;

/* the graphics mode mode - unused so far */
mem_t vt52graph = 0;

/* the secondary cursor */
mem_t vt52mycol = 0;
mem_t vt52myrow = 0;

/* temp variables for column and row , do them here and not in the case: guess why */
mem_t vt52tmpr;
mem_t vt52tmpc;

/* an output buffer for the vt52 terminal */
const mem_t vt52buffersize = 4; 
char vt52outbuffer[vt52buffersize] = { 0, 0, 0, 0 };
mem_t vt52bi = 0;
mem_t vt52bj = 0;

/* the reader from the buffer */
char vt52read() {
  if (vt52bi<=vt52bj) { vt52bi = 0; vt52bj = 0; } /* empty, reset */
  if (vt52bi>vt52bj) return vt52outbuffer[vt52bj++];
  return 0;
}

/* the avail from the buffer */
mem_t vt52avail() { return vt52bi-vt52bj; }

/* putting something into the buffer */
void vt52push(char c) {
  if (vt52bi < vt52buffersize) vt52outbuffer[vt52bi++]=c; 
}

/* clear the buffer */
void vt52clear() {
  vt52bi=0;
}

/* something little */
uint8_t vt52number(char c) {
  uint8_t b=c;
  if (b>31) return b-32; else return 0;
}

/*
 * The VT52 data registers for the graphics and wiring extension.
 * x,y are 14 bit and z is 7bit. Data is transferred in VT52 style
 * -> numerical value plus 32 to map the data to printable characters
 * Access is done through the the ESC x, ESC y and ESC z sequences:
 * ESC x #1 #2 
 * sets the xregister to (#1-32)+(#2-32)*128
 */

#if defined(DISPLAYHASGRAPH) || defined(VT52WIRING)
#define VT52HASREGISTERS
/* the three register variables */
uint16_t vt52regx = 0;
uint16_t vt52regy = 0;
uint8_t  vt52regz = 0;

/* one argument cache for two byte arguments */
uint8_t vt52arg = 0;
#endif

/* 
 * graphics code in VT52, if you want to control graphics from the character stream 
 * The ESC g <ch> sequence sends a graphics command as the second byte after the g
 * 
 * Valid values for g are
 *  s: set the graphics cursor
 *  p: plot a point
 *  l: draw a line
 *  L: draw a line and move the cursor to the endpoint
 *  r: draw a rectangle
 *  R: draw a filled rectangle
 *  c: draw a circle
 *  C: draw a filled circle
 * 
 */
#ifdef DISPLAYHASGRAPH
/* the grahics cursor of VT52 */
uint16_t vt52graphx = 0;
uint16_t vt52graphy = 0;

/* execute one graphics command */
void vt52graphcommand(uint8_t c) {
  switch(c) {
    case 's': /* set the cursor */
      vt52graphx=vt52regx;
      vt52graphy=vt52regy;
      break;
    case 'p': /* plot a point at the cursor */
      plot(vt52graphx, vt52graphy);
      break;
    case 'l': /* plot a line */
      line(vt52graphx, vt52graphy, vt52regx, vt52regy);
      break;
    case 'L': /* plot a line and update the cursor, needed for drawing shapes */
      line(vt52graphx, vt52graphy, vt52regx, vt52regy);
      vt52graphx=vt52regx;
      vt52graphy=vt52regy;
      break;
    case 'r': /* plot a rectangle */
      rect(vt52graphx, vt52graphy, vt52regx, vt52regy);
      break;
    case 'c': /* plot a circle */
      circle(vt52graphx, vt52graphy, vt52regx);
      break;
    case 'R': /* plot a filled rectangle */
      frect(vt52graphx, vt52graphy, vt52regx, vt52regy);
      break;
    case 'C': /* plot a filled circle */
      fcircle(vt52graphx, vt52graphy, vt52regx);
      break;
  }  
}
#endif

/* 
 * this is an odd part of the vt52 code with this, the terminal 
 * can control the digital and analog pins.
 * it is meant for situations where the terminal is controlled by a (powerful)
 * device with no or very few I/O pins. It can use the pins of the Arduino through  
 * the terminal. This works as long as everything stays within the terminals timescale
 * On a 9600 baud interface, the character processing time is 1ms, everything slower 
 * than approximately 10ms can be done through the serial line.
 */

#ifdef VT52WIRING
#define VT52HASREGISTERS
  void vt52wiringcommand(uint8_t c) {
    switch(c) { 
      case 'p': /* pinMode z */
        pinMode(vt52regz);
        break;
      case 'l': /* digital write low pin z*/
        digitalWrite(vt52regz, LOW);
        break;
      case 'h': /* digital write high pin z*/
        digitalWrite(vt52regz, HIGH);
        break;
      case 'r': /* digital read from pin z */
        vt52push(digitalRead(vt52regz)+32);
        break;      
      case 'a': /* analog write pin z to value x */
        analogWrite(vt52regz, vt52regx);
        break;
      case 'A': /* analog read from pin z */
        break;
      case 't': /* tone at pin z, frequency x, duration y */
        tone(vt52regz, vt52regx, vt52regy);
        break;
    }  
  }
#endif


/* vt52 state engine */
void dspvt52(char* c){
  
/* reading and processing multi byte commands */
  switch (vt52s) {
    case 'Y':
      if (dspesc == 2) { 
        dspsetcursory(vt52number(*c));
        dspesc=1; 
        *c=0;
        return;
      }
      if (dspesc == 1) { 
        dspsetcursorx(vt52number(*c)); 
        *c=0; 
      }
      vt52s=0; 
      break;
    case 'b':
      dspsetfgcolor(vt52number(*c));
      *c=0;
      vt52s=0;
      break;
    case 'c':
      dspsetbgcolor(vt52number(*c));
      *c=0;
      vt52s=0;
      break;
#ifdef VT52HASREGISTERS
    case 'x':
      if (dspesc == 2) { 
        vt52arg=vt52number(*c);
        dspesc=1; 
        *c=0;
        return;
      }
      if (dspesc == 1) { 
        vt52regx=vt52arg+vt52number(*c)*128;
        *c=0; 
      }
      vt52s=0; 
      break;
    case 'y':
      if (dspesc == 2) { 
        vt52arg=vt52number(*c);
        dspesc=1; 
        *c=0;
        return;
      }
      if (dspesc == 1) { 
        vt52regy=vt52arg+vt52number(*c)*127;
        *c=0; 
      }
      vt52s=0; 
      break;
    case 'z':
      vt52regz=vt52number(*c);
      *c=0;
      vt52s=0;
      break;
#endif
#ifdef DISPLAYHASGRAPH
    case 'g':
      vt52graphcommand(*c);
      *c=0;
      vt52s=0;
      break;      
#endif
#ifdef VT52WIRING
    case 'a':
      vt52wiringcommand(*c);
      *c=0;
      vt52s=0;
      break; 
#endif
  }
 
/* commands of the terminal in text mode */
  
  switch (*c) {
    case 'v': /* GEMDOS / TOS extension enable wrap */
      dspwrap=0;
      break;
    case 'w': /* GEMDOS / TOS extension disable wrap */
      dspwrap=1;
      break;
    case '^': /* Printer extensions - print on */
      dspprintmode=1;
      break;
    case '_': /* Printer extensions - print off */
      dspprintmode=0;
      break;
    case 'W': /* Printer extensions - print without display on */
      dspprintmode=2;
      break;
    case 'X': /* Printer extensions - print without display off */
      dspprintmode=0;
      break;
    case 'V': /* Printer extensions - print cursor line */
#if defined(ARDUINOPRT) && defined(DISPLAYCANSCROLL)
      for (mem_t i=0; i<dsp_columns; i++) prtwrite(dspgetc(i));
#endif
      break;
    case ']': /* Printer extension - print screen */
#if defined(ARDUINOPRT) && defined(DISPLAYCANSCROLL)
      for (mem_t j=0; j<dsp_rows; j++)
        for (mem_t i=0; i<dsp_columns; i++) prtwrite(dspgetrc(j, i));
#endif
      break;
    case 'F': /* enter graphics mode */
      vt52graph=1;
      break;
    case 'G': /* exit graphics mode */
      vt52graph=0;
      break;
    case 'Z': // Ident 
      vt52clear();
      vt52push(27);
      vt52push('/');
#ifndef ARDUINOPRT
      vt52push('K');
#else
      vt52push('L');
#endif
      break;
    case '=': // alternate keypad on 
    case '>': // alternate keypad off 
      break;
    case 'b': // GEMDOS / TOS extension text color 
    case 'c': // GEMDOS / TOS extension background color 
      vt52s=*c;
      dspesc=1;
      *c=0;
      return;
    case 'e': // GEMDOS / TOS extension enable cursor
      dspsetcursor(1);
      break;
    case 'f': // GEMDOS / TOS extension disable cursor 
      dspsetcursor(0);
      break;
    case 'p': // GEMDOS / TOS extension reverse video 
      dspsetreverse(1);
      break;
    case 'q': // GEMDOS / TOS extension normal video 
      dspsetreverse(0);
      break;
    case 'A': // cursor up
      if (dspmyrow>0) dspmyrow--;
      break;
    case 'B': // cursor down 
      if (dspmyrow < dsp_rows-1) dspmyrow++;
      break;
    case 'C': // cursor right 
      if (dspmycol < dsp_columns-1) dspmycol++;
      break; 
    case 'D': // cursor left 
      if (dspmycol>0) dspmycol--;
      break;
    case 'E': // GEMDOS / TOS extension clear screen 
      dspbufferclear();
      dspclear();
      break;
    case 'H': // cursor home 
      dspmyrow=dspmycol=0;
      break;  
    case 'Y': // Set cursor position 
      vt52s='Y';
      dspesc=2;
      *c=0;
      return;
    case 'J': // clear to end of screen 
      for (int i=dspmycol+dsp_columns*dspmyrow; i<dsp_columns*dsp_rows; i++) dspset(i, 0);
      break;
    case 'd': // GEMDOS / TOS extension clear to start of screen 
      for (int i=0; i<dspmycol+dsp_columns*dspmyrow; i++) dspset(i, 0);
      break;
    case 'K': // clear to the end of line
      for (mem_t i=dspmycol; i<dsp_columns; i++) dspsetxy(0, i, dspmyrow);
      break;
    case 'l': // GEMDOS / TOS extension clear line 
      for (mem_t i=0; i<dsp_columns; i++) dspsetxy(0, i, dspmyrow);
      break;
    case 'o': // GEMDOS / TOS extension clear to start of line
      for (mem_t i=0; i<=dspmycol; i++) dspsetxy(0, i, dspmyrow);
      break;
    case 'k': // GEMDOS / TOS extension restore cursor
      dspmycol=vt52mycol;
      dspmyrow=vt52myrow;
      break;
    case 'j': // GEMDOS / TOS extension save cursor
      vt52mycol=dspmycol;
      vt52myrow=dspmyrow;
      break;
    case 'I': // reverse line feed
      if (dspmyrow>0) dspmyrow--; else dspreversescroll(0);
      break;
    case 'L': // Insert line
      dspreversescroll(dspmyrow);
      break;
    case 'M': 
      vt52tmpr = dspmyrow;
      vt52tmpc = dspmycol;
      dspscroll(1, dspmyrow);
      dspmyrow=vt52tmpr;
      dspmycol=vt52tmpc;
      break;
#ifdef VT52REGISTERS
    case 'x': // set the x register 
    case 'y': // set the y register 
      vt52s=*c;
      dspesc=2;
      *c=0;
      return;
    case 'z': // set the z register
      vt52s=*c;
      dspesc=1;
      *c=0;
      return;
      break;
#endif   
#ifdef DISPLAYHASGRAPH 
    case 'g': // execute a graphics command 
      vt52s=*c;
      dspesc=1;
      *c=0;
      return;
      break;
#endif
#ifdef VT52WIRING
    case 'a':
      vt52s=*c;
      dspesc=1;
      *c=0;
      return;
      break;
#endif
  }
  dspesc=0;
  *c=0;
}
#endif

#else
#ifdef GRAPHDISPLAYDRIVER
/* a super simple display driver for graphics only systems, only handles drawing 
  and graphics cursor stuff, experimental for epapers */
#undef HASVT52
#define GBUFFERSIZE 80
static char gbuffer[GBUFFERSIZE];
void dspouts(char* s, address_t l) {
  int i;
  for (i=0; i<l-1 && i<GBUFFERSIZE-1; i++) gbuffer[i]=s[i];
  gbuffer[i]=0;
  dspprintstring(gbuffer);
}
/* single character handling is triggered by PUT, it controls cursor movement */
void dspwrite(char c) {
  switch(c) {
    case 0:
      return;
    case 10: // this is LF Unix style doing also a CR
      dspgraphcursor_x=0;
    case 11: // vertical tab - converted to line feed without carriage return
      dspgraphcursor_y+=dspfontsize;
      return; 
    case 12: // form feed is clear screen plus home
      dspclear();
    case 2: // we abuse start of text as a home sequence, may also be needed for Epaper later
      dspgraphcursor_x=0;
      dspgraphcursor_y=dspfontsize;
      return;
    case 13: // classical carriage return, no form feed, we stay in the same line
      dspgraphcursor_x=0;
      return;
    default: // eliminate all non printables
      if (c<32) return;
      break;
  }
  dspouts(&c, 1);
}
int dspstat(char c) { return 0; }
int dspgetcursorx() { return dspgraphcursor_x; }
int dspgetcursory() { return dspgraphcursor_y; }
void dspsetcursorx(int v) { dspgraphcursor_x = v; }
void dspsetcursory(int v) { dspgraphcursor_y = v; } 
char dspwaitonscroll() { return 0; };
char dspactive() {return 1; }
void dspsetupdatemode(char c) {}
#else
/* stubs for systems without a display driver, BASIC uses these functions */
const int dsp_rows=0;
const int dsp_columns=0;
void dspsetupdatemode(char c) {}
void dspwrite(char c){};
void dspbegin() {};
int dspstat(char c) {return 0; }
char dspwaitonscroll() { return 0; };
char dspactive() {return 0; }
void dspsetscrollmode(char c, mem_t l) {}
void dspscroll(mem_t a, mem_t b){}
char vt52read() { return 0; }
mem_t vt52avail() { return 0; }
#endif
#endif

/* 
 * Arduino Real Time clock. The interface here offers the values as number_t 
 * combining all values. 
 * 
 * The code does not use an RTC library any more all the rtc support is 
 * builtin now. 
 * 
 * A clock must activate the macro #define HASCLOCK to make the clock 
 * available in BASIC.
 * 
 * Four software models are supported
 *  - Built-in clocks of STM32, MKR, and ESP32 are supported by default
 *  - I2C clocks can be activated: DS1307, DS3231, and DS3232 
 *  - A Real Time Clock emulation is possible using millis()
 * 
 * rtcget accesses the internal registers of the clock. 
 * Registers 0-6 are bcd transformed to return 
 * seconds, minutes, hours, day of week, day, month, year
 * 
 * On I2C clocks registers 7-255 are returned as memory cells
 */

#ifdef ARDUINORTC
#define HASCLOCK
/*
 * I2C clocks DS1307, DS3231, and DS3232 using Wire directly
 * no buffering of values, access directly to the clock registers
 */

/* No begin method needed */
void rtcbegin() {}

/* get the time from the registers */
short rtcget(short i) {
  
    /* set the address of the read */
    Wire.beginTransmission(RTCI2CADDR);
    Wire.write(i);
    Wire.endTransmission();

    /* now read */
    Wire.requestFrom(RTCI2CADDR, 1);
    uint8_t v=Wire.read();
    
    switch (i) {
    case 0: 
    case 1:
      return (v & 0b00001111)+((v >> 4) & 0b00000111) * 10;      
    case 2:
      return (v & 0b00001111)+((v >> 4) & 0b00000011) * 10 ; /* only 24 hour support */
    case 3:
      return (v & 0b00001111);
    case 4:
      return (v & 0b00001111) + ((v >> 4) & 0b00000011) * 10;
    case 5:
      return (v & 0b00001111) + ((v >> 4) & 0b00000001) * 10;
    case 6:
      return (v & 0b00001111) + (v >> 4) * 10;
    default:
      return v;
   }
}

/* set the registers */
void rtcset(uint8_t i, short v) { 
  uint8_t b;

  /* to bcd if we deal with a clock byte (0-6) */
  if (i<7) b=(v%10 + ((v/10) << 4)); else b=v;
   
  switch (i) {
    case 0: 
    case 1:  
      b = b & 0b01111111; 
      break; 
    case 2:
    case 4:
      b = b & 0b00111111; /* only 24 hour support */
      break;
    case 3:
      b = b & 0b00000111;
      break;
    case 5:
      b = b & 0b00011111;
   }

/* send the address and then the byte */
   Wire.beginTransmission(RTCI2CADDR);
   Wire.write(i);
   Wire.write(b);
   Wire.endTransmission(); 
}
#else 
#if defined(HASBUILTINRTC) 
#define HASCLOCK
/*
 * Built-in clocks of STM32 and MKR are based on the RTCZero interface
 * an rtc object is created after loading the libraries
 */

/* begin method */
void rtcbegin() {
  rtc.begin(); /* 24 hours mode */
}

/* get the time */
short rtcget(short i) { 
    switch (i) {
    case 0: 
      return rtc.getSeconds();
    case 1:
      return rtc.getMinutes();      
    case 2:
      return rtc.getHours();
    case 3:
      // return rtc.getWeekDay();
      return 0;
    case 4:
      return rtc.getDay();
    case 5:
      return rtc.getMonth();
    case 6:
      return rtc.getYear();
    default:
      return 0;
   }
}

/* set the time */
void rtcset(uint8_t i, short v) { 
  switch (i) {
    case 0: 
      rtc.setSeconds(v);
      break;
    case 1: 
      rtc.setMinutes(v);
      break;    
    case 2:
      rtc.setHours(v);
      break;
    case 3:
      // rtc.setWeekDay(v);
      break;
    case 4:
      rtc.setDay(v);
      break;
    case 5:
      rtc.setMonth(v);
      break;
    case 6:
      rtc.setYear(v);
      break;
    default:
      return; 
   }
}
#else
#ifdef ARDUINORTCEMULATION
#define HASCLOCK
/* 
 * A millis() based real time clock emulation. It creates a 32bit Unix time 
 * variable and adds millis()/1000 
 */

/* the begin method - standard interface to BASIC*/
void rtcbegin() {}

/* the current unix time - initialized to the begin of the epoch */
long rtcutime = 0; 

/* the offset of millis()/1000 when we last set the clock */
long rtcutimeoffset = 0;

/* the current unix date - initialized to the begin of the epoch */
struct { uint8_t second; uint8_t minute; uint8_t hour; uint8_t weekday; uint8_t day; uint8_t month; uint16_t year; } 
  rtctime = { 0, 0, 0, 4, 1, 1, 1970 };

/* the number of days per month */
const int rtcmonthdays[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};


/* convert the time to a unix time number from https://de.wikipedia.org/wiki/Unixzeit  */
void rtctimetoutime() {
  int leapyear = ((rtctime.year-1)-1968)/4 - ((rtctime.year-1)-1900)/100 + ((rtctime.year-1)-1600)/400;
  long epochdays = (rtctime.year-1970)*365 + leapyear + rtcmonthdays[rtctime.month-1] + rtctime.day - 1;
  if ((rtctime.month > 2) && (rtctime.year%4 == 0 && (rtctime.year%100 != 0 || rtctime.year%400 == 0))) epochdays+=1;
  rtcutime = rtctime.second + 60*(rtctime.minute + 60*(rtctime.hour + 24*epochdays));
}

/* convert the unix time to time and date from https://de.wikipedia.org/wiki/Unixzeit */
void rtcutimetotime() {
  const unsigned long int secondsperday   =  86400ul; /*  24* 60 * 60 */
  const unsigned long int daysperyear     =    365ul; /* no leap year */
  const unsigned long int daysinfoury     =   1461ul; /*   4*365 +   1 */
  const unsigned long int daysinhundredy  =  36524ul; /* 100*365 +  25 - 1 */
  const unsigned long int daysinfourhundredy = 146097ul; /* 400*365 + 100 - 4 + 1 */
  const unsigned long int daynumberzero      = 719468ul; /* day number of March, 1 1970 */
  
  unsigned long int daynumber = daynumberzero + rtcutime/secondsperday;
  unsigned long int secondssincemidnight = rtcutime%secondsperday;
  unsigned long int temp;

  /* the weekday is based in the daynumber since March, 1, 1970 a SUNDAY */
  rtctime.weekday = daynumber % 7;

  /* leap years of the Gregorian calendar */
  temp = 4 * (daynumber + daysinhundredy + 1) / daysinfourhundredy - 1;
  rtctime.year = 100 * temp;
  daynumber -= daysinhundredy * temp + temp / 4;

  /* leap years of the Julian calendar */
  temp = 4 * (daynumber + daysperyear + 1) / daysinfoury - 1;
  rtctime.year += temp;
  daynumber -= daysperyear * temp + temp / 4;

  /* calculate day and month, reference March 1 */
  rtctime.month = (5 * daynumber + 2) / 153;
  rtctime.day = daynumber - (rtctime.month * 153 + 2) / 5 + 1;

  /* recalulate to a normal year */
  rtctime.month += 3;
  if (rtctime.month > 12) {
    rtctime.month -= 12;
    rtctime.year++;
  }

  /* calculate hours, months, seconds */
    rtctime.hour  = secondssincemidnight / 3600;
    rtctime.minute = secondssincemidnight % 3600 / 60;
    rtctime.second = secondssincemidnight % 60;
}

/* get the time elements -> standard interface to BASIC */
short rtcget(short i) { 
/* add to the last time we set the clock and subtract the offset*/
  rtcutime = millis()/1000 + rtcutimeoffset;
/* calulate the time data */
  rtcutimetotime();

  switch (i) {
    case 0: 
      return rtctime.second;
    case 1:
      return rtctime.minute;      
    case 2:
      return rtctime.hour;
    case 3:
      return rtctime.weekday;
    case 4:
      return rtctime.day;
    case 5:
      return rtctime.month;
    case 6:
      return rtctime.year;
    default:
      return 0;
   }
}

/* set the time elements -> standard interface to BASIC */
void rtcset(uint8_t i, short v) {
/* how much time has elapsed since we last set the clock */
  rtcutime = millis()/1000 + rtcutimeoffset;

/* generate the time structure */
  rtcutimetotime();

/* set the clock */
  switch (i) {
    case 0: 
      if (v>=0 && v<60) rtctime.second=v;
      break;
    case 1:
      if (v>=0 && v<60) rtctime.minute=v;
      break;     
    case 2:
      if (v>=0 && v<24) rtctime.hour=v;
      break;
    case 3:
      if (v>=0 && v<7) rtctime.weekday=v;
      break;
    case 4:
      if (v>0 && v<32) rtctime.day=v;
      break;
    case 5:
      if (v>0 && v<13) rtctime.month=v;
      break;
    case 6:
      if (v>=1970 && v<2100) rtctime.year=v;
      break;
    default:
      return;
   }

 /* recalulate the right offset by first finding the second value of the new date*/
  rtctimetoutime();
  
/* remember when we set the clock */
  rtcutimeoffset = rtcutime - millis()/1000;  
}
#else
#if defined(ARDUINO_ARCH_ESP32)
#define HASCLOCK
/* 
 * On ESP32 we use the builtin clock, this is a generic Unix time mechanism equivalent
 * to the code in hardware-posix.h
 */

/* no begin needed */
void rtcbegin() {}

/* get the time */
short rtcget(short i) { 
  struct tm rtctime;
  time_t now;
  time(&now);
  localtime_r(&now, &rtctime);

  switch (i) {
    case 0: 
      return rtctime.tm_sec;
    case 1:
      return rtctime.tm_min;      
    case 2:
      return rtctime.tm_hour;
    case 3:
      return rtctime.tm_wday;
    case 4:
      return rtctime.tm_mday;
    case 5:
      return rtctime.tm_mon+1;
    case 6:
      if (rtctime.tm_year > 100) return rtctime.tm_year-100; else return rtctime.tm_year;
    default:
      return 0;
  }

  return 0; 
}

/* set the time */
void rtcset(uint8_t i, short v) { 
  struct tm rtctime;
  struct timeval tv;

  /* get the time stucture from the system */
  time_t now;
  time(&now);
  localtime_r(&now, &rtctime);

  /* change what needs to be changed */
  switch (i) {
    case 0: 
      rtctime.tm_sec = v%60;
      break;
    case 1:
      rtctime.tm_min = v%60; 
      break;     
    case 2:
      rtctime.tm_hour = v%24;
      break;
    case 3:
      rtctime.tm_wday = v%7;
      break;
    case 4:
      rtctime.tm_mday = v;
      break;
    case 5:
      rtctime.tm_mon = v-1;
      break;
    case 6:
      if (v > 1900) v=v-1900; /* get years to the right value */
      if (v < 50) v=v+100; 
      rtctime.tm_year = v;
      break;
  }

  /* calculate the seconds and put it back*/
  time_t epocht = mktime(&rtctime);
  if (epocht > 2082758399){
    tv.tv_sec = epocht - 2082758399;  
  } else {
    tv.tv_sec = epocht;  
  }
  tv.tv_usec = 0;
  settimeofday(&tv, NULL);
}
#else
/* no clock at all, no operations */
void rtcbegin() {}
short rtcget(short i) { return 0; }
void rtcset(uint8_t i, short v) { }
#endif
#endif
#endif
#endif

/* 
 * External EEPROM is handled through an EFS filesystem object  
 * see https://github.com/slviajero/EepromFS 
 * for details. Here the most common parameters are set as a default.
*/
#ifdef ARDUINOEFS
#undef ARDUINOI2CEEPROM
#ifndef EFSEEPROMADDR
#define EFSEEPROMADDR 0x50
#endif
#ifdef EFSEEPROMSIZE
EepromFS EFS(EFSEEPROMADDR, EFSEEPROMSIZE);
#else
EepromFS EFS(EFSEEPROMADDR);
#endif
#endif

/* 
 * External EEPROM is handled through an EFS filesystem object in raw mode
 * see https://github.com/slviajero/EepromFS 
 * for details. Here the most common parameters are set as a default.
*/

#if defined(ARDUINOI2CEEPROM) && defined(ARDUINOI2CEEPROM_BUFFERED)
#ifndef I2CEEPROMADDR
#define I2CEEPROMADDR 0x50
#endif
#ifdef I2CEEPROMSIZE
EepromFS EFSRAW(I2CEEPROMADDR, I2CEEPROMSIZE);
#else
EepromFS EFSRAW(I2CEEPROMADDR);
#endif
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
 * Default is right now that Wifi is started at boot
 * This may change in the future.
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
	WiFi.setAutoReconnect(1);
#endif
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD)
	WiFi.begin(ssid, password);
#endif
#endif
}

/*
 * network stop methode, needed sometime to reinit networking
 * completely or to save power
 * 
 */
void netstop() {
#ifdef ARDUINOETH
/* to be done */
#else  
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
  WiFi.mode(WIFI_OFF);
#endif
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD)
  WiFi.end();
#endif
#endif
}

/*
 * network reconnect methode
 * 
 */
void netreconnect() {
#ifdef ARDUINOETH
/* */
#else 
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_ESP8266)
  WiFi.reconnect(); 
  bdelay(1000); 
#elif defined(ARDUINO_ARCH_SAMD)
  WiFi.end();
  WiFi.begin(ssid, password);
  bdelay(1000);
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

/* the interface to the usr function */
int mqttstat(char c) {
#if defined(ARDUINOMQTT)
  if (c == 0) return 1;
  if (c == 1) return mqttstate();
#endif
  return 0; 
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
	if (bmqtt.connected()) return 1;

/* try to reconnect the network */
  if (!netconnected()) { netreconnect(); bdelay(5000); }
  if (!netconnected()) return 0;
	
/* create a new random name right now */
	mqttsetname();

/* try to reconnect assuming that the network is connected */
	while (!bmqtt.connected() && timer < 400) {
		bmqtt.connect(mqttname);
		bdelay(timer);
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
	short i;

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
	short i=0;

	while(mqtt_charsforsend < MQTTBLENGTH && i<l) mqtt_obuffer[mqtt_charsforsend++]=m[i++];
	if (mqtt_obuffer[mqtt_charsforsend-1] == '\n' || mqtt_charsforsend > MQTTBLENGTH) {
		if (!mqttreconnect()) {ert=1; return;};
		if (!bmqtt.publish(mqtt_otopic, (uint8_t*) mqtt_obuffer, (unsigned int) mqtt_charsforsend-1, false)) ert=1;
		mqtt_charsforsend=0;
	}
} 

/* 
 * ins copies the buffer into a basic string 
 *  - behold the jabberwock - length gynmastics 
 * z.a has to be the loop variable and contain the length after this -> intended side effect
 */
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
/* this is the EEPROM dummy */
#if (defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) ) && defined(ARDUINOEEPROM)
  EEPROM.begin(EEPROMSIZE);
#endif
#if (defined(ARDUINO_ARCH_STM32)) && defined(ARDUINOEEPROM)
  eeprom_buffer_fill();
#endif
#if (defined(ARDUINO_ARCH_XMC)) && defined(ARDUINOEEPROM)
  EEPROM.begin();
#endif
/* an unbuffered EEPROM, typically used to store a program */
#if defined(ARDUINOI2CEEPROM) && !defined(ARDUINOI2CEEPROM_BUFFERED)
/* 
 * we test the size with a heuristic, but beware, EEPROM addressing
 * is circular. We only consider the two most common sizes, 4096 and 32768.
 * This is tricky.
 */

#ifndef I2CEEPROMSIZE
  int c4=eread(4094);
  int c32=eread(32766);
  eupdate(4094, 42);
  eupdate(32766, 84);
  if (ert !=0 ) return;
  if (eread(32766) == 84 && eread(4094) == 42) i2ceepromsize = 32767; else i2ceepromsize = 4096;
  eupdate(4094, c4);
  eupdate(32766, c32);  
#else
  i2ceepromsize = I2CEEPROMSIZE;
#endif
#endif
/* there also can be a raw EFS object */
#if defined(ARDUINOI2CEEPROM) && defined(ARDUINOI2CEEPROM_BUFFERED)
  EFSRAW.begin();
#ifndef I2CEEPROMSIZE
  i2ceepromsize = EFSRAW.esize();
#else
  i2ceepromsize = I2CEEPROMSIZE;
#endif
#endif
}

void eflush(){
/* code for the EEPROM dummy */
#if (defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_XMC) ) && defined(ARDUINOEEPROM) 
  EEPROM.commit();
#endif 
#if (defined(ARDUINO_ARCH_STM32)) && defined(ARDUINOEEPROM) 
  eeprom_buffer_flush();
#endif
/* flushing the I2C EEPROM */
#if defined(ARDUINOI2CEEPROM) && defined(ARDUINOI2CEEPROM_BUFFERED)
  EFSRAW.rawflush();
#endif
}

#if defined(ARDUINOEEPROM) && !defined(ARDUINOI2CEEPROM)
address_t elength() { 
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  return EEPROMSIZE;
#endif
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_XMC) || defined(ARDUINO_ARCH_STM32)
  return EEPROM.length(); 
#endif
#ifdef ARDUINO_ARCH_LGT8F 
  return EEPROM.length(); /* on newer LGT8 cores, older ones don't have this, set 512 instead */
#endif
  return 0;
}

void eupdate(address_t a, short c) { 
#if defined(ARDUINO_ARCH_ESP8266) ||defined(ARDUINO_ARCH_ESP32)|| defined(AARDUINO_ARCH_LGT8F) || defined(ARDUINO_ARCH_XMC)
  EEPROM.write(a, c);
#else
#if defined(ARDUINO_ARCH_STM32)
  eeprom_buffered_write_byte(a, c);
#else
  EEPROM.update(a, c); 
#endif
#endif
}

short eread(address_t a) { 
#ifdef ARDUINO_ARCH_STM32
  return (signed char) eeprom_buffered_read_byte(a); 
#else
  return (signed char) EEPROM.read(a); 
#endif
}
#else 
#if defined(ARDUINOI2CEEPROM)
address_t elength() { 
  return i2ceepromsize;
}

void eupdate(address_t a, short c) { 
#if defined(ARDUINOI2CEEPROM_BUFFERED)
  EFSRAW.rawwrite(a, c);
#else
/* go directly into the EEPROM */
  if (eread(a) != c) {
  /* set the address and send the byte*/
    Wire.beginTransmission(I2CEEPROMADDR);
    Wire.write((int)a/256);
    Wire.write((int)a%256);
    Wire.write((int)c);
    ert=Wire.endTransmission();
  /* wait the max time */
    bdelay(5);
  }
#endif
}

short eread(address_t a) { 
#ifdef ARDUINOI2CEEPROM_BUFFERED
  return (mem_t) EFSRAW.rawread(a); 
#else
/* read directly from the EEPROM */

/* set the address */
  Wire.beginTransmission(I2CEEPROMADDR);
  Wire.write((int)a/256);
  Wire.write((int)a%256);
  ert=Wire.endTransmission();

/* read a byte */
  if (ert == 0) {
    Wire.requestFrom(I2CEEPROMADDR, 1);
    return (mem_t) Wire.read();
  } else return 0;
 
#endif
}
#else
/* no EEPROM present */
address_t elength() { return 0; }
void eupdate(address_t a, short c) { return; }
short eread(address_t a) { return 0; }
#endif
#endif

/* 
 *	the wrappers of the arduino io functions
 *  awrite requires ESP32 2.0.2 core, else disable awrite() 
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

/* we normalize the pinMode as ESP32, ESP8266, and other boards behave rather
 *  differently. Following Arduino conventions we map 0 always to INPUT  
 *  and 1 always to OUTPUT, all the other numbers are passed through to the HAL 
 *  layer of the platform.
 *  Example: OUTPUT on ESP32 is 3 and 1 is assigned to INPUT.
 *  XMC also needs special treatment here.
 */
void pinm(number_t p, number_t m){
  uint8_t ml = m;
  uint8_t pl = p;
  switch (ml) {
    case 0:
      pinMode(pl, INPUT);
      break;
    case 1:
      pinMode(pl, OUTPUT);
      break;
    default:
      pinMode(pl, ml);
      break;
  }
}

/* read a pulse, units given by bpulseunit - default 10 microseconds */
void bpulsein() { 
  unsigned long t, pt;
  t=((unsigned long) pop())*1000;
  y=pop(); 
  x=pop(); 
  pt=pulseIn(x, y, t)/bpulseunit; 
  push(pt);
}

/* write a pulse in microsecond range */
void bpulseout(short a) { 
  short pin, duration;
  short value = 1;
  short intervall = 0;
  short repetition = 1;
  if (a == 5) { intervall=pop(); repetition=pop(); }
  if (a > 2) value=pop();
  duration=pop();
  pin=pop();
  
  while (repetition--) {
    digitalWrite(pin, value);
    delayMicroseconds(duration*bpulseunit);
    digitalWrite(pin, !value);
    delayMicroseconds(intervall*bpulseunit);
  }
}

void btone(short a) {
  number_t d = 0;
  number_t v = 100;
  if (a == 4) v=pop();
  if (a >= 3) d=pop();
	x=pop();
	y=pop();
#if defined(ARDUINO_TTGO_T7_V14_Mini32) && defined(HASTONE)
/* fabGL soundgenerator code of suggestes by testerrossa
 * pin numbers below 128 are real arduino pins while 
 * pin numvers from 128 onwards are sound generator capabilities
 * this is different from the original code
 * 
 * Sound generator capabilities are numbered as follows
 * 128: Sine wave 
 * 129: Symmetric square wave 
 * 130: Sawtooth
 * 131: Triangle
 * 132: VIC noise
 * 133: noise
 *
 * 256-511: square wave with variable duty cycle
 * 
 */
  if (x == 0) {
    soundGenerator.play(false);
    soundGenerator.clear();
    return;
  } 
  if (a == 2) d=60000;
  if (y == 128) soundGenerator.playSound(SineWaveformGenerator(), x, d, v); 
  if (y == 129) soundGenerator.playSound(SquareWaveformGenerator(), x, d, v);
  if (y == 130) soundGenerator.playSound(SawtoothWaveformGenerator(), x, d, v);
  if (y == 131) soundGenerator.playSound(TriangleWaveformGenerator(), x, d, v); 
  if (y == 132) soundGenerator.playSound(VICNoiseGenerator(), x, d, v);
  if (y == 133) soundGenerator.playSound(NoiseWaveformGenerator(), x, d, v);
  if (y >= 255 && y < 512 ) {
      y=y-255;
      SquareWaveformGenerator sqw;
      sqw.setDutyCycle(y);
      soundGenerator.playSound(sqw, x, d, v); 
  }
  return;
#endif

#ifndef ARDUINOTONEEMULATION
  if (x == 0) {
    noTone(y);
  } else if (a == 2) {
		tone(y, x);
	} else {
		tone(y, x, d);
	}
#else 
  if (x == 0) {
    playtone(0, 0, 0);
  } else if (a == 2) {
    playtone(y, x, 32767);
  } else {
    playtone(y, x, d);
  }
#endif	
}

/* 
 *	the byield function is called after every statement
 *	it allows two levels of background tasks. 
 *
 *	BASICBGTASK controls if time for background tasks is 
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

/* everything that needs to be done often - 32 ms */
void yieldfunction() {
#ifdef ARDUINOMQTT
  bmqtt.loop();
#endif
#ifdef ARDUINOUSBKBD
  usb.Task();
#endif
#ifdef ARDUINOZX81KBD
  (void) keyboard.peek(); /* scan once and set lastkey properly every 32 ms */
#endif
/* this was delay(YIELDTIME) originally - removed now  */
}

/* everything that needs to be done not so often - 1 second */
void longyieldfunction() {
#ifdef ARDUINOETH
  Ethernet.maintain();
#endif 
}

void yieldschedule() {
/* delay(0) is only needed on ESP8266! it calls the scheduler - no bdelay here!! */
 #if defined(ARDUINO_ARCH_ESP8266)
  delay(0);
 #endif
 }

/* 
 *	The file system driver - all methods needed to support BASIC fs access
 * 	Different filesystems need different prefixes and fs objects, these 
 * 	filesystems use the stream API
 */
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(STM32SDIO)
File ifile;
File ofile;
char tempname[SBUFSIZE];
#if defined(ARDUINOSD) || defined(STM32SDIO)
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
#if defined(ARDUINO_ARCH_ESP32) || defined(ARDUINO_ARCH_STM32)
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

/* these filesystems may have a path prefixes, we treat STM32SDIO like an SD here */
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
 * 
 * if SDPIN is empty it is the standard SS pin of the platfrom
 * 
 */

static int fsbegins = -1;
 
void fsbegin(char v) {
#ifdef ARDUINOSD 
#ifndef SDPIN
#define SDPIN
#endif
  if (SD.begin(SDPIN) && v) outsc("SDcard ok \n"); else outsc("SDcard not ok \n");	
#endif
#ifdef STM32SDIO
  int i = 1;
  while (i<100) {
    if (SD.begin(SD_DETECT_PIN)) {fsbegins=i; break; } else fsbegins=0; 
    bdelay(20); 
    i++;
  } 
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

int fsstat(char c) { 
#if defined(FILESYSTEMDRIVER)
  if (c == 0) return 1;
#endif
  return 0;
}

/*
 *	File I/O function on an Arduino
 * 
 * filewrite(), fileread(), fileavailable() as byte access
 * open and close is handled separately by (i/o)file(open/close)
 * only one file can be open for write and read at the same time
 */
void filewrite(char c) {
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(STM32SDIO)
	if (ofile) ofile.write(c); else ert=1;
#endif
#if defined(RP2040LITTLEFS)
	if (ofile) fputc(c, ofile); else ert=1;
#endif
#if defined(ARDUINOEFS)
	if (ofile) { 
    if (!EFS.fputc(c, ofile)) ert=-1; 
  } else ert=1;
#endif
}

char fileread(){
	char c;
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(STM32SDIO)
	if (ifile) c=ifile.read(); else { ert=1; return 0; }
	if (c == -1 || c == 255) ert=-1;
	return c;
#endif
#ifdef RP2040LITTLEFS
	if (ifile) c=fgetc(ifile); else { ert=1; return 0; }
	if (c == -1 || c == 255) ert=-1;
	return c;
#endif
#ifdef ARDUINOEFS
	if (ifile) c=EFS.fgetc(ifile); else { ert=1; return 0; }
	if (c == -1|| c == 255) ert=-1;
	return c;
#endif
	return 0;
}

int fileavailable(){
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(STM32SDIO)
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
#if defined(ARDUINOSD)
	ifile=SD.open(mkfilename(filename), FILE_READ);
	return (int) ifile;
#endif
#if defined(STM32SDIO)
  ifile=SD.open(filename);
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
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(STM32SDIO)
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
#if defined(ARDUINOSD) 
	if (*m == 'w') ofile=SD.open(mkfilename(filename), FILE_OWRITE);
/* ESP32 has FILE_APPEND defined */
#ifdef FILE_APPEND
  if (*m == 'a') ofile=SD.open(mkfilename(filename), FILE_APPEND);
#else
	if (*m == 'a') ofile=SD.open(mkfilename(filename), FILE_WRITE);
#endif
	return (int) ofile;
#endif
#if  defined(STM32SDIO)
  if (*m == 'w') ofile=SD.open(filename, FILE_OWRITE);
/* ESP32 has FILE_APPEND defined */
#ifdef FILE_APPEND
  if (*m == 'a') ofile=SD.open(filename, FILE_APPEND);
#else
  if (*m == 'a') ofile=SD.open(filename, FILE_WRITE);
#endif
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
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(STM32SDIO)
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
#if defined(ARDUINOSD) || defined(STM32SDIO)
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
#if defined(ARDUINOSD) || defined(STM32SDIO)
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
  return 0;
}

int rootisfile() {
#if defined(ARDUINOSD) || defined(STM32SDIO)
	return (! file.isDirectory());
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
	return 1;
#endif
#ifdef ARDUINO_ARCH_ESP32
	return (! file.isDirectory());
#endif
#endif
#ifdef RP2040LITTLEFS
	return (file->d_type == DT_REG);
#endif
#ifdef ARDUINOEFS
	return 1;
#endif	
	return 0;
}

const char* rootfilename() {
#if defined(ARDUINOSD)
	//return (char*) file.name();
 return rmrootfsprefix(file.name());
#endif
#if defined(STM32SDIO)
  return (char*) file.name();
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
	return 0; 
}

long rootfilesize() {
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(STM32SDIO)
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
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(STM32SDIO)
  file.close();
#endif 
#ifdef RP2040LITTLEFS
#endif
#ifdef ARDUINOEFS
#endif
}

void rootclose(){
#if defined(ARDUINOSD) || defined(STM32SDIO)
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
#if defined(ARDUINOSD)	
	SD.remove(mkfilename(filename));
	return;
#endif
#if defined(STM32SDIO)  
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
#if defined(ARDUINOSD) || defined(STM32SDIO)	
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
 *	The picoseria an own interrupt function. This is used to fill 
 *  the input buffer directly on read. Write is standard like in 
 *  the serial code.
 *  
 * As echoing is done in the interrupt routine, the code cannot be 
 * used to receive keystrokes from serial and then display the echo
 * directly to a display. To do this the write command in picogetchar
 * would have to be replaced with a outch() that then redirects to the 
 * display driver. This would be very tricky from the timing point of 
 * view if the display driver code is slow. 
 * 
 * The code for the UART control is mostly taken from PicoSerial
 * https://github.com/gitcnd/PicoSerial, originally written by Chris Drake
 * and published under GPL3.0 just like this code
 * 
 */
#ifdef USESPICOSERIAL
volatile static char picochar;
volatile static char picoa = 0;
volatile static char* picob = NULL;
static short picobsize = 0;
volatile static short picoi = 1;

/* this is mostly taken from PicoSerial */
const uint16_t MIN_2X_BAUD = F_CPU/(4*(2*0XFFF + 1)) + 1;

/* the begin code */
void picobegin(unsigned long baud) {
    uint16_t baud_setting;
    cli();               
    if ((F_CPU != 16000000UL || baud != 57600) && baud > MIN_2X_BAUD) {
      UCSR0A = 1 << U2X0;
      baud_setting = (F_CPU / 4 / baud - 1) / 2;
    } else {
      UCSR0A = 0;
      baud_setting = (F_CPU / 8 / baud - 1) / 2;
    }
/* assign the baud_setting */
    UBRR0H = baud_setting >> 8;
    UBRR0L = baud_setting;
/* enable transmit and receive */
    UCSR0B |= (1 << TXEN0) | (1 << RXEN0) | (1 << RXCIE0);
    sei();                   
}

/* the write code, sending bytes directly to the UART */
void picowrite(char b) {
    while (((1 << UDRIE0) & UCSR0B) || !(UCSR0A & (1 << UDRE0))) {}
    UDR0 = b;
}

/* 
 *  picogetchar: this is the interrupt service routine.  It 
 *  recieves a character and feeds it into a buffer and echos it
 *  back. The logic here is that the ins() code sets the buffer 
 *  to the input buffer. Only then the routine starts writing to the 
 *  buffer. Once a newline is received, the length information is set 
 *  and picoa is also set to 1 indicating an available string, this stops
 *  recevieing bytes until the input is processed by the calling code.
 */
void picogetchar(char c){
	if (picob && (! picoa)) {
    picochar=c;
		if (picochar != '\n' && picochar != '\r' && picoi<picobsize-1) {
      if (c == 127 || c == 8) {
        if (picoi>1) picoi--; else return;
      } else 
			  picob[picoi++]=picochar;
      picowrite(picochar);
		} else {
			picoa = 1;
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

/* the ins code of picoserial, called like this in consins */
void picoins(char *b, short nb) {
  char c;
  picob=b;
  picobsize=nb;
  picoa=0;
/* once picoa is set, the interrupt routine records characters 
 *  until a cr and then resets picoa to 0 */
  while (!picoa) byield();
  outcr();
  return;
 }

/* on an UART interrupt, the getchar function is called */
#ifdef USART_RX_vect
ISR(USART_RX_vect) {
  char c=UDR0;
  picogetchar(c);
}
#else
/* for MEGAs and other with many UARTs */
  ISR(USART0_RX_vect) {
  char c=UDR0;
  picogetchar(c);
}
#endif
#endif

/* 
 * blocking serial single char read for Serial
 * unblocking for Picoserial because it is not 
 * character oriented -> blocking is handled in 
 * consins instead.
 */
char serialread() {
#ifdef USESPICOSERIAL
	char c;
	c=picochar;
	picochar=0;
	return c;	
#else
	while (!SERIALPORT.available()) byield();
	return SERIALPORT.read();
#endif
}

/*
 *  serial begin code with a one second delay after start
 *	this is not needed any more 
 */
void serialbegin() {
#ifdef USESPICOSERIAL
	picobegin(serial_baudrate); 
#else
	SERIALPORT.begin(serial_baudrate);
#endif
	bdelay(1000);
}

/* state information on the serial port */
int serialstat(char c) {
  if (c == 0) return 1;
  if (c == 1) return serial_baudrate;
  return 0;
}

/* write to a serial stream */
void serialwrite(char c) {
#ifdef HASMSTAB
  if (c > 31) charcount+=1;
  if (c == 10) charcount=0;
#endif
#ifdef USESPICOSERIAL
	picowrite(c);
#else
/* write never blocks. discard any bytes we can't get rid of */
  SERIALPORT.write(c);  
/* if (Serial.availableForWrite()>0) Serial.write(c);	*/
#endif
}

/* check on a character, needed for breaking */
short serialcheckch() {
#ifdef USESPICOSERIAL
	return picochar;
#else
	if (SERIALPORT.available()) return SERIALPORT.peek(); else return 0; // should really be -1
#endif	
}

/* avail method, needed for AVAIL() */ 
short serialavailable() {
#ifdef USESPICOSERIAL
	return picoi;
#else
	return SERIALPORT.available();
#endif	
}

/* flush serial */
void serialflush() {
#ifdef USESPICOSERIAL
  return;
#else
  while (SERIALPORT.available()) SERIALPORT.read();
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
	  picoins(b, nb);
    return;
	}
#endif
	while(z.a < nb) {
  		c=inch();
  		if (id == ISERIAL || id == IKEYBOARD) outch(c); /* this is local echo */
  		if (c == '\r') c=inch(); 			/* skip carriage return */
  		if (c == '\n' || c == -1 || c == 255) { 	/* terminal character is either newline or EOF */
    		break;
  		} else if (c == 127 || c == 8) {
        if (z.a>1) z.a--;
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
#if !defined(ARDUINO_AVR_MEGA2560) && !defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_AVR_NANO_EVERY) && !defined(ARDUINO_NANO_RP2040_CONNECT) && !defined(ARDUINO_RASPBERRY_PI_PICO) && !defined(ARDUINO_SEEED_XIAO_M0)
#include <SoftwareSerial.h>
const int software_serial_rx = SOFTSERIALRX;
const int software_serial_tx = SOFTSERIALTX;
SoftwareSerial PRTSERIAL(software_serial_rx, software_serial_tx);
#endif

/* second serial port */
void prtbegin() {
	PRTSERIAL.begin(serial1_baudrate);
}

/* the open functions are not needed here */
char prtopen(char* filename, int mode) {}
void prtclose() {}

int prtstat(char c) {
  if (c == 0) return 1;
  if (c == 1) return serial1_baudrate;
  return 0;
}

void prtwrite(char c) {
	PRTSERIAL.write(c);
}

char prtread() {
	while (!PRTSERIAL.available()) byield();
	return PRTSERIAL.read();
}

short prtcheckch() {
	if (PRTSERIAL.available()) return PRTSERIAL.peek(); else return 0;
}

short prtavailable() {
	return PRTSERIAL.available();
}

void prtset(int s) {
  serial1_baudrate=s;
  prtbegin();
}
#endif


/* 
 * The wire code, direct access to wire communication
 * in master mode wire_slaveid is the I2C address bound 
 * to channel &7 and wire_myid is 0
 * in slave mode wire_myid is the devices slave address
 * and wire_slaveid is 0
 * ARDUINOWIREBUFFER is the maximum length of meesages the 
 * underlying library can process. This is 32 for the Wire
 * library
 */ 

/* this is always here, if Wire is needed by a subsysem */
#ifdef HASWIRE
/* default begin is as a master 
 * This doesn't work properly on the ESP32C3 platform
 * See  https://github.com/espressif/arduino-esp32/issues/6376 
 * for more details
 */
void wirebegin() {
#ifndef SDA_PIN   
  Wire.begin();
#else
  Wire.begin(SDA_PIN, SCL_PIN);
#endif
}
#endif

/* this is code needed for the OPEN/CLOSE and wireslave mechanisms */
#if (defined(ARDUINOWIRE) && defined(HASFILEIO))
uint8_t wire_slaveid = 0;
uint8_t wire_myid = 0;
#define ARDUINOWIREBUFFER 32
#ifdef ARDUINOWIRESLAVE
char wirereceivebuffer[ARDUINOWIREBUFFER];
short wirereceivechars = 0;
char wirerequestbuffer[ARDUINOWIREBUFFER];
short wirerequestchars = 0;
#endif

void wireslavebegin(char s) {
#ifndef SDA_PIN
  Wire.begin(s);
#else
  Wire.begin(SDA_PIN, SCL_PIN, s);
#endif
}

/* wire status - just checks if wire is compiled */
int wirestat(char c) {
  switch (c) {
    case 0: 
      return 1;
#ifdef ARDUINOWIRESLAVE
    case 1:
      return wirerequestchars;
#endif    
    default:
      return 0;
  }
}

/* available characters - test code ecapsulation prep for slave*/
short wireavailable() {
/* as a master we return 1, as a slave the received chars*/
  if (wire_myid == 0) return 1;
#ifdef ARDUINOWIRESLAVE
  else return wirereceivechars; 
#else
  else return 0;
#endif
}

#ifdef ARDUINOWIRESLAVE
/* event handler for receive */
void wireonreceive(int h) {
  wirereceivechars=h;
  if (h>ARDUINOWIREBUFFER) h=ARDUINOWIREBUFFER;
  for (int i=0; i<h; i++) wirereceivebuffer[i]=Wire.read();
}

/* event handler for request, deliver the message and forget the buffer */ 
void wireonrequest() {
  Wire.write(wirerequestbuffer, wirerequestchars);
  wirerequestchars=0;
}
#endif

/*
 *	as a master open sets the slave id for the communication
 *	no extra begin while we stay master
 */
void wireopen(char s, char m) {
	if (m == 0) {
		wire_slaveid=s;
/* we have been a slave and become master, restart wire*/
    if (wire_myid != 0) {
      wirebegin();
      wire_myid=0;    
    }
	} else if ( m == 1 ) { 
		wire_myid=s;
		wire_slaveid=0;
#ifdef ARDUINOWIRESLAVE
		wireslavebegin(wire_myid);
    Wire.onReceive(&wireonreceive);
    Wire.onRequest(&wireonrequest);
#endif
	} else 
		error(EORANGE);
}

/* input an entire string */
void wireins(char *b, uint8_t l) {
  if (wire_myid == 0) {
    z.a=0;
    if (l>ARDUINOWIREBUFFER) l=ARDUINOWIREBUFFER;
    if (!Wire.requestFrom(wire_slaveid, l)) ert=1;
    while (Wire.available() && z.a<l) b[++z.a]=Wire.read();
  } else {
#ifdef ARDUINOWIRESLAVE
    for (z.a=0; z.a<wirereceivechars && z.a<l; z.a++) b[z.a+1]=wirereceivebuffer[z.a];
    wirereceivechars=0; /* clear the entire buffer on read, bytewise read not really supported */
#endif
  }
  b[0]=z.a;
}

/* send an entire string - truncate radically */
void wireouts(char *b, uint8_t l) {
  if (l>ARDUINOWIREBUFFER) l=ARDUINOWIREBUFFER; 
  if (wire_myid == 0) {
    Wire.beginTransmission(wire_slaveid); 
#ifdef ARDUINO_ARCH_ESP32
    for(z.a=0; z.a<l; z.a++) Wire.write(b[z.a]);  
#else
    Wire.write(b, l);
#endif
    ert=Wire.endTransmission(); 
  } else {
#ifdef ARDUINOWIRESLAVE
    for (int i=0; i<l; i++) wirerequestbuffer[i]=b[i];
    wirerequestchars=l;
#endif
  }
}
#endif

/* plain wire without FILE I/O functions */
#if (defined(ARDUINOWIRE))
/* single byte access functions on Wire */
short wirereadbyte(short port) { 
    if (!Wire.requestFrom(port, 1)) ert=1;
    return Wire.read();
}
void wirewritebyte(short port, short data) { 
  Wire.beginTransmission(port); 
  Wire.write(data);
  Wire.endTransmission();
}
void wirewriteword(short port, short data1, short data2) {
  Wire.beginTransmission(port); 
  Wire.write(data1);
  Wire.write(data2);
  Wire.endTransmission();
}
#endif

#if (defined(ARDUINOWIRE) && !defined(HASFILEIO))
int wirestat(char c) {return 0; }
void wireopen(char s, char m) {}
void wireins(char *b, uint8_t l) { b[0]=0; z.a=0; }
void wireouts(char *b, uint8_t l) {}
short wireavailable() { return 0; }
#endif

#ifndef ARDUINOWIRE
void wirebegin() {}
int wirestat(char c) {return 0; }
void wireopen(char s, char m) {}
void wireins(char *b, uint8_t l) { b[0]=0; z.a=0; }
void wireouts(char *b, uint8_t l) {}
short wireavailable() { return 0; }
short wirereadbyte(short port) { return 0; }
void wirewritebyte(short port, short data) { return; }
void wirewriteword(short port, short data1, short data2) { return; }
#endif

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

/* radio status */
int radiostat(char c) {
#if defined(ARDUINORF24)
  if (c == 0) return 1;
  if (c == 1) return radio.isChipConnected();
#endif
  return 0; 
}

/* generate a uint64_t pipe address from the filename string for RF24 */
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
	if (!radio.write(b, l)) ert=1;
	radio.startListening();
#endif
}

/* radio available */
short radioavailable() {
#ifdef ARDUINORF24
  return radio.available();
#endif
  return 0;
}

/* 
 *	we always read from pipe 1 and use pipe 0 for writing, 
 *	the filename is the pipe address, by default the radio 
 *	goes to reading mode after open and is only stopped for 
 *	write
 */
void iradioopen(char *filename) {
#ifdef ARDUINORF24
	if (!radio.begin()) ert=1;
	radio.openReadingPipe(1, pipeaddr(filename));
	radio.startListening();
#endif
}

void oradioopen(char *filename) {
#ifdef ARDUINORF24
	if (!radio.begin()) ert=1;
	radio.openWritingPipe(pipeaddr(filename));
#endif
}

void radioset(int s) {
#ifdef ARDUINORF24
  if ((s<0) && (s>3)) {error(EORANGE); return; } 
  rf24_pa=(rf24_pa_dbm_e) s;
  radio.setPALevel(rf24_pa);
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
#ifdef ARDUINOAHT
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;
#endif
#ifdef ARDUINOBMP280
#include <Adafruit_BMP280.h>
Adafruit_BMP280 bmp;
#endif
#ifdef ARDUINOBME280
#include <Adafruit_BME280.h>
Adafruit_BME280 bme;
/* add your own code here */
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
#ifdef ARDUINOAHT
  aht.begin();
#endif
#ifdef ARDUINOBMP280
  bmp.begin(); 
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,   /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
#endif
#ifdef ARDUINOBME280
  bme.begin(); 
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
    case 4:
#ifdef ARDUINOAHT
      sensors_event_t humidity, temp;
      switch (v) {
        case 0:
          return 1;
        case 1:
          aht.getEvent(&humidity, &temp);
          return temp.temperature;
        case 2:
          aht.getEvent(&humidity, &temp);
          return humidity.relative_humidity;
      }       
#endif
      return 0;
    case 5:
#ifdef ARDUINOBMP280  
      switch (v) {
        case 0:
          return 1;
        case 1:
          return bmp.readTemperature();
        case 2:
          return bmp.readPressure() / 100.0;
        case 3:
          return bmp.readAltitude(1013.25);
      }       
#endif
      return 0;
    case 6:
#ifdef ARDUINOBME280
      switch (v) {
        case 0:
          return 1;
        case 1:
          return bme.readTemperature();
        case 2:
          return bme.readPressure() / 100.0;
        case 3:
          return bme.readAltitude(1013.25);
        case 4:
          return bme.readHumidity();
      }       
#endif
/* add your own sensor code here */
      return 0;  
    default:
      return 0;
  }
}

#else
void sensorbegin() {}
number_t sensorread(short s, short v) {return 0;};
#endif


/*
 * Experimental code to drive SPI SRAM 
 *
 * Currently only the 23LCV512 is implemented, assuming a 
 * 64kB SRAM
 * The code below is taken in part from the SRAMsimple library
 * 
 * two buffers are implemented: 
 * - a ro buffer used by memread, this buffer is mainly reading the token 
 *  stream at runtime. 
 * - a rw buffer used by memread2 and memwrite2, this buffer is mainly accessing
 *  the heap at runtime. In interactive mode this is also the interface to read 
 *  and write program code to memory 
 * 
 */

#ifdef ARDUINOSPIRAM
#define USEMEMINTERFACE
#define SPIRAMINTERFACE

/* 
 *  we use the standard slave select pin as default 
 *  RAMPIN
 */
#ifndef RAMPIN
#define RAMPIN SS
#endif

#define SPIRAMWRMR  1 
#define SPIRAMRDMR  5
#define SPIRAMREAD  3     
#define SPIRAMWRITE 2     
#define SPIRAMRSTIO 0xFF
#define SPIRAMSEQ   0x40
#define SPIRAMBYTE  0x00

/* the RAM begin method sets the RAM to sequential mode */
address_t spirambegin() {
  pinMode(RAMPIN, OUTPUT);
  digitalWrite(RAMPIN, LOW);
  SPI.transfer(SPIRAMRSTIO);
  SPI.transfer(SPIRAMWRMR);
  //SPI.transfer(SPIRAMBYTE);
  SPI.transfer(SPIRAMSEQ);
  digitalWrite(RAMPIN, HIGH);
  /* only 32 kB addressable with 16 bit integers because address_t has to fit into number_t 
    the interpreter would work also with 64kB but PEEK, POKE and the DARKARTS are broken then*/
  if (maxnum>32767) return 65534; else return 32766;  
}

/* the simple unbuffered byte read, with a cast to signed char */
mem_t spiramrawread(address_t a) {
  mem_t c;
  digitalWrite(RAMPIN, LOW);
  SPI.transfer(SPIRAMREAD);
  SPI.transfer((byte)(a >> 8));
  SPI.transfer((byte)a);
  c = SPI.transfer(0x00);
  digitalWrite(RAMPIN, HIGH);
  return c;
}

/* one read only buffer for access from the token stream 
    memread calls this */
address_t spiram_robufferaddr = 0;
mem_t spiram_robuffervalid=0;
const address_t spiram_robuffersize = 32;
mem_t spiram_robuffer[spiram_robuffersize];

/* one rw buffer for access to the heap and all the program editing 
    memread2 and memwrite2 call this*/
address_t spiram_rwbufferaddr = 0;
mem_t spiram_rwbuffervalid=0;
const address_t spiram_rwbuffersize = 32; /* also change the addressmask if you want to play here */
mem_t spiram_rwbuffer[spiram_rwbuffersize];
mem_t spiram_rwbufferclean = 1; 

const address_t spiram_addrmask=0xffe0;
/* const address_t spiram_addrmask=0xffd0; // 64 byte frame */

/* the elementary buffer access functions used almost everywhere */

void spiram_bufferread(address_t a, mem_t* b, address_t l) {
  digitalWrite(RAMPIN, LOW);
  SPI.transfer(SPIRAMREAD);
  SPI.transfer((byte)(a >> 8));
  SPI.transfer((byte)a);
  SPI.transfer(b, l);
  digitalWrite(RAMPIN, HIGH);
} 

void spiram_bufferwrite(address_t a, mem_t* b, address_t l) {
  digitalWrite(RAMPIN, LOW); 
  SPI.transfer(SPIRAMWRITE);
  SPI.transfer((byte)(a >> 8));
  SPI.transfer((byte)a);
  SPI.transfer(b, l);
  digitalWrite(RAMPIN, HIGH);
}

mem_t spiram_robufferread(address_t a) {
/* we address a byte known to the rw buffer, then get it from there */
  if (spiram_rwbuffervalid && ((a & spiram_addrmask) == spiram_rwbufferaddr)) {
    return spiram_rwbuffer[a-spiram_rwbufferaddr];
  }
  
/* page fault, we dont have the byte in the ro buffer, so read from the chip*/
  if (!spiram_robuffervalid || a >= spiram_robufferaddr + spiram_robuffersize || a < spiram_robufferaddr ) {
      spiram_bufferread(a, spiram_robuffer, spiram_robuffersize);
      spiram_robufferaddr=a;
      spiram_robuffervalid=1;
  }
  return spiram_robuffer[a-spiram_robufferaddr];
}

/* flush the buffer */
void spiram_rwbufferflush() {
  if (!spiram_rwbufferclean) {
    spiram_bufferwrite(spiram_rwbufferaddr, spiram_rwbuffer, spiram_rwbuffersize);
    spiram_rwbufferclean=1;
   }
}

mem_t spiram_rwbufferread(address_t a) {
/* page fault, do read, ignore the ro buffer buffer */
  address_t p=a & spiram_addrmask;
  if (!spiram_rwbuffervalid || (p != spiram_rwbufferaddr)) {
/* flush the buffer if needed */
    spiram_rwbufferflush();
/* and reload it */
    spiram_bufferread(p, spiram_rwbuffer, spiram_rwbuffersize);
    spiram_rwbufferaddr = p; /* we only handle full pages here */
    spiram_rwbuffervalid=1;
  }
  return spiram_rwbuffer[a-spiram_rwbufferaddr];
}

/* the buffered file write */
void spiram_rwbufferwrite(address_t a, mem_t c) {
  address_t p=a&spiram_addrmask;
/* correct the two buffers if needed */
  if (spiram_robuffervalid && a >= spiram_robufferaddr && a < spiram_robufferaddr + spiram_robuffersize) {
    spiram_robuffer[a-spiram_robufferaddr]=c;
  }
/* if we have the frame, write it, mark the buffer dirty and return */
  if (spiram_rwbuffervalid && p == spiram_rwbufferaddr) {
    spiram_rwbuffer[a-spiram_rwbufferaddr]=c;
    spiram_rwbufferclean=0;
    return;
  }
/* if we end up here the write was impossible because we dont have the frame, so flush and then get it */
  spiram_rwbufferflush();
  (void) spiram_rwbufferread(a);
  spiram_rwbuffer[a-spiram_rwbufferaddr]=c;
  spiram_rwbufferclean=0;
}

/* the simple unbuffered byte write, with a cast to signed char */
void spiramrawwrite(address_t a, mem_t c) {
  digitalWrite(RAMPIN, LOW);
  SPI.transfer(SPIRAMWRITE);
  SPI.transfer((byte)(a >> 8));
  SPI.transfer((byte)a);
  SPI.transfer((byte) c);
  digitalWrite(RAMPIN, HIGH);
/* also refresh the ro buffer if in the frame */
  if (a >= spiram_robufferaddr && a < spiram_robufferaddr + spiram_robuffersize && spiram_robufferaddr > 0) 
    spiram_robuffer[a-spiram_robufferaddr]=c;
/* and the rw buffer if needed) */
  if (a >= spiram_rwbufferaddr && a < spiram_rwbufferaddr + spiram_rwbuffersize && spiram_rwbufferaddr > 0) 
    spiram_rwbuffer[a-spiram_rwbufferaddr]=c;
}
#endif


/* the code to address EEPROMs directly */
#ifdef ARDUINOPGMEEPROM
#define USEMEMINTERFACE
#define EEPROMMEMINTERFACE
#else 
#undef EEPROMMEMINTERFACE
#endif


#if defined(USEMEMINTERFACE)
/* 
 * to handle strings in situations with a memory interface two more buffers are 
 * needed they store intermediate results of string operations. The buffersize 
 * limits the maximum string length indepents of how big strings are set
 * 
 * default is 128, on an MEGA 512 is possible
 */
#ifdef ARDUINO_AVR_MEGA2560
#define SPIRAMSBSIZE 512
#else
#define SPIRAMSBSIZE 128
#endif

/* the string buffers of the memory interface */
char spistrbuf1[SPIRAMSBSIZE];
char spistrbuf2[SPIRAMSBSIZE];
#endif

/*
 * A tone emulation based on the byield loop. The maximum frequency depends 
 * on the byield call speed. 10-20 kHz is possible. Will be unreliable if 
 * the loop functions need a lot of CPU cycles.
 */
#ifdef ARDUINOTONEEMULATION
static mem_t tone_enabled;
static mem_t tone_pinstate = 0;
static unsigned long tone_intervall;
static unsigned long tone_micros;
static int tone_duration;
static unsigned long tone_start;
static mem_t tone_pin;

void playtone(int pin, int frequency, int duration){

/* the pin we are using is reset every time this is called*/
  tone_pin=pin;
  pinMode(tone_pin, OUTPUT);
  digitalWrite(tone_pin, LOW);
  tone_pinstate=0;

 /* duration 0 or frequency 0 stops playing */ 
  if (duration == 0 || frequency == 0) {
    tone_enabled=0;
    return;
  }

/* calculate the toggle intervall in micros and remember where we are */
  tone_intervall=1000000/frequency/2;
  tone_micros=micros();

/* set the duration and remember where we are*/
  tone_duration=duration;
  tone_start=millis();

/* start the play */
  tone_enabled=1;
}

void tonetoggle() {

/* is this active? */
  if (!tone_enabled) return;

/* check if we are done playing */
  if (millis() > tone_duration+tone_start) {
    tone_enabled=0;
    digitalWrite(tone_pin, LOW);
    tone_pinstate=0;
    return;
  }

/* if not, check if the intervall is over */
  if (micros() > tone_intervall+tone_micros) {
    tone_micros=micros();
    tone_pinstate=!tone_pinstate;
    digitalWrite(tone_pin, tone_pinstate);
  }
}
#endif

/* 
 * the events API for Arduino with interrupt service routines
 * analogous to the timer API
 * 
 * we use raw modes here 
 *
 * #define CHANGE 1
 * #define FALLING 2
 * #define RISING 3
 *
 */

#ifdef HASEVENTS
/* interrupts in BASIC fire once and then disable themselves, BASIC reenables them */
void bintroutine0() {
  eventlist[0].active=1;
  detachInterrupt(digitalPinToInterrupt(eventlist[0].pin)); 
}
void bintroutine1() {
  eventlist[1].active=1;
  detachInterrupt(digitalPinToInterrupt(eventlist[1].pin)); 
}
void bintroutine2() {
  eventlist[2].active=1;
  detachInterrupt(digitalPinToInterrupt(eventlist[2].pin)); 
}  
void bintroutine3() {
  eventlist[3].active=1;
  detachInterrupt(digitalPinToInterrupt(eventlist[3].pin)); 
}

#ifndef ARDUINO_ARCH_MBED_RP2040
typedef int PinStatus;
#endif

mem_t enableevent(int pin) {
  mem_t interrupt;
  int i;

/* do we have the data */
  if ((i=eventindex(pin))<0) return 0;

/* can we use this pin */  
  interrupt=digitalPinToInterrupt(eventlist[i].pin);
  if (interrupt < 0) return 0;

/* attach the interrupt function to this pin */
  switch(i) {
    case 0: 
      attachInterrupt(interrupt, bintroutine0, (PinStatus) eventlist[i].mode); 
      break;
    case 1:
      attachInterrupt(interrupt, bintroutine1, (PinStatus) eventlist[i].mode); 
      break;
    case 2:
      attachInterrupt(interrupt, bintroutine2, (PinStatus) eventlist[i].mode); 
      break;
    case 3:
      attachInterrupt(interrupt, bintroutine3, (PinStatus) eventlist[i].mode); 
      break;
    default:
      return 0;
  }

/* now set it enabled in BASIC */
  eventlist[i].enabled=1; 
  return 1;

}

void disableevent(mem_t pin) {
  detachInterrupt(digitalPinToInterrupt(pin)); 
}
#endif


/* 
 * This code measures the fast ticker frequency in microseconds 
 * It leaves the data in variable F. Activate this only for test 
 * purposes.
 */
#undef FASTTICKERPROFILE

#ifdef FASTTICKERPROFILE
static unsigned long lastfasttick = 0;
static unsigned long fasttickcalls = 0;
static int avgfasttick = 0;
static long devfasttick = 0;

void fasttickerprofile() {
  if (lastfasttick == 0) { lastfasttick=micros(); return; }
  int delta=micros()-lastfasttick;
  lastfasttick=micros();
  avgfasttick=(avgfasttick*fasttickcalls+delta)/(fasttickcalls+1);
  fasttickcalls++; 
  vars[5]=avgfasttick;
}
#endif



// defined HARDWARE_H
#endif
