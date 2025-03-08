/*
 * Stefan's basic interpreter.  
 *
 * Playing around with frugal programming. See the licence file on 
 * https://github.com/slviajero/tinybasic for copyright/left.
 *   (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 * Author: Stefan Lenz, sl001@serverfabrik.de
 * 
 * Architectures and the definitions from the Arduino IDE. 
 * 
 * Arduino hardware settings , set here what you need or
 * use one of the predefined configurations below.
 * 
 * If HARDWAREHEURISTICS is set, some of the settings below 
 * are activated automatically for some platforms. If you want
 * to control ALL settings below manually, undef HARDWAREHEURISTICS 
 *
 * Input/output methods ARUINOPICOSERIAL, ARDUINOPS2
 *  ARDUINOUSBKBD, ARDUINOZX81KBD, ARDUINOI2CKBD, GIGAUSBKBD
 *	ARDUINOPRT, DISPLAYCANSCROLL, ARDUINOLCDI2C,
 *	ARDUINOTFT, ARDUINONOKIA51, ARDUINOILI9488,
 *  ARDUINOSSD1306, ARDUINOMCUFRIEND
 * Storage ARDUINOEEPROM, ARDUINOSD, ESPSPIFFS, RP2040LITTLEFS
 * storage ARDUINOEFS, SM32SDIO, ESP32FAT
 * sensors ARDUINOWIRE, ARDUINOSIMPLEWIRE, ARDUINOSENSORS
 * network ARDUINORF24, ARDUNIOMQTT 
 * memory ARDUINOSPIRAM
 * real time clocks ARDUINORTC, ARDUINORTCEMULATION
 * 
 * Secial features
 *  ARDUINOPGMEEPROM: makes the EEPROM primary program store
 *  HASMSTAB: counts characters, prerequisite for MSTAB in languages
 *  BUILDIN: include buildin programs 
 *  
 * Instead of setting all this manually, you can use one of the 
 * predefined hardware models. They are stored in board and are sources
 * here later in the code to override some of the defaults. Custom configurations
 * coming with BASIC are: 
 * 
 *  dummy.h: 
 *      do nothing.
 *  avrlcd.h:
 *      a AVR system with a LCD shield
 *  wemosshield.h: 
 *      a ESP8266 UNO lookalike with a modified datalogger shield
 *      great hardware for small BASIC based IoT projects.
 *  megashield.h: 
 *      an Arduino Mega with Ethernet Shield optional keyboard 
 *      and i2c display
 *  megatft.h: 
 *  duetft.h:
 *      TFT 7inch screen systems, standalone
 *  nanoboard.h:
 *      Arduino Nano Every board with PS2 keyboard and sensor 
 *      kit.
 *  megaboard.h:
 *      A board for the MEGA with 64 kB RAM, SD Card, and real time
 *      clock.
 *  unoboard.h:
 *      A board for an UNO with 64kB memory and EEPROM disk
 *      fits into an UNO flash only with integer
 *  esp01board.h:
 *      ESP01 based board as a sensor / MQTT interface
 * rp2040board.h:
 *      A ILI9488 hardware design based on an Arduino connect RP2040.
 * rp2040board2.h:
 *      like the one above but based on the Pi Pico core
 * esp32board.h:
 *      same like above with an ESP32 core
 * mkboard.h:
 *      a digital signage and low energy board
 * tdeck.h: 
 *      the LILYGO T-Deck - not yet finished
 * ttgovga.h:
 *      the TTGO VGA 1.4 board with VGA output
 */
#define PREDEFINEDBOARD "boards/dummy.h"

/* undef this if you really want it */
#define HARDWAREHEURISTICS

#undef ARDUINOPICOSERIAL 
#undef ARDUINOPS2
#undef ARDUINOUSBKBD
#undef ARDUINOZX81KBD
#undef ARDUINOI2CKBD
#undef GIGAUSBKBD
#undef ARDUINOPRT
#define DISPLAYCANSCROLL
#define ARDUINOLCDI2C
#undef ARDUINONOKIA51
#undef ARDUINOILI9488
#undef ARDUINOSSD1306
#undef ARDUINOMCUFRIEND
#undef ARDUINOEDP47
#undef ARDUINOGRAPHDUMMY
#undef LCDSHIELD
#undef ARDUINOTFT
#undef ARDUINOVGA
#undef TFTESPI
#undef ARDUINOEEPROM
#define ARDUINOI2CEEPROM
#undef ARDUINOEFS
#undef ARDUINOSD
#undef ESPSPIFFS
#undef ESP32FAT
#undef RP2040LITTLEFS
#undef STM32SDIO
#undef GIGAUSBFS
#undef ARDUINORTC
#undef ARDUINORTCEMULATION
#undef ARDUINOTONEEMULATION
#undef ARDUINOWIRE
#undef ARDUINOSIMPLEWIRE
#undef ARDUINOWIRESLAVE
#undef ARDUINORF24
#undef ARDUINOETH
#undef ARDUINOMQTT 
#undef ARDUINOSENSORS
#undef ARDUINOSPIRAM 
#undef STANDALONE
#undef STANDALONESECONDSERIAL
// #define ALTSERIAL Serial

