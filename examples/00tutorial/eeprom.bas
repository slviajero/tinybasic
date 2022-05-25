10 REM "EEPROM demo"
20 REM "@E, and the array @E()"
100 PRINT "Size of EEPROM", @E
110 IF @E>0 GOTO 200
120 PRINT "No EEPROM found - test skipped"
130 STOP
200 PRINT "Storing numbers to eeprom array, size =", @E
210 FOR I=1 TO 10
220 @E(I)=I*I
230 NEXT I
240 PRINT "Recalling numbers from eeprom"
250 FOR I=1 TO 10
260 PRINT I, @E(I)
270 NEXT
300 PRINT "Restart BASIC later and check @E(10)"

