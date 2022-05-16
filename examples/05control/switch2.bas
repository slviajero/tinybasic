10 REM "Switch2 from Arduino examples"
20 REM ""
100 REM "setup() put your setup code here, to run once:"
110 FOR T=1 TO 6 : PINM T, 1 : NEXT
200 REM "loop() put your main code here, to run repeatedly:"
210 FOR I
220 IF AVAIL(1)=0 GOTO 220
230 GET C
240 C=C-"a"+1
250 ON C GOTO 300, 320, 340, 360, 380
260 FOR T=1 TO 6 : DWRITE T, 1 : NEXT
270 GOTO 400
300 DWRITE 2, 1
310 GOTO 400
320 DWRITE 3, 1
330 GOTO 400
340 DWRITE 4, 1
350 GOTO 400
360 DWRITE 5, 1
370 GOTO 400
380 DWRITE 6, 1
400 NEXT