/* the memory size, set to 0 means determine automatically 
 leaving it undefined means zero. setting it larger than 16 bit
 make the address_t 32bit */

#define MEMSIZE 0

/* experimental features, don't use unless you know the code */
/* 
 * this setting uses the EEPROM as program storage 
 * The idea is to create a virtual memory layout starting from 0 with the EEPROM
 * from elength() and then adding the BASIC RAM to it. himem and top need to be 
 * handled carefully. 
 */
#undef ARDUINOPGMEEPROM

/* IO control, emulate real tab by counting characters  */
#define HASMSTAB

/* VT52 settings */
#define HASVT52
#define VT52WIRING 


/* 
 * Experimental BUILDIN feature, implemented as a filesystem. 
 * Buildin BASIC programs are stored in the flash memory of the Arduino.
 * They appear as files in the filesystem.
 */
#undef HASBUILDIN
/* this is the demo module */
#define BUILDINMODULE "buildin/buildin.h"
/* a small tutorial */
// #define BUILDINMODULE "buildin/buildin-tutorial.h"
/* test program for Arduinos */
// #define BUILDINMODULE "buildin/buildin-arduinotest.h"
/* a games module */
// #define BUILDINMODULE "buildin/buildin-games.h"

/* interrupts for the EVENT command, this is needed to use Arduino interrupts */
#define ARDUINOINTERRUPTS

/* 
 * handle the break condition in the background. 
 * Off by default but needed for some keyboards. 
 * On an UNO 170 bytes overhead if on and 5% 
 * performance loss. If any system runs background 
 * tasks anyway, better to switch it on.
 */
#undef BREAKINBACKGROUND

/* 
 * This code measures the fast ticker frequency in microseconds 
 * It counts the number of tokens drawn plus statements executed.
 */
#undef FASTTICKERPROFILE

/* the default EEPROM dummy size */
#define EEPROMSIZE 1024

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

#define PS2DATAPIN 3
#define PS2IRQPIN  2

/* 
 *  Pin settings for the ZX81 Keyboard
 *  first the 8 rows, then the 5 columns or the keyboard
 * 
 * MEGAs have many pins and default is to use the odd pins on the side
 * UNOs, NANOs, and others use the lower pins by default avoiding the 
 * pin 13 which is LED and doesn't work with standard schematics 
 */
#ifdef ARDUINO_AVR_MEGA2560
#define ZX81PINS 37, 35, 33, 31, 29, 27, 25, 23, 47, 45, 43, 41, 39
#else
#define ZX81PINS 7, 8, 9, 10, 11, 12, A0, A1, 2, 3, 4, 5, 6
#endif

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

/*
 *	BREAKCHAR is the character stopping the program on Ardunios
 *  BREAKPIN can be set, it is a pin that needs to go to low to stop a BASIC program
 *    This should be done in hardware*.h
 *  BREAKSIGNAL can also be set, should be done in hardware*.h
 */
#define BREAKCHAR '#'

/* set this is you want pin 4 on low interrupting the interpreter */
/* #define BREAKPIN 4 */
#undef BREAKPIN

/* the primary serial stream aka serial aka sream 1 */
#ifndef ALTSERIAL
#define SERIALPORT Serial
#else 
#define SERIALPORT ALTSERIAL
#endif

/* the secondary serial port aka prt aka stream 4 */
#ifndef PRTSERIAL
#define PRTSERIAL Serial1
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
/* #define I2CEEPROMADDR 0x057 */
/* #define I2CEEPROMSIZE 4096 */
/* definition for an external EEPROM */
#define I2CEEPROMADDR 0x050

/* is the I2C EEPROM buffered */
#define ARDUINOI2CEEPROM_BUFFERED

/*
 * Sensor library code - configure your sensors here, will go to a 
* separate file in the future
 */
