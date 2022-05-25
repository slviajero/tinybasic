10 REM "Extended math functions"
20 REM ""
100 INPUT "Number= ",A
110 PRINT "The square root of", A, "is", SQR(A)
200 INPUT "Base= ", B
210 INPUT "Exponent= ", E
220 PRINT B, "to the power of", E, "is", POW(B, E)
300 INPUT "Value between 0 and 1000= ", V
310 IF V<0 OR V>1000 THEN PRINT "Range Error" : GOTO 300
320 M=MAP(V, 1000, 0, 200, 100)
330 PRINT "V mapped to the range of 200 and 100 is", M
400 END
