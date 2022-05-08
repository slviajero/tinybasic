10 REM "Melody from Arduino examples - small version"
20 REM "Read tones T(), and durations D()"
50 READ N
60 DIM T(N) : FOR I=1 TO N : READ T(I) : NEXT
70 DIM D(N) : FOR I=1 TO N : READ D(I) : NEXT
100 REM "setup() put your setup code here, to run once:"
110 FOR I=1 TO N
120 D=1000/D(I)
130 PLAY 8, T(I), D
140 DELAY D*13/10
150 NEXT
160 PLAY 8, 0, 1
170 END
200 REM "loop() put your main code here, to run repeatedly:"
1200 REM "The melody of N notes and durations"
1210 DATA 8
1220 DATA 247, 185, 185, 208, 185, 0, 233, 247
1230 DATA 4, 8, 8, 4, 4, 4, 4, 4
