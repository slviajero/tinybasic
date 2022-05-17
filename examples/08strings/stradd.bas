10 REM "StringAdditionOperator from Arduino examples"
20 REM ""
30 DIM A$(64), B$(64), C$(64)
100 REM "setup() put your setup code here, to run once:"
110 A$="You added "
120 B$="this string"
130 PRINT: PRINT
140 PRINT "Adding strings together (concatenation):"
150 PRINT 
160 C$=A$ : C$(LEN(C$)+1)=STR(123)
170 PRINT C$
180 C$=A$ : C$(LEN(C$)+1)=STR(12345)
190 PRINT C$
200 C$=A$ : C$(LEN(C$)+1)="A"
210 PRINT C$
220 C$=A$ : C$(LEN(C$)+1)="abc"
230 PRINT C$
220 C$=A$ : C$(LEN(C$)+1)=B$
230 PRINT C$
240 V=AREAD(AZERO)
250 A$="Sensor value: "
260 C$=A$ : C$(LEN(C$)+1)=STR(V)
270 PRINT C$
280 C$=A$ : C$(LEN(C$)+1)=STR(MILLIS(1))
290 PRINT C$
300 END
