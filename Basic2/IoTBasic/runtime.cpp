/*
 * Stefan's basic interpreter - runtime environment runtime.cpp.
 * 
 * This is the Arduino runtime environment for BASIC. It maps the functions 
 * needed for the various subsystems to the MCU specific implementations. 
 *
 * Author: Stefan Lenz, sl001@serverfabrik.de
 *
 * Configure the hardware settings in hardware.h.
 *
 */

#include "Arduino.h"
#include "hardware.h"
#include "runtime.h"

/* if the BASIC interpreter provides a loop function it will superseed this one */
void __attribute__((weak)) bloop() {};

/*
 * defining the systype variable which informs BASIC about the platform at runtime
 */

#if defined(ARDUINO_ARCH_AVR)
uint8_t bsystype = SYSTYPE_AVR;
#elif defined(ARDUINO_ARCH_ESP8266)
uint8_t bsystype = SYSTYPE_ESP8266;
#elif defined(ARDUINO_ARCH_ESP32)
uint8_t bsystype = SYSTYPE_ESP32;
#elif defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED_RP2040)
uint8_t bsystype = SYSTYPE_RP2040;
#elif defined(ARDUINO_ARCH_SAM) && defined(ARDUINO_ARCH_SAMD)
uint8_t bsystype = SYSTYPE_SAM;
#elif defined(ARDUINO_ARCH_XMC)
uint8_t bsystype = SYSTYPE_XMC;
#elif defined(ARDUINO_ARCH_SMT32)
uint8_t bsystype = SYSTYPE_SMT32;
#elif defined(ARDUINO_ARCH_RENESAS)
uint8_t bsystype = SYSTYPE_NRENESA;
#else
uint8_t bsystype = SYSTYPE_UNKNOWN;
#endif

/* 
 *  Global variables of the runtime env.
 */

int8_t id; // active input stream 
int8_t od; // active output stream 
int8_t idd = ISERIAL; // default input stream in interactive mode 
int8_t odd = OSERIAL; // default output stream in interactive mode 
int8_t ioer = 0; // the io error variable, always or-ed with ert in BASIC


/* counts the outputed characters on streams 0-3, used to emulate a real tab */
#ifdef HASMSTAB
uint8_t charcount[3]; /* devices 1-4 support tabing */
#endif

/* the pointer to the buffer used for the &0 device */
char* nullbuffer = ibuffer;
uint16_t nullbufsize = BUFSIZE; 

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
 * The I2C keyboard code - tested on ESP32 T-Deck only 
 * Implementation is generic enough to work on other platforms.
 * No headers are needed as the I2C library is included in the
 * Arduino core and the keyboard implementation of the T-Deck
 * is very generic.
 */
#ifdef ARDUINOI2CKBD
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
#if defined(HASWIRE) || defined(HASSIMPLEWIRE)
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
 * This is the monochrome library of Oli Kraus used for Nokia and SSD1306 displays
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
#if defined(ARDUINO_UNOR4_WIFI)
#include <WiFiS3.h>
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
 * ESP32FAT tested on ESP32
 * supports formating in BASIC
 */ 
#ifdef ESP32FAT
#define FILESYSTEMDRIVER
#ifdef ARDUINO_ARCH_ESP32
#include <FS.h>
#include <FFat.h>
#endif
#endif

/*
 * TFT_eSPI is a library for the T-Deck from Lilygo
 * #include "utilities.h" provided by Lilygo is coded directly into hardware.h
 * For some reason this has to be included after FS and FFat. Not yet fully 
 * understood why.
 */
#ifdef TFTESPI
#include <TFT_eSPI.h>
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
#undef ESP32FAT
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
#undef ESP32FAT
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
 * Arduino default serial baudrate and serial flags for the 
 * two supported serial interfaces. Serial is always active and 
 * connected to channel &1 with 9600 baud. 
 * 
 * channel 4 (ARDUINOPRT) can be either in character or block 
 * mode. Blockmode is set as default here. This means that all 
 * available characters are always loaded to a string -> inb()
 */
const uint16_t serial_baudrate = 9600;
uint8_t sendcr = 0;

#ifdef ARDUINOPRT
uint32_t serial1_baudrate = 9600; /* this is not const because it can be changed */
uint8_t blockmode = 1;
#else 
const int serial1_baudrate = 0;
uint8_t blockmode = 0;
#endif

/* 
 *  Input and output functions.
 * 
 * ioinit(): called at setup to initialize what ever io is needed
 * outch(): prints one ascii character 
 * inch(): gets one character (and waits for it)
 * checkch(): checks for one character (non blocking)
 * ins(): reads an entire line (uses inch except for pioserial)
 *
 */
void ioinit() {

/* a standalone system runs from keyboard and display */
#ifdef STANDALONE
  idd = IKEYBOARD;
  odd = ODSP;
#endif

/* run standalone on second serial, set the right parameters */
#ifdef STANDALONESECONDSERIAL
  idd = ISERIAL1;
  odd = OPRT;
  blockmode = 0;
  sendcr = 0;
#endif

/* signal handling - by default SIGINT which is ^C is always caught and 
  leads to program stop. Side effect: the interpreter cannot be stopped 
  with ^C, it has to be left with CALL 0, works on Linux, Mac and MINGW
  but not on DOSBOX MSDOS as DOSBOS does not handle CTRL BREAK correctly 
  DOS can be interrupted with the CONIO mechanism using BREAKCHAR. 
*/ 
  signalon();

/* this is only for RASPBERRY - wiring has to be started explicitly */
  wiringbegin();

/* all serial protocolls, ttl channels, SPI and Wire */
  serialbegin();
  
#ifdef ARDUINOPRT
  prtbegin();
#endif
#ifdef ARDUINOSPI
  spibegin();
#endif
#ifdef HASWIRE
  wirebegin();
#endif
/* filesystems and networks */
  fsbegin();
#ifdef ARDUINOMQTT
  netbegin();  
  mqttbegin();
#endif

/* the keyboards */
#if defined(HASKEYBOARD) || defined(HASKEYPAD)
  kbdbegin();
#endif
/* the displays */
#if defined(DISPLAYDRIVER) || defined(GRAPHDISPLAYDRIVER)
  dspbegin();
#endif
#if defined(ARDUINOVGA) || defined(POSIXFRAMEBUFFER)
  vgabegin();  /* mind this - the fablib code and framebuffer is special here */
#endif
/* sensor startup */
#ifdef ARDUINOSENSORS
  sensorbegin();
#endif
/* clocks and time */
#if defined(HASCLOCK)
  rtcbegin();
#endif

/* the eeprom dummy */
  ebegin();

/* activate the iodefaults */
  iodefaults();
}

void iodefaults() {
  od=odd;
  id=idd;
}

/* 
 *  Layer 0 - The generic IO code 
 *
 * inch() reads one character from the stream, mostly blocking
 * checkch() reads one character from the stream, unblocking, a peek(), 
 *  inmplemented inconsistently
 * availch() checks availablibity in the stream
 * inb() a block read function for serial interfacing, developed for 
 *  AT message receiving 
 */ 

/* this is odd ;-) */
int cheof(int c) { if ((c == -1) || (c == 255)) return 1; else return 0; }

/* the generic inch code reading one character from a stream */
char inch() {
  switch(id) {
  case ONULL:
    return bufferread();
  case ISERIAL:
    return serialread();   
#ifdef ARDUINOPRT
  case ISERIAL1:
    return prtread();
#endif
#if defined(HASKEYBOARD) || defined(HASKEYPAD) || defined(HASVT52)        
  case IKEYBOARD:
#if defined(HASVT52)
    if (vt52avail()) return vt52read(); /* if the display has a message, read it */
#endif
#if defined(HASKEYBOARD) || defined(HASKEYPAD)  
    return kbdread();
#endif
#endif
#if defined(HASWIRE)
  case IWIRE:
    return wireread();
#endif
#ifdef HASRF24
  case IRADIO:
    return radioread();
#endif
#ifdef ARDUINOMQTT
   case IMQTT:
    return mqttread();
#endif
#ifdef FILESYSTEMDRIVER
  case IFILE:
    return fileread();
#endif      
  }
  return 0;
}

/* 
 * checking on a character in the stream, this is 
 * normally only used for interrupting a program,
 * for many streams this is just mapped to avail
 */
char checkch(){
  switch (id) {
  case ONULL:
    return buffercheckch();
  case ISERIAL:
    return serialcheckch();
#ifdef FILESYSTEMDRIVER
  case IFILE:
    return fileavailable();
#endif
#ifdef HASRF24
  case IRADIO:
    return radioavailable();
#endif
#ifdef ARDUINOMQTT
  case IMQTT:
    return mqttcheckch();
#endif   
#ifdef HASWIRE 
  case IWIRE:
    return 0;
#endif
#ifdef ARDUINOPRT
  case ISERIAL1:
    return prtcheckch(); 
#endif
  case IKEYBOARD:
#if defined(HASKEYBOARD)  || defined(HASKEYPAD)
    return kbdcheckch(); /* here no display read as this is only for break and scroll control */
#endif
    break;
  }
  return 0;
}

/* character availability */
uint16_t availch(){
  switch (id) {
  case ONULL:
    return bufferavailable();
  case ISERIAL:
    return serialavailable(); 
#ifdef FILESYSTEMDRIVER
  case IFILE:
    return fileavailable();
#endif
#ifdef HASRF24
  case IRADIO:
    return radioavailable();
#endif        
#ifdef ARDUINOMQTT
  case IMQTT:
    return mqttavailable();
#endif        
#if defined(HASWIRE)
  case IWIRE:
    return wireavailable();
#endif
#ifdef ARDUINOPRT
  case ISERIAL1:
    return prtavailable();
#endif
  case IKEYBOARD:
#if defined(HASKEYBOARD) || defined(HASKEYPAD) || defined(HASVT52)
#if defined(HASVT52)
    if (vt52avail()) return vt52avail(); /* if the display has a message, read it */
#endif
#if defined(HASKEYBOARD) || defined(HASKEYPAD) 
    return kbdavailable();
#endif
#endif
    break;
  }
  return 0;
}

/* 
 *  the block mode reader for esp and sensor modules 
 *  on a serial interface, it tries to read as many 
 *  characters as possible into a buffer
 *  blockmode = 1 reads once availch() bytes
 *  blockmode > 1 implements a timeout mechanism and tries 
 *    to read until blockmode milliseconds have expired
 *    this is needed for esps and other sensors without
 *    flow control and volatile timing to receive more 
 *    then 64 bytes 
 */
 
uint16_t inb(char *b, uint16_t nb) {
  long m;
  uint16_t z;
  int16_t i = 0; // check this 

  if (blockmode == 1) {
    i=availch();
    if (i>nb-1) i=nb-1;
    b[0]=(unsigned char)i;
    z=i;
    b[i+1]=0;
    b++;
    while (i--) {*b++=inch();}  
  } else if (blockmode > 1) {
    m=millis();
    while (i < nb-1) {
      if (availch()) b[++i]=inch();
      if (millis() > m+blockmode) break;
    }
    b[0]=(unsigned char)i;
    z=i;
    b[i+1]=0;
  } else {
    b[0]=0;
    z=0;
    b[1]=0;
  } 
  return z;
}

/*
 * reading from the console with inch 
 * this version does not work for picoserial
 */
