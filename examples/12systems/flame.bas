10 REM "Flame sensor and buzzer demo"
20 REM "Buzzer pin 2 and flame sensor pin A1"
30 B=2 
40 F=AZERO+1
50 REM "The flame sensor threshold"
60 T=250
100 REM "setup() put your setup code here, to run once:"
110 PINM B, 1
120 PINM F, 0
130 DWRITE B, 0
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 V=AREAD(F)
230 IF V>T THEN DELAY 2000 : CONT
240 DWRITE B, 1
250 DELAY 250 
260 DWRITE B, 0
270 DELAY 500
280 NEXT

