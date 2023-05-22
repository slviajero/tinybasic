10 REM "Elementary find demo"
20 REM ""
30 A0=10
40 DIM B(8)
50 DIM A$(20)
100 PRINT "Find variable on heap"
110 PRINT "Address of A0 is", FIND(A0)
120 PRINT "Find array on heap"
130 PRINT "Address of B() is", FIND(B())
140 PRINT "Find string on heap"
150 PRINT "Address of A$ is", FIND(A$)
200 PRINT "Variables A-Z are not on heap"
210 PRINT "Try to find A gives", FIND (A)
300 PRINT "String representation in memory"
310 B$="ABCDEFGH"
320 B=FIND(B$)
330 FOR I=B TO B+7+2
340 PRINT PEEK(I);" ";
350 NEXT
360 PRINT
400 END
