# Stefan's IoT BASIC in a nutshell

## BASIC language sets

This BASIC interpreter is structured in language sets. They can be compiled into the code separately. With every language set there are more features and command. This makes it adaptable to the purpose. The manual is structured according to the language sets. 

The intepreter has two data types - numbers and strings. The number type can be set at compile time. It is either an integer or foating point. Depending on the definition of number_t in the code the interpreter can use everything from 16 bit integers to 64 bit floats as basic number type. String variables are part of the Apple 1 language sets. They are static, i.e. they reserve the entire length of the string on the heap. Depending on the definition of the string index type in the code, strings can be either 255 characters or 65535 characters maximum length. 

Keywords ad variable names are not case sensitive. They are printed as uppercase with the LIST command. Strings and string constants are case sensitive. 

In the core language set 26 static variables with names A-Z are supported. The Apple 1 languages set has two character variables, arrays and string variables. 

There is a set of examples program in examples/00tutorial. They are reffered to here as "the tutorial".

## Core language set

### Introduction

The core language set is based on the Palo Alto BASIC language. This is the grandfather of all the Tinybasics currently on the market. Commands are PRINT, LET, INPUT, GOTO, GOSUB, RETURN, IF, FOR, TO, STEP, NEXT, STOP, REM, LIST, NEW, RUN, ABS, INT, RND, SIZE.

In the core language set there are 26 static variables A-Z and a special array @() which addresses the free memory. If an EEPROM is present or the EEPROM dummy is compiled to the code, the array @E() addresses the EEPROM. 

### PRINT

PRINT prints a list of expressions and string constants. The separator ';'' causes data to be concatenated while ','' inserts a space. This is different from MS BASIC print where ','' prints into data fields and ';'' inserts spaces is numbers are printed. Formatted printing is supported with the '#' modifier. 

Examples: 

PRINT "Hello World"

PRINT A, B, C

PRINT #6, A, B, C

Tutorial programs: hello.bas and table.bas.

BASIC I/O is stream based. Streams are numbered. The console I/O on a POSIX system or Serial on an Arduino is stream number 1. Other prefefined stream numbers are 2 for displays, 4 for secondary serial and 16 for files. Please look at the file I/O section and the hardware driver chapter for more information. 

Printing to a display would be done with 

PRINT &2, "Hello World"

while 

PRINT &16, "Hello World"

prints to a file.

### LET

LET assigns an expression to a variable. It can be ommited and is only added to the language set to ensure compatibility. Typical LET statement would be 

LET A=10

A=B/2+C

Expression evaluation is done left to right with multiplication and division precedence before addition and subtraction. Comparision have lowest precedence and evalute true to -1 and false to 0. This is not C style boolean logic but closer to the old MS BASIC definition. With this, NOT, AND and OR in the Apple 1 language set can be used as bitwise operators because NOT -1 is 0.

Strings evaluate to the ASCII value of the first character.

A="A" : PRINT A

outputs 65.

### INPUT

INPUT accepts a list of string constants and variable. String constants are output and variable request a user input. A "?" prompt is only displayed if not string constant has been supplied. Example: 

INPUT "Input a number", A

Unlike MS BASIC comma separated lists cannot be used as input values.

INPUT A, B

would prompt for two separate number inputs even if two number like e.g. 17,19 are enters after the first prompt. This saves memory. Please look at IOT extension for a way to split lists.

In version 1.3. INPUT cannot read elements of string arrays. Only not indexed string variables are implemented.

### GOTO



### GOSUB and RETURN

### IF 

### FOR loops

### STOP

### REM

### LIST

### NEW

### RUN

### ABS

### INT

### RND

### SIZE

### Storing programs with SAVE and LOAD

### Special variables and arrays 

## Apple 1 language set

### String variables, arrays, DIM, and LEN

### Logical expressions NOT, AND, OR

### Memory access with PEEK and POKE

### THEN

### END

### CLR

### HIMEM

### TAB

### SGN

### Multidim and String Array capability

## Stefan's extension language set

### FOR loops with CONT and BREAK

### Command line CONT

### ELSE

### Character output with PUT and GET

### Low level functions USR

### Low level calls CALL

### Setting system properties with SET

### SQR

### POW

### MAP

### CLS

### Debugging with DUMP

### Error Message capability

### VT52 capability

## Arduino I/O language set

## PINM 

### DWRITE, DREAD, and LED

### AWRITE, AREAD, and AZERO

### Timing with MILLIS and DELAY

### PULSEIN and TONE extensions

## File I/O language set

### Opening and closing files with OPEN and CLOSE

### DOS commands CATALOG, DELETE, FDISK

### Reading and writing with INPUT, PRINT

### Character IO with GET and PUT

## Float language set

### SIN, COS, TAN, ATAN

### LOG and EXP

### Floating point precision

## Dartmouth language set

### DATA, READ, and RESTORE

### DEF FN

### ON 

## Darkarts language set

### MALLOC

### FIND

### EVAL

## IOT language set

### AVAIL

### String commands INSTR, VAL, and STR

### SENSOR

### SLEEP

### NETSTAT

## Graphics language set

# Hardware drivers 

## I/O Streams




