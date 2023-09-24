10 REM "Calculate the first N prime numbers"
20 REM "and output them!"
30 REM "This version uses @ and BREAK"
200 N=100
210 IF N=0 THEN END
220 IF N>@ THEN PRINT "Maximum is ", @ : GOTO 200
230 @(1)=2: @(2)=3 
240 FOR I=3 TO N
250 K=@(I-1)+2
260 L=SQR(K)
270 FOR J=1
280 IF K%@(J)=0 THEN K=K+2 : GOTO 260
290 IF @(J)>L THEN BREAK
300 NEXT J
310 @(I)=K
320 NEXT I
360 PRINT "The first", N, "primes are:"
370 FOR J=1 TO N 
380 PRINT J, @(J)
390 NEXT 
500 END
