# Stefan's IoT BASIC in a nutshell

## BASIC language sets

This BASIC interpreter is structured in language sets. They can be compiled into the code separately. With every language set there are more features and command. This makes it adaptable to the purpose. The manual is structured according to the language sets. 

The intepreter has two data types - numbers and strings. The number type can be set at compile time. It is either an integer or foating point. Depending on the definition of number_t in the code the interpreter can use everything from 16 bit integers to 64 bit floats as basic number type. String variables are part of the Apple 1 language sets. They are static, i.e. they reserve the entire length of the string on the heap. Depending on the definition of the string index type in the code, strings can be either 255 characters or 65535 characters maximum length. 

Keywords ad variable names are not case sensitive. They are printed as uppercase with the LIST command. Strings and string constants are case sensitive. 

There is a set of examples program in examples/00tutorial. They are referred to here as "the tutorial".

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

Like PRINT, the & modifier can be used to specify an input stream.

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

File names must be strings. To load a program you need to enter

LOAD "myprog.bas"

i.e. with the filename enclosed quotes.

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

@S is the I/O error status. See the file I/O section for more information on it.

The variables @O, @I, @C, and @A can be used for byte I/O on any stream. 

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

Error messages are stored in Arduino program memory. For systems with low program memory undefining the #HASERRORMSG flag removes explicit error messages. 

### Terminal emulation and VT52 capability on display systems

BASIC has a minimal set of terminal control characters build in. Compiling the code with the HASVT52 option adds a subset of the VT52 terminal commands for systems with displays. 

Most importantly 

PUT &2, 12

clears the screen.

The special variables @X, @Y contain the cursor position. The variables are read write. 

@X=10: @Y=10: PUT "X"

would write the letter X at position 10, 10.

The array @D() is the display character buffer by column and row. It is also read write and can be used to display characters directly.

Please look into the hardware section for the display driver commands.

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

### PULSEIN and PLAY extensions

Two more compley Arduino I/O function are available from BASIC. 

PULSEIN reads a pulse on a pin. The first argument is the pin number, the second whether a LOW=0 or HIGH=1 state is expected. The third argument is the timeout in milliseconds. Note the difference to the original Arduino pulseIN(). The low level Arduino commands expects the pulse length in microseconds. 

PLAY is a wrapper around the Arduino tone() function for Arduino systems. For ESP32 VGA systems it is mapped to the sound generator function of the FabGL library. More information on this can be found in the hardware section. 

## File I/O language set

### Introduction 

For systems with a filesystem there is a set of file access commands. Only one directory is supported. The file system command cannot handle subdirectories. One open file for read and one open file for write or append is supported. The file I/O commands mask the different underlying filesystems. Currently EEPROM EFS, SD, ESPSPIFFS and LittleFS are supported. Look for more information in the hardware section.

### Opening and closing files with OPEN and CLOSE

Files are opened with the OPEN command. Open needs a filename and optionally a file open mode as argument. As a file open mode 0 is read, 1 is write and 2 is append. Read mode is the default is no argument is given. Examples:

OPEN "data.txt"

OPEN "data.txt", 0

OPEN "temp.txt", 1

OPEN "append.txt", 2

The first two commands are equivalent, the file "data.txt" is opened for read. The second line opens "temp.txt" for write. A new file is created if it doesn't exist. If the file exists writing starts at the beginning and existing data is overwritten. The third commands opens the file for append. 

File name lengths are filesystem specific. The maximum length in BASIC is 32 characters.

Files that have been written need to be closed not to lose the data. CLOSE without an argument closes the read file. This is not really necessary. CLOSE 1 or CLOSE 2 close the write file. Opening a new file automatically closes an open file. 

OPEN and CLOSE and be used to open and close other I/O streams as well. For this, a file modifier can be added. Example:

OPEN &7, 64, 1

This command opens the Wire stream for write addressing the device 64 as a target. See the hardware section for more information.

### DOS commands CATALOG, DELETE, FDISK

