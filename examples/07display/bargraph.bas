10 REM "BarGraph from Arduino examples"
20 REM ""
30 A=AZERO
40 DATA 10
50 DATA 2, 3, 4, 5, 6, 7, 8, 9, 10, 11
60 READ N
70 DIM L(N)
80 FOR I=1 TO N : READ L(I) : NEXT
100 REM "setup() put your setup code here, to run once:"
110 FOR I=1 TO N : PINM L(I), 1 : NEXT
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=AREAD(A)
230 V=MAP(S, 0, 1023, 1, N+1)
240 FOR J=1 TO N
250 IF J<V THEN DWRITE L(J), 1 ELSE DWRITE L(J), 0
260 NEXT
270 NEXT

