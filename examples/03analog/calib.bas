10 REM "Calibration from Arduino examples"
20 REM "S is Sensor pin, L is the LED pin"
30 REM "M is the minimum, N is the maximum"
40 S=AZERO
50 L=9
60 V=0 
70 M=1023
80 N=0
90 T=MILLIS(1000)
100 REM "setup() put your setup code here, to run once:"
110 PINM LED, 1
120 DWRITE LED, 1
130 IF MILLIS(1000)-T>5 GOTO 190
140 V=AREAD(S)
150 IF V>M THEN N=V
160 IF V<M THEN M=V
170 GOTO 130
190 DWRITE LED, 0
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 V=AREAD(S)
230 IF V>N THEN V=N
240 IF V<M THEN V=M
250 V=MAP(V, M, N, 0, 255)
260 AWRITE L, V
270 NEXT
