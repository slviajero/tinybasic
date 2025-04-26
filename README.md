# Stefan's BASIC

## The idea

My attempt to create a basic interpreter from scratch. The project was inspired by Steve Wozniak's statement that the Apple 1 basic interpreter was the biggest challenge in his professional life. Bill Gates also had started his career and his fortune by writing a basic interpreter for the Altair microcomputer around the some time. 

The project has outgrown its beginnings by now. It became a full featured BASIC interpreter with IoT and microcontroller specific features. There is an underlying hardware abstraction layer making the interpreter
useable on a number of architectures like Arduino, ESP8266, ESP32, SAMD, RP2040 and various ARMs. Posix and MSDOS.  

Arithmetic is 16 bit, 32bit or float depending on the compiler settings and the platform. The full set of Dartmouth language features like ON GOSUB and (multiline) DEF FN is implemented. Strings are Apple 1 style. They are essentially character arrays like in C and remain static in memory. Some compatibility features with other BASIC interpreters are implemented.

A large number of BASIC extensions are now available. Multiline functions, structured programming, multidimensional arrays and string arrays. Compatibility modes for MS BASICs, Palo Alto dialects and the Apple and Cromecon BASICs are included. 

The language is scalable and can run from 2 kB memory Arduino UNOs to large memory machines with MBs of BASIC memory. 

Read the tutorials for more info on how to use BASIC on Arduino in practice: 
- Intro: https://www.hackster.io/sl001/basic-on-arduinos-5b4e24
- Cotrolling Ciruits: https://www.hackster.io/sl001/basic-on-arduinos-volume-2-built-in-examples-a172a8
- Simple Displays: https://www.hackster.io/sl001/basic-on-arduinos-volume-3-simple-displays-4d64f7

Building standalone computer with BASIC:
- AIM65 lookalike: https://www.hackster.io/sl001/aim65-lookalike-computer-from-scratch-8d94d6
- 8266 based IoT standalone computer: https://www.hackster.io/sl001/the-10-euro-iot-computer-with-esp-8266-e93681
- TFT screen with an Arduino DUE: https://www.hackster.io/sl001/homecomputer-from-scratch-a04fb9
- VGA Computer: https://www.hackster.io/sl001/a-vga-computer-based-on-esp32-and-fabgl-with-sound-graphics-976894
- An RP2040 box: https://www.hackster.io/sl001/arduino-rp2040-standalone-iot-computer-running-basic-433144

Special systems:
- A BASIC robot: https://www.instructables.com/Build-a-Robot-Car-With-an-80s-Style-Home-Computer-/

## What is special - peripheral and Arduino I/O support

Just like the home computer BASICs of the old times, BASIC is a small standalone operating system. It supports a number of peripherals and can be extended to new peripherals fast as there are generic device driver stubbs in the code.

Supported platforms right now are all 8bit Arduino AVR systems, LGT8 systems, the various Arduino 32bit systems like the MKR and GIGA. Of cource the RP2040, plain Rasberry PI Pico, ESP8266 and ESP32, Infineon XMC, Seeeduino Xiao and STM32. A full list of supported platforms can be found here https://github.com/slviajero/tinybasic/wiki/Hardware-Platforms.

For Raspberry PI with Linux, frame buffer graphics and wiring is supported just like on the microcontroller platforms.

Filesystems like Arduino SD, SPIFFS and LittleFS are supported on microcontrollers. EEPROMS can be used
as BASIC filesystem. This includes I2C external EEPROMS.

Memory extension with SPI SRAM modules is possible.

LCD displays, OLEDs, Nokia5110, TFT and VGA monitors are supported. Graphics is supported on all graphic capable displays.

PS2 keyboard support and keypads are added for standalone computer projects. A ZX81 keyboard can be used as well. USB keyboard support is added as an experimental feature.

Microcontroller specific features are EEPROM access, EEPROM program storage and autorun, control of digital and analog I/O as well as the delay function, Wire library support, RF2401 support andsimple MQTT / Wifi support on ESP.

## Files in this archive 

Basic1: The 1.x version of the interpreter. This version is stable but will not be developed further. I do bugfixes now and then. 

Files of the 1.x versions
- basic.c is the program source
- basic.h is the header file
- IoTBasic/IoTBasic.ino is a copy of basic.c
- hardware-arduino.h is the hardware definition file for Arduino 
- hardware-posix.h is the hardware definition file for Posix
- TinyVT52 is a VT52 terminal emulation for Arduinos, this is unfinished

Basic2: The 2.x version. It separates the interpreter from the runtime envirnoment. There is a Arduino IDE version runtime.cpp and a Posix version runtime.c. The interface of the runtime environment for both platforms is idential just as the BASIC interpreter file basic.c or IotBasic.ino.

