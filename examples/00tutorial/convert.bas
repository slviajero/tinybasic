10 REM "Convert strings to numbers and vice versa"
20 REM ""
100 INPUT "Input a number: ", A$
110 A=VAL(A$)
120 PRINT "Two times the number is ", 2*A
200 INPUT "Input a number: ", A
210 A$=STR(A)
220 PRINT "The number is '";A$;"' "
300 END
