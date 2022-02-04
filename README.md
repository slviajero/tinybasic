# Stefan's Tinybasic

## The idea

My attempt to create a small basic interpreter from scratch using frugal programming techniques and minimal memory. The project was inspired by Steve Wozniak's statement that the Apple 1 basic interpreter was the biggest challenge in his professional life. Bill Gates also had started his career and his fortune by writing a basic interpreter for the Altair microcomputer around the some time.

The program was originally written in C with minimal library support. All code was done by hand. The C stack is not used for arithmetic to keep it minimal. The interpreter uses a set of global variables, an own stack and a static memory and variable array to emulate the low memory environment of the early microcomputers. 

The project has outgrown its beginnings by now. It became a full featured BASIC interpreter with some IoT and microcontroller features. 

Arithmetic originally was only 16 bit integer. It is 16 bit, 32bit or float. The full set of logical expresssions with NOT, AND, OR is implemented C style. Conditions are part of the arithemtic and not separate like in many basic dialects. This makes the call stack of the recursive descent deeper but simplifies other code. To reduce the memory footprint of this part of the runtime code no arguments are passed in the C functions. Instead, an own multi purpose 16 bit stack is added. 

Memory access by default is 8bit. For memory, stack and variable access, array logic is used and not C pointers. This allows all pointers to be integers which can be stored on the arithmetic stack if needed. 

The interpreted can be compliled with standard gcc on almost any architecture or in the Arduino IDE without changes. 

Look at the WIKI https://github.com/slviajero/tinybasic/wiki for more information.

See also:
- https://en.wikipedia.org/wiki/Recursive_descent_parser
- https://rosettacode.org/wiki/BNF_Grammar
- https://en.wikipedia.org/wiki/Tiny_BASIC
- https://github.com/slviajero/tinybasic/wiki/Unforgotten---Dr.-Wang's-BASIC
- https://github.com/slviajero/tinybasic/wiki/The-original-Apple-1-BASIC-manual

## Language features in a nutshell 

The intepreter is compatible with two of the 1976 early basic dialects. It implements the full language set of Dr. Wang's Palo Alto Tinybasic from the December 1976 edition of Dr. Dobbs (https://github.com/slviajero/tinybasic/wiki/Unforgotten---Dr.-Wang's-BASIC). This is a remarkably complete little language with many useful features. 

The interpreter also implements the specification of Apple Integer BASIC sold for the Apple 1 computer (https://github.com/slviajero/tinybasic/wiki/The-original-Apple-1-BASIC-manual). It should be fully Apple 1 compatible.

I have never worked with a computer running any of the two BASIC dialects. All the implementation has been done from the manuals cited above.

Many things have been added in addition to these two languages. As the main target hardware are Arduino or ESP microcontrollers, I added EEPROM access, EEPROM program storage and autorun, control of digital and analog I/O as well as the delay function. SPIFFS and SD filesystems are included, Wire library support, RF2401 support and very simple MQTT / Wifi support on ESP. Device drivers for TFT and VGA integrated from the UTFT and FABLIB projects.

The interpreter has grown to include the full Dartmouth language set like READ/DATA, DEFFN and ON/GOTO. 

For further information, please look at: https://github.com/slviajero/tinybasic/wiki

## Files in this archive 

basic.c is the program source. It can be compiled directly with gcc. No makefile is needed.

TinybasicArduino/TinybasicArduino.ino is an exact copy of basic.c, nothing needs to be added or adapted except the defs at the beginning of the code.

utility/monitor.py is a little serial monitor to interact with the running BASIC interpreter on the Arduino. It allows very simple loading of files into the Arduino and saving of output to a file on a computer. utility/arduinoterm is a wrapper of monitor.py.

utility/dosify converts the code to tcc 2.01 ready format to be compiled in DOSBOX.

The various programs with the extension .bas are test files for the interpreter (arduinotests, testprograms) and some 1976 BASIC games (basicgames).

## What's next

More IoT functionality. 

