200 REM "Servo variables - AS is the servo angles
210 REM "Calibration AZ is zero point, AF is multiplicator for full 180"
220 AZ=50: AF=200
230 AS=90
9999 END
12000 REM "Servo action code - AS is servo angle"
12010 REM "Calibration AZ zero point, AF angle factor"
12020 PINM 4,1
12030 DWRITE 4,0
12040 P=AZ+AS*AF/180
12050 FOR I=1 TO 40: DELAY 20: PULSE 4,P: NEXT
12060 RETURN
