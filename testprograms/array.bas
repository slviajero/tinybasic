100 REM "Stefan's BASIC Array test program"
110 REM "Trying dimensioning, format print"
120 REM "FOR loops and the Wang array"
200 DIM B(10)
210 DIM A(8)
220 A(2)=10 
230 PRINT #4, A (2 ), "=10"
240 FOR I =1 TO 10 
250 B (I )=I *I 
260 NEXT 
300 FOR J =1 TO 10 
310 PRINT #3, J ,B (J ), " = ", J*J
320 NEXT 
400 FOR I =1 TO 10 
410 @ (I )=I *I 
420 NEXT 
500 FOR J =1 TO 10 
510 PRINT J ,@ (J )
520 NEXT 
600 END