uint16_t consins(char *b, uint16_t nb) {
  char c;
  uint16_t z;
 
  z=1;
  while(z < nb) {
      c=inch();
      if (id == ISERIAL || id == IKEYBOARD) {
        outch(c); /* this is local echo */
      }
      if (c == '\r') c=inch();      /* skip carriage return */
      if (c == '\n' || c == -1 || c == 255) {   /* terminal character is either newline or EOF */
        break;
      } else if (c == 127 || c == 8) {
        if (z>1) z--;
      } else {
        b[z++]=c;
      } 
  }
  b[z]=0;
  z--;
  b[0]=z; /* cast to signed char is not good */
  return z;
}

/* 
 *  ins() is the generic reader into a string, by default 
 *  it works in line mode and ends reading after newline
 *
 *  the first element of the buffer is the lower byte of the length
 *
 *  for streams providing entire strings as an input the 
 *  respective string method is called
 *
 *  all other streams are read using consins() for character by character
 *  input until a terminal character is reached
 */
uint16_t ins(char *b, uint16_t nb) {
  switch(id) {
  case ONULL:
    return bufferins(b, nb);
  case ISERIAL:
    return serialins(b, nb);
#if defined(HASKEYBOARD) || defined(HASKEYPAD)
  case IKEYBOARD:
    return kbdins(b, nb);
#endif
#ifdef ARDUINOPRT
  case ISERIAL1:
    return prtins(b, nb);
 #endif   
#if defined(HASWIRE)
  case IWIRE:
    return wireins(b, nb);
#endif
#ifdef HASRF24
  case IRADIO:
    return radioins(b, nb);
#endif
#ifdef ARDUINOMQTT
  case IMQTT:
    return mqttins(b, nb);  
#endif
#ifdef FILESYSTEMDRIVER
  case IFILE:
    return consins(b, nb);
#endif
  default:
    b[0]=0; b[1]=0;
    return 0;
  }  
}

/*
 * outch() outputs one character to a stream
 * block oriented i/o like in radio not implemented here
 */
void outch(char c) {

/* do we have a MS style tab command, then count characters on stream 1-4 but not in fileio */
/* this does not work for control characters - needs to go to vt52 later */

#ifdef HASMSTAB
  if (od > 0 && od <= OPRT) {
    if (c > 31) charcount[od-1]+=1;
    if (c == 10) charcount[od-1]=0;
  }
#endif

  switch(od) {
  case ONULL:
    bufferwrite(c);
    break;
  case OSERIAL:
    serialwrite(c);
    break;
#ifdef ARDUINOPRT
  case OPRT:
    prtwrite(c);
    break;
#endif    
#ifdef FILESYSTEMDRIVER
  case OFILE:
    filewrite(c);
    break;
#endif
#ifdef ARDUINOVGA
  case ODSP: 
    vgawrite(c);
    break;
#elif defined(DISPLAYDRIVER) || defined(GRAPHDISPLAYDRIVER)
  case ODSP: 
    dspwrite(c);
    break;
#endif
#ifdef ARDUINOMQTT
  case OMQTT:
    mqttwrite(c); /* buffering for the PRINT command */
    break;
#endif
  default:
    break;
  }
  byield(); /* yield after every character for ESP8266 */
}

/*
 *  outs() outputs a string of length x at index ir - basic style
 *  default is a character by character operation, block 
 *  oriented write needs special functions
 */
void outs(char *b, uint16_t l){
  uint16_t i;

  switch (od) {
#ifdef HASRF24
    case ORADIO:
      radioouts(b, l);
      break;
#endif
#if (defined(HASWIRE))
    case OWIRE:
      wireouts(b, l);
      break;
#endif
#ifdef ARDUINOMQTT
    case OMQTT:
      mqttouts(b, l);
      break;
#endif
#ifdef GRAPHDISPLAYDRIVER
    case ODSP:
      dspouts(b, l);
      break;
#endif
    default:
      for(i=0; i<l; i++) outch(b[i]);
  }
  byield(); /* triggers yield after each character output */
}

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
#if defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_XMC) || defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_RENESAS)
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
#if defined(ARDUINO_ARCH_RENESAS)
  return freeRam() - 2000;
#endif
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_STM32) 
  return freeRam() - 4000;
#endif
#if defined(ARDUINO_ARCH_ESP32) 
#if defined(ARDUINO_TTGO_T7_V14_Mini32)
  return freeRam() - 4000; 
#else
  return freeRam() - 4000;
#endif
#endif
#if defined(ARDUINO_ARCH_XMC) 
  return freeRam() - 2000;
#endif
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_LGT8F) 
  int overhead=256;
#ifdef ARDUINO_AVR_MEGA2560
  overhead+=96;
#endif
#if defined(ARDUINOWIRE) || defined(ARDUINOSIMPLEWIRE) 
  overhead+=192;
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

/* the NRENESA board have a buildin RTC as well */
#if defined(ARDUINO_ARCH_RENESAS)
#define HASBUILTINRTC
#include "RTC.h"
RTCTime rtc;
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
 * Start the SPI bus - this is a little mean as some libraries also 
 * try to start the SPI which may lead to on override of the PIN settings
 * if the library code is not clean - currenty no conflict known.
 * 
 * The startup on the T-Deck seems to be tricky and require precautions.
 * The startup sequence used here is from the HelloWorld example of the
 * TFT_eSPI library.
 */
void spibegin() {
#ifdef ARDUINOSPI
#ifdef ARDUINO_TTGO_T7_V14_Mini32
/* this fixes the wrong board definition in the ESP32 core for this board */
  SPI.begin(14, 2, 12, 13);
#else 
#ifdef TFTESPI
  //! The board peripheral power control pin needs to be set to HIGH when using the peripheral
  pinMode(BOARD_POWERON, OUTPUT);
  digitalWrite(BOARD_POWERON, HIGH);

  //! Set CS on all SPI buses to high level during initialization
  pinMode(BOARD_SDCARD_CS, OUTPUT);
  pinMode(RADIO_CS_PIN, OUTPUT);
  pinMode(BOARD_TFT_CS, OUTPUT);

  digitalWrite(BOARD_SDCARD_CS, HIGH);
  digitalWrite(RADIO_CS_PIN, HIGH);
  digitalWrite(BOARD_TFT_CS, HIGH);

  pinMode(BOARD_SPI_MISO, INPUT_PULLUP);
  SPI.begin(BOARD_SPI_SCK, BOARD_SPI_MISO, BOARD_SPI_MOSI);
#else
  SPI.begin();
#endif
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
uint8_t rgbtovga(uint8_t r, uint8_t g, uint8_t b) {
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

 
#ifndef LCDSHIELDPINS 
#ifdef ARDUINO_ESP8266_WEMOS_D1R1
#define LCDSHIELDPINS 0,2,4,14,12,13
#define LCD3VOLTS
#else
#define LCDSHIELDPINS 8,9,4,5,6,7
#endif
#endif
const int dsp_rows=2;
const int dsp_columns=16;
LiquidCrystal lcd(LCDSHIELDPINS);
void dspbegin() { lcd.begin(dsp_columns, dsp_rows); dspsetscrollmode(1, 1);  }
void dspprintchar(char c, uint8_t col, uint8_t row) { lcd.setCursor(col, row); if (c) lcd.write(c);}
void dspclear() { lcd.clear(); }
void dspupdate() {}
void dspsetcursor(uint8_t c) { if (c) lcd.blink(); else lcd.noBlink(); }
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0; }
#define HASKEYPAD
/* elementary keypad reader left=1, right=2, up=3, down=4, select=<lf> */
char keypadread(){
	int a=analogRead(A0);
#ifndef LCD3VOLTS
	if (a >= 850) return 0;
	else if (a>=600 && a<850) return 10;
	else if (a>=400 && a<600) return '1'; 
	else if (a>=200 && a<400) return '3';
	else if (a>=60  && a<200) return '4';
	else return '2';
#else
  if (a >= 1000) return 0;
  else if (a>=900 && a<1000) return 10;
  else if (a>=700 && a<900) return '1'; 
  else if (a>=400 && a<700) return '3';
  else if (a>=100  && a<400) return '4';
  else return '2';
#endif
}
/* repeat mode of the keypad - off means block, on means return immediately */
uint8_t kbdrepeat=0;
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
void dspprintchar(char c, uint8_t col, uint8_t row) { lcd.setCursor(col, row); if (c) lcd.write(c); }
void dspclear() { lcd.clear(); }
void dspupdate() {}
void dspsetcursor(uint8_t c) { if (c) lcd.blink(); else lcd.noBlink(); }
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0; }
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
void dspprintchar(char c, uint8_t col, uint8_t row) { char b[] = { 0, 0 }; b[0]=c; if (c) u8g2.drawStr(col*dspfontsize+2, (row+1)*dspfontsize, b); }
void dspclear() { u8g2.clearBuffer(); u8g2.sendBuffer(); dspfgcolor=1; }
void dspupdate() { u8g2.sendBuffer(); }
void dspsetcursor(uint8_t c) {}
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0;}
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) {}
void vgacolor(uint8_t c) { dspfgcolor=c%3; u8g2.setDrawColor(dspfgcolor); }
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
void dspsetcursor(uint8_t c) {}
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0;}
void rgbcolor(uint8_t r, uint_8 g, uint8_t b) {}
void vgacolor(uint8_t c) { dspfgcolor=c%3; }
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
void dspprintchar(char c, uint8_t col, uint8_t row) { char b[] = { 0, 0 }; b[0]=c; if (c) u8g2.drawStr(col*dspfontsize+2, (row+1)*dspfontsize, b); }
void dspclear() { u8g2.clearBuffer(); u8g2.sendBuffer(); dspfgcolor=1; }
void dspupdate() { u8g2.sendBuffer(); }
void dspsetcursor(uint8_t c) {}
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0;}
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) {}
void vgacolor(uint8_t c) { dspfgcolor=c%3; u8g2.setDrawColor(dspfgcolor); }
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
void dspprintchar(char c, uint8_t col, uint8_t row) {  if (c) tft.drawChar(col*dspfontsize, row*dspfontsize, c, dspfgcolor, dspbgcolor, 2); }
void dspclear() { 
  tft.fillScreen(dspbgcolor); 
  dspfgcolor = dspdefaultfgcolor; 
  dspfgvgacolor = dspdefaultfgvgacolor; 
}
void dspupdate() {}
void dspsetcursor(uint8_t c) {}
void dspsavepen() { dsptmpcolor=dspfgcolor; dsptmpvgacolor=dspfgvgacolor; }
void dsprestorepen() { dspfgcolor=dsptmpcolor; dspfgvgacolor=dsptmpvgacolor; }
void dspsetfgcolor(uint8_t c) { vgacolor(c); }
void dspsetbgcolor(uint8_t c) { }
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0; }
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) { dspfgvgacolor=rgbtovga(r, g, b); dspfgcolor=tft.color565(r, g, b);}
void vgacolor(uint8_t c) {  
  short base=128;
  dspfgvgacolor=c;
  if (c==8) { dspfgcolor=tft.color565(64, 64, 64); return; }
  if (c>8) base=255;
  dspfgcolor=tft.color565(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
}
void plot(int x, int y) { tft.drawPixel(x, y, dspfgcolor); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1, dspfgcolor); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, y0, x1, y1, dspfgcolor);}
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, y0, x1, y1, dspfgcolor); }
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
void dspprintchar(char c, uint8_t col, uint8_t row) {  if (c) tft.drawChar(col*dspfontsize, row*dspfontsize, c, dspfgcolor, dspbgcolor, 2); }
void dspclear() { 
  tft.fillScreen(dspbgcolor); 
  dspfgcolor = dspdefaultfgcolor; 
  dspfgvgacolor = dspdefaultfgvgacolor; 
}
void dspupdate() {}
void dspsetcursor(uint8_t c) {}
void dspsavepen() { dsptmpcolor=dspfgcolor; dsptmpvgacolor=dspfgvgacolor; }
void dsprestorepen() { dspfgcolor=dsptmpcolor; dspfgvgacolor=dsptmpvgacolor; }
void dspsetfgcolor(uint8_t c) { vgacolor(c); }
void dspsetbgcolor(uint8_t c) { }
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0; }
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) { dspfgvgacolor=rgbtovga(r, g, b); dspfgcolor=tft.color565(r, g, b);}
void vgacolor(uint8_t c) {  
  short base=128;
  dspfgvgacolor=c;
  if (c==8) { dspfgcolor=tft.color565(64, 64, 64); return; }
  if (c>8) base=255;
  dspfgcolor=tft.color565(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
}
void plot(int x, int y) { tft.drawPixel(x, y, dspfgcolor); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1, dspfgcolor); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, y0, x1, y1, dspfgcolor);}
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, y0, x1, y1, dspfgcolor); }
void circle(int x0, int y0, int r) { tft.drawCircle(x0, y0, r, dspfgcolor); }
void fcircle(int x0, int y0, int r) { tft.fillCircle(x0, y0, r, dspfgcolor); }
#endif

