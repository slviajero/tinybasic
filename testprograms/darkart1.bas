100 REM "A demo program for the arcane features of BASIC"
110 REM "Volume one of the dark arts: access to memory"
120 REM "A variable B0 searched by malloc"
200 REM "First construct a string in the input buffer containing"
210 REM "the VARIABLE token -124 and the name"
220 @I$=-124 : @I$(2)="B0" 
230 REM "Search function"
240 A=USR (3,0)
250 PRINT "Adress of variable B0 is "; A
260 IF A=0 THEN PRINT "Variable was never defined."
270 B0=109
280 A=USR (3,0)
290 PRINT "Adress of variable B0 is "; A
300 REM "A string variable is searched and created"
310 REM "Token values for strings is -123"
320 @I$=-123 : @I$(2)="C0"
330 A=USR (3,0)
340 PRINT "Adress of string C0 is "; A
350 IF A=0 THEN PRINT "String was never defined."
360 A=USR (4,15)
370 PRINT "bmalloc() was called to create string"
380 PRINT "Adress of string C0 is "; A
390 C0$="Hello World" : PRINT C0$
400 REM "Find the lengths of an object"
410 @I$=-123 : @I$(2)="C0"
420 A=USR(5,0)
430 PRINT "Byte length of string is "; A
500 REM "Create a buffer with no BASIC an user defined"
510 REM "token value 100 which is invisible to BASIC"
510 @$=100 : @I$(2)="C1"
520 A=USR (4,80)
530 PRINT "bmalloc() was called to create buffer"
540 PRINT "Adress of buffer C1 is "; A
550 PRINT "Storing data"
560 FOR I=0 TO 79 : POKE A+I, I+1 : NEXT
570 PRINT "Recalling data"
580 FOR I=0 TO 79 : PRINT PEEK(A+I); " "; : NEXT
590 PRINT
600 END
