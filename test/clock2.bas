100 REM "A little clock in BASIC"
120 @O=2: CLS
200 REM "Display the clock and wait for input"
210 @X=0: @Y=0
220 PRINT @T$(1,8);"  ": PRINT @T$(10);"  ";
230 GET &2, A: IF A=10 THEN GOSUB 3000
240 DELAY 300
250 GOTO 210
3000 REM "Input a time and date"
3010 @X=0: @Y=1
3020 FOR I=1 TO 6
3030 READ D, N$, MI, MA
3040 N=@T(D): GOSUB 5000: @T(D)=N
3050 NEXT
3060 RESTORE: CLS
3090 RETURN
3100 DATA 2, "Hour :", 0, 24, 1, "Minute: ", 0, 59, 0, "Second: ", 0, 59
3110 DATA 4, "Day: ", 1, 31, 5, "Month: ", 1, 12, 6, "Year: ", 1970, 2100
5000 REM "Input a number through the touchpad"
5010 PUT 27,"j": SET 13,1
5020 PUT 27, "l": PRINT N$;N;: PUT 27, "k"
5030 GET &2,A: IF A=0 THEN 5030
5040 IF A=10 THEN SET 13,0: GET &2, A: RETURN
5050 IF A="3" AND N<MA THEN N=N+1
5060 IF A="4" AND N>MI THEN N=N-1
5070 DELAY 200
5080 GOTO 5010
