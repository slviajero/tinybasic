10 REM "VirtualColorMixer from Arduino examples"
20 R=AZERO
30 G=AZERO+1
40 B=AZERO+2
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 PRINT AREAD(R), ",", AREAD(G), ",", AREAD(B)
250 NEXT
