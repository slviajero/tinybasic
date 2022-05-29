10 PRINT "Previous measurement ",@E(1)
20 PRINT "Starting new measurement"
30 L=0
40 FOR I=1 TO 10 
50 L=L+AREAD(AZERO)
60 DELAY 2000
70 NEXT 
80 @E(1)=L/10
90 PRINT "Average = ",@E(1)
