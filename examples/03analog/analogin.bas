10 REM "AnalogInput from Arduino examples"
20 REM "S is the analog sensor input, L the LED pin"
30 S=AZERO
40 L=13
50 V=0
100 REM "setup() put your setup code here, to run once:"
110 PINM L, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 V=AREAD(S)
230 DWRITE L, 1
240 DELAY V
250 DWRITE L, 0
260 NEXT
