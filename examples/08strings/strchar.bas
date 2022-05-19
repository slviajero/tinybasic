10 REM "StringCharacter from Arduino examples"
20 DIM A$(80), B$(80)
100 REM "setup() put your setup code here, to run once:"
110 PRINT: PRINT
120 PRINT "String character manipultions:"
130 A$="SensorReading: 456"
140 B$=A$(16, 16)
150 PRINT "The most significant digit is", B$ 
160 PRINT
170 B$=A$(1, 13)
180 B$(14)="="
190 B$(15)=A$(15)
200 PRINT B$
