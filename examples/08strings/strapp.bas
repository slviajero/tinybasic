10 REM "StringAppendOperator from Arduino examples"
20 DIM A$(80), B$(80)
100 REM "setup() put your setup code here, to run once:"
110 A$="Sensor "
120 B$="value"
130 PRINT: PRINT: PRINT "Appending to a String:"
140 PRINT 
150 PRINT A$
160 A$(LEN(A$)+1)=B$
170 PRINT A$
180 A$(LEN(A$)+1)=" for input "
190 PRINT A$
200 A$(LEN(A$)+1)="A"
210 PRINT A$
220 A$(LEN(A$)+1)=STR(0)
230 PRINT A$
240 A$(LEN(A$)+1)=": "
250 PRINT A$
260 A$(LEN(A$)+1)=STR(AREAD(AZERO))
270 PRINT A$

