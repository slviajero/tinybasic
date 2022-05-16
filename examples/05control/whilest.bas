10 REM "WhileStatementConditional from Arduino examples"
20 REM "P sensor pin, L led, K indicator, B button"
30 P=AZERO
40 L=9
50 K=13
60 B=2
70 N=1023
80 M=0
90 V=0
100 REM "setup() put your setup code here, to run once:"
110 PINM K, 1
120 PINM L, 1
130 PINM B, 0
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 IF DREAD(B)=1 THEN GOSUB 300 : GOTO 220 
230 DWRITE K, 0
240 V=AREAD(P)
250 V=MAP(V, N, M, 0, 255)
260 IF V>255 V=255
270 IF V<0 V=0
280 AWRITE L, V
290 NEXT
300 REM Calibration
310 DWRITE K, 1
320 V=AREAD(P)
330 IF V>M M=V
340 IF V<N N=V
350 RETURN
