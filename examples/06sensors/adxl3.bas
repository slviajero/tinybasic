10 REM "ADXL3xx from Arduino examples"
20 REM "ground pin=18, power=19, x=A3, ..." 
30 G=18
40 P=19
50 X=AZERO+3
60 Y=AZERO+2
70 Z=AZERO+1
100 REM "setup() put your setup code here, to run once:"
110 PINM G, 1
120 PINM P, 1
130 DWRITE G, 0
140 DWRITE P, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 PRINT #8, AREAD(X), AREAD(Y), AREAD(Z)
230 DELAY 100
250 NEXT