#ifdef ARDUINOSENSORS
#undef ARDUINODHT
#define DHTTYPE DHT22
#define DHTPIN 2
#define ARDUINOSHT
#define ARDUINOMQ2
#define MQ2PIN A0
#undef ARDUINOLMS6
#undef ARDUINOAHT
#undef ARDUINOBMP280
#undef ARDUINOBME280
#endif


#if defined(ARDUINOSHT) || defined(ARDUINOLMS6) || defined(ARDUINOAHT) || defined(ARDUINOBMP280) || defined(ARDUINOBME280)
#define ARDUINOSIMPLEWIRE
#endif

/*
 * Keyboard configuation code, currently only GERMAN and US are supported
 * This is used for PS2, FABGL and USB. USB very raw.
 * ZX81 is not supported with different languages, I only have a UK one.
 */

#define ARDUINOKBDLANG_GERMAN
//#define ARDUINOKBDLANG_US 


/*
 * The heuristics. These are things that normally would make sense on a 
 * board.
 */

/* 
 *  Buffer sizes depending on what we are doing.
 */

 #if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_LGT8F)
 /* the small memory model with shallow stacks and small buffers */
 #define BUFSIZE     80
 #define STACKSIZE   15
 #define GOSUBDEPTH  4
 #define FORDEPTH    4
 #define LINECACHESIZE 4
 #else 
 /* the for larger microcontrollers */
 #define BUFSIZE     128
 #define STACKSIZE   64
 #define GOSUBDEPTH  8
 #define FORDEPTH    8
 #define LINECACHESIZE 16
 #endif
 
#ifdef HARDWAREHEURISTICS
/* UNOS are very common. Small memory, we put the program into EEPROM and make everything small */
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_DUEMILANOVE) || defined(ARDUINO_AVR_NANO)
#define ARDUINOEEPROM
#define ARDUINOPICOSERIAL
#define ARDUINOPGMEEPROM
#undef  LINECACHESIZE
#endif
/* on a DUEMILA we allocate just as little main memory as possible, currenly not working because sketch too big 
 * needs to be checked */
#if defined(ARDUINO_AVR_DUEMILANOVE)
#define MEMSIZE 128
#undef  HASMSTAB
#endif
/* tested for LTQF32 Mini EVB - very low memory as core needs a lot */
#if defined(ARDUINO_ARCH_LGT8F)
#define ARDUINOEEPROM
#define ARDUINOPICOSERIAL
#undef LINECACHESIZE
#undef HASMSTAB
#define MEMSIZE 256
#define ARDUINOPGMEEPROM
#endif
/* all AVR 8 bit boards have an EEPROM (most probably) */
#if defined(ARDUINO_ARCH_AVR)
#define ARDUINOEEPROM
#endif
/* megaAVR boards have an EEPROM */
#if defined(ARDUINO_ARCH_MEGAAVR)
#define ARDUINOEEPROM
#endif
/* LEONARDO boards */
#if defined(ARDUINO_AVR_LEONARDO)
/* to be done */ 
#endif
/* all ESPs best are compiled with ESPSPIFFS predefined */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#define ESPSPIFFS
#endif
/* all ESPs best have EEPROM emulation */
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_ESP32)
#define ARDUINOEEPROM
#define ESPSPIFFS
#endif
/* all RP2040 boards best are compiled with RP2040LITTLEFS predefined */
#if defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED_RP2040)
#define RP2040LITTLEFS
#endif
/* use the EEPROM dummy of the NRESESA boards */
#if defined(ARDUINO_ARCH_RENESAS)
#define ARDUINOEEPROM
#endif
/* for XMC there is an EEPROM emulation, which needs: https://github.com/slviajero/XMCEEPROMLib */
#if defined(ARDUINO_ARCH_XMC)
#define ARDUINOEEPROM
#endif
/* the China Nano clones. Super fast but with very small flash. */ 
#if defined(ARDUINO_ARCH_LGT8F)
#undef HASMSTAB
#endif
#endif

/*
 * Now source the the hardware models if this is set.
 */
#ifdef PREDEFINEDBOARD
#include PREDEFINEDBOARD
#endif

/*
 * Here, dependencies are handled. Some settings require others to be set
 * first. 
 * 
 * Examples:
 *  Many filesystems and displays need SPI.
 *  Some components need Wire. 
 *  Some platforms do not have tone, so we need to emulate it.
 */

