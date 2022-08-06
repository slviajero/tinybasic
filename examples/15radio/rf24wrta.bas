10 REM "Avanced rf24 writer"
20 REM "Read an analog input on request of a master"
40 REM "The analog pin to read from"
50 AP=AZERO
100 REM "Open the radio pipe for write and read "
110 OPEN &8, "radi1", 1
120 OPEN &8, "radi2", 0
130 IF @S THEN PRINT "Open error": END
140 PINM AP, 0
200 REM "Main loop, wait for a message and then react"
210 FOR I
220 IF AVAIL(8)=0 THEN DELAY 100: GOTO 220
230 INPUT &8, A$
240 IF @S THEN PRINT "Input error": @S=0
250 IF A$="send" THEN PRINT &8, AREAD(AP)
260 IF @S THEN PRINT "Output error"
270 A$="": @S=0 
280 DELAY 1000
290 NEXT
