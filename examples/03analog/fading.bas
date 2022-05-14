10 REM "Fading from Arduino examples"
20 REM "L is the LED pin"
30 L=9
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 FOR V=0 TO 255 STEP 5
230 AWRITE L, V
240 DELAY 30
250 NEXT
260 FOR V=255 TO 0 STEP -5
270 AWRITE L, V
280 DELAY 30
290 NEXT
300 NEXT
