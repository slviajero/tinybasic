10 @$="uvwxyz"
15 PRINT @$
30 @$(2 )=@$(3 ,5 )
40 PRINT @$
50 PRINT LEN (@$)
60 PRINT @$(3 ,3 )
70 PRINT @$(3 ),
80 PRINT @$(1 ,3 )
90 PRINT @$(3 ,LEN (@$))
100 DIM R$(20 )
110 INPUT R$
120 PRINT R$(2 ,6 )
210 @$="huhu"
220 IF @$="haha"THEN PRINT "is haha"
230 IF @$="huhu"THEN PRINT "is huhu"
240 IF "huhu"="huhu"THEN PRINT "is huhu"
250 IF "huhu"<> "haha"THEN PRINT "is not haha"
260 IF "huhu"="huhu"AND "haha"="haha"THEN PRINT "the same"
270 IF "huhu"=@$THEN PRINT "is huhu again"
300 end
