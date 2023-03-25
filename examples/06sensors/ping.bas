10 REM "Ping from Arduino examples"
20 REM "BASIC Pulsein uses 10 microseconds units!"
30 P=7
40 DEF FNC(X)=X/29*10/2
50 DEF FNI(X)=X/74*10/2
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 PINM P, 1
230 REM "Write the pulse as fast as we can"
240 DWRITE P, 0: DWRITE P, 1 : DWRITE P, 0
250 PINM P, 0
260 REM "Maximum pulse timeout 100 ms"
270 D=PULSE(P, 1, 100)
280 PRINT FNI(D), "in, ";
290 PRINT FNC(D), "cm"
300 DELAY 100
310 NEXT 
 
