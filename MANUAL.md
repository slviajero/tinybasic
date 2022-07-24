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

PRINT ends a line with LF. No CR is sent at the end of a line.

### LET

LET assigns an expression to a variable. It can be ommited and is only added to the language set to ensure compatibility. Typical LET statement would be 

LET A=10

A=B/2+C

Expression evaluation is done left to right with multiplication and division precedence before addition and subtraction. Comparision have lowest precedence and evalute true to -1 and false to 0. This is not C style boolean logic but closer to the old MS BASIC definition. With this, NOT, AND and OR in the Apple 1 language set can be used as bitwise operators because NOT -1 is 0.

Strings evaluate to the ASCII value of the first character.

A="A" : PRINT A

outputs 65.

The operator % calculates the integer divisions modulus. Example:

B=A%2 

to test divisibility.

### INPUT

INPUT accepts a list of string constants and variable. String constants are output and variable request a user input. A "?" prompt is only displayed if not string constant has been supplied. Example: 

INPUT "Input a number", A

INPUT "First number: ", A, "Second number: ", B

Unlike MS BASIC comma separated lists cannot be used as input values.

INPUT A, B

would prompt for two separate number inputs even if two number like e.g. 17,19 are enters after the first prompt. This saves memory. Please look at IOT extension for a way to split lists.

In version 1.3. INPUT cannot read elements of string arrays. Only not indexed string variables are implemented.

Like PRINT, the & modifier can be used to specify an input channel.

INPUT &2, A

would read data from the keyboard of a Arduino standalone system.

### GOTO

GOTO branches to the line number specified. Expressions are accepted. Examples:

GOTO 100

GOTO 100+I

The later statement can be used to program ON GOTO constructs of Dartmouth BASIC.

BASIC has a line cache and remembers jump targets to speed up the code. 

### GOSUB and RETURN

GOSUB is identical to GOTO and saves the location after the GOSUB statement to a return stack. RETURN ends the execution of the subroutine. 

The GOSUB stack is shallow with a depth of 4 elements on small Arduinos. This can be increased at compile time.

### IF 

IF expects an expression and executes the command after it if the condition is true. 0 is interpreted as false and all non zero values as true. Examples:

IF A=0 GOTO 100

IF B=A PRINT "Equal"

There is not THEN in the core language set. THEN is part of the Apple 1 language set.

### FOR loops

FOR have the form

FOR I=1 TO 10 STEP 2: PRINT I: NEXT I

Specifying the parameter I in NEXT is optional. STEP can be ommited and defaults to STEP 1. 

Unline in other BASIC dialects the loops. 

FOR I=10: PRINT I: NEXT

and 

FOR I: PRINT I: NEXT

are legal. They generate infinite loops that can be interrupted with the BREAK command which 
is part of the extension language set. 

All parameters in FOR loops are evaluated once when the FOR statement is read. FOR loops use the memory location as jump target in NEXT. They are faster than GOTO loops. 

The statement NEXT I, J to terminate two loops is not supported. Each loop needs to have their own NEXT statement.

### STOP

STOP stops a program. There is no END in the core language set.

### REM

Comment line start with the REM statement. As BASIC tokenizes the entire input, comments should be enclosed in comments. Example:

10 REM "This is a comment"

### LIST

Lists the program lines. Examples: 

LIST 

LIST 10

LIST 10, 100

The first statement lists the entire program, the second only line 10. The last statement lists all line between 10 and 100.

### NEW

Deletes all variables and the program. 

### RUN

Starts to run a program. A line can be specified as first line to be executed. RUN deletes all variables and clears the stacks. GOTO linnumber can be used for a warm start of a program.

### ABS

Calculates the absolute value of a number. Example:

A=ABS(B)

### INT

Calculates the integer value of a number. Example:

A=INT(B)

### RND

Calculates a random number. The builtin random number generator is a 16/32 bit congruence code and is good enough for games and simple applications but repeats itself rather fast. The argument of the function is the upper bound. 

On an integer BASIC 

PRINT RND(8)

would produce numbers from 0 to 7, while 

PRINT RND(-8)

would produce numbers between -8 and -1. 

The random number seed can be changes by using the special variable @R. See the "special variable" section for more information.

### SIZE

Outputs the space between the top of the program and the bottom of the variable heap. It is the free memory the interpreter has. 

### Storing programs with SAVE and LOAD

LOAD and SAVE are part of the core language set if the hardware has a filesystem. On POSIX systems (Mac, Windows, Linux) they take a file name as argument. Default is "file.bas". 

Arduino systems with only EEPROM support save the program to the EEPROM in binary format, overwriting all data on the EEPROM by it. 

If a file system is compiled to the Arduino code. The commands work like on a POSIX system. LOAD "!" and SAVE "!" can be used in these cases to access programs stored in EEPROM.

### Special variables and arrays 

The character @ is a valid first character in variable names and addresses special variables. These variables give access to system properties. 

@() is an array of numbers starting from the top of memory down to the program area. It can be used like any other array in BASIC. Indexing starts at 1. 

@ is the upper array boundary of the array @() i.e. the number of elements in the array. 

The program area is protected by BASIC. The maximum index prevents a program to overwrite the program.

@E() is the EEPROM array. It indexes the entire EEPROM area down from the highest address. 

@E is the number of elements in the EEPROM array. Unlike with the memory array @() there is no protection of saved programs in the EEPROM. @E() can overwrite the program space. 

@R is the random number seed. It can be written and read. 

@O is the currently active output stream.

@I is the currently active input stream. 

@C is the next character of the input stream or output stream. 

@A is the number of available character of the current input stream.

The variables @O, @I, @C, and @A can be used for byte I/O on any stream. See the examples section for more information on this.

The array @t() is the real time clock array. See the hardware drivers section for more information.

## Apple 1 language set

### Introduction

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




