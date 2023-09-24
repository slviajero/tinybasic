10 REM "ON GOSUB/GOTO demo program"
20 REM ""
100 PRINT "For the solution to all problems:"
110 READ I 
120 IF I=-1 END
130 ON I GOSUB 850, 860, 870
140 GOTO 110
850 PRINT "Meditation"; : GOTO 900
860 PRINT "Alcohol"; : GOTO 900
870 PRINT "Chocolate"; : GOTO 900
900 PRINT " is the solution"
910 RETURN
1000 DATA 1,2,3,6,1000,-10,0,-9999,-1
