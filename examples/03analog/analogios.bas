10 REM "AnalogInOutSerial from Arduino examples"
20 REM "A is the analog pin, L the LED pin"
30 A=AZERO
40 L=9
50 S=0
60 O=0
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=AREAD(A)
230 O=MAP(S, 0, 1023, 0, 255)
240 AWRITE L, O
250 PRINT "sensor = ", S, "output = ", O  
260 DELAY 2
270 NEXT
