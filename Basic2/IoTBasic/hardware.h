/*
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
 *  - Many people asked good questions and helped me. Thank you to all!
 *
 * Hardware definition file coming with IotBasic.ino aka basic.c
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

/* 
 * Arduino hardware settings , set here what you need or
 * use one of the predefined configurations below.
 * 
 * If HARDWAREHEURISTICS is set, some of the settings below 
 * are activated automatically for some platforms. If you want
 * to control all settings below manually, undef HARDWAREHEURISTICS 
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
 *	leave this unset if you use the definitions below
 */

#undef HARDWAREHEURISTICS

#undef ARDUINOPICOSERIAL 
#undef ARDUINOPS2
#undef ARDUINOUSBKBD
#undef ARDUINOZX81KBD
#undef ARDUINOI2CKBD
#undef GIGAUSBKBD
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
#undef TFTESPI
#undef ARDUINOEEPROM
#undef ARDUINOI2CEEPROM
#define ARDUINOEFS
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
//#define ALTSERIAL Serial1

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

/* 
 * Experimental BUILDIN feature, implemented as a filesystem. 
 * Buildin BASIC programs are stored in the flash memory of the Arduino.
 * They appear as files in the filesystem.
 */
#undef HASBUILDIN
/* this is the demo module */
#define BUILDINMODULE "buildin.h"
/* a small tutorial */
// #define BUILDINMODULE "buildin/buildin-tutorial.h"
// #define BUILDINMODULE "buildin/buildin-arduinotest.h"
// #define BUILDINMODULE "buildin/buildin-games.h"

/* interrupts for the EVENT command, this is needed to use Arduino interrupts */
#undef ARDUINOINTERRUPTS

/* 
 * handle the break condition in the background. 
 * Off by default but needed for some keyboards. 
 * On an UNO 170 bytes overhead if on and 5% 
 * performance loss. If any system runs background 
 * tasks anyway, better to switch it on.
 */
#undef BREAKINBACKGROUND

/* 
 * Predefined hardware configurations, this assumes that all of the 
 *	above are undef. The configurations are used for some common 
 *  board configurations. In the future this must go into a separate
 *  file and folder.
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
 * TDECK: 
 *   the LILYGO T-Deck - not yet finished
 *   
 */

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
#undef TDECK

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
 * certain hardware.
 */

/* 
 *  Buffer sizes depending on what we are doing.
 */

 #if defined(ARDUINO_ARCH_AVR)
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
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_DUEMILANOVE)
#define ARDUINOEEPROM
#define ARDUINOPICOSERIAL
#define ARDUINOPGMEEPROM
#undef LINECACHESIZE
#endif
/* on a DUEMILA we allocate just as little main memory as possible, currenly not working because sketch too big 
 * needs to be checked */
#if defined(ARDUINO_AVR_DUEMILANOVE)
#define MEMSIZE 128
#endif
/* all AVR 8 bit boards have an EEPROM (most probably) */
#if defined(ARDUINO_ARCH_AVR)
#define ARDUINOEEPROM
#endif
/* megaAVR boards have an EEPROM */
#if defined(ARDUINO_ARCH_MEGAAVR)
#define ARDUINOEEPROM
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
#endif

/*
 * The hardware models.
 * These are predefined hardware configurations.
 */

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
/* #define ARDUINOMQTT */ /* currently broken */
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
#undef ARDUINOPICOSERIAL 
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
#undef ARDUINOPICOSERIAL
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
#undef ARDUINOPICOSERIAL
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#undef ARDUINOEEPROM
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
#undef ARDUINOPICOSERIAL
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#undef ARDUINOEEPROM
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
#undef ARDUINOPICOSERIAL
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
#undef  ARDUINOPICOSERIAL
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
 *  The Liligo T-Deck configuration
 *    supports the internal FFAT right now, SD not yet done
 */
