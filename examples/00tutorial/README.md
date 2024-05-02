# BASIC tutorial, the commands and the language sets

## Programs 

### Palo Alto BASIC language set

This is the minimal BASIC language set also found in many Tinybasic adaptions. The orginal language was described in a Dr. Dobbs Article. It has the commands PRINT, LET, INPUT, GOTO, GOSUB, RETURN, IF, FOR, TO, STEP, NEXT, STOP, LIST, NEW, RUN, ABS, INT, RND, SIZE, REM. Note that there is no THEN in Palo Alto BASIC. This is what you get when compiling with #define BASICMINIMAL. INT, LOAD, and SAVE were not present in Palo Alto BASIC but added here nevertheless.

- Hello World: hello.bas - say hello.
- Table: table.bas - print a formatted table of values.
- Random: random.bas - create random number and show the distribution of them.
- Sort: sort.bas - sort a list of random numbers
- TicTacToe: tictac.bas - play tictactoe against the computer who moves first.
- EEPROM: eeprom.bas - store data to the EEPROM array and access it later

### Apple 1 Integer BASIC language set

The Apple 1 language set has arrays, strings and a number of additional commands. It is compatible to Apple Integer BASIC. Additional commands are NOT, AND, OR, LEN, SGN, PEEK, POKE, DIM, THEN, TAB, END, CLR, HIMEM. 

- Memory: memory.bas - access BASIC memory with PEEK and POKE, find memory layout
- Array: array.bas - array code test program, including @() and @E()
- String1: string1.bas - string constants and variables
- String2: string2.bas - substrings, splitting and concatenating
- String3: string3.bas - characters and strings
- String4: string4.bas - string arrays 
- Logic: logic.bas - boolean expressions
- Parable: parable.bas - draws a parable using TAB to format output

### Stefan's extension

These are a set of useful commands and features missing in Apple Integer BASIC. Some of then I took from other BASIC interpreters, some I missed in the past and added with different features. Commands in this language set are CONT, SQR, POW, MAP, DUMP, BREAK, GET, PUT, SET, CLS, USR, CALL, ELSE.

- StopCont: stopcont.bas - stop and continue a running program
- Euler9: euler9.bas - solve the Euler 9 number theory problem
- Loops: loop.bas - demo for loops in BASIC including while, until constructs
- Conditions: cond.bas - demo for conditions 
- Mathematics: math.bas - demo for square root, powers, and mapping
- Primes: primes.bas - caclulate N prime numbers 
- Primes Loop: primesl.bas - caclulate N prime numbers using special features of BASIC
- Hardware Information: hinv.bas - display the internals of BASIC
- Small Hinv: shinv.bas - display the essentials of a system
- System Information: sysinf.bas - display the language features of BASIC
- PutGet: putget.bas - output single characters
- Dump: dump.bas - show the BASIC memory
- Debug: debug.bas - switch a program to debug mode
- EndMe: endme.bas - ending / resetting the interpreter
- Fibonacci: fibo.bas - print the Golden ration on an Integer BASIC
- Clock: clock.bas - shows time and date and on Arduinos also the temperature
- Serial1: serial1.bas - read from and write to the second serial port
- Mandel: mandel.bas - text graphics of the Mandelbrot set

### Dartmouth extension

For compatibility with the Dartmouth style BASIC interpreters, the following commands have been added const char DATA, READ, RESTORE, DEF, FN, ON.

- ReadData: readdata.bas - reading data 
- OnGo: ongo.bas - demo for ON GOTO type conditions
- Functions: func.bas - demo for DEF FN statements
- Eliza: eliza.bas - the BASIC written psychologist, legendary program from 1979

### File I/O functions

File I/O is supported with the command set CATALOG, OPEN, CLOSE, FDISK, DELETE. 

- FileIO: fileio.bas - write and read a data file 

### Floating point functions 

The classical BASIC floating point functions SIN, COS, TAN, ATAN, LOG, EXP are part of the floating point laguage set.

- Trigonometry: trig.bas - calculate a table of trigonometric functions
- Stirling: stir.bas - caculate factorials and compare to Stirlings function

### The Dark Arts

This language set has a few functions for low level access to the interpreter mechanism. Currently MALLOC, FIND and EVAL are implemented.

- Malloc: malloc.bas - allocate memory and find it by identifier
- Eval: eval.bas - plot a function
- ClrVar: clrvar.bas - delete heap objects

### IoT Extensions 

This language set is work in progress. There are a few string helper functions already finished which will be explained here. The networking and serial protocol features will be explained in another section of the examples tree.

