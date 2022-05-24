10 REM "Strings in BASIC (1)"
20 REM "Dimensioning and comparing"
100 PRINT "Strings can be constants"
110 PRINT "Strings", "can", "be", "constants"
200 PRINT "String variables"
210 A$="... without DIM - 32 chars max"
220 PRINT A$
230 PRINT "Length of this string", LEN(A$)
240 DIM B$(80)
250 B$="... dimensioning to any length, the memory is the limit"
260 PRINT B$
270 PRINT "Length of this string", LEN(B$)
300 IF A$<>B$ THEN PRINT "Strings are different"
310 IF A$=A$ THEN PRINT "Strings are equal"
320 IF A$ THEN PRINT "String is not empty"
330 C$=""
340 IF C$ THEN PRINT "This never shows"
350 PRINT "... because empty strings evaluate to false"
400 END

