10 REM "Calculates the Fibonacci numbers"
20 REM "And the Golden Ratio on an Integer BASIC"
200 REM "Main program"
210 N=19
220 DIM A(N)
230 A(1)=1
240 A(2)=1
250 FOR I=3 TO N
260 IF A(I)>16000 THEN BREAK
270 A(I)=A(I-1)+A(I-2)
280 Q=A(I): D=A(I-1)
290 PRINT #4;I,Q,D;
300 GOSUB 500
310 NEXT
320 END
500 REM "Print a decimal ratio of a fraction"
520 Z=INT(Q/D)
530 PRINT " ";Z;".";
540 FOR J=1 TO 10
550 Z=Q%D
560 Q=Z*10
570 Z=INT(Q/D)
580 PRINT Z;
590 NEXT
600 PRINT " "
610 RETURN
