100 REM "Dark arts volume 2: writing self modifying programs"
110 REM "by calling the tokenizer with USR(8,)"
120 DIM A$(80)
130 INPUT "Input a formula y=f(x) :",A$
140 PRINT "Formula read : ";A$
150 IF A$="end" OR A$="" THEN END
160 @$=A$
170 A=USR (8,310)
180 FOR X=1 TO 10
190 GOSUB 300
200 PRINT X,Y
210 NEXT 
220 GOTO 130
230 END 
300 REM "310 is the line to be modified" 
310 REM "always the call to USR(8)!!"
310 Y=X
320 RETURN 
