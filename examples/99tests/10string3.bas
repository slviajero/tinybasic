10 REM "Strings in BASIC (3)"
20 REM "chars and strings"
100 DIM A$(80)
110 PRINT "Integers are chars"
120 FOR I=1 TO 26
130 A$(I)=64+I
140 NEXT I
150 PRINT A$
200 PRINT "Chars are chars or integers, mind the ()"
210 FOR I=1 TO 10
220 C=A$(I, I)
230 PRINT A$(I, I), (A$(I, I)), C
240 NEXT I
300 PRINT "Chars are taken from the beginning"
310 FOR I=1 TO 10
320 C=A$(I)
330 PRINT C, A$(I)
340 NEXT I
400 PRINT "Chars are signed, mind the ()"
410 A$=230
420 PRINT (A$)
500 END

