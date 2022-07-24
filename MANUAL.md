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

See 


### LET

### INPUT

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


