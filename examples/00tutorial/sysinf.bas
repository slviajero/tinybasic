10 REM "Language set information from USR"
20 REM ""
100 PRINT "*** Language set information"
200 U=USR(0,1)
210 PRINT "Language identifier:", U 
220 PRINT "Palo Alto Basic with Stefan's extensions" 
230 IF U AND 1 THEN PRINT "+ Apple Integer BASIC and strings/arrays"
250 IF U AND 2 THEN PRINT "+ Arduino I/O functions"
260 IF U AND 4 THEN PRINT "+ File I/O"
270 IF U AND 8 THEN PRINT "+ Dartmouth BASIC extensions"
280 IF U AND 16 THEN PRINT "+ Graphics language features"
290 IF U AND 32 THEN PRINT "+ Darkarts language features"
300 IF U AND 64 THEN PRINT "+ IoT language extensions" 
310 IF USR(0,3) THEN PRINT "+ Floating point" ELSE PRINT "Integer arithemtic"

