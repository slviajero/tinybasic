# tinybasic

My attempt to create a small basic interpreter from scratch using frugal programming techniques and minimal memory. The project was inspired by Steve Wozniak's statement that the Apple 1 basic interpreter was the biggest challenge in his professional life. Bill Gates also had started his career and hir fortune by writing a basic interpreter for the Altair microcomputer. 

The program is written in C but only putchar and getchar are used from the libraries. All other code is done by hand. The C stack is not used for arithemtic or runtime pruposes to keep it minimal. The interpreter uses a set of global variables, an own stack and a static memory and variable array to emulate the low memory environment of the early microcomputers. 

>>
