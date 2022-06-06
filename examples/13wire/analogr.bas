10 REM "I2C analog slave"
20 REM "A is the analog port"
30 A=AZERO
100 REM "the setup()"
110 PRINT "Open slave mode on port 8"
120 OPEN &7, 8, 1 
200 REM "loop() ASCII data transfer of 6 chars"
210 FOR I
220 V=AREAD(A)
230 PRINT &7, #6, INT(V)
240 DELAY 300
250 NEXT I
