# Automated test scripts

## Test Programs How-To 

This folder contains a set of test porgrams for individual features of the interpreter. Run the tests after changes to see if anything is broken. Testing is supported on platforms with a UNIX command line and bash as the program testscripts is a simple shell script. It compares the output of a program with the stored result. If a test has failed the test output is preserved. 

## Elementary Tests 

These tests check the language features. They are approximately ordered by language sets. Some overlap exists. Many of the tests have been written since the beginning of the project and are part of the tutorial. Floating point tests are written to work with BASICFULL on a MacOS X system with 32bit floats. 

### Core language set

00hello.bas - test the hello world program

01print.bas - simple PRRINT statements, formating and integer output

03let.bas - assignments and integer arithmetic, testing the parser and simple variables

04goto.bas - simple GOTO statements

05gosub.bs - simple GOSUB and RETURN statements

06for.bas - simple FOR loops including the extended features like BREAK and CONT 

### Strings, arrays and the heap

07array.bas - one dimensional arrays, including autodimension, memory array @() and EEPROM array @E()

08string1.bas - simple string operations, autodimension and compare

09string2.bas - substrings and concatenation

10string3.bas - strings and character variables

11string4.bas - string arrays 

12clrvars.bas - clearing variables, heap sizes and one darkart feature 

### Apple 1 and Stefan's extensions language set

13cond.bas - simple IF THEN ELSE test

14fibo.bas - testing integer arithemtic including the modulo operator % 

15logic.bas - logic operators NOT, AND, OR

16parable.bas - Apple 1 stype TAB

### File I/O subsystem

17fileio.bas - Open, close, write and read to a file 

### Dartmouth language set

18func.bas - DEF FN mechanism and READ DATA

19readdata.bas - READ, DATA and RESTORE

20ongo.bas - ON GOTO, READ and DATA

21ongosub.bas - ON GOSUB, READ and DATA

### Floating point numbers

22trig.bas - a table of trigonometric functions SIN, COS, TAN and ATAN

23stir.bas - calculate stirlings function, testing EXP, LOG, SQR, ATAN and DEF FN

24mandel.bas - calculate the Mandelbrot set, iterations and float comparisons 

### Darkarts language set 

25malloc.bas - allocating memory with MALLOC, PEEK and POKE, FIND, CLR with argument

26eval.bas - self modifying programs with EVAL

### IoT language set

27splitstr.bas - splitting a string with INSTR

28convert.bas - converting a string to a number with VAL and back with STR

### Timers 

29timestop.bas - program stop with AFTER GOTO

30every.bas - output with EVERY GOSUB

31error.bas - trapping errors with the ERROR GOTO command

### Complex tests

These programs are longer test programs testing a group of features. 

32euler9.bas - solution of the Euler 9 problem for 1008. Tests integer arithmetic and core BASIC features.

33primes.bas - calculates the first 100 primes. Tests arrays and integer arithemtic.

34primesl.bas - uses the memory array @() to store the prime table. 

35math.bas - tests SQR, MAP and POW as well as DATA and READ

36tictac.bas - runs a game of tic tac toe against the computer. Integer arithmetic and GOSUB.

37love.bas - print a nice message. Tests array and character conversion.

38poetry2.bas - write a poem. Test the random number generator and ON GOTO.

39eliza.bas - the simple psycologist. Strings, READ DATA and a lot of commands.

## Errors and Error handling

These tests stress test BASIC, provoke errors or go to limits.

40forfor.bas - FOR loop stress test and error handling 

41arrayfun.bas - Array test program with redim and range errors

42gosub.bas - GOSUB stress test

43gogogo.bas - GOTO, IF THEN ELSE stress tests

44deffn.bas - DEF FN some function tests 

45ongo.bas - a few ON GO oddities

46output.bas - print output with error conditions 

## Structured BASIC extensions

47while.bas - while loops 

48until.bas - until loops 

49switch.bas - switch and case statements 

50teststruct.bas - block statements and ifs

## Some games as test programs 

51rocket.bas - the lunar lander program, tests arithemtic and input handling







