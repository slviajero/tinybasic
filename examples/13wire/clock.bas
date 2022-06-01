10 REM "Directly access a real time clock"
20 REM "Wire address of DS3231 is 104"
30 C=104
40 DIM A$(32)
50 REM "Function to glue together BCD nibbles"
60 DEF FNN(X)=(X AND 15)+10*INT(X/16)
100 REM "Read out the clock"
110 OPEN &7, C
120 @S=0
130 PUT &7, 0
140 INPUT &7, A$
150 IF @S<>0 THEN PRINT "Clock Error": END
200 REM "Interpret the data"
210 PRINT "Raw data"
220 FOR I=1 TO 32 : PRINT I, (A$(I)) : NEXT
230 PRINT "Information"
240 S=FNN(A$(1))
250 PRINT "Seconds", S
260 M=FNN(A$(2))
270 PRINT "Minutes", M 
280 T=INT(A$(3)/64)
290 IF T THEN GOSUB 800 ELSE GOSUB 900
790 END
800 PRINT "12 hours clock"
810 T=(A$(3) AND 31): H=FNN(T)
820 PRINT "Hours", H;
840 T=((A$(3)/32) AND 1)
850 IF T THEN PRINT "pm" ELSE PRINT "am"
890 RETURN
900 PRINT "24 hours clock"
910 T=(A$(3) AND 63): H=FNN(T)
920 PRINT "Hours", H
990 RETURN
