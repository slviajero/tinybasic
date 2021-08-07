90 REM "A little test program for the intepreter"
91 REM "See https://projecteuler.net/problem=9 for details"
92 REM "This program uses the number theory approach"
100 INPUT "UMFANG = ",K 
110 IF K =0 THEN END 
130 IF K %2 =0 THEN U =K /2 :GOTO 200 
140 PRINT "UMFANG UNGERADE - KEINE LOESUNG":GOTO 100 
200 F =0 
220 X =SQR (U /2 )
240 FOR I =1 TO X 
260 IF U %I <> 0 THEN 820 
290 FOR J =1 TO I 
340 D =I +J 
360 IF U %D <> 0 THEN 810 
550 IF D %2 =0 THEN 810 
560 R =I :Q =J 
570 IF R =Q THEN 610 
580 IF R >Q THEN R =R -Q 
590 IF Q >R THEN Q =Q -R 
600 GOTO 570 
610 IF R <> 1 THEN 810 
730 A =I *I -J *J 
740 B =2 *I *J 
750 C =I *I +J *J 
760 PRINT "PRIMITIVES TRIPEL ",A ," ",B ," ",C 
770 PRINT "UMFANG ",A +B +C 
780 S =K /(A +B +C )
790 PRINT "SKALIERE MIT ",S 
800 PRINT "LOESUNG ",A *S ," ",B *S ," ",C *S 
805 F =1 
810 NEXT 
820 NEXT 
825 IF F =0 THEN PRINT "KEINE LOESUNG"
830 GOTO 100 
