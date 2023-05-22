10 REM "Elementary malloc demo"
20 REM ""
100 PRINT "Allocate 16 bytes of memory with identifier 1"
110 M=MALLOC(1, 16)
120 PRINT "Address of memory segment", M
200 PRINT "Storing data"
210 FOR I=0 TO 15
220 POKE M+I, I*2 
230 NEXT I 
300 PRINT "Retrieving data"
310 FOR I=0 TO 15
320 PRINT I, PEEK(M+I)
330 NEXT I 
400 END
