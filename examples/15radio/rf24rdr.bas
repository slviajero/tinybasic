10 REM "Simple rf24 reader"
100 REM "open the radio pipe for read"
110 OPEN &8, "radi1", 0
200 REM "receive a message"
210 FOR I
220 IF AVAIL(8)=1 THEN INPUT &8, A$ : PRINT A$
250 DELAY 100
260 NEXT
