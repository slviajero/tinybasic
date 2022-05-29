10 REM "Pulse: measure distances, integer version"
20 REM "Ultrasonic sensor P1 is trigger, P2 is echo"
30 P1=23 
40 P2=22
50 REM "The timeout -> integer range"
60 T1=180
100 REM "setup() put your setup code here, to run once:"
110 PINM P1, 1
120 PINM P2, 0
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 REM "Send a pulse, approx 50 microseconds"
230 DWRITE P1,0
240 DWRITE P1,1
250 DWRITE P1,0
260 V=PULSEIN(P2, 1, T1)
270 D=MAP(V, 0, 18000, 0, 29700)
275 PUT &2, 12
280 PRINT &2, "Time:", V
290 PRINT &2, "Dist:", D;
300 DELAY 1000
310 GET &2, A
320 IF A=10 THEN END
330 NEXT
