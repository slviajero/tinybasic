10 REM "Calculate Factorial and Stirlings function"
20 REM ""
100 REM "Factorial"
110 L=20 
120 DIM F(L)
130 F(1)=1
140 FOR I=2 TO L
150 F(I)=F(I-1)*I 
160 NEXT 
200 REM "Stirling"
210 P1=4*ATAN(1)
220 DEF FNS(N)=SQR(2*P1*N)*EXP(N*(LOG(N)-1))
300 REM "Print table"
305 PRINT "N";: TAB(4)    
310 PRINT "FACT(N)";: TAB(6)
315 PRINT "STIR(N)";: TAB(6)
320 PRINT "Accuracy (%)"
330 FOR I=1 TO L
340 PRINT #4, I, #12, F(I), FNS(I), (F(I)-FNS(I))/F(I)*100
350 NEXT
400 END
