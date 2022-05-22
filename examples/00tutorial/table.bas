10 REM "Print a table of values"
100 INPUT "Start= ", S
110 INPUT "End= ", E
120 PRINT #8, "X", "X*X", "X*X*X"
130 FOR I=S TO E
140 PRINT #8, I, I*I, I*I*I
150 NEXT
