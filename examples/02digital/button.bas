10 REM "Button from Arduino examples"
20 B=2
30 L=LED
40 S=0
100 REM "setup() put your setup code here, to run once:"
110 PINM L, 1
120 PINM B, 0 
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=DREAD(B)
230 IF S=1 THEN DWRITE L, 1 ELSE DWRITE L, 0
250 NEXT
