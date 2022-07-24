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

The Apple 1 language set is based on an early version of the Apple Integer BASIC. I never worked with the language and just took the information from the the manual. The language set adds many useful features like arrays, strings, two letter variables, boolean expressions and a few functions. 

### Variables 

In addition to the static variable A-Z all two letter combinations which are not a keyword are legal variable names. Examples: 

A0, BX, TT 

Variables are placed on a heap that is searched every time a variable is used. Variables which are used often should be initialised early in the code. This makes BASIC programs faster. Static variables A-Z are accessed approximately 30% faster than heap variables. They should be used for loops.

### Strings

String variables can also have two characters followed by the $ symbol. Example: 

ME$="Hello World"

Unlike in the original Apple 1 BASIC, strings can be used without explicitely dimensioning them. They are auto dimensioned to length 32. Strings are static. The entire space is allocated on the heap and stays reserved. This is very different from MS BASIC with a dynamic heap. The saves memory but requires a garbage collector. Static strings like in this BASIC need more memory but make the execution of time critical code more deterministic. 

A string can be dimensioned with DIM to any length. Example: 

DIM A$ (100), C$ (2)

A string of maximum 100 bytes and a string of 2 bytes are created. The maximum length of a string is 65535 in the default settings of the code. See the hardware section of the manual for more information on this.

Strings are arrays of signed integer. String handling is done through the substring notation. Example: 

A$="Hello World"

PRINT A$ (1,4), A$ (6,6), A$ (6)

yields 

Hello W World 

as an output. 

Substrings can be the lefthandside of an expression

A$="Hello World"

A$ (12)=" today"

PRINT A$

outputs

Hello world today

Please look into the tutorial files string1.bas - string3.bas for more information. The commands LEFT$, RIGHT$, and MID$ do not exist and are not needed.

The length of a string can be found with the LEN command. Example:

A$="Hello"
PRINT LEN(A$)

### Arrays

Arrays are autodimensioned to length 10. They start with index 1. If a different array length is needed the DIM command can be used. Example: 

DIM A(100)

Array variables can be used like normal variables except that they cannot be active variables in a FOR loop.

### Logical expressions NOT, AND, OR

Logical operators NOT, AND, and OR have lowest precedence. They are at the same time bitwise logical operations on integers. Example:

PRINT NOT 0

yields -1 which is an integer with all bits set. Logical expressions like 

IF A=10 AND B=11 THEN PRINT "Yes"

can be formed. 

### Memory access with PEEK and POKE

PEEK is a function which reads one byte of BASIC memory. Example: 

PRINT PEEK(0) 

outputs the first byte of BASIC memory.

POKE writes a byte of memory. Example: 

POKE 1024, 0 

If PEEK and POKE are used with negative numbers they address the EEPROM of an Arduino. -1 is the first memory cell of the EEPROM. Peek and poke return bytes as signed 8 bit integers. 

### THEN

THEN is added for comaptibity reasons. Typical statements would be 

IF A=0 THEN PRINT "Zero"

IF B=10 THEN 100 

### END

END ends a program. On systems with EEPROM dummies in flash memory, END also flushed the buffer. On system with file operations, END flushed the file buffers. 

### CLR

CLR removes all variables and stacks. 

### HIMEM

HIMEM is the topmost free memory cell. The difference SIZE-HIMEM is the size of the stored program. 

### TAB

TAB outputs n whitespace characters. Example: 

TAB 20: PRINT "Hello"

### SGN

The SGN function is 1 for positive arguments and -1 for negative arguments. SGN(0)=0. 

### Multidim and String Array capability

If BASIC is compiled with the HASMULTIDIM option, arrays can be twodimensional. Example: 

DIM A(8,9)

A(5,6)=10

The compile option HASSTRINGARRAYS activates one dimensional string arrays.

DIM A$ (32,10)

dimensions an array of 10 strings of length 32. Assignments are done use double subscripts

A$ ()(3)="Hello"

A$ ()(4)="World"

PRINT A$ ()(3), A$ ()(4)

outputs 

Hello World

The first pair of parentheses are the substring part and the second pair the array index.

## Stefan's extension language set

### Introduction

The language extensions here are things I found useful and that were missing in Apple Integer BASIC. They are rather simple extensions of the language.

### FOR loops with CONT and BREAK

FOR loops can be ended with BREAK. Example: 

10 FOR I=1 TO 10

20 PRINT I

30 IF I=8 THEN BREAK

40 NEXT

will end the loop after the 8th iteration. BREAK always ends the innermost loop just like the C break command.

Alternatively CONT can be used to move forward in a loop. Example: 

10 FOR I=1 TO 10 

20 IF I%2=0 THEN CONT

30 PRINT I

40 NEXT

will only display the odd values. CONT skips the entire rest of the loop and skips to the next I. 

Using CONT and BREAK reduces the number of GOTO statements in a program and makes them more readable. 

### Command line CONT

In interactive mode, CONT has a different function. It restarts a program previously stopped with STOP.

### ELSE