/* 
 *  DUE has no tone, we switch to emulation mode automatically.
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
 * 
 * HASIMPLEWIRE avoids the overhead of importing the entire 
 * filesystem code. It only activates plain Wire and provides
 * the WIRE command in basic.
 */

/* a clock needs wire */
#ifdef ARDUINORTC
#define HASSIMPLEWIRE
#endif

/* a display needs wire */
#if defined(ARDUINOLCDI2C) || defined(ARDUINOSSD1306) 
#define HASSIMPLEWIRE
#endif

/* EEPROM storage needs wire */
#if defined(ARDUINOEFS)
#define HASSIMPLEWIRE
#endif

/* external EEPROMs also need wire */
#if defined(ARDUINOI2CEEPROM)
#define HASSIMPLEWIRE
#endif

/* plain Wire support also needs wire ;-) */
#if defined(ARDUINOWIRE) || defined(ARDUINOWIRESLAVE)
#define HASWIRE
#endif

/* and I2C Keyboard needs wire */
#if defined(ARDUINOI2CKBD)
#define HASSIMPLEWIRE
#endif

#if defined (ARDUINOSIMPLEWIRE)
#define HASSIMPLEWIRE
#endif

/* radio needs SPI */
#if defined(ARDUINORF24)
#define ARDUINOSPI
#endif

/* a filesystem needs SPI */
#if defined(ARDUINOSD) || defined(ESPSPIFFS) || defined(ESP32FAT)
#define ARDUINOSPI
#endif

/* networking may need SPI */
#if defined(ARDUINOMQTT)
#define ARDUINOSPI
#endif

/* the NOKIA and ILI9488 display needs SPI */
#if defined(ARDUINONOKIA51) || defined(ARDUINOILI9488) || defined(TFTESPI)
#define ARDUINOSPI
#endif

/* the RAM chips */
#if defined(ARDUINOSPIRAM)
#define ARDUINOSPI
#endif

/* Networking and keyboards need the background task capability */
#if defined(ARDUINOMQTT) || defined(ARDUINOETH) || defined(ARDUINOUSBKBD) || defined(ARDUINOZX81KBD) || defined(ARDUINOI2CKBD) || defined(GIGAUSBKBD)
#define BASICBGTASK
#endif

/* the ESP8266 needs to run the background task calls for the scheduler */
#if defined(ARDUINO_ARCH_ESP8266)
#define BASICBGTASK
#endif

/* Break in background needs background tasks as well */
#if defined(BREAKINBACKGROUND)
#define BASICBGTASK
#endif

/* picoserial is not a available on many platforms */
#ifdef ARDUINOPICOSERIAL
#ifndef UCSR0A
#undef ARDUINOPICOSERIAL
#endif
#endif

/* 
 *  incompatibilities and library stuff
 */
/* these platforms have no EEPROM and no emulation built-in */
#if defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED_RP2040) || defined(ARDUINO_ARCH_MBED_GIGA)
#undef ARDUINOEEPROM
#endif

/*
 * SD filesystems with the standard SD driver
 * for MEGA 256 a soft SPI solution is needed 
 * if standard shields are used, this is a patched
 * SD library https://github.com/slviajero/SoftSD
 */
#ifdef ARDUINOSD
#define FILESYSTEMDRIVER
#endif

/*
 * ESPSPIFFS tested on ESP8266 and ESP32
 * supports formating in BASIC
 */ 
#ifdef ESPSPIFFS
#define FILESYSTEMDRIVER
#endif

/*
 * ESP32FAT tested on ESP32
 * supports formating in BASIC
 */ 
#ifdef ESP32FAT
#define FILESYSTEMDRIVER
#endif

/*
 * RP2040 internal filesystem 
 * This is test code from https://github.com/slviajero/littlefs
 * and the main branch is actively developed
 */
#ifdef RP2040LITTLEFS
#define FILESYSTEMDRIVER
#endif

/*
 * STM32 SDIO driver for he SD card slot of the STM32F4 boards (and others)
 */
#ifdef STM32SDIO 
#define FILESYSTEMDRIVER
#endif

/* 
 *  The USB filesystem of the GIGA board
 */
#ifdef GIGAUSBFS
#define FILESYSTEMDRIVER
#define NEEDSUSB
#endif

/*
 * external flash file systems override internal filesystems
 * currently BASIC can only have one filesystem
 */ 
#ifdef ARDUINOSD
#undef ESPSPIFFS
#undef ESP32FAT
#undef RP2040LITTLEFS
#undef GIAGUSBFS
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
#undef GIGAUSBFS
#define FILESYSTEMDRIVER
#endif

