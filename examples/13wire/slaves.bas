10 REM "Slave sender from Arduino examples"
20 REM ""
100 REM "the setup()"
110 PRINT "Open slave mode on port 8"
120 OPEN &7, 8, 1 
200 REM "the loop()"
210 REM "send exactly 6 byte to the buffer"
220 FOR I
230 PRINT &7, "hello "
240 DELAY 100
250 IF USR(7, 1)=0 THEN PRINT "Master requested data"
260 NEXT
