10 REM "DUMP demo program"
20 REM ""
100 PRINT "This is how the BASIC memory and EEPROM looks like"
110 T=HIMEM-SIZE
120 PRINT "The end of the program is at", T
130 DUMP 0, T
140 END
