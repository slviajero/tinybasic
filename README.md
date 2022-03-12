# Stefan's IoT BASIC

## The idea

My attempt to create a small basic interpreter from scratch using frugal programming techniques and minimal memory. The project was inspired by Steve Wozniak's statement that the Apple 1 basic interpreter was the biggest challenge in his professional life. Bill Gates also had started his career and his fortune by writing a basic interpreter for the Altair microcomputer around the some time.

The program was originally written in C with minimal library support. All code was done by hand. The C stack is not used for arithmetic to keep it minimal. The interpreter uses a set of global variables, an own stack and a static memory and variable array to emulate the low memory environment of the early microcomputers. 

The project has outgrown its beginnings by now. It became a full featured BASIC interpreter with IoT and microcontroller specific features. There is an underlying hardware abstraction layer making the interpreter
useable on a number of architectures like Arduino AVR, ESP8266, ESP32, SAMD, RP2040 and ARM. 

Arithmetic is 16 bit, 32bit or float depending on the compiler settings and the platform. The full set of logical expresssions with NOT, AND, OR is implemented C style. Conditions are part of the arithemtic and not separate like in many basic dialects. This makes the call stack of the recursive descent deeper but simplifies other code. To reduce the memory footprint of this part of the runtime code no arguments are passed in the C functions. Instead, an own multi purpose 16 bit stack is added. 

Memory access by default is 8bit. Addresses are 16 bit by default. Depending on the platform BASIC memory can be between 512kB and 64 kB. Larger memory models are possible by changing the underlying address type.

Filesystems like Arduino SD, ESPSPIFFS and LittleFS are supported on microcontrollers. EEPROMS can be used
as BASIC filesystem using the EepromFS library.

Small LCD displays, TFT and VGA monitors are supported. The later two with graphics features. 

There are many microcontroller specific features. I added EEPROM access, EEPROM program storage and autorun, control of digital and analog I/O as well as the delay function, Wire library support, RF2401 support and very simple MQTT / Wifi support on ESP.

The interpreted can be compliled with standard gcc on almost any architecture or in the Arduino IDE without changes. 

Look at the WIKI https://github.com/slviajero/tinybasic/wiki for more information.

See also:
- https://en.wikipedia.org/wiki/Recursive_descent_parser
- https://rosettacode.org/wiki/BNF_Grammar
- https://en.wikipedia.org/wiki/Tiny_BASIC
- https://github.com/slviajero/tinybasic/wiki/Unforgotten---Dr.-Wang's-BASIC
- https://github.com/slviajero/tinybasic/wiki/The-original-Apple-1-BASIC-manual

## Language features in a nutshell 

The interpreter includes most of the Dartmouth language set. Differences are mainly the string handling which was taken from Apple 1 integer BASIC. Autodimensioning of arrays and strings was taken from ECMA BASIC.

The intepreter is compatible with two of the 1976 early basic dialects. It implements the full language set of Dr. Wang's Palo Alto Tinybasic from the December 1976 edition of Dr. Dobbs (https://github.com/slviajero/tinybasic/wiki/Unforgotten:-Palo-Alto-BASIC). This is a remarkably complete little language with many useful features. 

The interpreter also implements the specification of Apple Integer BASIC sold for the Apple 1 computer (https://github.com/slviajero/tinybasic/wiki/The-original-Apple-1-BASIC-manual). It should be fully Apple 1 compatible.

I/O handling and some of the microcontroller BASIC features are new and are not compatible to the BASIC dialects above.

The interpreter is not meant to be compatible to any BASIC dialect. I ported most of the games of 101 BASIC games from 1977 as test programs to test and check compatibility. The main restriction is that arrays can only be one dimensional and that there are no string arrays. All data objects remain in the same memory location once they are defined. There is no garbage collection. This makes the behaviour of the BASIC interpreter deterministic, real time capable and fast. 

Programs are always fully tokenized at input. This includes keywords, numbers, strings and variables names. No lexical analysis is done or needed at runtime. The stored BASIC program resembles more a byte code language than a stored interpreter code. 

The core interpreter loop runs at approximately one token every 7 micro seconds on an Arduino UNO. On an ESP it runs at 1.4 micro seconds per token. 

For further information, please look at: https://github.com/slviajero/tinybasic/wiki

## Files in this archive 

basic.c is the program source. basic.h the header file. It can be compiled directly with gcc for most OSes. No makefile is needed.

TinybasicArduino/TinybasicArduino.ino is an exact copy of basic.c. In addition to this file and basic.h you need hardware.h in your Arduino sketch directory. These are the platform specific definitions and a thin OS like layer for hardware abstraction. This code is under construction and will change soon. All platforms from tiny AVR168 up to the powerful ESP32 are covered by this one file. 

(This has changed - until version 1.2.1 everything was in one file. 1.3b separates hardware from interpreter).

utility/monitor.py is a little serial monitor to interact with the running BASIC interpreter on the Arduino. It allows very simple loading of files into the Arduino and saving of output to a file on a computer. utility/arduinoterm is a wrapper of monitor.py.

utility/dosify converts the code to tcc 2.01 ready format to be compiled in DOSBOX.

The various programs with the extension .bas are test files for the interpreter (arduinotests, testprograms) and some 1976 BASIC games (basicgames).

## What's next

More IoT functionality. More devices.

