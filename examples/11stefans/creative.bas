10 REM "The Creative Computing Benchmark 1977"
15 T=MILLIS (1)
20 FOR N=1 TO 100:A=N
30 FOR I=1 TO 10
40 A=SQR (A):R=R+RND (1)
50 NEXT I
60 FOR I=1 TO 10
70 A=A*A:R=R+RND (1)
80 NEXT I
90 S=S+A:NEXT N
100 PRINT "Accuracy:", ABS (1010-S/5)
110 PRINT "Random:", ABS (1000-R)
120 T=MILLIS (1)-T
130 PRINT "Expired time:",T, "ms"
140 PRINT "An Apple 2 needed 120000 ms for this"

