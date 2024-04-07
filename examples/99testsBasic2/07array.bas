10 REM "Array test program"
20 REM "Dimensioning, storing and recalling"
30 REM "Arrays are dimensioned with DIM like A(), B()"
40 REM "Arrays autodimension to size 10 like C()"
50 REM "@E() is the EEPROM array"
60 REM "@() is the end of memory array"
100 DIM B(28)
110 DIM A(18)
120 I=0: J=0
200 PRINT "Testing indexing A(I), B() is zero"
210 FOR I=1 TO 18
220 A(I)=I*I
230 NEXT
240 FOR I=1 TO 18
250 PRINT I, A(I), "=", I*I
260 NEXT
270 FOR I=1 TO 28
280 PRINT I, B(I), "=", 0
290 NEXT
300 PRINT "Testing A(2)"
310 A(2)=127 
320 PRINT #4, A(2), "= 127"
400 PRINT "Autodimensioning C"
410 FOR I=1 TO 10 
420 C(I)=RND(I)
430 NEXT
440 FOR I=1 TO 10
450 PRINT I, C(I)
460 NEXT
500 PRINT "Testing B() and @()"
510 FOR I=1 TO 10 
520 B(I)=I*6: @(I)=I*6
530 NEXT 
540 PRINT "Is the size of the memory array larger than zero? ", @>0
550 FOR J=1 TO 4 
560 PRINT #3,J,B(J),@(J);" = ";6*J
570 NEXT 
600 PRINT "Testing EEPROM"
610 PRINT "Size of EEPROM array=", @E
620 IF @E=0 THEN PRINT "No EEPROM" : END
630 FOR I=1 TO 10 
640 @E(I)=I*I 
650 NEXT 
660 FOR J=1 TO 10 
670 PRINT #3,J,@E(J);" = ";J*J
680 NEXT 
700 END
