10 REM "StateChangeDetection from Arduino examples"
20 B=2
30 L=LED
40 S=0
50 T=0
60 C=0
100 REM "setup() put your setup code here, to run once:"
110 PINM B, 0
120 PINM L, 1 
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=DREAD(B)
230 IF S=T THEN 270
240 IF S=0 THEN PRINT "off" : GOTO 270
250 C=C+1
260 PRINT "on"
270 PRINT "number of button pushes: ", C
280 DELAY 50
290 T=S
300 IF C%4=0 THEN DWRITE L, 1 ELSE DWRITE L, 0
310 NEXT
