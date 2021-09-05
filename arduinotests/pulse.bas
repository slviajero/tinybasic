100 REM "Test of the ultasonic sensor using basics pulsein"
110 REM "Pulsein in BASIC returns results in 10 microseconds"
200 REM "The setup() from line 200-299"
210 P1=7 : REM "Trigger pin"
220 P2=8 : REM "Echo pin"
230 PINM P1, 1
240 PINM P2, 0
250 T1=80 : REM "The timeout is 50 ms"
300 REM "The loop() from line 300-499"
310 DWRITE 7,0
320 DELAY 1
330 DWRITE 7,1
340 DELAY 1
350 DWRITE 7,0
360 I=PULSEIN(P2, 1, T1)
370 PRINT "Time in 1/100 ms : ", I, "Distance in mm : ", (I*33)/20
380 DELAY 1000
390 GET A
400 IF A=32 THEN 500
499 GOTO 300
500 REM "The cleanup from line 500 - 599"
599 END
600 REM "The functions from 600"
610 RETURN