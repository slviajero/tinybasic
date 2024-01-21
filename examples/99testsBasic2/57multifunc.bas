10 REM "Function multiline definitions"
20 REM ""
100 DEF FNS(X)=X*X
110 DEF FNT(X)
120 RETURN X*X*X
130 FEND
140 DEF FNQ(X)
150 RETURN FNS(X)*X
160 FEND
170 DEF FNR(X)
180 PRINT FNQ(X)
190 FEND
200 READ N
210 PRINT "The square of",N," is: "; FNS(N)
220 PRINT "X is",X
230 PRINT "The cube of",N," is: "; FNT(N)
240 PRINT "This can be done like this as well: "; FNQ(N)
250 PRINT "Or even like this: ";: FNR(N)
300 REM "Odd stuff"
310 PRINT "Prints ";: A=FNR(): PRINT "Returns: ",A 
320 FNQ(9): FNS(11): FNT(13)
500 DATA 10
