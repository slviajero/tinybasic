10 REM "debounce from Arduino examples"
11 REM "Using millis with a divisor here"
20 P=2
30 L=13
40 L0=1 : B0=0
50 T=0 : D=50
100 REM "setup() put your setup code here, to run once:"
110 PINM P, 0
120 PINM L, 1
130 DWRITE L, L0
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 R=DREAD(P)
230 IF R<>B0 THEN L0=MILLIS(D)
240 IF MILLIS(D)-L0=0 GOTO 270
250 IF R<>B0 THEN B0=P
260 IF B0=1 THEN L0=NOT L0
270 DWRITE L, L0
280 NEXT