#ifdef TFTESPI
#define DISPLAYDRIVER
#define DISPLAYHASCOLOR
#define DISPLAYHASGRAPH
/* 
 * first draft od the code taken from the hello world demo 
 * of Lilygo. This code only works on the 2.0.14 version of the ESP32 core.
 * Physical display dimensions is 320x240. We use the 12pf font for a first 
 * test. With this font we can display 26 columns and 20 rows. This is set 
 * statically in the code at the moment. 
 */
/* constants for 16 bit fonts */
const int dsp_rows=15;
const int dsp_columns=20;
char dspfontsize = 16;
const char dspfontsizeindex = 2;
/* constants for 8 bit fonts */
/*
const int dsp_rows=30;
const int dsp_columns=40;
char dspfontsize = 8;
const char dspfontsizeindex = 1;
*/
/* unchanged from the ILI code */
typedef uint16_t dspcolor_t;
const uint16_t dspdefaultfgcolor = 0xFFFF;
const uint8_t dspdefaultfgvgacolor = 0x0F;
dspcolor_t dspfgcolor = dspdefaultfgcolor;
dspcolor_t dspbgcolor = 0;
dspcolor_t dsptmpcolor = 0;
uint8_t dspfgvgacolor = dspdefaultfgvgacolor;
uint8_t dsptmpvgacolor = 0;
/* this is new for TFT_eSPI*/
TFT_eSPI tft;
/* change the brightness, taken from HelloWorld as well */
void tftespisetBrightness(uint8_t value)
{
    static uint8_t level = 0;
    static uint8_t steps = 16;
    if (value == 0) {
        digitalWrite(BOARD_BL_PIN, 0);
        delay(3);
        level = 0;
        return;
    }
    if (level == 0) {
        digitalWrite(BOARD_BL_PIN, 1);
        level = steps;
        delayMicroseconds(30);
    }
    int from = steps - level;
    int to = steps - value;
    int num = (steps + to - from) % steps;
    for (int i = 0; i < num; i++) {
        digitalWrite(BOARD_BL_PIN, 0);
        digitalWrite(BOARD_BL_PIN, 1);
    }
    level = value;
}
void dspbegin() { 
  tft.begin(); 
  tft.setRotation(1);
  tft.setTextDatum(6); /* upper left*/
  tft.setTextColor(dspfgcolor);
  tft.fillScreen(dspbgcolor); 
  dspsetscrollmode(1, 4); /* scrolling is on, scroll 4 lines at once */
  pinMode(BOARD_BL_PIN, OUTPUT);
  tftespisetBrightness(16); /* maximum brightness */
}
void dspprintchar(char c, uint8_t col, uint8_t row) {  
  if (c) tft.drawChar(col*dspfontsize, row*dspfontsize, c, dspfgcolor, dspbgcolor, dspfontsizeindex);  
}
void dspclear() { 
  tft.fillScreen(dspbgcolor); 
  dspfgcolor = dspdefaultfgcolor; 
  dspfgvgacolor = dspdefaultfgvgacolor; 
}
void dspupdate() {}
void dspsetcursor(uint8_t c) {}
void dspsavepen() { dsptmpcolor=dspfgcolor; dsptmpvgacolor=dspfgvgacolor; }
void dsprestorepen() { dspfgcolor=dsptmpcolor; dspfgvgacolor=dsptmpvgacolor; }
void dspsetfgcolor(uint8_t c) { vgacolor(c); }
void dspsetbgcolor(uint8_t c) { }
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0; }
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) { dspfgvgacolor=rgbtovga(r, g, b); dspfgcolor=tft.color565(r, g, b);}
void vgacolor(uint8_t c) {  
  short base=128;
  dspfgvgacolor=c;
  if (c==8) { dspfgcolor=tft.color565(64, 64, 64); return; }
  if (c>8) base=255;
  dspfgcolor=tft.color565(base*(c&1), base*((c&2)/2), base*((c&4)/4)); 
}
void plot(int x, int y) { tft.drawPixel(x, y, dspfgcolor); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1, dspfgcolor); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, y0, x1, y1, dspfgcolor);}
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, y0, x1, y1, dspfgcolor); }
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
void dspprintchar(char c, uint8_t col, uint8_t row) {}
void dspclear() {}
void dspupdate() {}
void dspsetcursor(uint8_t c) {}
void dspsetfgcolor(uint8_t c) {}
void dspsetbgcolor(uint8_t c) {}
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0; }
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) { dspfgcolor=0; }
void vgacolor(uint8_t c) {  
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
void dspprintchar(char c, uint8_t col, uint8_t row) { if (c) tft.printChar(c, col*dspfontsize, row*dspfontsize); }
void dspclear() { 
  tft.clrScr();  
  dspfgcolor = dspdefaultfgcolor; 
  dspfgvgacolor = dspdefaultfgvgacolor; 
  vgacolor(dspfgvgacolor);
}
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) { 
  tft.setColor(r,g,b); 
  dspfgcolor=(r << 16) + (g << 8) + b;
  dspfgvgacolor=rgbtovga(r, g, b);
}
void vgacolor(uint8_t c) {
  short base=128;
  dspfgvgacolor=c;
  if (c==8) { tft.setColor(64, 64, 64); return; }
  if (c>8) base=255;
  tft.setColor(base*(c&1), base*((c&2)/2), base*((c&4)/4));  
}
void dspupdate() {}
void dspsetcursor(uint8_t c) {}
void dspsavepen() { dsptmpcolor=dspfgcolor; dsptmpvgacolor=dspfgvgacolor; }
void dsprestorepen() { dspfgcolor=dsptmpcolor; dspfgvgacolor=dsptmpvgacolor; }
void dspsetfgcolor(uint8_t c) { vgacolor(c); }
void dspsetbgcolor(uint8_t c) { }
void dspsetreverse(uint8_t c) {}
uint8_t dspident() {return 0;}
void plot(int x, int y) { tft.drawPixel(x, y); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, y0, x1, y1); }
void frect(int x0, int y0, int x1, int y1)  { tft.fillRect(x0, y0, x1, y1); }
void circle(int x0, int y0, int r) { tft.drawCircle(x0, y0, r); }
void fcircle(int x0, int y0, int r) { tft.fillCircle(x0, y0, r); }
#endif

/* 
 * this is the VGA code for fablib 
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
fabgl::SoundGenerator soundGenerator;

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

int vgastat(uint8_t c) {return 0; }

/* scale the screen size */
void vgascale(int* x, int* y) {
	*y=*y*10/16;
}

void rgbcolor(uint8_t r, uint8_t g, uint8_t b) {
	short vga;
	if (r>191 || g>191 || b>191) vga=8; else vga=0;
	vga=vga+r/128+g/128*2+b/128*4;
	vga_graph_pen=fabgl::Color(vga);
}

void vgacolor(uint8_t c) { vga_graph_pen = fabgl::Color(c%16); }
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
int vgastat(uint8_t c) {return 0; }
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
#if defined(ARDUINO_TTGO_T7_V14_Mini32) && defined(ARDUINOVGA)
fabgl::PS2Controller PS2Controller;
char fabgllastchar = 0;
#else
#if defined(ARDUINO) && defined(ARDUINOPS2)
PS2Keyboard keyboard;
#else 
#if defined(ARDUINO) && defined(ARDUINOUSBKBD)
USBHost usb;
KeyboardController keyboard(usb);
char usbkey=0;
#else 
#if defined(ARDUINOZX81KBD)
ZX81Keyboard keyboard;
#else
#if defined(ARDUINOI2CKBD)
/* with an I2C keyboard we remeber the last key, this is similar to the USB keyboard */
char i2ckey=0;
#endif
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
#if ARDUINOKBDLANG_GERMAN
      if (usbkey>31 && usbkey<128) usbkey=usbkeymapGerman[usbkey-32]; 
#else 
      if (usbkey>31 && usbkey<128) usbkey=usbkeymapUS[usbkey-32]; 
#endif
  }
}
#endif

/* 
 * keyboard controller code 
 */

#ifdef ZX81KEYBOARD
const byte zx81pins[] = {ZX81PINS}; 
#endif

/* 
 *  A helper function for the I2C keyboard code. We get one character 
 *  from the board. A -1 is an I2C error condition for a non reply of 
 *  the keyboard. 
 */
#ifdef ARDUINOI2CKBD
char i2ckbdgetkey() {
  char c=0;
  Wire.requestFrom(I2CKBDADDR, 1);
    if (Wire.available()) {
      c=Wire.read();
      if (c == -1) { ioer=1; c=0; }
    }
    return c;
}
#endif

void kbdbegin() {
#ifdef PS2KEYBOARD
#ifdef ARDUINOKBDLANG_GERMAN
	keyboard.begin(PS2DATAPIN, PS2IRQPIN, PS2Keymap_German);
#else
  keyboard.begin(PS2DATAPIN, PS2IRQPIN, PS2Keymap_US);
#endif
#else
#ifdef PS2FABLIB
	PS2Controller.begin(PS2Preset::KeyboardPort0);
#ifdef ARDUINOKBDLANG_GERMAN
  PS2Controller.keyboard()->setLayout(&fabgl::GermanLayout);
#else
  PS2Controller.keyboard()->setLayout(&fabgl::USLayout);
#endif
#else 
#ifdef USBKEYBOARD
/* nothing to be done here */
#else
#ifdef ZX81KEYBOARD
  keyboard.begin(zx81pins);
#else
#ifdef ARDUINOI2CKBD
/* this is from the T-Deck examples, time needed for the keyboard to start */
  delay(500);
#endif
#endif
#endif
#endif
#endif
}

uint8_t kbdstat(uint8_t c) {return 0; }

uint16_t kbdins(char* b, uint16_t nb) { return consins(b, nb); }

