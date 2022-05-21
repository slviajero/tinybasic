10 REM "RowColumnScanning from Arduino examples"
20 REM ""
30 N=8
40 DIM R(N), C(N), P(N*N)
50 FOR I=1 TO N : READ R(I) : NEXT
60 FOR I=1 TO N : READ C(8) : NEXT
70 REM "Row and Column pins"
80 DATA 2, 7, 19, 5, 13, 18, 12, 16
90 DATA 6, 11, 10, 3, 17, 4, 8, 9
100 REM "The cursor position"
110 X=5 : Y=5
200 REM "setup() put your setup code here, to run once:"
210 FOR I=1 TO N 
220 PINM C(I), 1
230 PINM R(I), 1
240 DWRITE C(I), 1
250 NEXT
260 FOR I=1 TO N*N
270 P(I)=1
270 NEXT
300 REM "loop() put your main code here, to run repeatedly:"
310 FOR I
320 GOSUB 500
330 GOSUB 600
350 NEXT
360 END
500 REM "Read sensors subroutine"
510 P(X+N*Y)=1
520 X=7-MAP(AREAD(AZERO), 0, 1023, 0, 7)
530 Y=MAP(AREAD(AZERO+1), 0, 1023, 0, 7)
540 P(X+N*Y)=0
550 RETURN
600 REM "Refresh screen subroutine"
610 FOR R=1 TO N
620 DWRITE R(R), 1
630 FOR C=1 TO N
640 P=P(R+C*8)
650 DWRITE C(C), P
660 IF P=0 THEN DWRITE C(C), 1
670 NEXT
680 DWRITE R(R), 0
690 NEXT
700 RETURN


