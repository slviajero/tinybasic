10 DIM B(10,10),C(8,8)
20 FOR I=1 TO 10
30 FOR J=1 TO 10
40 B(I,J)=12*I+J
50 NEXT
60 NEXT
100 FOR I=1 TO 8
110 FOR J=1 TO 8
120 C(I,J)=B(J,I)
130 NEXT
140 NEXT
200 FOR I=1 TO 8
210 FOR J=1 TO 8
220 PRINT C(I,J),B(J,I)
230 NEXT
240 NEXT
250 FOR I=1 TO 10
260 FOR J=1 TO 10
270 IF B(I,J)<>12*I+J THEN PRINT "fail": BREAK
280 NEXT
290 NEXT
300 FOR I=1 TO 10
310 PRINT B(I),B(I,1)
320 NEXT