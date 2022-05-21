10 REM "SerialCallResponse from Arduino examples"
20 REM "First, second and third sensor = 0"
30 F=0
40 S=0
50 T=0
60 B=0
100 REM "setup() put your setup code here, to run once:"
110 PINM 2, 0
120 IF AVAIL(1)>0 GOTO 200
130 PRINT "A";
140 DELAY 300
150 GOTO 120
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 IF AVAIL(1)<=0 GOTO 290 
230 GET B
240 F=AREAD(AZERO)/4
250 DELAY 10
260 S=AREAD(AZERO)/4
270 T=MAP(DREAD(2), 0, 1, 0, 255)
280 PUT F, S, T
290 NEXT
