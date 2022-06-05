10 REM "Wire register engine reader slave"
20 REM "The address A, N the number of registers R()"
30 REM "Maximum number of registers is 32"
40 N=16 
50 DIM R(N)
60 REM "A function in X which is cyclic 1..N"
70 DEF FNM(X)=(A+X-2)%N+1
100 REM "the setup()"
110 PRINT "Open slave on port 8"
120 OPEN &7, 8, 1 
130 A=1
200 REM "the loop()"
210 REM "Wait for command and reply"
220 FOR I
230 REM "Call the register state engine"
240 GOSUB 1000
300 REM "Publish the register content"
310 FOR K=1 TO N
320 A$(K)=R(FNM(K))
330 NEXT K
340 PRINT &7, A$
400 REM "Process commands"
410 IF AVAIL(7)=0 THEN DELAY 100: CONT
420 REM "A command was received"
430 INPUT &7, A$
440 REM "First byte is the address, discard if invalid"
450 IF A$(1)>0 AND A$(1)<=N THEN A=ABS(A$(1)) ELSE CONT
500 REM "Add command sequences here"
800 REM "Write received data to the registers"
810 FOR J=2 TO LEN(A$)
820 R(FNM(J-1))=A$(J)
830 NEXT J
840 NEXT I
850 END
1000 REM "Update registers with payload"
1010 REM "Put your payload code here, loop is only demo"
1020 FOR K=1 TO N/2
1030 R(K)=K
1040 NEXT K
1050 RETURN  
