100 REM "String test program"
200 @$="uvwxyz"
210 PRINT @$
220 @$(2 )=@$(3 ,5 )
230 PRINT @$, " is uwxy"
240 PRINT LEN (@$), "is 4"
250 PRINT @$(3 ,3 ), "is x"
260 PRINT @$(3 ), "is xy"
270 PRINT @$(1 ,3 ), "is uwx"
280 PRINT @$(3 ,LEN (@$)), "is xy"
300 DIM R$(20 )
310 INPUT R$
320 PRINT R$(2 ,6 )
400 @$="huhu"
410 IF @$="haha"THEN PRINT "is haha"
420 IF @$="huhu"THEN PRINT "is huhu"
430 IF "huhu"="huhu"THEN PRINT "is huhu"
440 IF "huhu"<> "haha"THEN PRINT "is not haha"
450 IF "huhu"="huhu"AND "haha"="haha"THEN PRINT "the same"
460 IF "huhu"=@$THEN PRINT "is huhu again"
500 end
