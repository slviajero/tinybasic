10 REM "Midi from Arduino examples"
20 REM "Baudrate 31250 not yet implemented"
30 C=144
40 V=69
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 FOR N=30 TO 90
230 PUT C, N, V
240 DELAY 100
250 PUT C, N, 0
260 DELAY 100
270 NEXT
280 NEXT
