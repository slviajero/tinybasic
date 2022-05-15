10 REM "PhysicalPixel from Arduino examples"
20 REM "Uses the buildin LED, needs HASIOT"
30 L=LED
100 REM "setup() put your setup code here, to run once:"
110 PINM L, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 IF AVAIL(1)=0 GOTO 250
230 GET C
240 IF C="H" DWRITE L, 1
250 IF C="L" DWRITE L, 0
250 NEXT
