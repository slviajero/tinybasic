10 PINM AZERO, 0
20 OPEN &9,"stefansbasic/sensor",1
30 A=0 
40 FOR I=1 TO 5
50 A=A+AREAD(AZERO)
60 DELAY 1000
70 NEXT
80 A=A/5
90 A=(A-465)/390
100 A=INT((1-A)*1000)
110 PRINT "wg", A, SENSOR(1,1), SENSOR(1,2), @T$(1,8)
120 PRINT &9, "wg", A, SENSOR(1,1), SENSOR(1,2), @T$(1,8)
130 DELAY 30000
140 GOTO 30
