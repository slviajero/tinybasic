10 REM "Scan the I2C bus for devices"
20 REM ""
100 REM "the setup()"
110 FOR I=1 TO 127
120 @S=0
130 REM "Try to open a device and send a byte"
140 OPEN &7, I
150 PUT &7, 0
160 IF @S=0 THEN PRINT "device found on ",I
170 NEXT
200 END
