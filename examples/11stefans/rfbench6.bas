300 T=MILLIS(1) 
400 K=0
430 DIM M(5)
500 K=K+1
510 LET A=K/2*3+4-5
520 GOSUB 820
530 FOR L=1 TO 5
540 NEXT L
600 IF K<1000 THEN 500
700 PRINT MILLIS(1)-T, "ms"
705 PRINT "Apple Soft    BASIC: 28600 ms"
710 PRINT "Apple Integer BASIC: 18500 ms"
800 END
820 RETURN
