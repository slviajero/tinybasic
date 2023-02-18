# Automated test scripts

## Test Programs How-To 

This folder contains a set of test porgrams for individual features of the interpreter. Run the tests after changes to see if anything is broken. Testing is supported on platforms with a UNIX command line and bash as the program testscripts is a simple shell script. It compares the output of a program with the stored result. If a test has failed the test output is preserved. 

## Tests 

### Core language set

00hello.bas - test the hello world program

01print.bas - simple PRRINT statements, formating and integer output

03let.bas - assignments and integer arithmetic, testing the parser and simple variables

04goto.bas - simple GOTO statements

05gosub.bs - simple GOSUB and RETURN statements

06for.bas - simple FOR loops including the extended features like BREAK and CONT 
