10 REM "Extended math functions"
20 REM ""
100 READ A
110 PRINT "The square root of", A, "is", SQR(A)
200 READ B
210 READ E
220 PRINT B, "to the power of", E, "is", POW(B, E)
300 READ V
310 IF V<0 OR V>1000 THEN PRINT "Range Error" : GOTO 300
320 M=MAP(V, 1000, 0, 200, 100)
330 PRINT V, "mapped to the range of 200 and 100 is", M
400 END
500 DATA 2, 3, 5, 567
