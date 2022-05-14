10 REM "tonePitchFollower from Arduino examples"
100 REM "setup() put your setup code here, to run once:"
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 S=AREAD(AZERO)
230 T=MAP(S, 400, 1000, 120, 1500)
240 PLAY 9, T, 10
250 DELAY 1
260 NEXT
