10 A$="1234567890"
20 FOR I=1 TO 10
30 PRINT A$(I),LEN(A$(I)),VAL(A$(I)),@V,@S
40 T$=MID$(A$,I)
50 PRINT T$,LEN(T$),VAL(T$),@V,@S
60 NEXT
