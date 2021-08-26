100 REM "The Blink program is like hello world"
110 REM "Using the Arduino builtin LED"
200 REM "The setup() from line 200-299"
210 PINM 13, 0
300 REM "The loop() from line 300-499"
310 DWRITE 13, 1
320 DELAY 1000
330 DWRITE 13, 0
340 DELAY 1000
350 GET A
360 IF A = 32 then 500 : REM "Space to end"
499 GOTO 300
500 REM "The cleanup from line 500 - 599"
599 END
600 REM "The functions from 600"
610 RETURN