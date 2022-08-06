# BASIC example programs

## BASIC linguistics

BASIC comes in many dialects. No language has spread so widely. In the 70s and 80s when homecomputers first came up, every manufacturer created their own version. The BASIC interpreter was at the same time the OS of the computer and was build to fit the hardware. 

This BASIC interpreter is not meant to be compatible with any of the existing dialects. It is not derived from the Arduino Tinybasic code which is itself a version of the old 68000 Tinybasic. It is an independent full featured programming language combining features from various BASIC dialects. The basis is Palo Alto BASIC and Apple Integer BASIC. Imagine Apple Integer BASIC not being discontinued and replaced by Microsoft BASIC but developed further into a floating point capable full featured language. Other ingredients like the autodimensioning of arrays were taken from the ECMA 55 and 116 standards. Original Dartmouth BASIC features are also included. I added a few features of my own. Things that were missing in other interpreters.

Main target platform is now Arduino and ESP. Many of the I/O features of these systems are supported - I2C / Wire communication, networking, filesystems, radio, sensors. POSIX systems like Mac, Windows, and Raspbian are also supported. The later with wiring code. 

As the language has become fairly large and complex (10000 lines of C source code, almost 100 keywords and commands) the programs in this folder are meant to serve as a tutorial how to use the interpreter and showcase its features.

## Programs and folders

00tutorial is a set of demo programs how to use features and commands of BASIC. These programs should work on most platforms as they use almost no Arduino specific features. 

01-10 are the standard Arduino examples from  https://docs.arduino.cc/built-in-examples/ ported to BASIC. Please look at the original page for wiring and other details. These programs are mainly to showcase how to program an Arduino with BASIC and how the Arduino I/O functions are mapped to BASIC commands. Almost all examples could be ported showing that the Wiring interface is fully implemented and transparent in BASIC.

11stefan are benchmarks and examples for this BASIC dialect. They adress specific features on performance and timing in BASIC. As the interpreter has no dynamic memory functions it runs (almost) real time capable at a millisecond time scale.

12systems are demo programs for standalone BASIC systems. 

13wire shows the properties of the I2C interface commands in BASIC. Again, some of the Arduino examples of Wire have been ported. Other example programs have been added.

14games are examples from David Ahl's legendary book 101 BASIC computer games. All games have been ported now.

15radio are examples for the use of the RF2401 radio interface.

16mqtt are examples for the MQTT code.

## Main differences between BASIC and other BASIC dialects.

While I tried to stay as closely as possible to the mainstream of the BASIC language, a few features are really very different. This is due to the heritage of Apple Integer BASIC and Palo Alto BASIC.

Strings are arrays of signed integers in the range from -128 to 127. They have a fixed static maximal length. If a string is used without declaring it with DIM, it has a default length of 32. The classical Microsoft BASIC string expressions do not work. Apple Integer BASIC substring notation is used instead. The string examples in 00tutorial show how it is done. Strings can be any length. Default maximum string length is 65536 i.e. 16 bit string length counter. 

In all arithemtic expressions strings evaluate as integers yielding the first characters signed ASCII values. Only if the righthand and lefthand side of an expression or condition is a string value, string assignment and compare code is triggered. 

This BASIC has only one number type. Depending on the compiler settings it is either float with 32 or 64 bit precision or integer with 16 or 32 precision. Float is default for all POSIX like platforms and MSDOS while integer is default for Arduinos and ESPs. Changing the number type in the C code automatically adapts the precision of BASIC arithemtic. With float activated, all canonical math functions like SIN, COS, TAN, LOG, and EXP are there. SQR is always there, for integers it calculates a best approximation integer value. The exponentiation operator ^ is replaced by a two valued function POW, C style. Floating point BASIC have full floating point exponentiation while integer BASIC use an integer only multiplication algorith. RND is an Apple Integer BASIC style random number generator with a 16 bit cycle. The Arduino map() function made it to BASIC as MAP. It calculates accurate integer fractions.

Many Arduino I/O functions are ported. Analog and digital input and output work just like in C++, pulses, delays and millis are there and the tone() function made it to BASIC as PLAY command.

The most importent difference is I/O in general. BASIC has I/O streams roughly comparable to the concept of Enterprise BASIC of the early 80s. Currently the console stream (&1), the display and keyboard stream(&2), the serial stream (&4), the I2C stream (&7), the radio interface stream (&8), the MQTT network stream (&9) and the file I/O stream (&16) are supported. These streams can be accessed through the OPEN, CLOSE, PRINT, INPUT, PUT, GET, and AVAIL commands and functions.

File I/O is implemented, including SD, SPIFF, and LittleFS support. 

A major difference to other BASIC are the special variables. These variables start with @ as a variable name. They allow access to low level features of the interpreter and the underlying platform. @() is the memory array, @E() the EEPROM array. @T$ is the timer string, while @T() are the real time clock values @D() is the display buffer array. There are a few other examples, all shown in the programs in this folder.

The commands EVAL, MALLOC, and FIND are the dark arts of BASIC programming. It allows run time modification of the running program, access to the variable heap, and creating of buffers.

SET, CALL and USR access low level features of the interpreter. These commands can be extended to integrate own code.

Arrays can be 2d, string arrays 1d. 
