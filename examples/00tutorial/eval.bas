10 REM "Demo for the EVAL command"
20 REM ""
30 DIM A$(80)
100 INPUT "Enter a function (y=x): ",A$
110 EVAL 1010,A$
200 PRINT "X    Y"
210 FOR X=-4 TO 4.001 STEP 0.5
220 GOSUB 1000: PRINT #4, X, Y
230 NEXT
300 END
1000 REM "The function at the end"
1010 Y=X
1020 RETURN
