100 REM "Tests the millis functions and calculates"
110 REM "runtimes of certain operations"
120 REM "Write result to serial line"
130 a$="milliseconds"
140 b$="microseconds"
200 if usr(0,0)=32 then n=1000 else n=10
210 m=1000
220 print "Running ";n;"*";m;" operations:"
300 s=millis(1)
310 for i=1 to n
320 for j=1 to m
330 next j
340 next i
350 e=millis(1)
360 print "Empty loops",e-s, a$
370 l=e-s
400 s=millis(1)
420 for i=1 to n
430 for j=1 to m
440 rem 
450 next j
460 next i
470 e=millis(1)
480 print "REM statements", e-s, a$
490 b=e-s
500 s=millis(1)
510 for i=1 to n
520 for j=1 to m
530 a=67
540 next j
550 next i
560 e=millis(1)
570 print "Assignments", e-s, a$
580 c=e-s
700 s=millis(1)
710 for i=1 to n
720 for j=1 to m
730 a=j/i
740 next j
750 next i
760 e=millis(1)
770 print "Divisions", e-s, a$
780 d=e-s
800 dim q(n)
810 s=millis(1)
820 for i=1 to n
830 for j=1 to m
840 q(i)=j
850 next j
860 next i
870 e=millis(1)
880 print "Array assignments", e-s, a$
890 e=e-s
1000 print "Loop time", l/n, b$
1010 print "Token processing", (b-l)/n, b$
1020 print "Constant assignment time", (c-l)/n, b$
1030 print "Division time", (d-l)/n, b$
1040 print "Array access time", (e-l)/n, b$

