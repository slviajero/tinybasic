10 REM "IfStatementConditional from Arduino examples"
20 REM "A analog pin, led pin, threshold"
30 A=AZERO
40 L=13
50 T=400
100 REM "setup() put your setup code here, to run once:"
110 PINM L, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 V=AREAD(A)
230 IF V>T THEN DWRITE L, 1 ELSE DWRITE L, 0
240 PRINT V
250 DELAY 1
260 NEXT
