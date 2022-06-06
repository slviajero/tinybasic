10 REM "Master writer from Arduino examples"
20 REM ""
100 REM "the setup()"
110 PRINT "Open master mode on port 8"
120 OPEN &7, 8 
130 A$="x is "
200 REM "the loop()"
220 FOR I
230 A$(6)=I
240 @S=0
250 PRINT &7, A$
260 IF @S THEN PRINT "Transmission error"
270 DELAY 500 
280 NEXT
