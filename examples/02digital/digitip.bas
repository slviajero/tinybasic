10 REM "DigitalInputPullup from Arduino examples"
100 REM "setup() put your setup code here, to run once:"
110 PINM 2, 2
120 PINM LED, 1 
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=DREAD(2)
230 PRINT S
240 IF S=1 THEN DWRITE LED, 0 ELSE DWRITE LED, 1
250 NEXT
