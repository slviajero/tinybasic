10 REM "Avanced rf24 writer"
20 REM "Read a message from a slave and print it"
100 REM "Open the radio pipe for write and read "
110 OPEN &8, "radi2", 1
120 OPEN &8, "radi1", 0
130 IF @S THEN PRINT "Open error": END
200 REM "Main loop, wait for a message and then react"
210 FOR I
220 PRINT &8, "send"
230 IF @S THEN PRINT "Output error": @S=0: GOTO 280
240 DELAY 200
250 IF AVAIL(8)=0 THEN PRINT "No data error": GOTO 280
260 INPUT &8, A$
270 IF @S THEN PRINT "Input error": @S=0: ELSE PRINT A$
280 A$="": @S=0 
290 DELAY 1000
300 NEXT
