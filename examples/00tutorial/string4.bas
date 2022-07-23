10 REM "Strings in BASIC (4)"
20 REM "String arrays"
30 DIM B$(10,10)
100 REM "Read a string"
110 FOR I=1 TO 9
120 READ B$()(I)
130 NEXT I
200 REM "Print the entire string"
210 FOR I=1 TO 9
220 PRINT B$()(I)
230 NEXT I 
300 REM "Substrings and arrays"
310 FOR I=1 TO 9
320 PRINT B$(1,1)(I)
330 NEXT I
400 DATA "the", "quick", "brown", "fox", "jumps", "over", "the", "lazy", "dog"
