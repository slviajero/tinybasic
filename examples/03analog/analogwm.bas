10 REM "AnalogWriteMega from Arduino examples"
20 REM "L is the lowest LED pin, H the highest"
30 L=2
40 H=13
100 REM "setup() put your setup code here, to run once:"
120 FOR T=L TO H
130 PINM T, 1
140 NEXT
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 FOR T=L TO H
230 FOR B=0 TO 254
240 AWRITE T, B
250 DELAY 2
260 NEXT
270 FOR B=255 TO 0 STEP -1
280 AWRITE T, B
290 DELAY 2
300 NEXT
310 DELAY 100
320 NEXT
330 NEXT
