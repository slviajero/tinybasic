10 REM "Convert strings to numbers and vice versa"
20 REM ""
100 INPUT "Input a number: ", A$
110 REM "Clear the error flag"
120 @S=0
130 A=VAL(A$)
140 IF @S=0 THEN PRINT "Conversion successful, number is ", A ELSE PRINT "Number error"
200 INPUT "Input a number: ", A
210 A$=STR(A)
220 PRINT "The number is '";A$;"' "
300 END
