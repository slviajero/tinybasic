10 REM "Arrays from Arduino examples"
20 T=100
30 N=6
40 DIM L(N)
50 FOR J=1 to N: READ L(J): NEXT
60 DATA 2, 7, 4, 6, 5, 3
100 REM "setup() put your setup code here, to run once:"
110 FOR J=1 to N
120 PINM L(J), 1
130 NEXT
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 FOR J=1 TO N
230 DWRITE L(J), 1
240 DELAY T
250 DWRITE L(J), 0
260 NEXT
270 FOR J=N TO 1 STEP -1
280 DWRITE L(J), 1
290 DELAY T
300 DWRITE L(J), 0
310 NEXT
320 NEXT
