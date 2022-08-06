10 REM "Simple mqtt reader"
20 REM "receive a message and switch the led"
100 REM "open a mqtt topic for read"
110 OPEN &9, "iotbasic/command", 0
120 PINM LED, 1
200 REM "wait for a message"
210 FOR I
220 IF AVAIL(9)=0 THEN 280
230 INPUT &9, A$
240 REM "ESP led is inverted"
250 IF A$="on" THEN DWRITE LED, 0
260 IF A$="off" THEN DWRITE LED, 1
270 PRINT "Received message", A$
280 DELAY 1000
290 NEXT
