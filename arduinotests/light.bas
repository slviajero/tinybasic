100 REM "Test Program - measure intensity of light"
110 REM "Displays on an 4 row LCD"
120 REM "Assumes a LM393 4 pin light sensor on A0"
200 REM "The setup() from line 200-299"
210 B=0  : REM "Maximum Intensity"
220 C=1023 : REM "Minimum Intensity"
230 A0=54 : REM "AT Mega A0 pin, set to 14 on an Uno"
240 SET 2,1
300 REM "The loop() from line 300-499"
310 PUT 12
320 GET D
330 IF D=32 THEN 500
340 A=1024-AREAD(A0) : REM "Higher values -> more light"
350 IF A>B THEN B=A
360 IF A<C THEN C=A
370 PRINT "Intensity: ", A
380 PRINT "Minimum:   ", C
390 PRINT "Maximum:   ", B;
400 DELAY 1000
499 GOTO 300
500 REM "The cleanup from line 500 - 599"
510 SET 2,0
599 END
600 REM "The functions from 600"
610 RETURN
