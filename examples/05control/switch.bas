10 REM "Switch from Arduino examples"
20 REM "N is sensor minimum, M maximum"
30 N=0
40 M=600
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 V=AREAD(AZERO)
230 R=MAP(V, N, M, 1, 4)
240 ON R GOSUB 300, 320, 340, 360
250 DELAY 1
260 NEXT
300 PRINT "DARK"
310 RETURN
320 PRINT "DIM"
330 RETURN
340 PRINT "MEDIUM"
350 RETURN
360 PRINT "BRIGHT"
370 RETURN
