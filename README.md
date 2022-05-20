# Stefan's IoT BASIC

## The idea

My attempt to create a basic interpreter from scratch. The project was inspired by Steve Wozniak's statement that the Apple 1 basic interpreter was the biggest challenge in his professional life. Bill Gates also had started his career and his fortune by writing a basic interpreter for the Altair microcomputer around the some time. 

The project has outgrown its beginnings by now. It became a full featured BASIC interpreter with IoT and microcontroller specific features. There is an underlying hardware abstraction layer making the interpreter
useable on a number of architectures like Arduino AVR, ESP8266, ESP32, SAMD, RP2040 and ARM. 

Arithmetic is 16 bit, 32bit or float depending on the compiler settings and the platform. The full set of Dartmouth language features like ON GOSUB and DEF FN is implemented. Strings are Apple 1 style. They are essentially integer arrays like in C. Conditions are also C style meaning part of the arithmetic.

Filesystems like Arduino SD, ESPSPIFFS and LittleFS are supported on microcontrollers. EEPROMS can be used
as BASIC filesystem using the EepromFS library.

Small LCD displays, OLEDs, Nokia5110m TFT and VGA monitors are supported. Graphics is supported on all graphic capable displays.

PS2 keyboard support and keypads are added for standalone computer projects.

Microcontroller specific features are EEPROM access, EEPROM program storage and autorun, control of digital and analog I/O as well as the delay function, Wire library support, RF2401 support and very simple MQTT / Wifi support on ESP.

The interpreter can be compliled with standard gcc on almost any architecture or in the Arduino IDE without changes. 

Look at the WIKI https://github.com/slviajero/tinybasic/wiki for more information.

See also:
- https://en.wikipedia.org/wiki/Recursive_descent_parser
- https://rosettacode.org/wiki/BNF_Grammar
- https://en.wikipedia.org/wiki/Tiny_BASIC
- https://github.com/slviajero/tinybasic/wiki/Unforgotten:-Palo-Alto-BASIC
- https://github.com/slviajero/tinybasic/wiki/The-original-Apple-1-BASIC-manual

## Language features in a nutshell 

The interpreter includes most of the Dartmouth language set. Differences are mainly the string handling which was taken from Apple 1 integer BASIC. Autodimensioning of arrays and strings was taken from ECMA BASIC.

The intepreter is compatible with two of the 1976 early basic dialects. It implements the full language set of Dr. Wang's Palo Alto Tinybasic from the December 1976 edition of Dr. Dobbs (https://github.com/slviajero/tinybasic/wiki/Unforgotten:-Palo-Alto-BASIC). This is a remarkably complete little language with many useful features. 

The interpreter also implements the specification of Apple Integer BASIC sold for the Apple 1 computer (https://github.com/slviajero/tinybasic/wiki/The-original-Apple-1-BASIC-manual). It should be fully Apple 1 compatible.

I/O handling and some of the microcontroller BASIC features are new and are not compatible to the BASIC dialects above.

The interpreter is not meant to be compatible to any BASIC dialect. I ported most of the games of 101 BASIC games from 1977 as test programs to test and check compatibility. The main restriction is that arrays can only be one dimensional and that there are no string arrays. All data objects remain in the same memory location once they are defined. There is no garbage collection. This makes the behaviour of the BASIC interpreter deterministic, real time capable and fast. 

Programs are always fully tokenized at input. This includes keywords, numbers, strings and variables names. No lexical analysis is done or needed at runtime. The stored BASIC program resembles more a byte code language than a stored interpreter code. This is the concept Steve Wozniak used on the Apple 1. 

The core interpreter loop runs at approximately one token every 7 micro seconds on an Arduino UNO. On an ESP it runs at 1.4 micro seconds per token. 

For further information, please look at: https://github.com/slviajero/tinybasic/wiki

## Files in this archive 

basic.c is the program source. basic.h the header file. TinybasicArduino/TinybasicArduino.ino is an exact copy of basic.c. 

In addition to this file and basic.h you need a hardware definition file.

For the Arduino IDE place hardware-arduino.h in your Arduino sketch directory. These are the platform specific definitions and a thin OS like layer for hardware abstraction. All platforms from tiny AVR168 up to the powerful ESP32 and RP2040 are covered by this one file. 

For POSIX OSes you need hardware-posix.h in your working directory. GCC compiles BASIC for Linux, Mac (primary dev platform), Windows (with MINGW), and MSDOS (Turbo C 2).

For simple Arduino boards I prepared file versions in Arduino cloud 

Use one of these two links.

A bare minimum Arduino system is prepared here

https://create.arduino.cc/editor/sl001/ade3b2af-6781-4dfc-b9dd-4eec95405723/preview


A quite complete ESP8266 system without network support can be found here

https://create.arduino.cc/editor/sl001/27a26f15-c23b-408d-8d39-e6948aead495/preview


The single file versions contain the full code but are not maintained regularly. They always will be behind the newest code. Please use them only as quick and dirty demos. 

utility/monitor.py is a little serial monitor to interact with the running BASIC interpreter on the Arduino. It allows very simple loading of files into the Arduino and saving of output to a file on a computer. utility/arduinoterm is a wrapper of monitor.py.

utility/dosify converts the code to tcc 2.01 ready format to be compiled in DOSBOX.

The various programs with the extension .bas are test files for the interpreter (arduinotests, testprograms) and some 1976 BASIC games (basicgames).

## What's next

More IoT functionality. More devices.

