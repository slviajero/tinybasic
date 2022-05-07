10 REM "Fade from Arduino examples"
20 L=9
30 B=0
40 F=5
100 REM "setup() put your setup code here, to run once:"
110 PINM L, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 AWRITE L, B
230 B=B+F
240 IF B<=0 OR B>=255 THEN F=-F
250 DELAY 30
260 NEXT
