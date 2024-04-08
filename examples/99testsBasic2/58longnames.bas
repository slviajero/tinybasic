10 REM "Long names test program"
100 alpha=10
110 a=19
120 print a, "is not", alpha
200 name$="stefan"
210 na$="steve"
220 print name$, "is not", na$
300 dim squares(10)
310 for number=1 to 10: squares(number)=number*number: next
320 for number=1 to 10
330 print number, squares(number), "is not", sq(number)
340 next