CATALOG lists the files in the filesystem. Am optional string pattern can be used to list only file that begin with it. Example: 

CATALOG

CATALOG "da"

The first command lists all file, the second one all file beginning with the pattern "da".

DELETE deletes one file. The name has to match exactly. No pattern matching is supported.

FDISK formats a file system. This is not supported for all file systems. Arguments depend on the hardware implementation. Please look in the hardware section for more information.

### Reading and writing with INPUT, PRINT, and the error variable @S

Files can be written and read with PRINT and INPUT adding the file stream modifier. The I/O stream number for files is 16. Example:

PRINT &16, "Hello World"

writes "Hello World" to the open write file.

INPUT &16, A\$

reads the string A\$ from the file. INPUT can have comma seperated arguments. Each variable expects the input in a new line of the file. Like for in console input, INPUT does not split the I/O. Example:

INPUT &16, A, B

would read the first line of the file as a number into variable A and the second line into variable B.

I/O operations ususally report no errors on the console and keep the the program running if an error occurs. The variable @S contains the error status of the operation. @S has to be reset explicitly before using it because it remembers and error status of previous operations and is never reset by the interpreter. Example:

@S=0

INPUT &16, A$

IF @S=-1 THEN PRINT "End of file reached"

@S takes the value -1 for the end of file condition. For general error it takes the value 1.

Tutorial: fileio.bas

### Character IO with GET and PUT

Like PRINT and INPUT, the byte I/O command can take a stream modifier. Example: 

GET &16, A

would read one byte from the file putting the signed ASCII value to variable A.

## Float language set

### Introduction

Floating point arithemtic is taken from the standard C float library of the platform. The size of the numbers is the size of the base type number_t in the C code. It is set to float as a default but can be changed to double. 

### SIN, COS, TAN, ATAN

The trigonometric function are standard BASIC. They are using radians as angle format. Examples: 

PI=4* ATAN(1)

PRINT SIN(PI)* SIN(PI)+COS(PI)* COS(PI)

PI is not a predefined constant but can be calculated using ATAN. The second line should output 1.

Tutorial: trig.bas

### LOG and EXP

LOG is the natural logarithm and EXP the exponantial function. 

Tutorial: stir.bas

### Floating point precision 

Floating point numbers are displayed as integers if they are smaller then 1e8. Numbers from 1e8 on are displayed in exponential notation. Numbers smaller then 1e-8 are also displayed in exponential notation. 

The biggest accurate integer in a 32 bit float is 16777216. The number can be recalled in BASIC by USR(0, 5).

## Dartmouth language set

### Introduction

Since the early days of BASIC there were a few language features which I summarized as Dartmouth extension. DATA and DEF were in the First Edition of Dartmouth BASIC in the early 1960. ON appeared in the Fourth Edition in 1968.

### DATA, READ, and RESTORE

Data statements can be anywhere in the program. Data items can be strings or numbers. Example:

10 DATA "Hello", 3.141, "A"

READ A\$ : PRINT A\$

READ A: PRINT A

READ C: PRINT C

Will print 

Hello

3.141

65

Strings have to be in quotes. Reading data with the READ command converts the data in the same way than the assignment commands. Strings reading numbers with contain the respective ASCII character. Numbers reading strings will contain the numerical ASCII code. 

Unlike other BASIC versions, reading past the end of DATA will not lead to an error. The status variable @S will contain the value 1 once one reads past the last DATA item. @S is to reset explicitely after this. 

RESTORE resets the data pointer. 

Unlike other BASIC versions READ cannot have an argument list in the current version. 

Tutorial: readdata.bas

### DEF FN

Functions can have one numerical argument and a two character name. Example: 

10 DEF FN TK(X) = X+SIN(X)

Functions have to be DEFed before use. 

Tutorial: func.bas

### ON 

ON is used in combination with GOTO or GOSUB arguments. Examples: 

10 ON X GOTO 100,200,300

Tutorial: ongo.bas 

## Darkarts language set

### Introduction 

