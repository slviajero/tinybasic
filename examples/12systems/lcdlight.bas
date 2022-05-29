10 REM "LCDLight - display light intensity"
100 REM "setup() put your setup code here, to run once:"
110 REM "Minimum and maximum intensity and the PIN"
120 B=0 : C=100
130 A=AZERO+1
140 REM "Set the DISPLAY stream &2 as output device"
150 @O=2
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 GET &2, D
230 IF D=10 THEN @O=1: END
240 IF D=49 THEN B=0: C=100
250 V=MAP(AREAD(A), 1023, 0, 0, 99)
260 IF V>B THEN B=V
270 IF V<C THEN C=V
280 GOSUB 500
290 DELAY 1000
300 NEXT
400 END
500 REM "Screen refresh"
510 CLS
520 PRINT #2,"Intensity: "; V
530 PRINT #2 "Min:"; C;" Max:"; B;" "; 
540 RETURN
 
