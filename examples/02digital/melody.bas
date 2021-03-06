10 REM "Melody from Arduino examples"
20 REM "Read Notes N0(), tones T(), and durations D()"
30 READ N
40 DIM N0(N) : FOR I=1 TO N : READ N0(I) : NEXT
50 READ N
60 DIM T(N) : FOR I=1 TO N : READ T(I) : NEXT
70 DIM D(N) : FOR I=1 TO N : READ D(I) : NEXT
100 REM "setup() put your setup code here, to run once:"
110 FOR I=1 TO N
120 D=1000/D(I)
130 IF T(I)=0 THEN T=0 ELSE T=N0(T(I))
140 PLAY 8, T, D
150 DELAY D*13/10
160 NEXT
170 PLAY 8, 0
180 END
200 REM "loop() put your main code here, to run repeatedly:"
1000 REM "Pitches of 89 notes"
1010 DATA 89
1020 DATA 31, 33, 35, 37, 39, 41, 44, 46
1030 DATA 49, 52, 55, 58, 62, 65, 69, 73
1040 DATA 78, 82, 87, 93, 98, 104, 110, 117
1050 DATA 123, 131, 139, 147, 156, 165, 175, 185
1060 DATA 196, 208, 220, 233, 247, 262, 277, 294
1070 DATA 311, 330, 349, 370, 392, 415, 440, 466 
1080 DATA 494, 523, 554, 587, 622, 659, 698, 740 
1090 DATA 784, 831, 880, 932, 988, 1047, 1109, 1175 
1100 DATA 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865
1110 DATA 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960
1120 DATA 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699 
1130 DATA 4978
1200 REM "The melody of N notes and durations"
1210 DATA 8
1220 DATA 37, 32, 32, 34, 32, 0, 36, 37
1230 DATA 4, 8, 8, 4, 4, 4, 4, 4
