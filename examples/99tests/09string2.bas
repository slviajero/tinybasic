10 REM "Strings in BASIC (2)"
20 REM "Substrings, splitting and concatenating"
100 DIM A$(80), B$(80)
110 A$="the quick brown fox jumps over the lazy dog"
120 PRINT A$
130 PRINT "The first 3 characters-", A$(1,3)
140 PRINT "The rest of the string-", A$(4)
150 PRINT "Splitting ..."
160 FOR I=1 TO LEN(A$)
170 PRINT A$(I, I); 
180 IF A$(I, I)=" " THEN PRINT 
190 NEXT I
200 PRINT
210 C$=A$(1, 19)
220 D$=A$(20)
230 PRINT C$
240 PRINT D$
250 PRINT "Concatenating ..."
260 B$=C$ : B$(LEN(B$)+1)=D$
270 PRINT B$
300 END

