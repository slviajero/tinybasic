10 REM "Dimmer from Arduino examples"
20 REL "L is the LED pin"
30 L=9
100 REM "setup() put your setup code here, to run once:"
110 PINM L, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 IF AVAIL(1) THEN GET B : AWRITE L, B
230 NEXT
