# tinybasic

My attempt to create a small basic interpreter from scratch using frugal programming techniques and minimal memory. The project was inspired by Steve Wozniak's statement that the Apple 1 basic interpreter was the biggest challenge in his professional life. Bill Gates also had started his career and hir fortune by writing a basic interpreter for the Altair microcomputer. 

The program is written in C but only putchar and getchar are used from the libraries. All other code is done by hand. The C stack is not used for arithemtic or runtime pruposes to keep it minimal. The interpreter uses a set of global variables, an own stack and a static memory and variable array to emulate the low memory environment of the early microcomputers. 

Arithmetic is 16 bit. The full set of logical expresssions with NOT, AND, OR is implemented C style. Conditions are part of the arithemtic and not separate like in many basic dialects. This makes the call stack of the recursive descent deeper but simplifies other code. To reduce the memory footprint of this part of the runtime code no arguments are passed in the C functions. Instead, an own multi purpose 16 bit stack is added. 

Memory access is strictly 16bit and 8bit. For memory, stack and variable access, array logic is used and not C pointers. This allows all pointers to be short integers which can be stored on the arithmetic stack if needed. 

The interpreted can be compliled with standard gcc on almost any architecture or in the Arduino IDE (set the ARDUINO definition for this).

On an Arduino, programs can be saved on the EEPROM and also autorun from there.

See also:
- https://en.wikipedia.org/wiki/Recursive_descent_parser
- https://rosettacode.org/wiki/BNF_Grammar
- https://en.wikipedia.org/wiki/Tiny_BASIC

Language features in a nutshell: 

16bit arithmetic with a range -32768 to 32767 

26 integer variable A-Z 

Expressions include basic arithmetic, conditions and logical operators AND, OR, NOT.

Conditions work C style as part of the expression evaluation with 0 as FALSE and all other values as TRUE.

Functions ABS, SGN, SQR, RND, FRE, PEEK are implemented. 

SQR is an approximate square root. It is exact for perfect squares.

RND is a 16 bit constant seed random number generator which always delivers the same sequence.

FRE takes a dummy argument and has the value of the rest of the core basic memory.

PEEK access to memory works in a standard way. On an arduino, negative values of the argument access the EEPROM.

Basic statements are PRINT, INPUT, LET, IF THEN, GOTO, FOR TO STEP - NEXT - BREAK, GOSUB - RETURN.

PRINT is pretty standard, printed objects are concatenated without spaces.

INPUT is standard with one string and a variable list. Entering # as first character of input end the program (Arduino and Unix).

LET is standard and can be ommited. 

IF THEN is standard, THEN allows a line number or a statement as an argument.

GOTO expects an expression, evaluates it and then jumps to the line number. This is similar to the old small basic variants.

FOR TO STEP NEXT is implemented with a maximum nesting size of 4. STEP 0 is legal and generates an infinite loop.

BREAK is "apocryphal" and not generally found in basic. It can be used to abort a FOR loop in the middle, clearing the stack.

GOSUB, RETURN work in a standard way but GOSUB accepts expressions like GOTO (it is the same function).

Program control statements include RUN, CLR, NEW, LIST, DUMP, SAVE, LOAD. 

RUN starts the program. It has no argument. Running programs on an Arduino can be interrupted using #. This is not implemented on Unix. 

CLR sets all variable to zero.

NEW sets all variables to zero and deletes the entire program.

LIST doesn't allow an argument - only full listing is supported.

DUMP prints the initial section of the memory as a decimal dump.

END ends a program and preserves the interpreter state completely. It is more like STOP in standard basic. STOP is not implemented.

CONT restarts the program where it ended, using only the here variable. 

REM is a comment but behaves different than in other BASICs. It requires one string argument. This is because the tokenizer will try to tokenize every command and the only way to store characters in a program is to convert them into the internal string format.

SAVE on the arduino writes a program to the EEPROM, LOAD reloads it. An EEPROM autorun feature is planned. On other platforms a file "file.bas" is read or written for an ASCII output of the program. This is rudimentary and not well tested. Move the test file "euler9.bas" to "file.bas" to try it out.
 
DWRITE, DREAD, AWRITE, AREAD, PINM, and DELAY are the Arduino I/O functions.

POKE is added to access the basic memory. Like for peek, negative values access the EEPROM.

SET is a general command changing internal interpreter settings: set -1 sets a program saved in EEPROM to autorun on Arduino, set 1 sets the program back to normal mode, set 0 removes the program flag and makes it unloadable.

Target machines and compilation:

No makefile or headers are provided. All is in one C file, function definitions are in a initial section in the code. On Mac und Linux the definition #undef ARDUINO has to be used. With this putchar and getchar are the only functions in the standard library for input and output. #define ARDUINO replaces these functions with Serial.write and Serial.read for the arduino. The code can be directly compiled in the arduino ide with the build in C++ compiler. The standard serial library costs 180 bytes RAM and 1 kB of flash memory. It is the single biggest library on an arduino. Current arduino code size is 8 kB, memory demand is 1.2 kB for a 512 byte basic memory. My test system is a Arduino Uno, 16 kB flash, 2 kB memory.

Interpreter architecture:

The interpreter is build with the goal of minimal RAM footprint. Static global variables are used for most interpreter functions. The C stack is avoided. A set of global variables controls the interpreter

mem: the basic memory array of 8 bit characters.

stack: the 16 bit arithemtic stack and the integer stack pointer sp.

ibuffer: a generic string input buffer and its pointer bi.

x, y: 16 bit accumulators.

xc, yc: 8 bit accumulators.

here, here2 and here3: program memory pointes implemented as 16 bit integers.

ir: a general character pointer for the internal string operations.

FOR and GOSUB have own shallow stacks.

The intepreter is written in 3 layers

Layer 0: variable handling, errors, stack, mathemetics and input/output. Layer 0 functions do not change the interpreter state.

Layer 1: lexical analysis, storing and retrieving programs, expression evaluation. Layer 1 changes global variables and the intepreter state. 

Layer 2: Statements, control commands and the statement loop.

Arduino I/O: DWRITE, DREAD, AWRITE, AREAD, PINM, and DELAY 

Syntax of the Arduino functions: 

"PINM pin, mode": mode is 0 for OUTPUT, 1 for INPUT, 2 for INPUT_PULLUP

"DWRITE pin, value": value can be 0 or 1

"A = DREAD(pin)"

"AWRITE pin, value": value can be between 0 and 255.

"A = AREAD(pin)": A can have a value between 0 and 1023.

"DELAY time": time is measured in miliseconds, maximum delay is 32 seconds. 

The pin value is never checked for validity on the BASIC level. For all analog functions the numeric pin values need to be known. No translation to the conventinal A0-Ax values is done. See https://www.arduino.cc/reference/de/language/functions/analog-io/analogwrite/ for more information.

Weaknesses and know bugs:

Error handling is rudimentay. Many things are allowed that should be forbidden. This is an anarchic piece of software.




