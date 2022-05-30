10 REM "Serial monitor in second serial port"
20 REM ""
30 DIM A$(80),B$(80)
200 REM "The loop() function"
210 INPUT A$
220 IF A$="end" THEN END
230 PRINT &4,A$
240 DELAY 100
250 FOR I=0
260 GET &4,A
270 IF A=0 OR I=80 THEN BREAK
280 B$(I+1)=A
290 NEXT
300 PRINT I,"bytes received"
310 PRINT B$
320 GOTO 200
400 END
