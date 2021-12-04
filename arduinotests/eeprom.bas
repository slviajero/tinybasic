10 PRINT "Previous measurement ",@E(1)
20 PRINT "Starting new measurement"
30 L=0
40 FOR I=1 TO 60
50 L=L+AREAD (AZERO )/10
60 DELAY 1000
70 NEXT 
80 @E(1)=L/60
90 PRINT "Average = ",@E(1)