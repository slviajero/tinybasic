10 REM "BlinkWithoutDealy from Arduino examples"
20 L=0
30 P=0
40 T=1000
100 REM "setup() put your setup code here, to run once:"
110 PINM LED, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 C=MILLIS(T)
230 IF C>P THEN P=C : L=(L+1)%2
240 DWRITE LED, L
250 NEXT