- Convert: convert.bas - convert strings to numbers and vice versa
- Splitstring: splitstr.bas - split a string into substrings
- Nullbuffer: nullbuffer.bas - use the input buffer as a target for PRINT and INPUT

### Timers and Interrupts

BASIC has timers and interrupts build in. They can trigger a GOTO or GOSUB command on a time or external event.

- Timer: timer.bas - blink while doing something in the background
- Timestop: timestop.bas - stop a program after 10 seconds runtime

### Graphics

BASIC graphics commands are plotting and setting colours.

- MandelVGA: mandelv.bas - plot the Mandelbrot set with VGA colours.
- Plot: plot.bas - simple plot test program

### More programs to play with

- Calculator: calc.bas - RPN pocket calculator in BASIC
- CalculatePI: cpinew1.bas - calculate PI to arbitrary precision (thanks to Guido for this program)
- CalculatePI2: cpinew2.bas - using the dark arts feature and char / string conversion
- Tinybasic: tinybasic.bas - a BASIC interpreter for Palo Alto BASIC written in BASIC, requires V2

## The structured language set

- CalculatorS: calcs.bas - RPN pocket calculator in BASIC with WHILE and SWITCH
- Euler: euler.bas - Calculate the GCD using a multiline function

### More BASIC features

In addition to these language sets there are the Arduino IO functions. These features are explained in 01basics-10starter.

IoT and networking functions are also explained in a special section.

## BASIC language features by program 

hello.bas: printing with PRINT, string constants in ""

table.bas: inputting data with INPUT, FOR NEXT loops, formatted printing using #8

random.bas: use of the memory array @(), conditional expressions IF, random numbers RND

sort.bas: use of the memory array @(), conditional expression IF GOTO 

tictac.bas: GOSUB RETURN subroutines, % as a modulo operator

eeprom.bas: use of the EEPROM array @E() and the size of the EEPROM @E, STOP command

memory.bas: use of SIZE and HIMEM, PEEK and POKE, memory as signed char bytes, ABS

array.bas: use of DIM to dimension arrays, EEPROM @E() and end of RAM array @()

string1.bas: string constants, variables, dimensioning with DIM and autodimensioning, LEN

string2.bas: using substring to split and concatenate strings

string3.bas: converting chars, integers and strings, not fully Apple Integer compatible, rather an extension

logic.bas: NOT, AND, OR

parable.bas: TAB command to format output 

stopcont.bas: STOP and CONT command

euler9.bas: SQR for integer or float values, CONT to control loops (apocryphal)

loop.bas: FOR NEXT STEP, ending loops with BREAK, iterating with CONT, while, until

cond.bas: IF THEN ELSE, ELSE and THEN with line numbers or statements

math.bas: simple mathematical functions (integer or float) SQR, POW and 

primes.bas: SQR, arrays 

primesl.bas: special array @(), variable @, BREAK 

hinv.bas: USR command for access to system internals

shinv.bas: USR command and essential system information

sysinf.bas: identify language features of the interpreter

putget.bas: GET and PUT for single character I/O

dump.bas: the DUMP program to examine the memory

debug.bas: SET command, runtime debug

endme.bas: CALL command to end a program

fibo.bas: BREAK command, arrays, GOSUB, modulo operator %

clock.bas: time array @T()

serial1.bas: use of the stream &4 for the second serial port

readdata.bas: READ, DATA and @S 

ongo.bas: ON GOTO statements

func.bas: function definitions

eliza.bas: using BASIC string functions, the original MID$ code is preserved to show the use of Apple Integer BASIC substrings

fileio.bas: OPEN for read and write, CLOSE, PRINT, INPUT and GET for files

trig.bas: SIN, COS, TAN. Calculate PI from ATAN.

stir.bas: EXP, LOG, SQR. Calculate PI from ATAN.

malloc.bas: FIND and MALLOC to allocate memory segments

eval.bas: EVAL command to modify a program

convert.bas: VAL and STR functions to convert numbers and strings

splitstr.bas: INSTR function 

timer.bas: EVERY command to trigger time subroutines

timestop.bas: AFTER command to brach after a given time

calc.bas: GOSUB with an expression, @S with READ and DATA, VAL

calcs.bas: WHILE and WEND, SWITCH CASE SWEND, @S with READ and DATA, VAL

tinybasic.bas: VAL and the @V construct. The MS string compatibility functions MID, RIGHT, LEFT. Needs V2.0.

nullbuffer.bas: PRINT to stream &0, INPUT from it.

euler.bas: Multiline functions with DEF FN and FEND, WHILE WEND, READ DATA handling with @S