/*
 * The build in file system for read only programs, only 
 * if there is no other file system present
 */
#if defined(HASBUILDIN)
#define FILESYSTEMDRIVER
#include BUILDINMODULE
#endif

/*
 * Software SPI only on Mega2560
 */
#ifndef ARDUINO_AVR_MEGA2560
#undef SOFTWARE_SPI_FOR_SD
#endif

/*
 * I used these two articles 
 * https://randomnerdtutorials.com/esp8266-deep-sleep-with-arduino-ide/
 * https://randomnerdtutorials.com/esp32-deep-sleep-arduino-ide-wake-up-sources/
 * for this very simple implementation - needs to be improved (pass data from sleep
 * state to sleep state via EEPROM)
 */
/* this is done only here now to make sure HASCLOCK is set properly */ 

#if defined(ARDUINO_ARCH_SAMD) 
#define HASBUILTINRTC
#endif

/* STM32duino have the same structure */
#if defined(ARDUINO_ARCH_STM32)
#define HASBUILTINRTC
#endif

/* the NRENESA board have a buildin RTC as well */
#if defined(ARDUINO_ARCH_RENESAS)
#define HASBUILTINRTC
#endif

/* 
 * global variables for a standard LCD shield. 
 * Includes the standard Arduino LiquidCrystal library
 */
#ifdef LCDSHIELD 
#define DISPLAYDRIVER
#define HASKEYPAD
#undef DISPLAYHASCOLOR
#undef DISPLAYHASGRAPH
#endif
 
/* 
 * A LCD display connnected via I2C, uses the standard 
 * Arduino I2C display library.
 */ 
#ifdef ARDUINOLCDI2C
#define DISPLAYDRIVER
#undef DISPLAYHASCOLOR
#undef DISPLAYHASGRAPH
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
#endif

/*
 * The driver for the TFT_eSPI graphics of the T-Deck from Lilygo
 * currently not finished 
 */

#ifdef TFTESPI
#define DISPLAYDRIVER
#define DISPLAYHASCOLOR
#define DISPLAYHASGRAPH
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
#define PS2FABLIB
#define HASKEYBOARD
#define DISPLAYHASCOLOR
#define DISPLAYHASGRAPH
#endif 

#if defined(ARDUINO) && defined(ARDUINOPS2)
#define PS2KEYBOARD
#define HASKEYBOARD
#endif 

#if defined(ARDUINO) && defined(ARDUINOUSBKBD)
#define HASKEYBOARD
#define USBKEYBOARD
#endif

#if defined(ARDUINOZX81KBD)
#define HASKEYBOARD
#define ZX81KEYBOARD
#endif

/* BREAKINBACKGROUND is needed here to avoid slowdown, probably also for the other keyboards */
#if defined(ARDUINOI2CKBD)
#define HASKEYBOARD
#define I2CKEYBOARD
#define BREAKINBACKGROUND
#endif

/* set the HASKEYBOARD feature to tell BASIC about it */
#if defined(GIGAUSBKBD)
#define HASKEYBOARD
#define GIGAUSBKEYBOARD
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
 * The following software models are supported
 *  - Built-in clocks of STM32 and MKR and NRESAS and XIAO are supported by default 
 *      as RTCZero type code
 *  - Built-in clocks of ESP32 are supported by default with a time structure type code
 *  - Built-in clocks of GIGA the same as ESP32
 *  - I2C clocks can be activated: DS1307, DS3231, and DS3232
 *  - A Real Time Clock emulation is possible using millis()
 * 
 * rtcget accesses the internal registers of the clock. 
 * Registers 0-6 are bcd transformed to return 
 * seconds, minutes, hours, day of week, day, month, year
 * 
 * On I2C clocks registers 7-255 are returned as memory cells
 */

 #if defined(ARDUINORTC) || defined(HASBUILTINRTC) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINORTCEMULATION) || defined(ARDUINO_ARCH_MBED_GIGA)
 #define HASCLOCK
 #endif

/* 
 * External EEPROM is handled through an EFS filesystem object  
 * see https://github.com/slviajero/EepromFS 
 * for details. Here the most common parameters are set as a default.
*/
#ifdef ARDUINOEFS
#undef ARDUINOI2CEEPROM
#endif

/* 
 *  Tell BASIC we have a second serial port
 */
#ifdef ARDUINOPRT
#define HASSERIAL1
#endif

/* 
 *  Tell BASIC we have a radio adapter
 */
