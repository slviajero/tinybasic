10 REM "Mqtt modem - use an ESP modules to send mqtt"
20 REM "The I/O buffer"
30 DIM A$(128)
100 REM "Setup section, open a read and write topic"
110 OPEN &9,"iotbasic/data",1
120 OPEN &9,"iotbasic/command"
200 REM "The loop, input from serial and send/receive"
210 INPUT "",A$
220 IF A$="#" THEN END
230 PRINT &9,A$
240 DELAY 100
250 REM "The first character printed indicates the status"
260 IF NETSTAT=3 THEN PRINT "+";
270 IF NETSTAT=1 THEN PRINT "0";
280 IF NETSTAT=0 THEN PRINT "-";
290 IF AVAIL(9)=0 THEN PRINT: GOTO 210
300 INPUT &9,A$
310 PRINT A$
320 GOTO 210
