10 REM "Sort a set of numbers"
100 INPUT "How many numbers? ", N
110 REM "Preallocate all local vars before using @()"
120 I=0: J=0: T=0
200 PRINT "Original order"
210 FOR I=1 TO N
220 @(I)=INT(RND(10))
230 PRINT I, @(I)
240 NEXT
300 FOR I=1 TO N-1
310 FOR J=I+1 TO N
320 IF @(I)<=@(J) GOTO 360
330 T=@(I)
340 @(I)=@(J)
350 @(J)=T
360 NEXT
370 NEXT
400 PRINT "Sorted list"
410 FOR I=1 TO N
420 PRINT I, @(I)
430 NEXT
