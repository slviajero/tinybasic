10 REM "Simple rf24 writer"
100 REM "open the radio pipe for write "
110 OPEN &8, "radi1", 1
200 REM "send a message"
210 FOR I
220 A$="radio "
230 A$(7)=STR(I)
240 PRINT &8, A$
250 DELAY 1000
260 NEXT

