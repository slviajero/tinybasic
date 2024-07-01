10 REM "This program created a buildin.h file to be compiled into BASIC"
20 DIM NA$(32,20), A$(128), B$(128)
30 N=0
40 P$="*"
50 OPEN "buildin.h", 1
1000 REM "Build the file list"
1010 FOR N=1
1020 READ N$
1030 IF (N$)=0 THEN BREAK
1040 PRINT "File: ", N$
1050 NA$()(N)=N$
1060 NEXT
1070 N=N-1
1080 PRINT "Processing",N,"files"
1100 REM "Create the structures"
1110 FOR I=1 TO N
1120 @S=0: N$=NA$()(I)
1130 OPEN N$
1140 IF @S<>0 THEN PRINT "File error in", N$: END
1200 REM "Process one file"
1210 PRINT &16,"const char buildin_pgm";I;"[] PROGMEM = {" 
1220 FOR J=1
1230 INPUT &16, A$
1240 IF @S=-1 THEN BREAK
1250 IF A$="" THEN CONT
1260 B$=""
1300 FOR K=1 TO LEN(A$)
1310 IF A$(K,K)='"' THEN B$=B$+'\"' ELSE B$=B$+A$(K,K)
1320 NEXT
1330 PRINT &16, '"';B$;'\n"'
1400 NEXT
1410 PRINT &16, '"\f"'
1420 PRINT &16,"};"
1430 PRINT &16, "const char buildin_pgm";I;"_name[] PROGMEM = ";'"';P$;N$;'";'
1600 NEXT
1700 PRINT &16, "const char* const buildin_programs[] PROGMEM = {"
1710 FOR I=1 TO N
1720 PRINT &16, "buildin_pgm";I;","
1730 NEXT I
1740 PRINT &16, "0"
1750 PRINT &16, "};"
1800 PRINT &16, "const char* const buildin_program_names[] PROGMEM = {"
1810 FOR I=1 TO N
1820 PRINT &16, "buildin_pgm";I;"_name,"
1830 NEXT I
1840 PRINT &16, "0"
1850 PRINT &16, "};"
10000 REM "The programs to be included into the buildin.h
10010 DATA "cpinew2.bas" 
10020 DATA "euler9.bas", "fibo.bas", "primes.bas", "stir.bas"
10030 DATA "tictac.bas"
10100 DATA 0

