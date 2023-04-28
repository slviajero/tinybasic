10 REM "Hardware and system information from USR"
20 REM ""
100 PRINT "*** System Information (constants)"
200 U=USR(0,0)
210 PRINT "System type", U; ": ";
220 IF U=32 THEN PRINT "POSIX"; : GOTO 300
230 IF U=33 THEN PRINT "MSDOS"; : GOTO 300
233 IF U=34 THEN PRINT "MINGW"; : GOTO 300
237 IF U=35 THEN PRINT "RASPPI"; : GOTO 300
240 If U=1 THEN PRINT "AVR"; : GOTO 300
250 IF U=2 THEN PRINT "ESP8266"; : GOTO 300
260 IF U=3 THEN PRINT "ESP32"; : GOTO 300
270 IF U=4 THEN PRINT "RP2040"; : GOTO 300
275 IF U=5 THEN PRINT "SAM"; : GOTO 300
280 IF U=6 THEN PRINT "XMC"; : GOTO 300
285 IF U=7 THEN PRINT "SMT32"; : GOTO 300
290 PRINT "Unknown";
300 IF USR(0,3) THEN PRINT " Floating point "; ELSE PRINT " Integer ";
400 PRINT  USR(0,4); " byte arithmetic"
450 PRINT "Memory size",USR (0,9), "EEPROM size",USR (0,10)
800 PRINT "*** IO system variables"
810 PRINT "Input stream", USR(0,48), "default", USR(0,49)
830 PRINT "Output stream", USR(0,50), "default", USR(0,51)
