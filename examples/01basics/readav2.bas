10 REM "ReadAnalogVoltage from Arduino examples - integer version"
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=AREAD(AZERO)
230 V=S*5/1023
240 PRINT V;".";
250 S=S-V*1023/5
260 V=S*50/1023
270 PRINT V
280 NEXT
