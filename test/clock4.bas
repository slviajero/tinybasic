100 REM "A little clock in BASIC"
120 @O=2: A=0: CLS
200 REM "Display the clock and wait for input"
210 @X=0: @Y=0
220 PRINT @T$(1,8);"  ": PRINT @T$(10);"    ";
230 IF NOT A THEN @D(16)=" "
240 ELSE @D(16)="A": IF @T(2)=@E(2) AND @T(1)=@E(1) THEN GOSUB 4000
250 GET &2, C: IF C=10 THEN GOSUB 3000 ELSE IF C="1" THEN A=NOT A
260 GOTO 210
3000 REM "Input a time and date on line 2"
3010 @X=0: @Y=1: I=1: SET 13,1
3020 DELAY 200
3030 RESTORE 1+(I-1)*4: READ D, N$, L, H
3035 IF I<7 THEN N=@T(D) ELSE N=@E(D)
3040 PUT 27,"j", 27, "l": PRINT N$;N;: PUT 27, "k"
3060 GET &2, C: IF C=0 THEN 3060
3070 IF C="1" AND I>1 THEN I=I-1
3080 IF C="2" AND I<8 THEN I=I+1
3090 IF C="3" AND N<H THEN N=N+1
3100 IF C="4" AND N>L THEN N=N-1
3120 IF C=10 THEN SET 13,0: GET &2, C: RETURN
3130 IF I<7 THEN @T(D)=N ELSE @E(D)=N
3140 GOTO 3020
3600 DATA 2, "Hour :", 0, 23, 1, "Minute: ", 0, 59, 0, "Second: ", 0, 59
3610 DATA 4, "Day: ", 1, 31, 5, "Month: ", 1, 12, 6, "Year: ", 1970, 2100
3620 DATA 2, "Alarm h: ", 0, 24, 1, "Alarm m: ", 0, 59
4000 REM "The alarm routine"
4010 DELAY 500
4030 RETURN