The dark arts language set contain a set of command which can cause evil. BASIC is a beginner language and protects the user from dangerous things. This makes it somehow rigid. The three dark arts commands give access to the inner working of the heap and the program storage. They have side effects and can destroy a running program.

### MALLOC

MALLOC allocates a junk of memory on the heap and returns the address. The function receives two arguments. One is the identifier. It is a 16 bit integer. The second argument is the number of bytes to be reserved. Example: 

A = MALLOC(1, 64)

This reserves 64 bytes which can be accessed with PEEK and POKE as 8 bit signed integers. 

MALLOC helps to reserve storage in a controlled way. It is no side effects and can be used savely as long as the POKEs stay within the allocated range.

Tutorial: malloc.bas

### FIND

FIND finds a memory segement on the heap. It can be used to find memory segements allocated with MALLOC but it also finds variables, strings and arrays. 

In all of these examples the argument of the function is the name of an object and not an expression. It has to be the pure string or array without any subscripts.

All heap variables can be searched. Example: 

A0=10 

PRINT FIND(A0)

This command outputs the address of the variable A0. The variable is stored with the least significant byte first. The size of the variable area is the size of number_t. It is 2 bytes for integer BASICs and 4 bytes for floating point BASICs.

Arrays are searched by specifiying the array name without arguments. Example:

DIM A(8)

PRINT FIND(A())

Arrays are stored like variables with the lowest array element first. 

Strings can be searched as well. Example: 

A\$="Hello World"

PRINT FIND(A\$)

This command outputs the address of a string. If A$ is a pure string and not a string array, the first byte is the least significant byte of the length of the string. The second byte is the most significant byte of the length for 16 bit strings. All following bytes are payload. Strings are not necessarily 0 terminated. One has to process the length in all code accessing strings directly.

If a memory segment allocated with MALLOC needs to be searched, a numerical expression can be specified. Example: 

A = MALLOC(1, 64)

PRINT FIND(1)

If the segment to be searched is stored in a variable, FIND has to be called like in this example:

M=1

PRINT FIND((M))

Note the inner braces around M. They are needed here as they trigger expression evaluation. This is unusual for BASIC interpreters. 

FIND itself has no side effects but POKEing around in the heap area can kill the interpreter if the structure of the heap list is destroyed.

Tutorial: find.bas

### EVAL

EVAL adds a new line to the BASIC code at runtime. Doing so has many side effects and EVAL does not protect you in any way. Examples: 

10 EVAL 20, "PRINT X"

20 PRINT "Hello World"

RUN 

LIST 

In the examples above line 20 is replaced by the string in the EVAL command.

EVAL includes the line to the program heap. This changes memory addresses of all code behind the inserted line. FOR loops, GOSUB commands, the linenumber cache and READ/DATA statements use plain memory addresses. This means that the DATA pointer, all active FOR loops and active GOSUBs break. A safe way to use EVAL is to change only lines further down in the code outside of any active FOR loop and to RESTORE the DATA pointer after EVAL. 

See eval.bas in the tutorial for more information.

## IOT language set

### Introduction

The IoT language set are commands needed to use IoT services. They help to implement I/O function. This part of the interpreter is under active development. Some commands may change in the future.

### AVAIL

AVAIL returns the number of characters ready for input on an input stream. Example: 

PRINT AVAIL(4)

returns the number of characters on input stream 4 which is the secondary serial port. The function is hardware dependent.

### String commands INSTR, VAL, and STR

INSTR finds a character in a string and returns the index. Example: 

A\$="1, 2, 3"

A=INST(",", A\$)

finds the first comma in the string. INSTR can be used to split strings. See splitstr.bas in the tutorial for more information.

VAL scans a string for a number and returns the value. If no number is found the return value is 0 and the error status @S is set to 1. Example: 

A\$="125"

A=VAL(A$)

STR converts a number to a string. Example: 

A\$=STR(125)

Tutorial: splitstr.bas, converst.bas

### SENSOR

