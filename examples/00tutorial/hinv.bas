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
300 IF USR(0,3) THEN PRINT " Floating point" ELSE PRINT " Integer"
400 PRINT "Number type size", USR(0,4)
410 PRINT "Maximum (accurate) number",USR (0,5)
420 PRINT "Adress type size",USR (0,6)
430 PRINT "Maximum address",USR (0,7)
440 PRINT "String index size",USR (0,8)
450 PRINT "Memory size",USR (0,9)
460 PRINT "EEPROM size",USR (0,10)
470 PRINT "GOSUB depth",USR (0,11)
480 PRINT "FOR depth",USR (0,12)
490 PRINT "Stacksize",USR (0,13)
500 PRINT "Input buffer length",USR (0,14)
520 PRINT "Keyword buffer length",USR (0,15)
530 PRINT "Default array length", USR(0,16)
540 PRINT "Default string length", USR(0,17)
600 PRINT "*** System variables"
610 PRINT "End of program (top)", USR(0,24)
620 PRINT "Current program location (here)", USR(0,25)
630 PRINT "End of free memory (himem)", USR(0,26)
640 PRINT "Number of stored variables", USR(0,27)
650 PRINT "Free heap (inaccurate)", USR(0,28)
660 PRINT "GOSUB stackpointer", USR(0,29)
670 PRINT "FOR stackpointer", USR(0,30)
680 PRINT "FOR Nesting counter", USR(0,31)
690 PRINT "Stackpointer", USR(0,32)
700 PRINT "Data pointer", USR(0,33)
800 PRINT "*** IO system variables"
810 PRINT "Input stream", USR(0,48)
820 PRINT "Default input stream", USR(0,49)
830 PRINT "Output stream", USR(0,50)
840 PRINT "Default output stream", USR(0,51)
