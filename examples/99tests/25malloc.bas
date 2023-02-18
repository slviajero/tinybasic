10 REM "Elementary malloc demo"
20 REM ""
100 PRINT "Allocate 16 bytes of memory with identifier 1"
110 M=MALLOC(1, 16)
120 PRINT "Address of memory segment", M
200 PRINT "Storing data"
210 FOR I=0 TO 15
220 POKE M+I, I*2 
230 NEXT I 
300 PRINT "Retrieving data"
310 FOR I=0 TO 15
320 PRINT I, PEEK(M+I)
330 NEXT I 
400 F=FIND(1)
410 PRINT "Memory address of buffer 1 is", F
500 A0=10
510 F=FIND(A0)
520 PRINT "Memory address of variable A0 is", F
600 DIM A(8)
610 F=FIND(A())
620 PRINT "Memory address of array A() is", F
700 A$="Hello World"
710 F=FIND(A$)
720 PRINT "Memory address of string A$ is", F 
800 PRINT "HIMEM is now ", HIMEM
810 CLR 1
820 PRINT "HIMEM after CLR", HIMEM
