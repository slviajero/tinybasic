100 REM "USR function demo for number conversion routines"
110 REM "Converts a number to a string"
120 N=-189
130 B=USR (7, N)
140 PRINT "Number of characters processed ";B
150 PRINT "Number string is ";@$
160 @$="32000"
170 PRINT "Number string is ";@$
180 B=USR (6, 0)
190 PRINT "Parsed number "; B
200 END
