10 REM "AnalogInput from Arduino examples"
20 S=AZERO
30 L=13
40 V=0
100 REM "setup() put your setup code here, to run once:"
110 PINM L, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 V=AREAD(S)
230 DWRITE L, 1
240 DELAY V
250 DWRITE L, 0
260 NEXT