SENSOR is the generic sensor interface. It handles sensor drivers in the hardware dependent code. The first argument of the function is the sensor number. The second argument can be used to indentify interal properties of the sensor. If the second argument is zero, SENSOR returns if the sensor code is available. Example: 

10 IF NOT SENSOR(1, 0) THEN PRINT "no DHT11 sensor detected": END

20 PRINT SENSOR(1, 2)

The first command looks if there is a DHT temperature sensor. The second command read the temperature value.

### SLEEP

SLEEP puts a system into deep sleep mode. It requires one argument which is the sleep time in milliseconds. The command is currently only implemented on ESP8266 and ESP32 systems. The microcontroller restarts after deep sleep. This command needs to be used with an autostart program.

### NETSTAT

In command mode NETSTAT displays the network status. It is implemented for all Wifi systems and Arduino Ethernet. As a arithemtic constant it returns a network status. With 0 the system is not connected. 1 means connected but no MQTT connection. 3 is network and MQTT connected.

The network code is under active development.

## Graphics language set

### Introduction

The graphics language set is a thin layer over the low level graphics functions of the graphics driver. They try to be as generic as possible and avoid driver specific code. Some of the commands behave differently on different platforms. The graphics commands were first implemented with an SD1963 TFT in mind. Currently various monochrome and color displays are supported.

### COLOR

Sets the color of the graphics pen. COLOR either accepts either 3 arguments for rgb color or one argument for vga like colors. The exact color palette depends on the graphics driver. Example: 

COLOR 255 

COLOR 100, 100, 100

### PLOT

PLOT draws one pixel on the display. The coordinate system starts at the upper left corner of the display. First arguments is the x coordinate, second argument is the y coordinate. Example:

PLOT 100, 100

A pixel is drawn in the colour specified by the COLOR command.

### LINE

LINE draws a line from one point to another. Example: 

LINE 100, 100, 200, 200

### CIRCLE and FCIRCLE

CIRCLE and FCIRCLE draw a circle or filled circle around a point on the canvas with radius r. The last argument is the radius. Example:

CIRCLE 100, 100, 50

### RECT and FRECT

RECT and FRECT draw a rectangle or filled rectangle. The first pair of argument is the left lower corner. The second pair is the right upper corner. Example: 

RECT 100, 100, 200, 200

# Hardware drivers 

## I/O Streams

### Introduction

BASIC I/O goes through input output streams. Every stream has an integer number. Sending data to a stream sends it through to a device driver. These device drivers handle the underlying OS or microcontroller cores. The logic is as close as possible to the Arduino stream class. 

Streams are

1: Console input and output

2: Keyboard or keypad input and display output

4: Secondary serial stream

7: I2C protocol 

8: RF24 radio interface

9: MQTT network connection

16: File I/O

All other stream numbers are currently unused but will be allocated to drivers in the future. The stream numbers below 32 should not be used for extensions. 

Stream numbers are agruments of the modifier of OPEN, CLOSE, PRINT, INPUT, GET and PUT. Typical commands would look like:

INPUT &2, A$

PRINT &4, "Hello world"

GET $2, A

PUT &16, "A", "B"

The expression after & is the I/O stream number.

There is a default I/O stream compiled into the code. For systems not compiled with the STANDALONE option this default stream is the console stream 1. If the STANDALONE option is used, the default stream is 2. 

The active I/O stream can be changed by stetting the special variable @O for output and @I for input. The two code lines

10 @O=2: PRINT "Hello World"

and 

10 PRINT &2, "Hello World"

both write "Hello World" to the display. 

The active I/O stream is always reset to the default stream once the interpreter changes to interactive mode. This is a failsafe to avoid a program to shut out a user permanently.

The default stream can be changed permanently with the SET command.

SET 3,0 sets the default output stream to console.

SET 3,1 sets the default output stream to display.

SET 5,0 sets the default input stream to console.

SET 5,1 sets the default input stream to display.

### Serial Stream

Stream 1 is the default serial I/O stream on Arduino IDE programmed microcontrollers and it is standard input and output on POSIX like systems. The behaviour of stream 1 differed between the two version.

