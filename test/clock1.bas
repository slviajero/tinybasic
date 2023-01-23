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
3020 N=@T(2): N$="Hour: ": MI=0: MA=24: GOSUB 5000: @T(2)=N 
3030 N=@T(1): N$="Minute: ": MI=0: MA=59: GOSUB 5000: @T(1)=N 
3040 N=@T(0): N$="Second: ": MI=0: MA=59: GOSUB 5000: @T(0)=N 
3050 N=@T(4): N$="Day: ": MI=1: MA=31: GOSUB 5000: @T(4)=N 
3060 N=@T(5): N$="Month: ": MI=1: MA=12: GOSUB 5000: @T(5)=N 
3070 N=@T(6): N$="Year: ": MI=1970: MA=2100: GOSUB 5000: @T(6)=N 
3080 CLS
3090 RETURN
5000 REM "Input a number through the touchpad"
5010 PUT 27,"j": SET 13,1
5020 PUT 27, "l": PRINT N$;N;: PUT 27, "k"
5030 GET &2,A: IF A=0 THEN 5030
5040 IF A=10 THEN SET 13,0: GET &2, A: RETURN
5050 IF A="3" AND N<MA THEN N=N+1
5060 IF A="4" AND N>MI THEN N=N-1
5070 DELAY 200
5080 GOTO 5010
