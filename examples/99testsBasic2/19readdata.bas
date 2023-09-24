10 REM "READ and DATA test code"
20 REM ""
100 PRINT "Reading strings"
110 READ A$
120 IF A$="" THEN GOTO 150
130 PRINT A$; " ";
140 GOTO 110
150 PRINT 
200 PRINT "Reading numbers and putting characters"
200 FOR I=1 TO 11
210 READ A 
220 PUT A 
230 NEXT 
240 PRINT
300 PRINT "Reading beyond the end"
310 PRINT "Status byte is", @S
320 PRINT "Attempting to read"
330 READ A 
370 PRINT "Status byte is", @S, "now"
380 PRINT "Read data was", A 
400 RESTORE 
410 READ A$, B$, C$
420 PRINT "Data pointer is now", DATA
430 RESTORE 2
440 PRINT "Reading from data pointer 2 now"
450 READ A$
460 PRINT "Result is", A$
500 END
600 DATA "the", "quick", "brown", "fox", "jumps"
610 DATA "over", "the", "lazy", "dog", ""
620 DATA 72, 69, 76, 76, 79, 32, 87, 79, 82, 76, 68

