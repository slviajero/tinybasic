10 REM "StringCaseChange from Arduino examples"
20 DIM A$(80), B$(80)
30 DEF FNU(X)=((X>="A")AND(X<="Z"))
40 DEF FNL(X)=((X>="a")AND(X<="z"))
100 REM "setup() put your setup code here, to run once:"
110 A$="<html><head><body>"
120 FOR I=1 TO LEN(A$)
130 IF FNL(A$(I)) THEN B$(I)=(A$(I,I)-32) ELSE B$(I)=A$(I,I) 
140 NEXT I
150 PRINT B$
160 A$="</BODY></HTML>"
170 FOR I=1 TO LEN(A$)
180 IF FNU(A$(I)) THEN B$(I)=(A$(I,I)+32) ELSE B$(I)=A$(I,I) 
190 NEXT I
200 PRINT B$
