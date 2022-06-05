10 REM "Wire register reader"
20 REM "The address and the registers"
30 A=1
40 DIM R(16)
50 DEF FNM(X)=((X-1)%16+1)
100 REM "the setup()"
110 PRINT "Open slave on port 8, reset regs"
120 OPEN &7, 8, 1 
130 FOR I=1 TO 16: R(I)=0: NEXT
200 REM "the loop()"
210 REM "Wait for command and reply"
220 FOR I
230 IF AVAIL(7)=0 THEN GOSUB 800: DELAY 100: CONT
240 INPUT &7, A$
250 A=FNM(A$(1))
260 FOR N=2 TO LEN(A$)
270 R(A)=A$(N)
280 A=FNM(A+1)
300 NEXT
310 END
800 REM "Set registers for read"
810 FOR I=1 TO 16
820 A$(I)=R(FNM(A+I))
830 NEXT 
840 PRINT &7, A$
850 RETURN 
