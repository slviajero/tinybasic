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
400 END
500 DATA "the", "quick", "brown", "fox", "jumps"
510 DATA "over", "the", "lazy", "dog", ""
520 DATA 72, 69, 76, 76, 79, 32, 87, 79, 82, 76, 68