ELSE can be used after THEN in one line.

IF A=0 THEN PRINT "Zero" ELSE PRINT "Not zero"

In programs ELSE can also be on the subsequent line

10 IF A=0 THEN PRINT "Zero"

20 ELSE PRINT "Not zero"

### Character output with PUT and GET

Single characters can be written and read with the PUT and GET statement. Like PRINT and INPUT, these commands can take the I/O stream as an argument after the & modifier. Example: 

GET &2, A 

reads a character into variable A from the keyboard I/O stream. 

PUT &2, "H", 65

writes to characters to the display I/O stream. The behaviour of PUT and GET is hardware dependent. On Arduinos they are non blocking byte streams. On POSIX systems GET is blocking input.

### Low level functions USR

USR is a two argument function accessing system properties. See the program hinv.bas in the tutorial for examples. USR can also be used to quickly add functionality to the interpreter. This will be explained in the hardware section of the manual. 

Example for USR:

PRINT "The EEPROM size is ", USR(0,10)

### Low level calls CALL

CALL expects a numerical expression as an argumen. Currently only CALL 0 is implemented. It flushes all open files and ends the interpreter on POSIX systems. On Arduinos CALL 0 restarts the interpreter. 

### Setting system properties with SET

SET is a two argument function setting system property. Please look into the hardware section of the manual for more information.

Example: 

SET 0, 1

activates the debug mode of the interpreter. 

SET 0, 0

ends the debug mode.

### SQR

On an integer BASIC the function calculates an integer approximation of the square root. It is the number closest to the exact result. It can be larger or smaller then the exact result. Example: 

PRINT SQR(8)

In floating point system the library function is used. 

See math.bas in the tutorial for more information.

### POW

This two argument function calulates the n-th power of a number. It replaces the '^' operator of some BASIC dialects. Example: 

PRINT POW(2,4)

In integer systems POW calculates the result in a loop. It returns 0 for negative arguments of the exponent. In floating point systems the C function pow() is used and the result is returned.

See math.bas in the tutorial for more information.

### MAP

MAP is taken from the Arduino library. It maps an integer in a certain range to another ranges using (at least) 32 bit arithmetic. This is useful on 16 bit integer systems. Example: 

M=MAP(V, 1000, 0, 200, 100)

maps the variable V from the range 0 to 1000 to the range 100 to 200.

See math.bas in the tutorial for more information.

### CLS

Clears the screen on display systems. See VT52 capabilities for more information.

### Debugging with DUMP

DUMP writes a memory and EEPROM range to the output. Example:

DUMP 0, 100

writes the first 100 bytes of the program memory. 

### Error Message capability

tbd.

### VT52 capability

tbd.

## Arduino I/O language set

### Introduction

Most of the standard Arduino I/O functions are available on BASIC. The Arduino standard examples have been ported to BASIC to showcase how they are used. Please look into the repo under examples/01basics to examples/10starter for these programs. 

### PINM 

PINM sets a pin to input, output, or input-pullup. Example: 

PINM 10, 0

PINM 11, 1

PINM 12, 2

sets pin 10 to input, pin 11 to output, and pin 12 to input with the pullup active. The values for the second argument may be implementation specific. Some microcontroller cores use additional values. The number given in BASIC is directly handed through to the pinMode() C++ function.

### DWRITE, DREAD, and LED

Digital I/O is done with DWRITE and DREAD. Examples: 

DWRITE 11, 1 

A=DREAD(10)

sets pin 11 to high and reads the value of pin 10 to variable A.

The constant LED is the builtin LED on system. This can be microcontroller dependent.

PINM LED, 1 

DWRITE LED, 1

lights the buildin led.

### AWRITE, AREAD, and AZERO

For analog I/O the command AWRITE and the function AREAD can be used. The pin number in these commands has to be the low level pin code of the microcontroller. For Arduino AVR systems the lowest analog pin number is the pin after the highest digital pin. On an UNO this is 14. The constant AZERO is the number of the lowest analog pin. Example: 

PINM AZERO, 1

PINM AZERO+1, 1

PINM AZERO+2, 0

sets the analog pin a0 and a1 of an Arduino to output and the pin a2 to input.

AWRITE AZERO, 100

AWRITE AZERO+1, 200

V=AREAD(AZERO+2)

write 100 to pin a0, 200 to pin a1 and reads pin a2. 

Pin numbering can be tricky. More information can be found in the hardware section.

### Timing with MILLIS and DELAY

DELAY triggers a delay of n ms. Example:

DELAY 1000 

waits for one second. 

The function MILLIS measures the time in milliseconds divided by a divisor. Example:

A=MILLIS(1) 

B=MILLIS(1000)

A contains the time since start in milliseconds. B is the time since start in seconds. The divisor can be used in 16 bit systems to control overflow of a variable.

### PULSEIN and TONE extensions

tbd.

## File I/O language set

### Introduction 

For systems with a filesystem there is a set of file access commands. Only one directory is supported. The file system command cannot handle subdirectories. One open file for read and one open file for write is supported. 

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




