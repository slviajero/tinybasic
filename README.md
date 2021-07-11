# tinybasic

My attempt to create a small basic interpreter from scratch using frugal programming techniques and minimal memory. The project was inspired by Steve Wozniak's statement that the Apple 1 basic interpreter was the biggest challenge in his professional life. Bill Gates also had started his career and hir fortune by writing a basic interpreter for the Altair microcomputer. 

The program is written in C but only putchar and getchar are used from the libraries. All other code is done by hand. The C stack is not used for arithemtic or runtime pruposes to keep it minimal. The interpreter uses a set of global variables, an own stack and a static memory and variable array to emulate the low memory environment of the early microcomputers. 

Currenty a basic set of commands is implemented PRINT, INPUT, GOTO, LET, IF THEN.

Arithmetic is 16 bit. The full set of logical expresssions with NOT, AND, OR is implemented C style. Conditions are part of the arithemtic and not separate like in many basic dialects. This makes the call stack of the recursive descent deeper but simplifies other code. To reduce the memory footprint of this part of the runtime code no arguments are passed in the C functions. Instead, an own multi purpose 16 bit stack is added. 

Memory access is strictly 16bit and 8bit. For memory, stack and variable access, array logic is used and not C pointers. This allows all pointers to be short integers which can be stored on the arithmetic stack if needed. 


See also:
- https://en.wikipedia.org/wiki/Recursive_descent_parser
- https://rosettacode.org/wiki/BNF_Grammar
- https://en.wikipedia.org/wiki/Tiny_BASIC


