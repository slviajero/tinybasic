# Example programs for from 101 BASIC games 

## 101 BASIC games 

This was a printed library of games in the 70s. Many examples of ultra short BASIC programs. They are ported here to the Arduino BASIC dialect. Many of the games run on Integer BASIC.

## Programs 

- TicTacToe: tictac.bas - a "computer plays first" tictactoe game
- TicTacToe (improved): tictac2.bas - a tictactoe game with board display
- Rocket: rocket.bas - the classical lunar lander simulation
- HighLow: hi-lo.bas - guessing a number between 1 and 100
- 23Matches: 23match.bas - remove 1, 2, or 3 matches from a pile of 23
- Number: number.bas - number guessing game, super simple almost Tinybasic compatible
- BuzzWork: buzzword.bas - create a buzzword bingo winning phrases, from the 70s but still up to date
- Guess: guess.bas - a simple number guessing showing many BASIC language features
- Love: love.bas - draws a square the the message love
- Dice: dice.bas - simulate the throwing of two dice
- Mathdice: mathdice.bas - add two dice values and see how fast you are
- Hurkle: hurkle.bas - find the hurkle in a grid
- RockScissorsPaper: rockscis.bas - classical games of rock, scissors, and paper
- Craps: craps.bas - casino game with two dice, real old code, presubably from the 60s
- Lunar: lunar.bas - kinematically realistic lunar lander program, needs float
- Nicomach: nicomach.bas - guessing a number from the modulo 3, 5, 7
- Chief: chief.bas - small math puzzle 
- Chemist: chemist.bas - calculate ratios against the stopwatch
- Letter: letter.bas - guess a letter 
- Name: name.bas - spell your name backward and sorted
- Change: change.bas - calculates the change of an amount
- Trap: trap.bas - trapping a number between an upper and lower bound
- Train: train.bas - trainer to estimate speed distance solutions
- Stars: stars.bas - find a number from a bargraph indicator
- Sinewave: sinewave.bas - plot a sine wave
- RussianRoulette: russian.bas - play russian roulette with spin after each click
- Reverse: reverse.bas - sort numbers by reversing
- Poetry: poetry1.bas and poetry2.bas - generate odd poetry 
- Litquiz: litquiz.bas - literary quiz 
- Kinema: kinema.bas - calculate kinematics 
- FlipFlop: flipflop.bas - flipping onjects puzzle, really hard
- Mugwump: mugwump.bas - find 4 mugwumps in a field
- Gunner: gunner.bas artillery shooting duell
- Hamurabi: govern ancient Sumer
- Gomoko: place 5 chips in a row - Go style

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

nicomach.bas: DELAY function, strings, modulo operator

chief.bas: using TAB to format text output, USR(0,3) to detect float capability

chemist.bas: timing with MILLIS()

letter.bas: conversion of strings to numbers with (A$)

name.bas: convert strings to arrays and back

change.bas: conditions IF THEN 

trap.bas: FOR LOOPS with CONT

train.bas: elementary BASIC features

stars.bas: random numbers, elementary BASIC syntax

sinewave.bas: IF THEN ELSE, USR(0,3) floating capability detection

russian.bas: IF THEN ELSE, FOR LOOPS with CONT and BREAK

reverse.bas: Arrays and subroutines with GOSUB

poetry1.bas and poetry2.bas: conditional expressions with GOTO numerical_expression or ON GOTO respectively

litquiz.bas: ON GOSUB, IF THEN ELSE as 2 line statement

kinema.bas: IF THEN ELSE, GOSUB, floating point arithmetic

flipflop.bas: characters in arrays, character output with PUT

mugwump.bas: arrays, IF THEN ELSE 2 line statements, GOSUB, CONT and BREAK in FOR loops

gunner.bas: I left this one as it was, wonderful GOTO mess example of the early BASIC days

hamurabi.bas: standard BASIC syntax, USR(0, 3) to find floating point capability

gomoko: arrays, standard BASIC, simple FOR NEXT and conditions

## Sources

Original programs of the book "BASIC Computer Games"

This is an unofficial mirror of the source code provided inside the book **"Basic Computer Games"**, aka **"101 BASIC Computer Games"**, by David H. Ahl, published in 1978. 

The files were taken from http://vintage-basic.net/games.html and ported to Stefan's Tinybasic to see how the interpreter would work. 

The archive itself was a fork from https://github.com/GReaperEx/bcg.

























