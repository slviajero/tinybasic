10 REM "Calculate the first N prime numbers"
20 REM "and output them!"
200 CLR 
210 INPUT "How many primes (0=stop)? ", N 
220 IF N=0 THEN END
230 DIM P(N): P(1)=2: P(2)=3 
240 FOR I=2
250 K=P(I)
260 K=K+2 
270 L=SQR(K)
280 J=1
290 D=P(J)
310 M=K%D 
320 IF M=0 THEN 260
330 IF D<L THEN J=J+1 :GOTO 290 
340 P(I+1)=K 
345 IF I+1=N THEN BREAK
350 NEXT
360 PRINT "The first", N, "primes are:"
370 FOR J=1 TO N 
380 PRINT J, P (J)
390 NEXT 
400 GOTO 200
500 END
