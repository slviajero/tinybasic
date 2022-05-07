10 REM "DigitalReadSerial from Arduino examples"
20 P=2
100 REM "setup() put your setup code here, to run once:"
110 PINM P, 0
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=DREAD(P)
230 PRINT S
240 DELAY 1
220 S=DREAD(P)
230 PRINT S
240 DELAY 1
250 NEXT
