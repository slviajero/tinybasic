1000 REM "Look into three cubes"
1010 DEF FNTC(I)=I^3+(I+1)^3+(I+2)^3
2000 FOR N=1 TO 150
2010 F=FNTC(N)
2020 D=INT(SQR(F))
2030 D2=D^2
2040 PRINT N,F,D2,F-D2
2050 NEXT
