10 M=USR(0, 9)
20 PRINT "Memory size is ";M 
30 PRINT "Himem is "; HIMEM
40 PRINT "Heap size is "; M-HIMEM-1
110 DIM A(8)
120 FOR I=1 TO 8: A(I)=I: NEXT 
130 FOR I=1 TO 8: PRINT I, A(I): NEXT 
140 A$="Hello World"
150 PRINT "Heap size is "; M-HIMEM-1
200 PRINT "A$ is ";A$
210 CLR A()
220 PRINT "A$ is ";A$
230 PRINT "Heap size is "; M-HIMEM-1
300 DIM A(20)
310 FOR I=1 TO 20: A(I)=I: NEXT 
320 FOR I=1 TO 20: PRINT I, A(I): NEXT
330 PRINT "Heap size is "; M-HIMEM-1
