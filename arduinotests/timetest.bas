100 REM "Tests the millis functions and calculates"
110 REM "runtimes of certain operations"
200 n=100
210 m=10000
220 print "Running ";n;"*";m;" operations:"
300 s=millis(1)
320 for i=1 to n
330 for j=1 to m: a=j/i : next j
340 next i
350 e=millis(1)
360 print "Integer divisions, start", s, "end :",e, "time: ",e-s, " milliseconds"
400 dim q(n)
410 s=millis(1)
420 s=millis(1)
440 for i=1 to n
450 for j=1 to m: q(i)=j : next j
460 next i
470 e=millis(1)
480 print "Array assignments, start", s, "end :",e, "time: ",e-s, " milliseconds"
500 s=millis(1)
520 for i=1 to n
530 for j=1 to m: a=j+i : next j
540 next i
550 e=millis(1)
800 s=millis(1)
820 for i=1 to n
830 for j=1 to m: next j
840 next i
850 e=millis(1)
860 print "Empty loops, start", s, "end :",e, "time: ",e-s, " milliseconds"
