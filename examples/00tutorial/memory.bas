10 REM "Memory command demo"
20 REM "HIMEN, SIZE, PEEK and POKE command"
100 PRINT "End of BASIC memory", HIMEM
110 PRINT "Total free memory", SIZE
120 T=HIMEM-SIZE
130 PRINT "End of BASIC program", T
200 PRINT "Storing 10 random numbers in memory"
210 FOR I=1 TO 10
220 POKE T+I, RND(127)
230 NEXT I
240 PRINT "Recalling the data"
250 FOR I=1 TO 10 
260 PRINT #4, I, PEEK(T+I)
270 NEXT I
300 PRINT "Storing signed data in memory"
310 POKE A+11, 127
320 PRINT PEEK(A+11), "is 127"
330 POKE A+11, -127
340 PRINT PEEK(A+11), "is -127"
350 POKE A+11, 255
360 PRINT PEEK(A+11), "is 255"
370 PRINT "BASIC memory bytes are signed char"
400 PRINT "Size of EEPROM array", @E
410 IF @E=0 THEN PRINT "No EEPROM" : END
420 PRINT "First 10 bytes of the EEPROM"
430 FOR I=-1 TO -10 STEP -1
440 PRINT #4, ABS(I), PEEK(I)
450 NEXT I
