10 REM "Solves the Euler 9 problem "
20 REM "See https://projecteuler.net/problem=9 for details"
30 REM "with a number theoretical approach"
100 PRINT "Enter the circumference K of a triangle"
110 PRINT "The program finds a pythagorean triangle"
120 PRINT "a^2 + b^2 = c^2"
130 K=1008
140 IF K=0 THEN END 
150 IF K%2=0 THEN U=K/2: GOTO 200 
160 PRINT "K is odd, no solution": GOTO 130 
200 REM "Main Program"
210 F=0 
220 X=SQR(U/2)
230 FOR I=1 TO X 
240 IF U%I<>0 THEN CONT 
250 FOR J=1 TO I 
260 D=I+J 
270 IF U%D<>0 THEN CONT 
280 IF D%2=0 THEN CONT 
290 R=I: Q=J 
300 GOSUB 800 
310 IF R<>1 THEN CONT 
320 A=I*I-J*J 
330 B=2*I*J 
340 C=I*I+J*J 
350 PRINT "Primitive triple",A ,B, C 
360 PRINT "Circumference=",A+B+C 
370 S=K/(A+B+C)
380 PRINT "Scale with",S 
390 PRINT "Solution ",A*S, B*S, C*S 
400 F=1 
420 NEXT 
430 NEXT 
440 REM "All solutions found"
450 IF F=0 THEN PRINT "No solution"
460 END
800 REM "Find GCD"
810 IF R=Q THEN RETURN 
820 IF R>Q THEN R=R-Q 
830 IF Q>R THEN Q=Q-R 
840 GOTO 810 