On microcontrollers stream 1 is a true byte I/O stream and non blocking. The command

GET A

will return immediately and return 0 in the variable A if no character is available on the stream. The function AVAIL(1) or the special variable @A will return the correct number of characters in the serial buffer. If there is no character, the result will be 0.

On POSIX systems, getchar() is used on OS level. This means that I/O on stream 1 is blocking. 

GET A

will wait for a character. AVAIL(1) or @A will always return 1 to make sure that a program like this

10 IF AVAIL(1) THEN GET A

runs correctly on both archtitectures.

For line oriented input with the INPUT command, the difference is irrelevant. INPUT works in the same way on both system types.

There is currently no mechanism to change the precompiled baud rate of the serial stream at runtime of a BASIC program. Default in the interpreter code is 9600 baud. This can be changed at compile time by changing serial_baudrate in the code.

A line end is LF, which means ASCII 10. This is UNIX style end of line. DOS and other systems usually send CR LF which is 13 10 in decimal ASCII. If you use a terminal program to interact with BASIC, "only LF", should be configured in the settings. Currently stream 1 cannot be reconfigured to use LF CR. Stream 4, the secondary serial stream, can use CR LF. This is mostly relevant for input. 

### Display and keyboard drivers

Keyboard and display are accessed through stream 2. Only one device can be present, i.e. one keyboard and one display. Multidisplay or multikeyboard systems are not supported. Stream 3 is reserved for these usecases but currently not implemented. 

BASIC currently supports either PS2 keyboards and the keypad of the LCDSHIELD as input device. For PS2 keyboards on standard Arduino systems the patches PS2keyboard library should be downloaded from my repo for optimal functionality. It implements keyboard.peek() which has a few advantages. 

PS2 keyboard input is buffered and non blocking. AVAIL(2) gives the correct number of characters in the keyboard buffer. 

GET &2, A

return 0 if no character is present and the ASCII value if a character is there. 

GET &2, A$ 

transfers the result directly in the string A$ as a first character. The string is empty if no key was pressed.

The keymap is compiled into the code. A few standard US or European keymaps are supported.

If the code is compiled with the LCDSHIELD option, the keys of the shield can be used to input data. The "select" key of the shield is mapped to ASCII 10 means "end of line". The arrow keys of the shield are mapped to the digits 1 to 4. 

INPUT &2, A

can be used on an lcd shield for number input. The command returns after select and A has a valid number. Alternatively 

INPUT &2, A$ 

will enter a string consisting of the digits 1 to 4. 

GET &2, A 

will contain either 0 if no key is pressed or the ASCII code of the key. The command will wait until the key is released. Keypad input is unbuffered. No interrupt or timer function is currently implemented on keypads. This will change for future releases of the code. 

ASCII output to a display is sent through the output stream 2. Displays are handled by a generic display driver. For low memory systems the display driver supports a non scrolling / unbuffered mode. Only a few basic display functions are supported. If there is enough memory, the display driver can be compiled in scrolling / buffered mode. For the the option DISPLAYCANSCROLL has to be defined in hardware-arduino.h. All characters are buffered in a display buffer which can be accessed byte wise. 

In non buffered mode the display driver has the following functions: 

The cursor of the display can be accessed special variables @X and @Y. Both variables are read/write. One can find out where the cursor is and set its position. Output with PUT &2 and PRINT &2 goes to the next cursor postion.

Sending ASCII character 12 clears the display. PUT &2, 12 is clear screen on the display.

Sending ASCII character 10 goes to the beginning of next line.

Sending ASCII character 13 goes to the next line leaving the column the same.

Sending ASCII character 127 goes back one cursor position and clears the character. It is "delete".

ASCII character 27 (ESC) is sets the terminal to esc mode and triggers vt52 character interpretation. 

ASCII character 3 (ETX) is used for page mode displays to trigger the redraw.

Buffered displays add scrolling. Once the cursor goes beyond the last line, the display is redraw with the first line disappearing. This is simple line based software scrolling. 

