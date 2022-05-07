10 REM "ReadAnalogVoltage from Arduino examples - float version"
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=AREAD(AZERO)
230 V=S*5/1023
240 PRINT V
250 NEXT
