10 REM "Identify devices on the I2C bus"
20 REM ""
100 REM "the setup()"
110 FOR I=1 TO 127
120 @S=0
130 REM "Try to open a device and send a byte"
140 OPEN &7, I
150 PUT &7, 0
160 IF @S=0 THEN PRINT "device found on ",I;": ";:170 GOSUB 500 
190 NEXT
200 END
500 REM "Search the device name"
510 IF I>=80 AND I<=87 THEN PRINT "EEPROM"
520 IF I=104 THEN PRINT "Real Time Clock"
530 RETURN

