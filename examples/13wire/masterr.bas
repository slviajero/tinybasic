10 REM "Master reader from Arduino examples"
20 REM ""
100 REM "the setup()"
110 PRINT "Open master mode on port 8"
120 OPEN &7, 8 
200 REM "the loop()"
210 REM "request 6 byte from the slave"
220 FOR I
230 INPUT &7, #6, A$
240 PRINT A$
250 DELAY 500 
260 NEXT

