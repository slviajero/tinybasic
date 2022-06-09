# Example programs for from 101 BASIC games 

## 101 BASIC games 

This was a printed library of games in the 70s. Many examples of ultra short BASIC programs. They are ported here to the Arduino BASIC dialect. Many of the games run on Integer BASIC.

## Programs 

- TicTacToe: tictac.bas - a "computer plays first" tictactoe game
- TicTacToe (improved): tictac2.bas - a tictactoe game with board display
- Rocket: rocket.bas - the classical lunar lander simulation
- HighLow: hi-lo.bas - guessing a number between 1 and 100
- 23Matches: 23match.bas - remove 1, 2, or 3 matches from a pile of 23
- Number: number guessing game, super simple almost Tinybasic compatible
- BuzzWork: create a buzzword bingo winning phrases, from the 70s but still up to date
- Guess: a simple number guessing showing many BASIC language features
- Love: draws a square the the message love
- Dice: simulate the throwing of two dice
- Mathdice: add two dice values and see how fast you are
- Hurkle: find the hurkle in a grid
- RockScissorsPaper: classical games of rock, scissors, and paper
- Craps: Casino game with two dice, real old code, presubably from the 60s
- Lunar: kinematically realistic lunar lander program, needs float

## BASIC language features

tictac.bas: modulo operator % 

tictac2.bas: string variables as array of chars  and conversion to integers

rocket.bas: formatted printing, TAB, strings 

hi-lo.bas: the random number generator, use of @R and MILLIS

23match.bas: the random number generator, use of @R and MILLIS

number.bas: the random number generator, use of @R and MILLIS, except RND and MILLIS really only Tinybasic

buzzword.bas: READ DATA RESTORE mechanism of Dartmouth BASIC, open loops, BREAK

guess.bas: FOR loops with BREAK and CONT features

love.bas: READ DATA RESTORE mechanism of Dartmouth BASIC, strings and arrays, single character output with PUT

dice.bas: formatted printing, arrays

mathdice.bas: timing with MILLIS(100), random numbers

hurkle.bas: save number input using VAL, @S, list parsing with INSTR

rockscic.bas: FOR NEXT loops with CONT

craps.bas: a GOTO nightmare to show how complex conditions were done back in the 60s

lunar.bas: find if interpreter has float with USR(0,3), POW function, formating output with PRINT #











