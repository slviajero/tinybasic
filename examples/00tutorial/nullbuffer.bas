10 REM "Demo of the BASIC nullbuffer feature"
20 REM "The variable @$ is the input buffer and used as I/O channel"
100 PRINT "ibuffer is", @$
110 @=""
120 PRINT "ibuffer is now", @$
130 A=10: B=20
140 PRINT &0,A,",",B
150 PRINT "ibuffer is now", @$
160 INPUT &0, C, D
170 PRINT "Read ", C, D, "from ibuffer"
200 END 