#if defined(TDECK)
/* these are the PIN definition from Lilygos utilities.h file */
#define BOARD_POWERON       10
#define BOARD_I2S_WS        5
#define BOARD_I2S_BCK       7
#define BOARD_I2S_DOUT      6
#define BOARD_I2C_SDA       18
#define BOARD_I2C_SCL       8
#define BOARD_BAT_ADC       4
#define BOARD_TOUCH_INT     16
#define BOARD_KEYBOARD_INT  46
#define BOARD_SDCARD_CS     39
#define BOARD_TFT_CS        12
#define RADIO_CS_PIN        9
#define BOARD_TFT_DC        11
#define BOARD_TFT_BACKLIGHT 42
#define BOARD_SPI_MOSI      41
#define BOARD_SPI_MISO      38
#define BOARD_SPI_SCK       40
#define BOARD_TBOX_G02      2
#define BOARD_TBOX_G01      3
#define BOARD_TBOX_G04      1
#define BOARD_TBOX_G03      15
#define BOARD_ES7210_MCLK   48
#define BOARD_ES7210_LRCK   21
#define BOARD_ES7210_SCK    47
#define BOARD_ES7210_DIN    14
#define RADIO_BUSY_PIN      13
#define RADIO_RST_PIN       17
#define RADIO_DIO1_PIN      45
#define BOARD_BOOT_PIN      0
#define BOARD_BL_PIN        42
#define BOARD_GPS_TX_PIN    43
#define BOARD_GPS_RX_PIN    44
#ifndef RADIO_FREQ
#define RADIO_FREQ           868.0
#endif
#ifndef RADIO_BANDWIDTH
#define RADIO_BANDWIDTH      125.0
#endif
#ifndef RADIO_SF
#define RADIO_SF             10
#endif
#ifndef RADIO_CR
#define RADIO_CR             6
#endif
#ifndef RADIO_TX_POWER
#define RADIO_TX_POWER       22
#endif
#define DEFAULT_OPA          100
/* and now the BASIC stuff begins */
/* either use the buildin FAT as a file system */
#undef ESP32FAT
/* or the SD card */
#define ARDUINOSD
#define SDPIN BOARD_SDCARD_CS
/* the display */
#define TFTESPI
#define DISPLAYCANSCROLL
/*the keyboard */
#define ARDUINOI2CKBD
#define SDA_PIN BOARD_I2C_SDA
#define SCL_PIN BOARD_I2C_SCL
#define I2CKBDADDR 0x55
/* can run standalone now */
#define STANDALONE
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
 *  - Built-in clocks of STM32 and MKR and NRESAS are supported by default as RTCZero type code
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

 #if defined(ARDUINORTC) || defined(HASBUILTINRTCZERO) || defined(ARDUINO_ARCH_ESP32) || defined(ARDUINORTCEMULATION) || defined(ARDUINO_ARCH_MBED_GIGA)
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
#if defined(ARDUINOPGMEEPROM) & ! defined(ARDUINO_ARCH_AVR)
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
 * This code measures the fast ticker frequency in microseconds 
 * It leaves the data in variable F. Activate this only for test 
 * purposes.
 */
#undef FASTTICKERPROFILE

/*
 * Does the platform has command line args and do we want to use them 
 * Arduinos don't have them
 */
#undef HASARGS

/*
 * How restrictive are we on function recursive calls to protect the stack
 * On 8 bit Arduinos this needs to be limited strictly
 */
#if defined(ARDUINO_ARCH_AVR) || defined(ARDUINO_ARCH_MEGAAVR)
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
 *  LEONARDO and DUEMILLA currently broken, to much flash allocated.
 */
#if defined(ARDUINO_ARCH_AVR)
#if defined(ARDUINO_AVR_DUEMILANOVE)
#define BASICMINIMAL
#endif
#if defined(ARDUINO_AVR_UNO) || defined(ARDUINO_AVR_LEONARDO)
#define BASICSIMPLE
#endif
#if defined(ARDUINO_AVR_MEGA2560)
#define BASICFULL
#endif
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
