10 REM "Simple mqtt writer"
20 REM "read an analog input and transfer the data"
30 AP=AZERO
100 REM "open a mqtt topic for write"
110 OPEN &9, "iotbasic/data", 1
120 PINM AP, 0
200 REM "send a message "
210 FOR I
220 A$="analog ": A$(8)=STR(AREAD(AP))
240 PRINT &9, A$
250 IF NETSTAT<>3 THEN PRINT "No connection"
260 DELAY 3000
270 NEXT
