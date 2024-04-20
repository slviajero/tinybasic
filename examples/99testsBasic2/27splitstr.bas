10 REM "Split a string into substrings"
20 REM ""
100 DIM A$(80)
110 A$="the quick brown fox jumps over the lazy dog"
200 FOR I
210 A=INSTR(A$, " ")
220 IF A=0 THEN PRINT A$: BREAK
230 PRINT A$(1,A-1)
240 A$=A$(A+1)
250 NEXT
300 END
