10 REM "Memsic2125 from Arduino examples"
20 REM "Two axis accelerometer read"
30 X=2
40 Y=3
50 DEF FNA(Z)=(Z/10-500)*8
100 REM "setup() put your setup code here, to run once:"
110 PINM X, 0
120 PINM Y, 0
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 A0=FNA(PULSE(X, 1, 1000))
230 A1=FNA(PULSE(Y, 1, 1000))
240 PRINT #8, A0, A1
250 DELAY 100
260 NEXT

