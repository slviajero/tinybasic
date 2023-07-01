10 REM "Structured if then else test"
100 A=0
110 IF A=0 THEN PRINT "A is 0"
120 IF A=1 THEN PRINT "Failed" ELSE PRINT "A is not 1"
130 IF A=0
140 THEN PRINT "A is 0"
150 IF A=1
160 THEN PRINT "Failed"
170 ELSE PRINT "A is not 1"
200 IF A=0 THEN DO PRINT "A is 0": DEND: PRINT "block ended"
210 IF A=1 THEN PRINT "Failed" : ELSE DO PRINT "A is 1": DEND: PRINT "block ended"
300 IF A=0 THEN DO
310 PRINT "A is 0"
320 PRINT "and there is another line in the block"
330 DEND
340 ELSE
350 DO
360 PRINT "Failed"
370 DEND
400 IF A=1 THEN DO
410 PRINT "Failed"
420 PRINT "and there is another line in the block"
430 DEND
440 ELSE
450 DO
460 PRINT "A is not one"
470 PRINT "and there is another line in the block"
480 DEND


