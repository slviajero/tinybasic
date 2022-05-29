10 REM "Count and record the PIR events"
20 REM "PIR Sensor pin"
30 P=22
100 REM "setup() put your setup code here, to run once:"
110 REM "Initialize the EEPROM - works integer BASIC only"
120 IF @E(1)<0 THEN @E(1)=0
130 C=@E(1)
140 PINM P, 0
150 PUT &2, 12
160 S=0
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 GOSUB 500 
230 IF DREAD(P)=0 AND S=1 THEN C=C+1
240 S=DREAD(P)
250 DELAY 1000
260 REM "write every 60 seconds to protect EEPROM"
270 IF I%60=0 THEN @E(1)=C
280 NEXT
300 END
500 REM "Refresh display and get input"
510 @X=0: @Y=0
520 PRINT &2, #5, "Count: ";C
530 PRINT &2, "Select to Reset";
540 GET &2, R
550 IF R=10 THEN @E(1)=0: C=0
560 RETURN
