10 REM "Slave receiver from Arduino examples"
20 REM ""
100 REM "the setup()"
110 PRINT "Open slave mode on port 8"
120 OPEN &7, 8, 1 
200 REM "the loop()"
220 FOR I
230 IF AVAIL(7)=0 THEN 270
240 INPUT &7, A$
250 X=(A$(6))
260 PRINT X
270 DELAY 100 
280 NEXT

