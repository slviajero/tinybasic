
100 REM "Test for the shield module code"
110 REM "Display text on the LCD and read the buttons"
120 REM "Compile BASIC with LCDSHIELD"
200 REM "The setup() from line 200-299"
210 I=0
220 A0=14 : REM "Analog pin A0 is pin 14"
220 SET 2,1 : REM "Activate LCD output"
300 REM "The loop() from line 300-499"
310 PUT 12 :REM "clear the screen"
320 I=I+1
330 A=AREAD (14)
340 PRINT "B: ";A
350 PRINT "Select: end";
360 IF A>600 AND A<800 THEN 500
370 DELAY 1000
499 GOTO 300
500 REM "The cleanup from line 500 - 599"
510 PUT 12
520 PRINT "End";
530 SET 2,0 : REM "Output to serial again"
599 END
600 REM "The functions from 600"
610 RETURN

