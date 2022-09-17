10 REM "Identify devices on the I2C bus"
20 REM ""
100 REM "the setup()"
110 FOR I=1 TO 127
120 @S=0
130 REM "Try to open a device and send a byte"
140 OPEN &7, I
150 PUT &7, 0
160 IF @S=0 THEN PRINT "device found on", #3, I;": ";:GOSUB 500
190 NEXT
200 PRINT 
210 END
500 REM "Search the device name"
505 IF I=56 THEN PRINT "AH10" : RETURN
510 IF I=60 THEN PRINT "Oled" : RETURN
520 IF I>=80 AND I<=87 THEN PRINT "EEPROM" : RETURN 
530 IF I=104 THEN PRINT "Real Time Clock" : RETURN
540 IF I=118 OR I=119 THEN PRINT "BMP/E280" : RETURN
590 PRINT "Unknown"
600 RETURN

