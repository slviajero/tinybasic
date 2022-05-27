10 REM "Demo of the PUT and GET command"
20 REM ""
100 PUT "H", 101, "l", 108, "o", 32, "W", 108, "r", 108, "d", 10
110 PRINT "Press any key to continue"
120 REM "GET is non blocking in Arduinos and blocking in POSIX"
130 GET C : IF C=0 THEN GOTO 120
140 PRINT "Key '"; 
150 PUT C
160 PRINT "' pressed"
170 END
