10 REM "CharacterAnalysis from Arduino examples"
20 REM "Defining a few of the Arduino char functions"
30 DEF FNN(X)=(X>="0")AND(X<="9")
35 DEF FNA(X)=((X>="A")AND(X<="X"))OR((X>="a")AND(X<="x"))
40 DEF FNC(X)=(X>31 AND X<127)
45 DEF FNX(X)=(X<32)
50 DEF FNW(C)=(X=32 OR X=9)
55 DEF FNU(X)=((X>="A")AND(X<="Z"))
60 DEF FNL(X)=((X>="a")AND(X<="z"))
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 IF NOT AVAIL(1) GOTO 400 
230 GET C
240 PRINT "You sent me: '";
250 PUT C
260 PRINT "' ASCII Value'", C
270 IF FNN(C) OR FNA(C) THEN PRINT "it's alphanumeric"
280 IF FNA(C) THEN PRINT "it's alphabetic"
290 IF FNC(C) THEN PRINT "it's ASCII"
300 IF FNW(C) THEN PRINT "it's whitespace" 
310 IF FNX(C) THEN PRINT "it's a control character"
320 IF FNN(C) THEN PRINT "it's a numeric digit"
330 IF FNA(C) AND NOT FNW(C) THEN PRINT "it's printable and not whitespace"
340 IF FNL(C) THEN PRINT "it's lowercase"
350 IF FNU(C) THEN PRINT "it's uppercase"
400 NEXT