Files of the 2.x version
- basic.c identical to IoTBasic/IoTBasic.ino, this is the interpreter
- basic.h is the header of the interpreter
- language.h is the configuration file for the language features
- Posix/runtime.c is the runtime environment for Posix environment
- Posix/hardware.h is the hardware configuration for Posix
- IoTBasic/runtime.cpp is the runtime environment for Arduino
- IoTBasic/hardware.h is the hardware configuration for Arduino
- runtime.h is identical for all platforms, it is the interface definition of the runtime code
- MSDOS and RaspPi contain the code with the right settings for there platforms and additional information.

In addition to this there are files for the buildin programs in version 2.x. 

- buildin.h is a demo for buildin programs
- buildin is a folder to store buildin headers, as demos it contains buildin-arduinotest.h, buildin-games.h, buildin-tutorial.h

To compile Basic2 you would check and edit hardware.h and language.h for your platform to set the hardware and language features. On Posix you would then do 

gcc basic.c runtime.c -lm

while on Arduino you would compile and upload in the IDE.

For Raspberry PI, some special settings and linker flags are required depending on the GPIO libraries used. Please look into the MANUAL if you want to compile on this platform.

utility/dosify converts the code to tcc 2.01 ready format to be compiled in DOSBOX, Basic2 is not tested on DOS.

examples contains a lot of demo programs and games ported to BASIC.

MANUAL.md is the BASIC manual.

## Software and documentation

Most of the builtin Arduino demos are ported to BASIC and published here https://github.com/slviajero/tinybasic/tree/main/examples. These programs are the BASIC versions of the C++ programs in https://docs.arduino.cc/built-in-examples/. Please look at this original Arduino website for wiring and project information.

The interpreter can be compliled with standard gcc on almost any architecture or in the Arduino IDE without changes. 

A manual of the BASIC interpreter is in the repo https://github.com/slviajero/tinybasic/blob/main/MANUAL.md

Look at the WIKI https://github.com/slviajero/tinybasic/wiki for more information.

## Language features in a nutshell 

The core interpreter includes most of the Dartmouth language set. Differences are mainly the string handling which was taken from Apple Integer BASIC. Autodimensioning of arrays and strings was taken from ECMA BASIC.

The intepreter is compatible with two of the 1976 early basic dialects. It implements the full language set of Dr. Wang's Palo Alto Tinybasic from the December 1976 edition of Dr. Dobbs (https://github.com/slviajero/tinybasic/wiki/Unforgotten:-Palo-Alto-BASIC). This is a remarkably complete little language with many useful features. 

The interpreter also implements the specification of Apple Integer BASIC sold for the Apple 1 and 2 computers (https://github.com/slviajero/tinybasic/wiki/The-original-Apple-1-BASIC-manual).

These two languages are the core of the BASIC interpreter. Additional feature can be added at compile time. The feature set is now comparable to many of the standard BASIC dialects of the 80s. 

I/O handling and some of the microcontroller BASIC features are new and are not compatible to the BASIC dialects above. They resemble the iostream library of Arduinos. Enterprise BASIC had something similar.

The main difference to the old BASIC interpreters is that data objects remain in the same memory location once they are defined. There is no garbage collection. This makes the behaviour of the BASIC interpreter deterministic, real time capable and fast. 

Programs are always fully tokenized at input. This includes keywords, numbers, strings and variables names. No lexical analysis is done or needed at runtime. The stored BASIC program resembles more a byte code language than a stored interpreter code. This is the concept Steve Wozniak used on the Apple 1. 

The core interpreter loop runs at approximately one token every 20 microseconds on an Arduino. Some modern platforms like STM32 can do up to 2 microseconds. These computers execute BASIC commands as fast the a 6502 CPU has executed its machine language. BASIC is fast enough to do most microcontroller tasks.

For further information, please look at: https://github.com/slviajero/tinybasic/wiki

There is a set of BASIC programs in the examples section https://github.com/slviajero/tinybasic/tree/main/examples of the repo. They showcase language features and use cases.

For more information on the language, please look in the manual at: https://github.com/slviajero/tinybasic/blob/main/MANUAL.md

## Libraries needed

The code is written to work standalone for many systems. As long as no complex device drivers are needed, you can simply compile it and start using BASIC.

Build-in features are EEPROM access, Real Time Clock access and handling of SPI memory.

One exceptions are the Arduino SAMD broards like the MKR1000, Zero or Vidor boards. You need to install the RTCZero library and the RTCLowPower for this. 

On STM32, the STM32ZRTC and the STM32LowPower library is needed. 

EEPROM emulations on XMC needs the XMCEEPROM library. 

Displays, filesystem, networking, and sensors need libraries. 

Please consult the wiki https://github.com/slviajero/tinybasic/wiki/Hardware-Platforms for this.

## Supported platforms: 

Arduino, ESP8266, ESP32, SAMD, RP2040 and various ARMs. Posix and MSDOS.  

While I have developed this also for STM32 boards, I don't support this any more. The reason for this is the large number of incompatible clones with all kinds of problems when uploading software and using serial on the USB port. This is not a weakness of STM32 but of the manufacturer of clones.  

## What's next

Code cleanup of the 2.x versions. Release 2.0.
