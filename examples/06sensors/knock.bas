10 REM "Knock from Arduino examples"
20 REM "L led pin, K piezo pin, T threshold"
30 L=13
40 K=AZERO
50 T=100
60 V=0
70 S=0
100 REM "setup() put your setup code here, to run once:"
110 PINM L, 1
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 V=AREAD(K)
230 IF V<T GOTO 270
240 S=NOT S
250 DWRITE L, S
260 PRINT "Knock!"
270 DELAY 100
280 NEXT
