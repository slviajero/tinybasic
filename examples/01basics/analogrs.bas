10 REM "AnalogReadSerial from Arduino examples"
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=AREAD(AZERO)
230 PRINT S
240 DELAY 1
250 NEXT
