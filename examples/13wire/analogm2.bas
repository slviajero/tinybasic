10 REM "Master code for analogr2"
20 REM "Display the analog value"
100 REM "setup()"
110 OPEN &7, 8
200 REM "loop()"
210 FOR I
220 @S=0
230 INPUT &7, A$
240 IF @S OR LEN(A$)<2 THEN PRINT "Data error":DELAY 500: CONT
250 L=A$(1): IF L<0 THEN L=L+255
260 H=A$(2)
270 PRINT H*256+L
280 DELAY 300 
290 NEXT

