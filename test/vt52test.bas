5 D=2
10 PUT &D, D
20 PRINT &D,"the quick brown fox"
30 PRINT &D,"jumps over the lazy dog"
35 DELAY 2000
40 PUT &D,27,"A",27,"L"
45 DELAY 1000
50 PRINT &D,"<this is an inserted line>"
55 DELAY 2000
60 PUT &D,27,"A",27,"M"
70 PUT &D,27,"B"
80 PRINT &D,"<deleted again>"
90 DELAY 1000
100 PUT &D,27,"A",27,"M"
110 PUT &D,27,"H",27,"I"
120 DELAY 1000
130 PRINT &D,"<reverse line feed and scroll>"
140 DELAY 1000
150 PUT &D,27,"H",27,"K"
160 PRINT &D,"<clear to end of line>"
170 DELAY 1000
180 PUT &D,27,"A"
190 PUT &D,27,"M"
200 PUT &D,27,"Y",32+3,32+5
210 PRINT &D,"I am here";
230 PUT &D,27,"j"
240 PRINT &D
250 PRINT &D,"the next line "
260 DELAY 1000
270 PUT &D,27,"k"
280 PRINT &D," - back again"
290 DELAY 1000
300 PUT &D,27,"J"
310 PRINT &D,"<clear to end of screen>";
320 DELAY 1000
330 PUT &D,27,"o"
340 PRINT &D,"gone";
350 PUT &D,13
355 DELAY 1000
360 PRINT &D,"carriage return"
370 DELAY 1000
380 PUT &D,27,"d"
400 PUT &D,27,"H"
410 FOR I=0 TO 15
420 PUT &D,27,"b",32+I
430 PRINT &D,"color"
435 delay 200
440 NEXT
500 FOR I=1 TO 10
510 PRINT &D
520 DELAY 500
530 NEXT
600 CLS
610 T=MILLIS(1)
620 PRINT &D,"the quick brown fox"
630 PRINT &D,"jumps over the lazy dog"
640 T=MILLIS(1)-T
650 PRINT &D,T,"ms for ";42;" chars"
660 PRINT &D,"bandwidth ",42*8*1000/T;" baud"

