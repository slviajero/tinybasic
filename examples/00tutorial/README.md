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

### Dartmouth extension

For compatibility with the Dartmouth style BASIC interpreters, the following commands have been added const char DATA, READ, RESTORE, DEF, FN, ON.

### File I/O functions

File I/O is supported with the command set CATALOG, OPEN, CLOSE, FDISK, DELETE. 

### Floating point functions 

The classical BASIC floating point functions SIN, COS, TAN, ATAN, LOG, EXP are part of the floating point laguage set.

### The Dark Arts

This language set has a few functions for low level access to the interpreter mechanism. Currently MALLOC, FIND and EVAL are implemented.

In addition to these language sets there are the Arduino IO functions. These features are explained in 01basics-10starter.

Graphics functions are ommited here, they will be explained in a special section of the examples code as well.

IoT and networking functions are also explained in a special section.

## BASIC language features by program 

hello.bas: printing with PRINT, string constants in ""

table.bas: inputting data with INPUT, FOR NEXT loops, formatted printing using #8

random.bas: use of the memory array @(), conditional expressions IF, random numbers RND

sort.bas: use of the memory array @(), conditional expression IF GOTO 

tictac.bas: GOSUB RETURN subroutines, % as a modulo operator

eeprom.bas: use of the EEPROM array @E() and the size of the EEPROM @E, STOP command

memory.bas: use of SIZE and HIMEM, PEEK and POKE, memory as signed char bytes

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


