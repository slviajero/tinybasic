10 REM "Various print statements"
20 PRINT "Hello World"; " says Stefan."
30 PRINT A0, B, K0, ":"; A$; ":"
40 A0=10: B=5: K0=6: A$="---"
50 PRINT A0, B, K0, ":"; A$; ":"
60 PRINT A0, B, #5, K0, ":"; A$; ":"
70 PRINT A0, #5, B, K0, ":"; A$; ":"
80 PRINT #5, A0, B, K0, ":"; A$; ":"
90 PRINT #5, A0, B, #0, K0, ":"; A$; ":"
100 PRINT #5, A0, #0, B, K0, ":"; A$; ":"
110 PRINT #5, #0, A0, B, K0, ":"; A$; ":"
120 PRINT #B, A0, K0;"x"
130 A0=-A0
140 PRINT #B, A0, K0;"x"
150 K0=-K0
160 PRINT #B, A0, K0;"x"
170 A0=A0*12: K0=K0*45
180 PRINT #B, A0, K0;"x"
190 B=-10
200 PRINT #B, A0, K0;"x"
210 B=10
220 PRINT #B, A0, K0;"x"
700 REM "Strings and more"
710 A$="Hello"
720 PRINT A$
730 PRINT (A$)
740 PUT 72, "H", 10
750 PUT "A", 13, "B", 10
760 PUT 9, "A", 10
770 FOR I=31 TO 126: PUT I: NEXT
780 PRINT 

