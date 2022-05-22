# Arduino examples 01.Basic ported to basic

## Programs 

### Palo Alto BASIC language set

This is the minimal BASIC language set also found in many Tinybasic adaptions. The orginal language was described in a Dr. Dobbs Article. It has the commands PRINT, LET, INPUT, GOTO, GOSUB, RETURN, IF, FOR, TO, STEP, NEXT, STOP, LIST, NEW, RUN, ABS, INT, RND, SIZE, REM. Note that there is no THEN in Palo Alto BASIC. This is what you get when compiling with #define BASICMINIMAL. INT was not present in Palo Alto BASIC but added here nevertheless.

- Hello World: hello.bas - say hello.
- Table: table.bas - print a formatted table of values.
- Random: random.bas - create random number and show the distribution of them.
- Sort: sort.bas - sort a list of random numbers
- TicTacToe: tictac.bas - play tictactoe against the computer who moves first.

### Apple 1 Integer BASIC language set

The Apple 1 language set has arrays, strings and a number of additional commands. It is compatible to Apple Integer BASIC. 

## BASIC language features 

hello.bas: printing with PRINT, string constants in ""

table.bas: inputting data with INPUT, FOR NEXT loops, formatted printing using #8

random.bas: use of the memory array @(), conditional expressions IF, random numbers RND

sort.bas: use of the memory array @(), conditional expression IF GOTO 

tictac.bas: GOSUB RETURN subroutines, % as a modulo operator





