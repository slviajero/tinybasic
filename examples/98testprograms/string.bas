100 REM "String test program"
200 PRINT "Test buffer array"
200 @$="uvwxyz"
210 PRINT "buffer is ", @$
220 @$(2)=@$(3 ,5)
230 PRINT @$, " is uwxy"
240 PRINT LEN (@$), "is 4"
250 PRINT @$(3 ,3), "is x"
260 PRINT @$(3), "is xy"
270 PRINT @$(1 ,3), "is uwx"
280 PRINT @$(3 ,LEN (@$)), "is xy"
300 DIM R$(20)
310 INPUT "your name :", R$
320 PRINT "letter 2 and 3 :", R$(2 ,3)
400 R$="huhu"
410 PRINT "String is huhu now :", R$
420 IF R$="haha" THEN PRINT "1 should not show"
430 IF R$="huhu" THEN PRINT "2 passed true eq"
440 IF "huhu"="huhu" THEN PRINT "3 passed eq"
450 IF "huhu"<> "haha"THEN PRINT "4 passed false neq"
460 IF "huhu"="huhu" AND "haha"="haha" THEN PRINT "5 passed AND"
470 IF "huhu"=R$ THEN PRINT "6 passed eq"
480 PRINT "Tests 2-6 should show here"
500 END