Each character in the display is buffered a display buffer. This buffer can be accessed through the special array D(). Writing to the array immediately display the character on the display. Reading from it gives the displayed character at the index position. The array starts from 1. The index advances by column and then by row. 

Currently LCD displays 16x2 and 20x4, Nokia 5110, ILI9488, SSD1306, and SD1963 are supported. All displays use 8x8 fonts. Nokia has 10 columns and 6 rows. SSD1306 character buffer dimension depend on the display size. 16x8 is the most common size. ILI9488 has 20 columns and 30 rows. It is portrait mode by default. A 7 inch SD1963 has 50 columns and 30 rows. 

Some displays use page based low level graphics drivers. These displays mirror the entire canvas in memory on a pixel basis. When one draws to the canvas, nothing is shown until an update command transfers the buffer to the display. SPI bus based monochrome display use this technique. The ILI9488 and SSD1306 Oled driver use this mechanism. By default these displays behave like the other displays and page mode is of. Drawing of graphics and text appears slow.

Activating the page mode is done with

SET 10, 1

after this all updates stay in memory until an ETX code is sent with 

PUT &2, 3

only then the display is redrawn. The display can be set back to character mode by 

SET 10, 0

For epaper displays page mode is default. Epaper disply integration is in preparation.

In addition to the displays, VGA output is supported with the FabGL library on ESP32 systems. These systems are described in the special systems section.

### Secondary serial stream

The secondary serial stream is addressed through stream number 4. For microcontroller platforms with a hardware secondary serial port, the Serial1 object on the Arduino library is used. This can be changed in hardware-arduino.h by modifiying the functions beginning with prt. For microcontrollers without multiple hardware serial ports Softwareserial is used. The RX and TX pins are set through the macro SOFTSERIALRX and SOFTSERIALTX. Default are pin 11 and 12. This conflicts with the SPI pins. If SPI is to be used the soft serial pins have to be changed.

Default baudrate on the secondary serial port is 9600 baud.

The secondary serial port implements a blockmode function. Blockmode is on by default. Without blockmode an INPUT command would wait until a LF is received. This means that 

INPUT &4, A$

would never return if the sender on the serial port does not send LF. If a modem with AT commands or any other device is on the input port, the BASIC program would stop indefinitely if the sender side misbehaves. With blockmode active, the INPUT command handles I/O differently. 

With the command 

SET 7, 1 

the blockmode parameter can be set to 1. In this mode the entire serial buffer is transfered to the string A$. The number of characters waiting in the serial buffer for transfer can be found by the function AVAIL(4). This number of character is in the string after INPUT &4 with the length of the string set to the value of AVAIL(4) before INPUT. Immediately after read, AVAIL(4) is zero. For secondary serial blockmode 1 is the default.

Setting the blockmode parameter to a value greater 1 the timeout mode is activated. INPUT reads all characters in the buffer and waits timeout microseconds for further input until it returns. It also returns if the string is full. Example: 

SET 7, 500

INPUT &4, A$

In this mode the INPUT command would read all characters, wait for 500ms for further input, and returns latest after this 500ms or when the string is full. 

For communication with modems, the secondary serial channel has one more parameter. Normally only LF is sent after the end of a line. Some modems and other devices on a serial port expect a LF CR sequence at the end of a line. The communication is blocked if CR is not sent. With the command 

SET 6, 1 

CR mode is activated. A print command to the serial channel like 

PRINT &4, "AT"

would print the characters "A", "T", followed by a LF (ASCII 10) and a CR (ASCII 13).

Answer of a model could be collected with the input sequence above.

Unlike the primary serial port, the baudrate of the secondary port can be changed by the BASIC program. Example:

SET 8, 31250 

would set the baudrate to the standard MIDI baudrate of 31250. Any allowed baudrate of the hardware platform can be used. The serial port is reset after this command.

### Wire communication

### Radio communication

### MQTT 

### Filesystems

## Special systems 

### ESP32 VGA with FabGL








