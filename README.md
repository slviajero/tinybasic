# Stefan's IoT BASIC

## The idea

My attempt to create a basic interpreter from scratch. The project was inspired by Steve Wozniak's statement that the Apple 1 basic interpreter was the biggest challenge in his professional life. Bill Gates also had started his career and his fortune by writing a basic interpreter for the Altair microcomputer around the some time. 

The project has outgrown its beginnings by now. It became a full featured BASIC interpreter with IoT and microcontroller specific features. There is an underlying hardware abstraction layer making the interpreter
useable on a number of architectures like Arduino AVR, ESP8266, ESP32, SAMD, RP2040 and ARM. 

Arithmetic is 16 bit, 32bit or float depending on the compiler settings and the platform. The full set of Dartmouth language features like ON GOSUB and DEF FN is implemented. Strings are Apple 1 style. They are essentially integer arrays like in C. Conditions are also C style meaning part of the arithmetic.


## What is special - peripheral and Arduino I/O support

Just like the home computer BASICs of the old times, IoTBasic is a small standalone operating system. It supports a number of peripherals and can be extended to new peripherals fast as there are generic device driver stubbs in the code.

Supported platforms right now are all 8bit Arduino AVR systems, LGT8 systems, the various Arduino 32bit systems like the MKR and of cource the RP2040, plain Rasberry PI Pico, ESP8266 and ESP32, Infineon XMC, Seeeduino Xiao and STM32. A full list of supported platforms can be found here https://github.com/slviajero/tinybasic/wiki/Hardware-Platforms.

Filesystems like Arduino SD, SPIFFS and LittleFS are supported on microcontrollers. EEPROMS can be used
as BASIC filesystem. This includes I2C external EEPROMS.

Memory extension with SPI SRAM modules is possible.

LCD displays, OLEDs, Nokia5110, TFT and VGA monitors are supported. Graphics is supported on all graphic capable displays.

PS2 keyboard support and keypads are added for standalone computer projects. A ZX81 keyboard can be used as well. USB keyboard support is added as an experimental feature.

Microcontroller specific features are EEPROM access, EEPROM program storage and autorun, control of digital and analog I/O as well as the delay function, Wire library support, RF2401 support andsimple MQTT / Wifi support on ESP.

## Software and documentation

Most of the builtin Arduino demos are ported to BASIC and published here https://github.com/slviajero/tinybasic/tree/main/examples. These programs are the BASIC versions of the C++ programs in https://docs.arduino.cc/built-in-examples/. Please look at this original Arduino website for wiring and project information.

The interpreter can be compliled with standard gcc on almost any architecture or in the Arduino IDE without changes. 

A manual of the BASIC interpreter is in the repo https://github.com/slviajero/tinybasic/blob/main/MANUAL.md

Look at the WIKI https://github.com/slviajero/tinybasic/wiki for more information.


## Language features in a nutshell 

The interpreter includes most of the Dartmouth language set. Differences are mainly the string handling which was taken from Apple Integer BASIC. Autodimensioning of arrays and strings was taken from ECMA BASIC.

The intepreter is compatible with two of the 1976 early basic dialects. It implements the full language set of Dr. Wang's Palo Alto Tinybasic from the December 1976 edition of Dr. Dobbs (https://github.com/slviajero/tinybasic/wiki/Unforgotten:-Palo-Alto-BASIC). This is a remarkably complete little language with many useful features. 

The interpreter also implements the specification of Apple Integer BASIC sold for the Apple 1 and 2 computers (https://github.com/slviajero/tinybasic/wiki/The-original-Apple-1-BASIC-manual).

These two languages are the core of the BASIC interpreter. Additional feature can be added at compile time.

I/O handling and some of the microcontroller BASIC features are new and are not compatible to the BASIC dialects above. They resemble the iostream library of Arduinos. Enterprise BASIC had something similar.


The main difference to the old BASIC interpreters is that data objects remain in the same memory location once they are defined. There is no garbage collection. This makes the behaviour of the BASIC interpreter deterministic, real time capable and fast. 

Programs are always fully tokenized at input. This includes keywords, numbers, strings and variables names. No lexical analysis is done or needed at runtime. The stored BASIC program resembles more a byte code language than a stored interpreter code. This is the concept Steve Wozniak used on the Apple 1. 

The core interpreter loop runs at approximately one token every 40 microseconds on an Arduino. ESP are a bit faster but not much as the memory bandwidth limits the performance in the loop. ESPs are much faster in calulations, though.

For further information, please look at: https://github.com/slviajero/tinybasic/wiki

There is a set of BASIC programs in the examples section https://github.com/slviajero/tinybasic/tree/main/examples of the repo. They showcase language features and use cases.

For more information on the language, please look in the manual at: https://github.com/slviajero/tinybasic/blob/main/MANUAL.md

## Files in this archive 

basic.c is the program source. basic.h is the header file. 

IoTBasic/IoTBasic.ino is a copy of basic.c. The Arduino and POSIX source code is the same.

In addition to this file and basic.h you need a hardware definition file.

For the Arduino IDE place hardware-arduino.h in your Arduino sketch directory. These are the platform specific definitions and a thin OS like layer for hardware abstraction. All platforms from tiny AVR168 up to the powerful ESP32 and RP2040 are covered by this one file. Actually this is not only a header but really a complete library. I will rewrite this code in the future and make it a library.

For POSIX OSes you need hardware-posix.h in your working directory. GCC compiles BASIC for Linux, Mac (primary dev platform), Windows (with MINGW), and MSDOS (Turbo C 2).

utility/monitor.py is a little serial monitor to interact with the running BASIC interpreter on the Arduino. It allows very simple loading of files into the Arduino and saving of output to a file on a computer. utility/arduinoterm is a wrapper of monitor.py.

utility/dosify converts the code to tcc 2.01 ready format to be compiled in DOSBOX.

examples contains a lot of demo programs and games ported to BASIC.

MANUAL.md is the BASIC manual.

The folder TinybasicArduino is kept for compatibility purposes. This is my working code. Things may be broken here.

TinyVT52 is a VT52 terminal emulation for Arduinos.

## Libraries needed

The code is written to work standalone for many systems. As long as no complex device drivers are needed, you can simply compile it and start using BASIC.

Build-in features are EEPROM access, Real Time Clock access and handling of SPI memory.

One exceptions are the Arduino SAMD broards like the MKR1000, Zero or Vidor boards. You need to install the RTCZero library and the RTCLowPower for this. 

On STM32, the STM32ZRTC and the STM32LowPower library is needed. 

EEPROM emulations on XMC needs the XMCEEPROM library. 

Displays, filesystem, networking, and sensors need libraries. 

lease consult the wiki https://github.com/slviajero/tinybasic/wiki/Hardware-Platforms for this.


## What's next

More IoT functionality. More devices. Code cleanup. Getting version 1.4. released. Then more cloud / networking functions.

