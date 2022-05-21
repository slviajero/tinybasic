# Arduino examples 08.Strings ported to basic

## Programs 

- CharacterAnalysis: charan.bas - Analyise character types
- StringAdditionOperator: stradd.bas - Adding strings and adding numerical data to strings
- StringAppend: strapp.bas - Appending to a string
- StringCaseChange: strcase.bas - String case change 
- StringCharacter: strchar.bas - String character manipulation

## BASIC language features 

The examples in the folder need #BASICINTEGER as a language stetting. This is a full featured integer BASIC.

DEF FN is used to define character functions in charan. This program illustrates how GET and AVAIL work to access the serial input stream.

DIM dimensions strings in stradd.bas and the other programs in the folder. s

stradd.bas and strapp.bas illustrate the concatenation of strings using the substring notation. 

LEN is the length of a string, STR converts a number to a string.

strcase.bas shows how numerical ASCII values and string characters can be mixed. This program has a subtlety. The brackets around the character expression in line 130 and 180 are needed. They trigger the numerical evaluation of a string expression. 

## Original files

See https://docs.arduino.cc/built-in-examples/ for further information
