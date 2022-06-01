10 REM "Direct access to EEPROMS with I2C"
20 REM "EEPROMS are on Wire addresses 80-87"
30 E=87
40 DIM A$(32), B$(32)
100 PRINT "Open the wire connection as master"
110 OPEN &7, E
200 PRINT "Read the EEPROM byte by byte"
210 GOSUB 800
300 PRINT "Clear 16 bytes at address (0,0)"
310 B$(1)=0 : B$(2)=0
320 FOR I=1 TO 16: B$(I+2)=0: NEXT 
330 PRINT &7, B$
340 REM "Give the EEPROM time to digest"
350 DELAY 5
400 PRINT "Read the data"
410 GOSUB 800
500 PRINT "Write a set of 16 bytes to address (0,0)"
510 B$(1)=0 : B$(2)=0
520 FOR I=1 TO 16: B$(I+2)=17-I: NEXT 
530 PRINT &7, B$
540 DELAY 5
600 REM "Read the data"
610 GOSUB 800
620 END
800 REM "Print the data"
810 PRINT "EEPROM data at (0,0)"
820 @S=0
830 PUT &7, 0, 0
840 IF @S<>0 PRINT "EEPROM Error" : END
850 INPUT &7, A$
855 PRINT "Read "; LEN(A$); " bytes"
860 FOR I=1 TO 32 
870 PRINT I, (A$(I))
880 NEXT
890 RETURN
