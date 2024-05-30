10 REM "Analyses the distribution of a set of random numbers"
100 INPUT "Number of samples? ", N
110 INPUT "Range of numbers? ", R
120 REM "Preallocate vars before using @"
130 S=0: A=0: B=0: I=0
200 FOR I=1 TO R
210 @(I)=0
220 NEXT
230 A=N
240 B=0
300 FOR I=1 TO N
310 S=INT(RND(R))+1
320 @(S)=@(S)+1
330 NEXT
400 PRINT "Distribution:"
410 PRINT "Value", "Frequency"
420 FOR I=1 TO R
430 PRINT #6, I, @(I)
440 IF @(I)<A LET A=@(I)
450 IF @(I)>B LET B=@(I)
460 NEXT
470 PRINT "Minimum= ", A
480 PRINT "Maximum= ", B
