10 REM "AnalogInOutSerial from Arduino examples"
20 A0=AZERO
30 A1=9
40 S=0
50 O=0
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=AREAD(A0)
230 O=MAP(S, 0, 1023, 0, 255)
240 AWRITE A1, O
250 PRINT "sensor = ", S, "output = ", O  
260 DELAY 2
270 NEXT
