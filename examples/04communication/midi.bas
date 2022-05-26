10 REM "Midi from Arduino examples"
20 REM "Second serial with midi baudrate 31250"
30 REM "&4 is the second serial port"
40 SET 8, 31250
50 C=144
60 V=69
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 FOR N=30 TO 90
230 PUT &4, C, N, V
240 DELAY 100
250 PUT &4, C, N, 0
260 DELAY 100
270 NEXT
280 NEXT