uint8_t kbdavailable(){
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
#else
#ifdef ARDUINOI2CKBD
/* do we have a key stored, then return it */
  if (i2ckey) {
    return 1; 
  } else {
    i2ckey=i2ckbdgetkey(); 
    if (i2ckey != 0) return 1;
  }
#endif
#endif
#endif
#endif
#endif
#ifdef HASKEYPAD
/* a poor man's debouncer, unstable state returns 0 */
  char c=keypadread();
  if (c != 0) {
    bdelay(2); 
    if (c == keypadread()) return 1;
  }
#endif	
	return 0;
}

char kbdread() {
  char c = 0;
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
#else
#ifdef ARDUINOI2CKBD
/* a key is stored from a previous kbdavailable() */
 if (i2ckey) {
    c=i2ckey;
    i2ckey=0;
  } else {
/* if not look for a key, no need to store it */
    c=i2ckbdgetkey();
  }
#endif
#endif
#endif
#endif
#ifdef HASKEYPAD
  if (c == 0) { /* we have no character from a real keyboard, ask the keypad */
	  c=keypadread();
/* if the keypad is in non repeat mode, block */
	  if (!kbdrepeat) while(kbdavailable()) byield();
  }
#endif	
	if (c == 13) c=10;
	return c;
}

char kbdcheckch() {
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
  if (kbdavailable()) return kbdread(); else return 0;
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
#else 
#ifdef ARDUINOI2CKBD
  if (!i2ckey) {
    i2ckey=i2ckbdgetkey();
  }
  return i2ckey;
#endif
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
 * dspprintchar(char c, uint8_t col, uint8_t row)
 * dspclear()
 * dspbegin()
 * dspupdate()
 * dspsetcursor(uint8_t c) 
 * dspsetfgcolor(address_t c)  
 * void dspsetbgcolor(address_t c)  
 * void dspsetreverse(uint8_t c)  
 * uint8_t dspident()  
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
uint8_t dspmycol = 0;
uint8_t dspmyrow = 0;

/* the escape state of the vt52 terminal */
uint8_t dspesc = 0;

/* which update mode do we have */
uint8_t dspupdatemode = 0;

/* how do we handle wrap 0 is wrap, 1 is no wrap */
uint8_t dspwrap = 0; 

/* the print mode */
uint8_t dspprintmode = 0;

/* the scroll control variables */
uint8_t dspscrollmode = 0;
uint8_t dsp_scroll_rows = 1;

uint8_t dspstat(uint8_t c) { return 0; }

void dspsetcursorx(uint8_t c) {
  if (c>=0 && c<dsp_columns) dspmycol=c;
  dspprintchar(0, dspmycol, dspmyrow); /* needed to really position the cursor if there is a hardware cursor */
}

void dspsetcursory(uint8_t r) {
  if (r>=0 && r<dsp_rows) dspmyrow=r;
    dspprintchar(0, dspmycol, dspmyrow); /* needed to really position the cursor if there is a hardware cursor */
}

uint8_t dspgetcursorx() { return dspmycol; }

uint8_t dspgetcursory() { return dspmyrow; }

uint8_t dspactive() {
	return od == ODSP;
}

/* to whom the bell tolls - implement this to you own liking */
void dspbell() {}

/*
 * control the update modes for page update displays
 * like Epaper, Nokia 5110 and SSD1306
 * dspgraphupdate is the helper for the graphics functions
 */

void dspsetupdatemode(uint8_t c) {
  dspupdatemode=c;
}

uint8_t dspgetupdatemode() {
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

#ifndef DISPLAYHASCOLOR
const uint16_t dspfgvgacolor = 0;
#endif

dspbuffer_t dspbuffer[dsp_rows][dsp_columns];

/* buffer access functions */
dspbuffer_t dspget(uint16_t i) {
  if (i>=0 && i<=dsp_columns*dsp_rows-1) return dspbuffer[i/dsp_columns][i%dsp_columns]; else return 0;
}

dspbuffer_t dspgetrc(uint8_t r, uint8_t c) { return dspbuffer[r][c]; }

dspbuffer_t dspgetc(uint8_t c) { return dspbuffer[dspmyrow][c]; }

/* this functions prints a character and updates the display buffer */
void dspsetxy(dspbuffer_t ch, uint8_t c, uint8_t r) {
  if (r>=0 && c>=0 && r<dsp_rows && c<dsp_columns) {
    dspbuffer[r][c]=(dspbuffer_t)ch | (dspfgvgacolor << 8);
    if (ch != 0) dspprintchar(ch, c, r); else dspprintchar(' ', c, r);
  }
}

void dspset(uint16_t i, dspbuffer_t ch) {
  uint8_t c=i%dsp_columns;
  uint8_t r=i/dsp_columns;
  dspsetxy(ch, c, r);
}

/* 0 normal scroll, 1 enable waitonscroll function */
void dspsetscrollmode(uint8_t c, uint8_t l) {
	dspscrollmode = c;
	dsp_scroll_rows = l;
}

/* clear the buffer */
void dspbufferclear() {
	uint8_t r,c;
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
void dspscroll(uint8_t scroll_rows, uint8_t scroll_top=0){
	uint8_t r,c;
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
void dspreversescroll(uint8_t line){
  uint8_t r,c;
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
dspbuffer_t dspget(uint16_t i) { return 0; }

dspbuffer_t dspgetrc(uint8_t r, uint8_t c) { return 0; }

dspbuffer_t dspgetc(uint8_t c) { return 0; }

void dspsetxy(dspbuffer_t ch, uint8_t c, uint8_t r) {
  if (ch != 0) dspprintchar(ch, c, r); else dspprintchar(' ', c, r);
}

void dspset(uint16_t i, dspbuffer_t ch) {
  uint8_t c=i%dsp_columns;
  uint8_t r=i/dsp_columns;
  dspsetxy(ch, c, r);
}

/* unbuffered display, the only thing we can do is go home */
void dspbufferclear() {
  dspmyrow=0;
  dspmycol=0;
}

char dspwaitonscroll() { return 0; }

/* a stub for dspwrite */
void dspscroll(uint8_t scroll_rows, uint8_t scroll_top=0){
  dspmyrow=dsp_rows-1;
}

void dspsetscrollmode(uint8_t c, uint8_t l) {}
void dspreversescroll(uint8_t a){}
#endif

/* the generic write function for displays with and without buffers */

void dspwrite(char c){
int8_t dspmycolt;

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
uint8_t vt52graph = 0;

/* the secondary cursor */
uint8_t vt52mycol = 0;
uint8_t vt52myrow = 0;

/* temp variables for column and row , do them here and not in the case: guess why */
uint8_t vt52tmpr;
uint8_t vt52tmpc;

/* an output buffer for the vt52 terminal */
const uint8_t vt52buffersize = 4; 
char vt52outbuffer[vt52buffersize] = { 0, 0, 0, 0 };
uint8_t vt52bi = 0;
uint8_t vt52bj = 0;

/* the reader from the buffer */
char vt52read() {
  if (vt52bi<=vt52bj) { vt52bi = 0; vt52bj = 0; } /* empty, reset */
  if (vt52bi>vt52bj) return vt52outbuffer[vt52bj++];
  return 0;
}

/* the avail from the buffer */
uint8_t vt52avail() { if (vt52bi > vt52bj) return vt52bi-vt52bj; else return 0; }

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
      for (uint8_t i=0; i<dsp_columns; i++) prtwrite(dspgetc(i));
#endif
      break;
    case ']': /* Printer extension - print screen */
#if defined(ARDUINOPRT) && defined(DISPLAYCANSCROLL)
      for (uint8_t j=0; j<dsp_rows; j++)
        for (uint8_t i=0; i<dsp_columns; i++) prtwrite(dspgetrc(j, i));
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
      for (uint8_t i=dspmycol; i<dsp_columns; i++) dspsetxy(0, i, dspmyrow);
      break;
    case 'l': // GEMDOS / TOS extension clear line 
      for (uint8_t i=0; i<dsp_columns; i++) dspsetxy(0, i, dspmyrow);
      break;
    case 'o': // GEMDOS / TOS extension clear to start of line
      for (uint8_t i=0; i<=dspmycol; i++) dspsetxy(0, i, dspmyrow);
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
void dspouts(char* s, uint16_t l) {
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
uint8_t dspactive() {return 1; }
void dspsetupdatemode(uint8_t c) {}
#else
/* stubs for systems without a display driver, BASIC uses these functions */
const int dsp_rows=0;
const int dsp_columns=0;
void dspsetupdatemode(uint8_t c) {}
void dspwrite(char c){};
void dspbegin() {};
uint8_t dspstat(uint8_t c) {return 0; }
char dspwaitonscroll() { return 0; };
uint8_t dspactive() {return 0; }
void dspsetscrollmode(uint8_t c, uint8_t l) {}
void dspscroll(uint8_t a, uint8_t b){}
char vt52read() { return 0; }
uint8_t vt52avail() { return 0; }
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
 * available in BASIC, this happens in runtime.h. 
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

#if defined(ARDUINORTC)
/*
 * I2C clocks DS1307, DS3231, and DS3232 using Wire directly
 * no buffering of values, access directly to the clock registers
 */

/* No begin method needed */
void rtcbegin() {}

/* get the time from the registers */
uint16_t rtcget(uint8_t i) {
  
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
void rtcset(uint8_t i, uint16_t v) { 
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
#elif defined(HASBUILTINRTC) 
/*
 * Built-in clocks of STM32 and MKR are based on the RTCZero interface
 * an rtc object is created after loading the libraries
 * for NRENESAs the interface is slightly different
 */
#ifndef ARDUINO_ARCH_RENESAS
/* begin method */
void rtcbegin() {
  rtc.begin(); /* 24 hours mode */ 
}

/* get the time */
uint16_t rtcget(uint8_t i) { 
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
void rtcset(uint8_t i, uint16_t v) { 
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
/* NRENESA code, interface different to my great ennui! */
/* begin method */
void rtcbegin() {
  RTC.begin(); /* 24 hours mode */ 
}

/* get the time */
uint16_t rtcget(uint8_t i) { 
    RTC.getTime(rtc);
    switch (i) {
    case 0: 
      return rtc.getSeconds();
    case 1:
      return rtc.getMinutes();      
    case 2:
      return rtc.getHour();
    case 3:
      return static_cast<int>(rtc.getDayOfWeek());
    case 4:
      return rtc.getDayOfMonth();
    case 5:
      return Month2int(rtc.getMonth());
    case 6:
      return rtc.getYear();
    default:
      return 0;
   }
}

/* set the time */
void rtcset(uint8_t i, uint16_t v) { 
  RTC.getTime(rtc);
  switch (i) {
    case 0: 
      rtc.setSecond(v);
      break;
    case 1: 
      rtc.setMinute(v);
      break;    
    case 2:
      rtc.setHour(v);
      break;
    case 3:
      rtc.setDayOfWeek(static_cast<DayOfWeek>(v));
      break;
    case 4:
      rtc.setDayOfMonth(v);
      break;
    case 5:
      rtc.setMonthOfYear(static_cast<Month>(v-1));
      break;
    case 6:
      rtc.setYear(v);
      break;
    default:
      return; 
   }
   RTC.setTime(rtc);
}
#endif
#elif defined(ARDUINORTCEMULATION)
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
uint16_t rtcget(uint8_t i) { 
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
void rtcset(uint8_t i, uint16_t v) {
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
#elif defined(ARDUINO_ARCH_ESP32)
/* 
 * On ESP32 we use the builtin clock, this is a generic Unix time mechanism equivalent
 * to the code in hardware-posix.h
 */

/* no begin needed */
void rtcbegin() {}

/* get the time */
uint16_t rtcget(uint8_t i) { 
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
void rtcset(uint8_t i, uint16_t v) { 
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
uint16_t rtcget(uint8_t i) { return 0; }
void rtcset(uint8_t i, uint16_t v) { }
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

/* the buffers of the outgoing and incoming MQTT topic, they are char!! */ 
char mqtt_otopic[MQTTLENGTH];
char mqtt_itopic[MQTTLENGTH];

/* 
 * the buffers for MQTT messages, input and output goes 
 * through these static buffers. 
 */
#define MQTTBLENGTH 128
volatile char mqtt_buffer[MQTTBLENGTH];
volatile uint16_t mqtt_messagelength;
volatile char mqtt_obuffer[MQTTBLENGTH];
volatile uint16_t mqtt_charsforsend;

/* the name of the client, generated pseudo randomly to avoind 
		naming conflicts */
char mqttname[12] = "iotbasicxxx";
void mqttsetname() {
	uint32_t m = millis();
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
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOR4_WIFI)
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
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOR4_WIFI)
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
#elif defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_UNOR4_WIFI)
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
uint8_t netconnected() {
#ifdef ARDUINOETH
  return bethclient.connected();
#else
	return WiFi.status() == WL_CONNECTED;
#endif
}

/*
 * mqtt callback function, using the byte type here
 * because payload can be binary - interface to BASIC 
 * strings need to be done
 *
 * mqtt event handling in BASIC can be triggered here
 * we leave the types Arduino style
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
uint8_t mqttstat(uint8_t c) {
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
uint8_t mqttreconnect() {
	uint16_t timer=10;
	uint8_t reconnect=0;

/* all good and nothing to be done, we are connected */
	if (bmqtt.connected()) return 1;

/* try to reconnect the network */
  if (!netconnected()) { netreconnect(); bdelay(5000); }
  if (!netconnected()) return 0;
	
/* create a new random name right now */
	mqttsetname();

/* try to reconnect assuming that the network is connected */
	while (!bmqtt.connected() && timer < 400) {
    if (*mqtt_user == 0) {
      bmqtt.connect(mqttname);
    } else {
      bmqtt.connect(mqttname, mqtt_user, mqtt_passwd);
    }
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
uint8_t mqttstate() {
	return bmqtt.state();
}

/* subscribing to a topic  */
void mqttsubscribe(const char* t) {
	uint16_t i;

	for (i=0; i<MQTTLENGTH; i++) {
		if ((mqtt_itopic[i]=t[i]) == 0 ) break;
	}
	if (!mqttreconnect()) {ioer=1; return;};
	if (!bmqtt.subscribe(mqtt_itopic)) ioer=1;
}

void mqttunsubscribe() {
	if (!mqttreconnect()) {ioer=1; return;};
	if (!bmqtt.unsubscribe(mqtt_itopic)) ioer=1;
	*mqtt_itopic=0;
}

/*
 * set the topic we pushlish, coming from OPEN
 * BASIC can do only one topic.
 * 
 */
void mqttsettopic(const char *t) {
	uint16_t i;

	for (i=0; i<MQTTLENGTH; i++) {
		if ((mqtt_otopic[i]=t[i]) == 0 ) break;
	}
}

/* 
 *	print a mqtt message 
 *  we buffer until we reach either cr or until the buffer is full
 *	this is needed to do things like PRINT A,B,C as one message
 */
void mqttouts(const char *m, uint16_t l) {
	uint16_t i=0;

	while(mqtt_charsforsend < MQTTBLENGTH && i<l) mqtt_obuffer[mqtt_charsforsend++]=m[i++];
	if (mqtt_obuffer[mqtt_charsforsend-1] == '\n' || mqtt_charsforsend > MQTTBLENGTH) {
		if (!mqttreconnect()) {ioer=1; return;};
		if (!bmqtt.publish(mqtt_otopic, (uint8_t*) mqtt_obuffer, (unsigned int) mqtt_charsforsend-1, false)) ioer=1;
		mqtt_charsforsend=0;
	}
} 

/* the write command is simple */
void mqttwrite(const char c) { mqttouts(&c, 1); }

/* return the messagelength as avail */
uint16_t mqttavailable() {
    return mqtt_messagelength;
}

/* checkch is just a dummy */
char mqttcheckch() {
    if (mqtt_messagelength>0) return mqtt_buffer[0]; else return 0;
}

/* 
 * ins copies the buffer into a basic string 
 *  - behold the jabberwock - length gynmastics 
 */
uint16_t mqttins(char *b, uint16_t nb) {
  uint16_t z;
  
	for (z=0; z<nb && z<mqtt_messagelength; z++) b[z+1]=mqtt_buffer[z];
 	b[0]=z;
 	mqtt_messagelength=0;
 	*mqtt_buffer=0;
  return z;
}

/* just one character to emulate basic get */
char mqttread() {
	char ch=0;
	uint16_t i;

	if (mqtt_messagelength>0) {
		ch=mqtt_buffer[0];
		for (i=0; i<mqtt_messagelength-1; i++) mqtt_buffer[i]=mqtt_buffer[i+1];
    mqtt_messagelength--;
	}
	
	return ch;
}

#else 
void netbegin() {}
uint8_t netconnected() { return 0; }
void mqttbegin() {}
uint8_t mqttstate() {return 0;}
void mqttsubscribe(char *t) {}
void mqttsettopic(char *t) {}
void mqttouts(char *m, uint16_t l) {}
uint16_t mqttins(char *b, uint16_t nb) { return 0; };
char mqttread() {return 0;};
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


uint16_t elength() { 
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
  return EEPROMSIZE;
#endif
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_XMC) || defined(ARDUINO_ARCH_STM32) || defined(ARDUINO_ARCH_RENESAS)
  return EEPROM.length(); 
#endif
#ifdef ARDUINO_ARCH_LGT8F 
  return EEPROM.length(); /* on newer LGT8 cores, older ones don't have this, set 512 instead */
#endif
  return 0;
}

void eupdate(uint16_t a, int8_t c) { 
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

int8_t eread(uint16_t a) { 
#ifdef ARDUINO_ARCH_STM32
  return (int8_t) eeprom_buffered_read_byte(a); 
#else
  return (int8_t) EEPROM.read(a); 
#endif
}
#else 
#if defined(ARDUINOI2CEEPROM)
uint16_t elength() { 
  return i2ceepromsize;
}

void eupdate(uint16_t a, int8_t c) { 
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
    ioer=Wire.endTransmission();
  /* wait the max time */
    bdelay(5);
  }
#endif
}

int8_t eread(uint16_t a) { 
#ifdef ARDUINOI2CEEPROM_BUFFERED
  return (int8_t) EFSRAW.rawread(a); 
#else
/* read directly from the EEPROM */

/* set the address */
  Wire.beginTransmission(I2CEEPROMADDR);
  Wire.write((int)a/256);
  Wire.write((int)a%256);
  ioer=Wire.endTransmission();

/* read a byte */
  if (ert == 0) {
    Wire.requestFrom(I2CEEPROMADDR, 1);
    return (int8_t) Wire.read();
  } else return 0;
 
#endif
}
#else
/* no EEPROM present */
uint16_t elength() { return 0; }
void eupdate(uint16_t a, int8_t c) { return; }
int8_t eread(uint16_t a) { return 0; }
#endif
#endif

/* 
 *	the wrappers of the arduino io functions
 *  awrite requires ESP32 2.0.2 core, else disable awrite() 
 */ 

uint16_t aread(uint8_t p) { return analogRead(p); }
uint8_t dread(uint8_t p) { return digitalRead(p); }
void awrite(uint8_t p, uint16_t v){ analogWrite(p, v); }
void dwrite(uint8_t p, uint8_t v){ if (v) digitalWrite(p, HIGH); else digitalWrite(p, LOW); }

/* we normalize the pinMode as ESP32, ESP8266, and other boards behave rather
 *  differently. Following Arduino conventions we map 0 always to INPUT  
 *  and 1 always to OUTPUT, all the other numbers are passed through to the HAL 
 *  layer of the platform.
 *  Example: OUTPUT on ESP32 is 3 and 1 is assigned to INPUT.
 *  XMC also needs special treatment here.
 *  uint8_t is here for a reason.
 */
void pinm(uint8_t p, uint8_t m){
  if (m == 0) m=INPUT; 
  else if (m == 1) m=OUTPUT;
  pinMode(p,m);
}

/* wrapper around pulsein */
uint32_t pulsein(uint8_t pin, uint8_t val, uint32_t t) { 
  return pulseIn(pin, val, t);
}

/* write a pulse in microsecond range */
void pulseout(uint16_t unit, uint8_t pin, uint16_t duration, uint16_t val, uint16_t repetition, uint16_t interval) { 
  
  pinMode(pin, OUTPUT);
  while (repetition--) {
    digitalWrite(pin, val);
    delayMicroseconds(duration*unit);
    digitalWrite(pin, !val);
    delayMicroseconds(interval*unit);
  }
}

/* the BREAKPIN mechanism */
#if defined(BREAKPIN) && defined(INPUT_PULLUP)
void breakpinbegin() { pinm(BREAKPIN, INPUT_PULLUP); }
uint8_t getbreakpin() { return dread(BREAKPIN); } 
#else 
/* there is no pins hence no breakpin */
void breakpinbegin() {}
uint8_t getbreakpin() { return 1; } /* we return 1 because the breakpin is defined INPUT_PULLUP */
#endif


/*
 * A tone emulation based on the byield loop. The maximum frequency depends 
 * on the byield call speed. 10-20 kHz is possible. Will be unreliable if 
 * the loop functions need a lot of CPU cycles.
 */
#if defined(ARDUINOTONEEMULATION)
uint8_t tone_enabled;
uint8_t tone_pinstate = 0;
uint32_t tone_intervall;
uint32_t tone_micros;
uint16_t tone_duration;
uint32_t tone_start;
uint8_t tone_pin;

void playtone(uint8_t pin, uint16_t frequency, uint16_t duration, uint8_t volume) {

/* the pin we are using is reset every time this is called*/
  tone_pin=pin;
  pinMode(tone_pin, OUTPUT);
  digitalWrite(tone_pin, LOW);
  tone_pinstate=0;

/* duration 0 is a long tone */
  if (duration == 0) duration=32767;

/* frequency 0 stops playing */ 
  if (frequency == 0) {
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
#elif defined(ARDUINO_TTGO_T7_V14_Mini32) && defined(PS2FABLIB)
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
void playtone(uint8_t pin, uint16_t frequency, uint16_t duration, uint8_t volume) {

/* frequency 0 is off */
  if (frequency == 0) {
    soundGenerator.play(false);
    soundGenerator.clear();
    return;
  } 

/* duration 0 is a long tone */
  if (duration == 0) duration=60000;
  
  if (pin == 128) soundGenerator.playSound(SineWaveformGenerator(), frequency, duration, volume); 
  if (pin == 129) soundGenerator.playSound(SquareWaveformGenerator(), frequency, duration, volume);
  if (pin == 130) soundGenerator.playSound(SawtoothWaveformGenerator(), frequency, duration, volume);
  if (pin == 131) soundGenerator.playSound(TriangleWaveformGenerator(), frequency, duration, volume); 
  if (pin == 132) soundGenerator.playSound(VICNoiseGenerator(), frequency, duration, volume);
  if (pin == 133) soundGenerator.playSound(NoiseWaveformGenerator(), frequency, duration, volume);
  if (pin >= 255 && pin < 512 ) {
      pin=pin-255;
      SquareWaveformGenerator sqw;
      sqw.setDutyCycle(pin);
      soundGenerator.playSound(sqw, frequency, duration, volume); 
  }
}
#else 
/* playtone is just a wrapper around tone and noTone */
void playtone(uint8_t pin, uint16_t frequency, uint16_t duration, uint8_t volume) {
  if (frequency == 0) {
    noTone(pin);
  } else if (duration == 0) {
    tone(pin, frequency);
  } else {
    tone(pin, frequency, duration);
  }
}
#endif

/* 
 *	The byield function is called after every statement
 *	it allows two levels of background tasks. 
 *
 *	BASICBGTASK controls if time for background tasks is 
 * 	needed, usually set by hardware features
 *
 * 	YIELDINTERVAL by default is 32, generating a 32 ms call
 *		to the network loop function. 
 *
 * 	LONGYIELDINTERVAL by default is 1000, generating a one second
 *		call to maintenance functions. 
 */

uint32_t lastyield=0;
uint32_t lastlongyield=0;

void byield() { 

/* the fast ticker for all fast timing functions */
  fastticker();

/* the loop function for non BASIC stuff */
  bloop();

#if defined(BASICBGTASK)
/* yield all 32 milliseconds */
  if (millis()-lastyield > YIELDINTERVAL-1) {
    yieldfunction();
    lastyield=millis();
  }

/* yield every second */
  if (millis()-lastlongyield > LONGYIELDINTERVAL-1) {
    longyieldfunction();
    lastlongyield=millis();
  }
 #endif
 
 /* call the background task scheduler on some platforms implemented in hardware-* */
  yieldschedule();
}

/* delay must be implemented to use byield() while waiting */
void bdelay(uint32_t t) { 
  unsigned long i;
  if (t>0) {
    i=millis();
    while (millis() < i+t) byield();
  } 
}

/* fastticker is the hook for all timing functions */
void fastticker() {
/* fastticker profiling test code */
#ifdef FASTTICKERPROFILE
  fasttickerprofile();
#endif
/* toggle the tone pin */
#ifdef ARDUINOTONEEMULATION
  tonetoggle();
#endif
}

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
}

/* everything that needs to be done not so often - 1 second */
void longyieldfunction() {
#ifdef ARDUINOETH
  Ethernet.maintain();
#endif 
#ifdef BREAKINBACKGROUND
  if (checkch() == BREAKCHAR) breakcondition=1;
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

/* typical file name length */
#define FBUFSIZE 32
 
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(ESP32FAT) || defined(STM32SDIO)
File ifile;
File ofile;
char tempname[FBUFSIZE]; /* this is needed for the catalog code as these platforms do not give static C strings back */
#if defined(ARDUINOSD) || defined(STM32SDIO)
File root;
File file;
#ifdef ARDUINO_ARCH_ESP32
const char rootfsprefix[2] = "/";
#else
const char rootfsprefix[1] = "";
#endif
#endif
#if defined(ESPSPIFFS) || defined(ESP32FAT)
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
#define FILE_OWRITE (O_READ | O_WRITE | O_CREAT | O_TRUNC)
/* this is the ESP8266 definition for kernel 3.0, 3.1 has fixed this */
/* #define FILE_OWRITE (sdfat::O_READ | sdfat::O_WRITE | sdfat::O_CREAT | sdfat::O_TRUNC) */
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

/* the buildin file system for ro access */
#ifdef HASBUILDIN
char* buildin_ifile = 0;
uint16_t buildin_ifilepointer = 0;
char* buildin_file = 0;
char* buildin_pgm = 0;
uint8_t buildin_rootactive = 0;
uint16_t buildin_rootpointer = 0;
char buildin_tempname[FBUFSIZE]; /* this is needed for the catalog code as strings need to be copied from progmem */
#endif


/* these filesystems may have a path prefixes, we treat STM32SDIO like an SD here */
#if defined(RP2040LITTLEFS) || defined(ESPSPIFFS) || defined(ESP32FAT) || defined(ARDUINOSD) 
char tmpfilename[10+FBUFSIZE];

/* add the prefix to the filename */
char* mkfilename(const char* filename) {
	uint8_t i,j;
	for(i=0; i<10 && rootfsprefix[i]!=0; i++) tmpfilename[i]=rootfsprefix[i];
	tmpfilename[i++]='/';
	for(j=0; j<FBUFSIZE && filename[j]!=0; j++) tmpfilename[i++]=filename[j];
	tmpfilename[i]=0;
	return tmpfilename;
}

/* remove the prefix from the filename */
const char* rmrootfsprefix(const char* filename) {
	uint8_t i=0;
	while (filename[i] == rootfsprefix[i] && rootfsprefix[i] !=0 && filename[i] !=0 ) i++;
	if (filename[i]=='/') i++;
	return &filename[i];
}
#endif

/* 
 *	filesystem starter for SPIFFS and SD on ESP, ESP32 and Arduino plus LittleFS
 *	the verbose option needs to be checked .
 *  
 *  Also here the driver for the buildin file system.
 * 
 * if SDPIN is empty it is the standard SS pin of the platfrom
 * 
 */

uint8_t fsstart;
 
void fsbegin() {
#ifdef ARDUINOSD 
#ifndef SDPIN
#define SDPIN
#endif
  if (SD.begin(SDPIN)) fsstart=1; else fsstart=0;	
#endif
#ifdef STM32SDIO
  static int fsbegins = 0;
  while (fsbegins<100) { 
    if (SD.begin(SD_DETECT_PIN)) { fsstart=1; break; } else fsstart=0;
    bdelay(20); 
    fsbegins++;
  } 
#endif
#if defined(ESPSPIFFS) && defined(ARDUINO_ARCH_ESP8266) 
 	if (SPIFFS.begin()) fsstart=1; else fsstart=0;
#endif
#if defined(ESPSPIFFS) && defined(ARDUINO_ARCH_ESP32) 
 	if (SPIFFS.begin()) fsstart=1; else fsstart=0;
#endif
#if defined(ESP32FAT) && defined(ARDUINO_ARCH_ESP32)
  if (FFat.begin()) fsstart=1; else fsstart=0;
#endif
#ifdef RP2040LITTLEFS
	myFS = new LittleFS_MBED();
	if (myFS->init()) fsstart=1; else fsstart=0;
#endif
#ifdef ARDUINOEFS
	uint8_t s=EFS.begin();
	if (s>0) {
		fsstart=s;
	} else {
		if (EFS.format(32)) fsstart=32; else fsstart=0;;
	}
#endif
#ifdef HASBUILDIN
/* hopefully nothing to be done */
#endif
}

uint8_t fsstat(uint8_t c) { 
#if defined(FILESYSTEMDRIVER)
  if (c == 0) return 1;
  if (c == 1) return fsstart;
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
#if defined(ARDUINOSD) || defined(ESPSPIFFS)||defined(ESP32FAT)||defined(STM32SDIO)
	if (ofile) { ofile.write(c); return; }
#endif
#if defined(RP2040LITTLEFS)
	if (ofile) { fputc(c, ofile); return; }
#endif
#if defined(ARDUINOEFS)
	if (ofile) { 
    if (!EFS.fputc(c, ofile)) ioer=-1; 
    return;
  }
#endif
/* if there is no open ofile, set the ioerror flag to notify BASIC about it */
  ioer=1;
}

char fileread() {
	char c;

/* the buildin file is active, we handle this first, else we allow for another FS */
#if defined(HASBUILDIN)
  if (buildin_ifile != 0) { 
    c=pgm_read_byte(buildin_ifile+buildin_ifilepointer);
    if (c != '\f') buildin_ifilepointer++; else { ioer=-1; c=255; }
    return c;
  }
#endif
#if defined(ARDUINOSD) || defined(ESPSPIFFS)||defined(ESP32FAT)||defined(STM32SDIO)
	if (ifile) c=ifile.read(); else { ioer=1; return 0; }
	if (c == -1 || c == 255) ioer=-1;
	return c;
#endif
#ifdef RP2040LITTLEFS
	if (ifile) c=fgetc(ifile); else { ioer=1; return 0; }
	if (c == -1 || c == 255) ioer=-1;
	return c;
#endif
#ifdef ARDUINOEFS
	if (ifile) c=EFS.fgetc(ifile); else { ioer=1; return 0; }
	if (c == -1|| c == 255) ioer=-1;
	return c;
#endif
	return 0;
}

int fileavailable(){
/* for the buildin file \f is the end of file, return true is it has not been reached */
#if defined(HASBUILDIN)
  char c;
  if (buildin_ifile != 0) { 
    c=pgm_read_byte(buildin_ifile+buildin_ifilepointer);
    if (c != '\f') return 1; else return 0;
  }
#endif
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(ESP32FAT) || defined(STM32SDIO)
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

uint8_t ifileopen(const char* filename){
/* we first try to open a file on the builin file system, then of the other ones */
#if defined(HASBUILDIN)
  byte i, file;
  char* name;

/* opening a new file */
  buildin_ifilepointer=0;
  buildin_ifile=0;

  file=0;
  while(1) {

/* find a name in progmem, 0 if no more name */
    name=(char*)pgm_read_ptr(&buildin_program_names[file]);    
    if (name == 0) break;

/* compare the name with the filename */  
    for(i=0;i<32;i++) {
    char c=pgm_read_byte(&name[i]);

/* we are at the end of the name, if filename is also ending, we got it */
      if (c == 0 && filename[i] == 0) {
        buildin_ifile=(char*)pgm_read_ptr(&buildin_programs[file]);
        return (buildin_ifile != 0);
      }

/* a character mismatch means we need to go to the next name */
      if (c != filename[i]) {
          file++;
          break;  
      }
    }
  }
#endif
#if defined(ARDUINOSD)
	ifile=SD.open(mkfilename(filename), FILE_READ);
	return ifile != 0;
#endif
#if defined(STM32SDIO)
  ifile=SD.open(filename);
  return ifile != 0;
#endif
#ifdef ESPSPIFFS
	ifile=SPIFFS.open(mkfilename(filename), "r");
	return ifile != 0;
#endif
#ifdef ESP32FAT
  ifile=FFat.open(mkfilename(filename), "r"); 
  return ifile != 0;
#endif
#ifdef RP2040LITTLEFS
	ifile=fopen(mkfilename(filename), "r");
	return ifile != 0;
#endif
#ifdef ARDUINOEFS
	ifile=EFS.fopen(filename, "r");
	return ifile != 0;
#endif
	return 0;
}

void ifileclose(){
#if defined(HASBUILDIN)
  if (buildin_ifile) {
    buildin_ifile=0;
    buildin_ifilepointer=0;   
  }
#endif
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(ESP32FAT) || defined(STM32SDIO)
	if (ifile) ifile.close();
#endif	
#ifdef RP2040LITTLEFS
	if (ifile) fclose(ifile);
#endif
#ifdef ARDUINOEFS
	if (ifile) EFS.fclose(ifile);	
#endif

}

uint8_t ofileopen(const char* filename, const char* m){
#if defined(ARDUINOSD) 
	if (*m == 'w') ofile=SD.open(mkfilename(filename), FILE_OWRITE);
/* ESP32 has FILE_APPEND defined */
#ifdef FILE_APPEND
  if (*m == 'a') ofile=SD.open(mkfilename(filename), FILE_APPEND);
#else
	if (*m == 'a') ofile=SD.open(mkfilename(filename), FILE_WRITE);
#endif
	return ofile != 0;
#endif
#if  defined(STM32SDIO)
  if (*m == 'w') ofile=SD.open(filename, FILE_OWRITE);
/* ESP32 has FILE_APPEND defined */
#ifdef FILE_APPEND
  if (*m == 'a') ofile=SD.open(filename, FILE_APPEND);
#else
  if (*m == 'a') ofile=SD.open(filename, FILE_WRITE);
#endif
  return ofile != 0;
#endif
#ifdef ESPSPIFFS
	ofile=SPIFFS.open(mkfilename(filename), m);
	return ofile != 0;
#endif
#ifdef ESP32FAT
  ofile=FFat.open(mkfilename(filename), m);
  return ofile != 0;
#endif
#ifdef RP2040LITTLEFS
	ofile=fopen(mkfilename(filename), m);
	return ofile != 0; 
#endif
#ifdef ARDUINOEFS
	ofile=EFS.fopen(filename, m);
	return ofile != 0; 
#endif
/* if no other filesystem has satisfied the write open BUILDIN reports the error */
#if defined(HASBUILDIN)
  ioer=-1;
  return 0;
#endif
	return 0;
}

void ofileclose(){
#if defined(ARDUINOSD) || defined(ESPSPIFFS)||defined(ESP32FAT)||defined(STM32SDIO)
  if (ofile) ofile.close();
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
/* we first open the buildin and remember we are processing it, if there is another root, we also open it */
#ifdef HASBUILDIN
  buildin_rootpointer=0;
  buildin_rootactive=1;
#endif
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
#ifdef ESP32FAT
  root=FFat.open("/");
#endif
#ifdef RP2040LITTLEFS
	root=opendir(rootfsprefix);
#endif
#ifdef ARDUINOEFS
	EFS.dirp=0;
#endif
}

uint8_t rootnextfile() {
/* we first iterate through the buildins, then fall through to the other filesystems */
#ifdef HASBUILDIN
  if (buildin_rootactive) {
    buildin_file=(char*)pgm_read_ptr(&buildin_program_names[buildin_rootpointer]);
    if (buildin_file != 0) {
      buildin_pgm=(char*)pgm_read_ptr(&buildin_programs[buildin_rootpointer]);
      buildin_rootpointer++;
      return 1;
    } else {
      buildin_rootactive=0;
    }
  }
#endif
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
#ifdef ESP32FAT
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

uint8_t rootisfile() {
#ifdef HASBUILDIN
  if (buildin_rootactive) return 1;
#endif
#if defined(ARDUINOSD) || defined(STM32SDIO)
	return (!file.isDirectory());
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
	return 1;
#endif
#ifdef ARDUINO_ARCH_ESP32
	return (! file.isDirectory());
#endif
#endif
#ifdef ESP32FAT
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
#ifdef HASBUILDIN
  if (buildin_rootactive) {
    for(int i=0; i<FBUFSIZE; i++) {
      buildin_tempname[i]=(char)pgm_read_byte(&buildin_file[i]);
      if (buildin_tempname[i] == 0) return buildin_tempname;
    }
  }
#endif
#if defined(ARDUINOSD)
  return rmrootfsprefix(file.name());
#endif
#if defined(STM32SDIO)
  return (char*) file.name();
#endif
#ifdef ESPSPIFFS
#ifdef ARDUINO_ARCH_ESP8266
  int i=0;
	String s=root.fileName();
 	for (i=0; i<s.length(); i++) tempname[i]=s[i];
 	tempname[i]=0;
	return rmrootfsprefix(tempname);
#endif
#ifdef ARDUINO_ARCH_ESP32
	return rmrootfsprefix(file.name());
#endif
#endif
#ifdef ESP32FAT
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

uint32_t rootfilesize() {
#ifdef HASBUILDIN
  int i=0;
  if (buildin_rootactive && buildin_pgm) {
    for (;;i++) {
      char ch=pgm_read_byte(&buildin_pgm[i]);
      if (ch == 0) return i;
    }
  }
#endif
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(ESP32FAT) || defined(STM32SDIO)
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
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(ESP32FAT) || defined(STM32SDIO)
  file.close();
#endif 
#ifdef RP2040LITTLEFS
#endif
#ifdef ARDUINOEFS
#endif
}

void rootclose(){
#ifdef HASBUILDIN
  buildin_rootactive=0;
#endif
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
#ifdef ESP32FAT
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
void removefile(const char *filename) {
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
#ifdef ESP32FAT
  FFat.remove(mkfilename(filename));
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
void formatdisk(uint8_t i) {
#if defined(ARDUINOSD) || defined(STM32SDIO)	
	return;
#endif
#ifdef ESPSPIFFS
	if (SPIFFS.format()) { SPIFFS.begin(); fsstart=1; } else { fsstart=0; }
#endif
#ifdef ESP32FAT
  FFat.end();
  if (FFat.format()) { FFat.begin(); fsstart=1; } else { fsstart=0; }
#endif
#ifdef RP2040LITTLEFS
  fs.reformat(&bd); /* bd: this comes from the header */
	return;
#endif
#ifdef ARDUINOEFS
	if (i>0) {
		if (EFS.format(i)) { EFS.begin(); fsstart=1; } else { fsstart=0; }
	} else ioer=1;
	return;
#endif
}

/* 
 *  The buffer code, a simple buffer to store output and 
 *  input data. It can be used as a device in BASIC using the 
 *  modifier &0. 
 */

/* use the input buffer variable from BASIC here, it is extern to runtime */
void bufferbegin() {}

/* write to the buffer, works only until 127 
  uses vt52 style commands to handle the buffer content*/
void bufferwrite(char c) {
  if (!nullbuffer) return;
  switch (c) {
  case 12: /* clear screen */
    nullbuffer[nullbuffer[0]+1]=0;
    nullbuffer[0]=0;
    break;
  case 10: 
  case 13: /* cr and lf ignored */
    break;
  case 8: /* backspace */
    if (nullbuffer[0]>0) nullbuffer[0]--;
    break;
  default:
    if (nullbuffer[0] < nullbufsize-1 && nullbuffer[0] < 127) {
      nullbuffer[++nullbuffer[0]]=c;
      nullbuffer[nullbuffer[0]+1]=0; /* null terminate */
    }    
    break;
  }
}

/* read not needed right now */
char bufferread() { return 0; }
uint16_t bufferavailable() { return 0; }
char buffercheckch() { return 0; }
void bufferflush() { }
uint16_t bufferins(char *b, uint16_t nb) { return 0; }

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
#ifdef ARDUINOPICOSERIAL
volatile char picochar;
volatile uint8_t picoa = 0;
volatile char* picob = NULL;
int16_t picobsize = 0;
volatile int16_t picoi = 1;
volatile uint16_t picoz; /* ugly */

/* this is mostly taken from PicoSerial */
const uint16_t MIN_2X_BAUD = F_CPU/(4*(2*0XFFF + 1)) + 1;

/* the begin code */
void picobegin(uint32_t baud) {
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
	if (picob && (!picoa)) {
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
			picoz=picoi;
			picoi=1;
		}
		picochar=0; /* every buffered byte is deleted */
	} else {
    	if (c != 10) picochar=c;
	}
}

/* the ins code of picoserial, called like this in consins */
uint16_t picoins(char *b, uint16_t nb) {
  picob=b;
  picobsize=nb;
  picoa=0;
/* once picoa is set, the interrupt routine records characters 
 *  until a cr and then resets picoa to 0 */
  while (!picoa) byield();
  outch(10);
  return picoz;
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
#ifdef ARDUINOPICOSERIAL
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
#ifdef ARDUINOPICOSERIAL
	picobegin(serial_baudrate); 
#else
	SERIALPORT.begin(serial_baudrate);
#endif
	bdelay(1000);
}

/* state information on the serial port */
uint8_t serialstat(uint8_t c) {
  if (c == 0) return 1;
  if (c == 1) return serial_baudrate;
  return 0;
}

/* write to a serial stream */
void serialwrite(char c) {
#ifdef ARDUINOPICOSERIAL
	picowrite(c);
#else
/* write never blocks. discard any bytes we can't get rid of */
  SERIALPORT.write(c);  
/* if (Serial.availableForWrite()>0) Serial.write(c);	*/
#endif
}

/* check on a character, needed for breaking */
char serialcheckch() {
#ifdef ARDUINOPICOSERIAL
	return picochar;
#else
	if (SERIALPORT.available()) return SERIALPORT.peek(); else return 0; // should really be -1
#endif	
}

/* avail method, needed for AVAIL() */ 
uint16_t serialavailable() {
#ifdef ARDUINOPICOSERIAL
	return picoi;
#else
	return SERIALPORT.available();
#endif	
}

/* flush serial */
void serialflush() {
#ifdef ARDUINOPICOSERIAL
  return;
#else
  while (SERIALPORT.available()) SERIALPORT.read();
#endif 
}

/* the serial ins */
uint16_t serialins(char *b, uint16_t nb) {
#ifdef ARDUINOPICOSERIAL
  return picoins(b, nb);
#else
  return consins(b, nb);
#endif  
}

/* 
 * Start a secondary serial port for printing and/or networking 
 * This is either Serial1 on a MEGA or DUE or Nano Every or a SoftwareSerial 
 * instance
 */
#ifdef ARDUINOPRT
#if !defined(ARDUINO_AVR_MEGA2560) && !defined(ARDUINO_SAM_DUE) && !defined(ARDUINO_AVR_NANO_EVERY) \
    && !defined(ARDUINO_NANO_RP2040_CONNECT) && !defined(ARDUINO_RASPBERRY_PI_PICO) \
    && !defined(ARDUINO_SEEED_XIAO_M0) && !defined(ARDUINO_ARCH_RENESAS)
#include <SoftwareSerial.h>
SoftwareSerial PRTSERIAL(SOFTSERIALRX, SOFTSERIALTX);
#endif

/* second serial port */
void prtbegin() {
	PRTSERIAL.begin(serial1_baudrate);
}

/* the open functions are not needed here */
char prtopen(char* filename, uint16_t mode) {}
void prtclose() {}

uint8_t prtstat(uint8_t c) {
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

char prtcheckch() {
	if (PRTSERIAL.available()) return PRTSERIAL.peek(); else return 0;
}

uint16_t prtavailable() {
	return PRTSERIAL.available();
}

void prtset(uint32_t s) {
  serial1_baudrate=s;
  prtbegin();
}

uint16_t prtins(char* b, uint16_t nb) {
    if (blockmode > 0) return inb(b, nb); else return consins(b, nb);
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
#if defined(HASWIRE) || defined(HASSIMPLEWIRE)
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
#if defined(HASWIRE)
uint8_t wire_slaveid = 0;
uint8_t wire_myid = 0;
#define ARDUINOWIREBUFFER 32
#ifdef ARDUINOWIRESLAVE
char wirereceivebuffer[ARDUINOWIREBUFFER];
short wirereceivechars = 0;
char wirerequestbuffer[ARDUINOWIREBUFFER];
short wirerequestchars = 0;
#endif

void wireslavebegin(uint8_t s) {
#ifndef SDA_PIN
  Wire.begin(s);
#else
  Wire.begin(SDA_PIN, SCL_PIN, s);
#endif
}

/* wire status - just checks if wire is compiled */
uint8_t wirestat(uint8_t c) {
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
uint16_t wireavailable() {
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
void wireopen(char s, uint8_t m) {
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
	} 
}

/* input an entire string */
uint16_t wireins(char *b, uint8_t l) {
  uint16_t z;
  if (wire_myid == 0) {
    z=0;
    if (l>ARDUINOWIREBUFFER) l=ARDUINOWIREBUFFER;
    if (!Wire.requestFrom(wire_slaveid, l)) ioer=1;
    while (Wire.available() && z<l) b[++z]=Wire.read();
  } else {
#ifdef ARDUINOWIRESLAVE
    for (z=0; z<wirereceivechars && z<l; z++) b[z+1]=wirereceivebuffer[z];
    wirereceivechars=0; /* clear the entire buffer on read, bytewise read not really supported */
#endif
  }
  b[0]=z;
  return z;
}

/* just a helper to make GET work, wire is string oriented */
char wireread() {
    char wbuffer[2];
    if (wireins(wbuffer, 1)) return wbuffer[1]; else return 0;
}

/* send an entire string - truncate radically */
void wireouts(char *b, uint8_t l) {
  int16_t z;
  if (l>ARDUINOWIREBUFFER) l=ARDUINOWIREBUFFER; 
  if (wire_myid == 0) {
    Wire.beginTransmission(wire_slaveid); 
#ifdef ARDUINO_ARCH_ESP32
    for(z=0; z<l; z++) Wire.write(b[z]);  
#else
    Wire.write(b, l);
#endif
    ioer=Wire.endTransmission(); 
  } else {
#ifdef ARDUINOWIRESLAVE
    for (int i=0; i<l; i++) wirerequestbuffer[i]=b[i];
    wirerequestchars=l;
#endif
  }
}
#endif

/* plain wire without FILE I/O functions */
#if defined(HASSIMPLEWIRE) || defined(HASWIRE)
/* single byte access functions on Wire */
int16_t wirereadbyte(uint8_t port) { 
    if (!Wire.requestFrom(port,(uint8_t) 1)) ioer=1;
    return Wire.read();
}
void wirewritebyte(uint8_t port, int16_t data) { 
  Wire.beginTransmission(port); 
  Wire.write(data);
  Wire.endTransmission();
}
void wirewriteword(uint8_t port, int16_t data1, int16_t data2) {
  Wire.beginTransmission(port); 
  Wire.write(data1);
  Wire.write(data2);
  Wire.endTransmission();
}
#endif

/*
#ifndef HASWIRE
void wirebegin() {}
uint8_t wirestat(uint8_t c) {return 0; }
void wireopen(char s, uint8_t m) {}
uint16_t wireins(char *b, uint8_t l) { b[0]=0; return 0; }
void wireouts(char *b, uint8_t l) {}
uint16_t wireavailable() { return 0; }
int16_t wirereadbyte(uint8_t port) { return 0; }
void wirewritebyte(uint8_t port, int16_t data) { return; }
void wirewriteword(uint8_t port, int16_t data1, int16_t data2) { return; }
#endif
*/

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
uint8_t radiostat(uint8_t c) {
  if (c == 0) return 1;
  if (c == 1) return radio.isChipConnected();
  return 0; 
}

/* generate a uint64_t pipe address from the filename string for RF24 */
uint64_t pipeaddr(const char *f){
	uint64_t t = 0;
	t=(uint8_t)f[0];
	for(short i=1; i<=4; i++) t=t*256+(uint8_t)f[i];
	return t;
} 


/* read an entire string */
uint16_t radioins(char *b, uint8_t nb) {
    uint16_t z;
    
    if (radio.available()) {
    	radio.read(b+1, nb);
    	if (!blockmode) {
        for (z=0; z<nb; z++) if (b[z+1]==0) break;		
    	} else {
    		z=radio.getPayloadSize();
      	if (z > nb) z=nb;
    	}
      b[0]=z;
	} else {
    b[0]=0; 
    b[1]=0;
    z=0;
	}
  return z;
}


 /* radio is not character oriented, this is only added to make GET work
    or single byte payloads, radio, like file is treated nonblocking here */   
char radioread() {
    char rbuffer[2];
    if (radioins(rbuffer, 1)) return rbuffer[1]; else return 0;
}

/* write to radio, no character mode here */
void radioouts(char *b, uint8_t l) {
	radio.stopListening();
	if (!radio.write(b, l)) ioer=1;
	radio.startListening();
}

/* radio available */
uint16_t radioavailable() {
  return radio.available();
}

/* 
 *	we always read from pipe 1 and use pipe 0 for writing, 
 *	the filename is the pipe address, by default the radio 
 *	goes to reading mode after open and is only stopped for 
 *	write
 */
void iradioopen(const char *filename) {
	if (!radio.begin()) ioer=1;
	radio.openReadingPipe(1, pipeaddr(filename));
	radio.startListening();
}

void oradioopen(const char *filename) {
	if (!radio.begin()) ioer=1;
	radio.openWritingPipe(pipeaddr(filename));
}

void radioset(uint8_t s) {
  if ((s<0) && (s>3)) {ioer=1; return; } 
  rf24_pa=(rf24_pa_dbm_e) s;
  radio.setPALevel(rf24_pa);
}

#else 
uint8_t radiostat(uint8_t c) { return 0; }
uint64_t pipeaddr(const char *f){ return 0; } 
uint16_t radioins(char *b, uint8_t nb) { b[0]=0; b[1]=0; return 0; }
void radioouts(char *b, uint8_t l) {}
uint16_t radioavailable() { return 0; }
void iradioopen(const char *filename) {}
void oradioopen(const char *filename) {}
void radioset(uint8_t s) {}
#endif


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

float sensorread(uint8_t s, uint8_t v) {
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
float sensorread(uint8_t s, uint8_t v) {return 0;};
#endif

/* 
 *  event handling wrappers, to keep Arduino specifics out of BASIC
 */

#ifdef ARDUINOINTERRUPTS
uint8_t pintointerrupt(uint8_t pin) { return digitalPinToInterrupt(pin); }
void attachinterrupt(uint8_t interrupt, void (*f)(), uint8_t mode) { attachInterrupt(interrupt, f, (PinStatus) mode); };
void detachinterrupt(uint8_t pin) { detachInterrupt(digitalPinToInterrupt(pin)); };
#else 
uint8_t pintointerrupt(uint8_t pin) { return 0; }
void attachinterrupt(uint8_t interrupt, void (*f)(), uint8_t mode) {  };
void detachinterrupt(uint8_t pin) {  };
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
uint16_t spirambegin() {
  pinMode(RAMPIN, OUTPUT);
  digitalWrite(RAMPIN, LOW);
  SPI.transfer(SPIRAMRSTIO);
  SPI.transfer(SPIRAMWRMR);
  //SPI.transfer(SPIRAMBYTE);
  SPI.transfer(SPIRAMSEQ);
  digitalWrite(RAMPIN, HIGH);
  return 65535;
}

/* the simple unbuffered byte read, with a cast to signed 8 bit integer */
int8_t spiramrawread(uint16_t a) {
  uint8_t c;
  digitalWrite(RAMPIN, LOW);
  SPI.transfer(SPIRAMREAD);
  SPI.transfer((uint8_t)(a >> 8));
  SPI.transfer((uint8_t)a);
  c = SPI.transfer(0x00);
  digitalWrite(RAMPIN, HIGH);
  return c;
}

/* one read only buffer for access from the token stream 
    memread calls this */
uint16_t spiram_robufferaddr = 0;
uint8_t spiram_robuffervalid=0;
const uint8_t spiram_robuffersize = 32;
int8_t spiram_robuffer[spiram_robuffersize];

/* one rw buffer for access to the heap and all the program editing 
    memread2 and memwrite2 call this*/
uint16_t spiram_rwbufferaddr = 0;
uint8_t spiram_rwbuffervalid=0;
const uint8_t spiram_rwbuffersize = 32; /* also change the addressmask if you want to play here */
int8_t spiram_rwbuffer[spiram_rwbuffersize];
uint8_t spiram_rwbufferclean = 1; 

const uint16_t spiram_addrmask=0xffe0;
/* const address_t spiram_addrmask=0xffd0; // 64 byte frame */

/* the elementary buffer access functions used almost everywhere */

void spiram_bufferread(uint16_t a, int8_t* b, uint16_t l) {
  digitalWrite(RAMPIN, LOW);
  SPI.transfer(SPIRAMREAD);
  SPI.transfer((uint8_t)(a >> 8));
  SPI.transfer((uint8_t)a);
  SPI.transfer(b, l);
  digitalWrite(RAMPIN, HIGH);
} 

void spiram_bufferwrite(uint16_t a, int8_t* b, uint16_t l) {
  digitalWrite(RAMPIN, LOW); 
  SPI.transfer(SPIRAMWRITE);
  SPI.transfer((uint8_t)(a >> 8));
  SPI.transfer((uint8_t)a);
  SPI.transfer(b, l);
  digitalWrite(RAMPIN, HIGH);
}

int8_t spiram_robufferread(uint16_t a) {
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

int8_t spiram_rwbufferread(uint16_t a) {
/* page fault, do read, ignore the ro buffer buffer */
  uint16_t p=a & spiram_addrmask;
  if (!spiram_rwbuffervalid || (p != spiram_rwbufferaddr)) {
/* flush the buffer if needed */
    spiram_rwbufferflush();
/* and reload it */
    spiram_bufferread(p, spiram_rwbuffer, spiram_rwbuffersize);
    spiram_rwbufferaddr=p; /* we only handle full pages here */
    spiram_rwbuffervalid=1;
  }
  return spiram_rwbuffer[a-spiram_rwbufferaddr];
}

/* the buffered file write */
void spiram_rwbufferwrite(uint16_t a, int8_t c) {
  uint16_t p=a&spiram_addrmask;
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
void spiramrawwrite(uint16_t a, int8_t c) {
  digitalWrite(RAMPIN, LOW);
  SPI.transfer(SPIRAMWRITE);
  SPI.transfer((uint8_t)(a >> 8));
  SPI.transfer((uint8_t)a);
  SPI.transfer((uint8_t) c);
  digitalWrite(RAMPIN, HIGH);
/* also refresh the ro buffer if in the frame */
  if (a >= spiram_robufferaddr && a < spiram_robufferaddr + spiram_robuffersize && spiram_robufferaddr > 0) 
    spiram_robuffer[a-spiram_robufferaddr]=c;
/* and the rw buffer if needed) */
  if (a >= spiram_rwbufferaddr && a < spiram_rwbufferaddr + spiram_rwbuffersize && spiram_rwbufferaddr > 0) 
    spiram_rwbuffer[a-spiram_rwbufferaddr]=c;
}
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
 * This code measures the fast ticker frequency in microseconds 
 * It leaves the data in variable F. Activate this only for test 
 * purposes.
 */

#ifdef FASTTICKERPROFILE
uint32_t lastfasttick = 0;
uint32_t fasttickcalls = 0;
uint16_t avgfasttick = 0;
int32_t devfasttick = 0;

void fasttickerprofile() {
  if (lastfasttick == 0) { lastfasttick=micros(); return; }
  int delta=micros()-lastfasttick;
  lastfasttick=micros();
  avgfasttick=(avgfasttick*fasttickcalls+delta)/(fasttickcalls+1);
  fasttickcalls++; 
}
#endif
