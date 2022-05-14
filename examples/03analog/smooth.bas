10 REM "Smoothing from Arduino examples"
20 REM "P is the input pin, N is the number of samples"
30 N=10
40 DIM R(N)
50 T=0
60 A=0
70 P=AZERO
100 REM "setup() put your setup code here, to run once:"
110 FOR J=1 TO N 
120 R(J)=0
130 NEXT
140 J=1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 T=T-R(J)
230 R(J)=AREAD(P)
240 T=T+R(J)
250 J=J+1
260 IF J>10 THEN J=1
270 A=T/N
280 PRINT A
290 DELAY 1
300 NEXT
