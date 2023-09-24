10 REM "Stopping after some time"
100 AFTER 10 GOTO 1000
110 T=MILLIS(1)
120 FOR I 
130 IF MILLIS(1)-T>100 BREAK 
140 NEXT
150 PRINT "Timer failed"
160 END
1000 PRINT "Program stopped with timer"
1010 STOP
