10 REM "I2C analog slave"
20 REM "A is the analog port"
30 A=AZERO
100 REM "the setup()"
110 PRINT "Open slave mode on port 8"
120 OPEN &7, 8, 1 
200 REM "loop() binary data transfer"
210 FOR I
220 V=AREAD(A)
230 A$(1)=INT(V)%256 
240 A$(2)=INT(V)/256
250 PRINT &7, A$
260 DELAY 300
270 NEXT I

