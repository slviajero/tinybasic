10 REM "TIC TAC TOE from 101 BASIC games"
20 REM "by David Ahl."
30 REM "Ported to Stefan's BASIC in 2021"
40 REM "The machine goes first"
100 PRINT "Welcome to tictactoe"
110 PRINT "The game board is numbered:"
120 PRINT
130 PRINT "1  2  3"
140 PRINT "8  9  4"
150 PRINT "7  6  5"
160 PRINT
200 REM "Main Program"
210 PRINT "Staring a new game"
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
660 END
800 REM "Subroutine to ask user for the move"
810 GOSUB 900
820 INPUT "Your move? ", M
830 RETURN
900 REM "Subroutine to display move"
910 PRINT "Computer moves:",M
920 RETURN
930 REM
