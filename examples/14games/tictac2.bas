10 REM "TIC TAC TOE from 101 BASIC games"
20 REM "by David Ahl."
30 REM "Ported to Stefan's BASIC in 2021"
40 REM "The machine goes first"
100 PRINT "Welcome to tictactoe"
200 REM "Main Program"
210 PRINT "Staring a new game"
220 A$="123894765"
230 A=9
240 M=A
250 GOSUB 800 
260 P=M
270 B=P%8+1
280 M=B
290 GOSUB 800 
300 Q=M
310 IF Q=(B+3)%8+1 GOTO 360
320 C=(B+3)%8+1
330 M=C
340 GOSUB 900 
350 GOTO 640 
360 C=(B+1)%8+1
370 M=C
380 GOSUB 800 
390 R=M
400 IF R=(C+3)%8+1 GOTO 450
410 D=(C+3)%8+1
420 M=D
430 GOSUB 900
440 GOTO 640 
450 IF P%2<>0 GOTO 500
460 D=(C+6)%8+1
470 M=D
480 GOSUB 900
490 GOTO 640 
500 D=(C+2)%8+1
510 M=D
520 GOSUB 800 
530 S=M
540 IF S=(D+3)%8+1 GOTO 590
550 E=(D+3)%8+1
560 M=E
570 GOSUB 900
580 REM "Game is a draw"
590 E=(D+5)%8+1
600 M=E
610 GOSUB 900
620 PRINT "The game is a draw."
630 GOTO 200
640 PRINT "and wins!  ********"
650 GOTO 200
800 REM "Subroutine to ask user for the move"
810 GOSUB 900
820 INPUT "Your move? ", M
830 IF M<1 OR M>9 GOTO 820
840 M$="x" : GOSUB 1100
850 IF X=1 GOTO 820
860 RETURN
900 REM "Subroutine to display move"
910 PRINT "Computer moves:", M
920 M$="o" : GOSUB 1100
930 GOSUB 1000
940 RETURN
1000 REM "Subroutine to display the board"
1010 PRINT A$(1,1); "|"; A$(2,2); "|"; A$(3,3)
1020 PRINT "-+-+-"
1030 PRINT A$(4,4); "|"; A$(5,5); "|"; A$(6,6)
1040 PRINT "-+-+-"
1050 PRINT A$(7,7); "|"; A$(8,8); "|"; A$(9,9)
1060 RETURN
1100 REM "Set move to board"
1110 FOR I=1 TO 9 
1120 IF (A$(I,I))=("0"+M) THEN BREAK
1130 NEXT
1140 IF I=10 THEN X=1: RETURN
1150 X=0
1160 B$=A$(I+1)
1170 A$(I)=M$
1180 A$(I+1)=B$
1190 RETURN
