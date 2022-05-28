10 REM "ON GOSUB/GOTO demo program"
20 REM ""
100 PRINT "For the solution to all problems"
110 INPUT "Type a number between 1-3?", I
200 ON I GOTO 250, 260, 270
210 REM "default"
220 PRINT "Wrong number" : GOTO 110
230 REM "The cases"
250 PRINT "Meditations"; : GOTO 300
260 PRINT "Alcohol"; : GOTO 300
270 PRINT "Chocolate"; : GOTO 300
300 PRINT " is the solution"
400 END
