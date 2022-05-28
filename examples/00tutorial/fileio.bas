10 REM "Simple FILE I/O demo"
20 REM ""
30 N=4
40 DIM A$(80)
100 PRINT "Write squares to file"
110 OPEN "daten.txt",1
120 FOR I=1 TO N
130 PRINT I, I*I
140 PRINT &16, I
150 PRINT &16, I*I
160 NEXT I
170 CLOSE 1
200 PRINT "Read data as numbers"  
210 OPEN "daten.txt"
220 FOR I=1 TO N
230 INPUT &16, A
240 INPUT &16, B
250 PRINT "Read "; A; "^2="; B
260 NEXT
270 CLOSE 0
300 PRINT "Read data as strings, use EOF status"
310 @S=0
320 OPEN "daten.txt"
330 IF @S<>0 THEN PRINT "OPEN failed" : END
340 FOR I
350 INPUT &16, A$
360 IF @S=-1 THEN BREAK 
370 PRINT "Line",I,"string '";A$; "' Status = " @S
380 NEXT
390 CLOSE 0
400 PRINT I, "lines read"
410 PRINT "Status =", @S
500 PRINT "Read Character by Character use EOF status"
510 @S=0
520 OPEN "daten.txt"
530 IF @S<>0 THEN PRINT "OPEN failed" : END
540 FOR I=1
550 GET &16, A
560 PRINT "Character ";I;" :",A
570 IF A=-1 THEN BREAK
580 NEXT
590 PRINT I, "characters read"
600 PRINT "Status =", @S
610 CLOSE 0
700 END 



