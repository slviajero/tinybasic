300 T=MILLIS(1) 
400 K=0
500 K=K+1
530 A=POW(K, 2)
540 B=LOG(K)
550 C=SIN(K)
600 IF K<100 THEN GOTO 500
700 PRINT MILLIS(1)-T, "ms"
705 PRINT "Apple Soft    BASIC: 10700 ms"
800 END
