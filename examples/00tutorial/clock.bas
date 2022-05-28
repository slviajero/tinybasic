10 REM "Show the time and date of the real time clock"
20 REM ""
100 REM "The day is never 0 if the clock runs"
110 IF @T(3)=0 THEN PRINT "No clock active" : END
200 PRINT @T(2);":";@T(1);":";@T(0)
210 PRINT @T(3);"/";@T(4);"/";@T(5)
230 PRINT "Temp:",@T(7)/100
240 PRINT "Clock string value"
250 PRINT @T$

