10 PINM 6,1 
20 DWRITE 6, 0
30 INPUT A
40 P=40+A*200/180
50 FOR I=1 TO 40: DELAY 20: PULSE 6, P: NEXT
60 GOTO 30
