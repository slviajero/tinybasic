10 REM "Master code for analogr2"
20 REM "Display the analog value"
100 REM "setup()"
110 OPEN &7, 8
200 REM "loop()"
210 FOR I
220 @S=0
230 INPUT &7, A
240 IF @S THEN PRINT "Data error" ELSE PRINT A
250 DELAY 500
260 NEXT I
