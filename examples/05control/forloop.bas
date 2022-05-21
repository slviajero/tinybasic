10 REM "ForLoopIterations from Arduino examples"
20 T=100
100 REM "setup() put your setup code here, to run once:"
110 FOR P=2 TO 7 
120 PINM P, 1
130 NEXT
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 FOR P=2 TO 7
230 DWRITE P, 1
240 DELAY 100
250 DWRITE P, 0
260 NEXT
270 FOR P=7 TO 2 STEP -1
280 DWRITE P, 1
290 DELAY 100
300 DWRITE P, 0
310 NEXT
320 NEXT
