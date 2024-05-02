10 REM 'Function to calculate the greatest common divisor (GCD)'
20 REM "Demo for a multipline function"
100 DEF FNG()
110 WHILE B <> 0
120 T = B
130 B = A % B
140 A = T
150 WEND
160 RETURN A
170 FEND
200 WHILE -1
210 READ A,B
220 IF @S<>0 THEN BREAK
230 PRINT "The greatest common denominator of",A,"and",B,"is: ";FNG() 
240 WEND 
250 END
300 DATA 18,27,54,63,128,371,34,85