#ifdef ARDUINORF24
#define HASRF24
#endif

/* 
 *  Tell BASIC we have MQTT
 */
#ifdef ARDUINOMQTT
#define HASMQTT
#endif

/* 
 *  handling the PROGMEM macros 
 */

 /*
 * if the PROGMEM macro is define we compile on the Arduino IDE
 *  we undef all hardware settings otherwise a little odd
 */
#ifdef ARDUINO_ARCH_MBED
#define PROGMEM
#endif

#ifdef PROGMEM
#define ARDUINOPROGMEM
#else
#undef ARDUINOPROGMEM
#define PROGMEM
#endif

/* on XMC we circumvent progmem */
#ifdef ARDUINO_ARCH_XMC
#undef ARDUINOPROGMEM
#define PROGMEM
#endif


/* the code to address EEPROMs directly */
/* only AVR controllers are tested with this, don't use elsewhere, there are multiple bugs */
#if defined(ARDUINOPGMEEPROM) & ! ( defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_LGT8F) )
#undef ARDUINOPGMEEPROM
#endif

#ifdef ARDUINOPGMEEPROM
#define USEMEMINTERFACE
#define EEPROMMEMINTERFACE
#else 
#undef EEPROMMEMINTERFACE
#endif

/* the SPI RAM interface */
#ifdef ARDUINOSPIRAM
#define USEMEMINTERFACE
#define SPIRAMINTERFACE
#endif

/* 
 * to handle strings in situations with a memory interface two more buffers are 
 * needed they store intermediate results of string operations. The buffersize 
 * limits the maximum string length indepents of how big strings are set. 
 * 
 * default is 128, on an MEGA 512 is possible
 */
#ifdef ARDUINO_AVR_MEGA2560
#define SPIRAMSBSIZE 512
#else
#define SPIRAMSBSIZE 128
#endif


/*
 * Does the platform has command line args and do we want to use them 
 * Arduinos don't have them
 */
#undef HASARGS

/*
 * How restrictive are we on function recursive calls to protect the stack
 * On 8 bit Arduinos this needs to be limited strictly
 */
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR) || defined(ARDUINO_ARCH_LGT8F)
#define FNLIMIT 4
#elif defined(ARDUINO_ARCH_ESP8266)
#define FNLIMIT 64
#elif defined(ARDUINO_ARCH_ESP32)
#define FNLIMIT 128
#elif defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED_RP2040)
#define FNLIMIT 128
#elif defined(ARDUINO_ARCH_SAM) && defined(ARDUINO_ARCH_SAMD)
#define FNLIMIT 64
#elif defined(ARDUINO_ARCH_XMC)
#define FNLIMIT 64
#elif defined(ARDUINO_ARCH_SMT32)
#define FNLIMIT 128
#elif defined(ARDUINO_ARCH_RENESAS)
#define FNLIMIT 32
#else
#define FNLIMIT 128
#endif

/* 
 *  here the runtime environment makes a good guess which 
 *  language can be supported by the boards. This is tricky
 *  it may not always work. 
 *  
 *  MEGAAVR does BASICFULL but this leaves little room for hardware drivers
 *  LEONARDO currently broken, to much flash allocated.
 */
#if defined(ARDUINO_ARCH_AVR)
#if defined(ARDUINO_AVR_DUEMILANOVE)
#define BASICPALOALTO
#endif
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_NANO)
#define BASICSIMPLE
#endif
#if defined(ARDUINO_AVR_LEONARDO)
#define BASICSMALL
#endif
#if defined(ARDUINO_AVR_MEGA2560)
#define BASICFULL
#endif
#elif defined(ARDUINO_ARCH_LGT8F)
#define BASICSIMPLE
#elif defined(ARDUINO_ARCH_MEGAAVR)
#define BASICFULL
#elif defined(ARDUINO_ARCH_ESP8266)
#define BASICFULL
#elif defined(ARDUINO_ARCH_ESP32)
#define BASICFULL
#elif defined(ARDUINO_ARCH_RP2040) || defined(ARDUINO_ARCH_MBED_RP2040)
#define BASICFULL
#elif defined(ARDUINO_ARCH_SAM) && defined(ARDUINO_ARCH_SAMD)
#define BASICFULL
#elif defined(ARDUINO_ARCH_XMC)
#define BASICFULL
#elif defined(ARDUINO_ARCH_SMT32)
#define BASICFULL
#elif defined(ARDUINO_ARCH_RENESAS)
#define BASICFULL
#else
#define BASICFULL
#endif
