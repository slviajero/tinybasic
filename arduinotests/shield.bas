
100 REM "Test for the shield module code"
110 REM "Display text on the LCD and read the buttons"
120 REM "Compile BASIC with LCDSHIELD"
130 I=0
140 SET 2,1:REM "Activate LCD output"
150 PUT 12 :REM "and clear screen"
160 I=I+1
170 A=AREAD (14):REM "Analog pin A0 is pin 14"
180 PRINT "B: ";A
190 PRINT "Select: end";
200 IF A>600 AND A<800 THEN 230
210 DELAY 1000
220 GOTO 150
230 PUT 12
240 PRINT "End";
250 SET 2,0:REM "Output to serial again"
260 END 