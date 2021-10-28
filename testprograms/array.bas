100 REM "Stefan's BASIC Array test program"
110 REM "Trying dimensioning, format print"
120 REM "FOR loops and the Wang array"
200 DIM B(10)
210 DIM A(8)
300 PRINT "Testing A(8)"
310 A(2)=127 
320 PRINT #4, A (2 ), " = 127"
400 PRINT "Testing B(10) and @()"
410 FOR I =1 TO 10 
420 B (I )=I *I : @(I )=I *I
430 NEXT 
440 FOR J =1 TO 10 
450 PRINT #3, J ,B (J ), @( J) " = ", J*J
460 NEXT 
500 PRINT "Testing EEPROM"
510 IF @E=0 THEN PRINT "No EEPROM" : END
520 FOR I =1 TO 10 
530 @E(I )=I *I 
540 NEXT 
550 FOR J =1 TO 10 
560 PRINT #3, J ,@E (J ), " = ", J*J
570 NEXT 
600 END
