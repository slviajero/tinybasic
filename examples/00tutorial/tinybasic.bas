10 REM "Tinybasic interpreter written in BASIC"
15 REM 
20 REM "Written in Stefan's Arduino BASIC aka IoT Basic Version 2(!!)"
25 REM "It also runs on MS BASIC interpreters. Set LL to 0 for this"
30 REM "and remove the ()() and @V constructs."
35 REM "The interpreter implements Dr. Wang'S Palo Alto BASIC "
40 REM "See http://www.bitsavers.org/pdf/interfaceAge/197612/092-108.pdf for"
45 REM "On the implemented language."
50 REM 
55 REM "Main components of this program:"
57 REM "GOSUB 11000: the lexer. It decomposes the string A$ into tokens."
60 REM "   There is one token produced in T, C, C$ after each call until"
62 REM "   the end of the line which is T=0."
65 REM "GOSUB 12000: the expression evaluation. This is a recursive decent"
67 REM "    parser using a stack ST. In the end the result is on the stack."
70 REM "GOSUB 14000: the command code. For each command there is one section."
72 REM "    At the end of the command a new token must be produced by calling"
75 REM "    The lexer with GOSUB 11000."
77 REM "GOSUB 13xxx: helper routines for the stack, parser and line storage."
80 REM "30000: keywords. First data is the number of keywords."
82 REM "The input loop from 2000-6000 ties it all together."
85 REM
100 REM "****************** Parameters and declarations ********************"
110 NL=100: REM "Maximum number of lines"
120 AS=100: REM "Number of elements in the @() array"
130 SS=16: REM "The size of the arithmetic stack"
140 LL=80: REM "The line length, set to 0 if you run on MS BASIC dialects."
150 GD=4: REM "Maximum depth of GOSUB statements"
160 FM=4: REM "Maximum number of active FOR loops (nested)"
200 REM "The debug flag, settings are"
210 REM "0: no debug, 1: lexer debug, 2: expression debug, 4: command debug"
220 DE=0
990 REM 
1000 REM "*************** essential declarations and functions *************"
1002 REM
1005 REM "The token T, the accu X and the error E"
1010 T=0: X=0: E=0
1015 REM "The command string C$ and the input buffer A$"
1020 IF LL>0 THEN DIM C$(LL), A$(LL), T$(LL)
1025 REM "Functions to handle character and case stuff"
1030 T=1: X=0: E=0: C$=""
1040 DEF FNNU(X)=(X=>48 AND X<=57)
1050 DEF FNAL(X)=(X>=65 AND X<=90)
1060 DEF FNUP(X)=X-ABS(X>=97 AND X<=122)*32
1070 REM "The data arrays for variables and one array"
1080 DIM VA(26), AR(AS)
1090 REM "The stacks for numbers and operators"
1100 DIM ST(SS)
1110 SI=0
1120 REM "functions of the interpreter"
1130 DEF FNTE(T)=(T=0 OR T=ASC(":")): REM "terminal symbol of a statement"
1200 REM "The program storage"
1210 DIM LN(NL)
1220 IF LL>0 THEN DIM LS$(LL, NL): GOTO 1240
1230 DIM LS$(NL)
1240 REM "HEre is the current line, 0 if interactive."
1250 HE=0
1260 REM "The tables for GOSUB and FOR with their indices GS and FS"
1270 DIM GH(GD), GI(GD): GS=0
1280 DIM FV(FM), FE(FM), FD(FM), FH(FM), FI(FM): FS=0
1995 REM
2000 REM "************************* input loop *****************************"
2010 INPUT "]", A$: I=1: GOSUB 11000: REM "Input a line and get the first token"
2020 REM "first token a line number -> store it or delete it"
2030 IF T<>-127 THEN 5000
2040 X=INT(X)
2050 T$=MID$(A$, I)
2055 REM "empty line, delete it"
2060 IF T$="" OR T$=" " THEN GOSUB 13500: GOTO 5800
2065 REM "insert the line"
2070 GOSUB 13200: GOTO 5800
5000 REM "no line number at the beginning, execute it, check on RUN first"
5010 REM "on RUN loop through the statements"
5020 IF T<>-104 THEN 5500
5030 HE=1
5040 IF LN(HE)=0 THEN GOTO 5800
5045 REM "get the actual line and execute it"
5050 A$=LS$()(HE): I=1: GOSUB 11000
5060 GOSUB 14000
5065 IF E<>0 THEN 5800
5070 IF T=-107 THEN PRINT "Stop at", LN(HE): GOTO 5800
5080 IF T=0 THEN HE=HE+1: GOTO 5040
5100 GOTO 5060
5495 REM "The interactive loop, no line fetching"
5500 GOSUB 14000: IF T<>0 GOTO 5500
5800 REM "Catch the error and get more input"
5810 IF E<>0 THEN SP=1: I=1: A$="": GS=0: FS=0: E=0
5995 GOTO 2000
11000 REM "******* the lexer, generates a token stream in T, X, C$ ********" 
11005 REM
11010 T=0: X=0: C$=""
11020 IF I>LEN(A$) THEN 11400
11030 C$=MID$(A$, I, 1): C=ASC(C$): C=FNUP(C): C$=CHR$(C)
11040 REM "a whitespace is skipped"
11050 IF C$=" " THEN I=I+1: GOTO 11010
11060 REM "a number - uses a BASIC special feature @V "
11070 IF FNNU(C) THEN T$=MID$(A$, I): X=VAL(T$): I=I+@V: T=-127: GOTO 11400
11080 REM "a string constant"
11090 IF C<>34 THEN 11200
11100 J=I
11110 I=I+1: IF MID$(A$, I, 1)<>CHR$(34) AND I<LEN(A$) GOTO 11110
11120 C$=MID$(A$, J+1, I-J-1): T=-125: X=I-J-1: I=I+1: GOTO 11400
11200 REM "a keyword is searched"
11210 RESTORE: READ N
11230 FOR J=1 TO N
11240 READ B$,S
11250 IF MID$(A$,I,LEN(B$))=B$ THEN I=I+LEN(B$): T=S: C$=B$: GOTO 11400
11260 NEXT
11270 REM "a variable is just a character"
11280 IF FNAL(C) THEN I=I+1: T=-124: GOTO 11400
11290 REM "Any operator and other characters"
11300 I=I+1: T=C
11400 REM "The debug hook of lex"
11410 IF DE<>1 THEN RETURN
11420 PRINT "** lex delivered token ",T, MID$(A$, I)
11430 RETURN
12000 REM "*************** the expression evaluator **********************"
12005 REM 
12010 REM "this is a classical recursive decent parser, using an own stack"
12020 REM "as BASIC has no local variables"
12025 REM
12030 REM "*** entry point for the expression - compare"
12040 GOSUB 12200: REM "Call addition"
12050 IF T<>ASC("=") THEN 12070
12060 GOSUB 11000: GOSUB 12000: GOSUB 13030: Y=X: GOSUB 13030: X=(X=Y): GOSUB 13050
12070 IF T<>ASC("#") AND T<>-119 THEN 12090
12080 GOSUB 11000: GOSUB 12000: GOSUB 13030: Y=X: GOSUB 13030: X=(X<>Y): GOSUB 13050
12090 IF T<>ASC(">") THEN 12110
12100 GOSUB 11000: GOSUB 12000: GOSUB 13030: Y=X: GOSUB 13030: X=(X>Y): GOSUB 13050
12110 IF T<>ASC("<") THEN 12130
12120 GOSUB 11000: GOSUB 12000: GOSUB 13030: Y=X: GOSUB 13030: X=(X<Y): GOSUB 13050
12130 IF T<>-121 THEN 12150
12140 GOSUB 11000: GOSUB 12000: GOSUB 13030: Y=X: GOSUB 13030: X=(X>=Y): GOSUB 13050
12150 IF T<>-120 THEN 12170
12160 GOSUB 11000: GOSUB 12000: GOSUB 13030: Y=X: GOSUB 13030: X=(X<=Y): GOSUB 13050
12170 RETURN
12200 REM "*** additions of all kind"
12210 IF T<>ASC("+") AND T<>ASC("-") THEN GOSUB 12300: GOTO 12230
12220 X=0: GOSUB 13050: REM "Push a zero"
12230 IF T<>ASC("+") THEN 12250
12240 GOSUB 11000: GOSUB 12300: GOSUB 13030: Y=X: GOSUB 13030
12245 X=(Y+X): GOSUB 13050: GOTO 12230
12250 IF T<>ASC("-") THEN 12270
12260 GOSUB 11000: GOSUB 12300: GOSUB 13030: Y=X: GOSUB 13030
12265 X=(X-Y): GOSUB 13050: GOTO 12230
12270 IF NOT DE THEN RETURN
12280 PRINT "** after add, token", T, "stack is ": GOSUB 13060
12290 RETURN
12300 REM "*** the term evaluation handles multiplication"
12310 GOSUB 12400
12320 GOSUB 11000
12330 IF T<>ASC("*") THEN 12350
12340 GOSUB 11000: GOSUB 12300: GOSUB 13030: Y=X: GOSUB 13030: X=X*Y: GOSUB 13050
12350 IF T<>ASC("/") THEN 12370 
12360 GOSUB 11000: GOSUB 12300: GOSUB 13030: Y=X: GOSUB 13030: IF Y=0 THEN E=-1: Y=1
12365 X=X/Y: GOSUB 13050
12370 IF T<>ASC("%") THEN 12390 
12380 GOSUB 11000: GOSUB 12300: GOSUB 13030: Y=X: GOSUB 13030: IF Y=0 THEN E=-1: Y=1
12385 X=X%Y: GOSUB 13050
12390 RETURN
12400 REM "*** the factor evaluation never calls nexttoken"
12410 IF T=-127 THEN GOSUB 13040: GOTO 12900: REM "a plain number"
12420 IF T=-124 THEN X=VA(C-64): GOSUB 13040: GOTO 12900: REM "a variable"
12430 IF T<>ASC("(") THEN 12480: REM "an expression in brackets"
12450 GOSUB 11000: GOSUB 12000: IF E GOTO 12900
12460 IF T<>ASC(")") THEN E=-1
12470 GOTO 12900
12480 IF T<>-103 THEN 12510: REM "the ABS function"
12490 GOSUB 11000: GOSUB 12430
12500 GOSUB 13030: X=ABS(X): GOSUB 13040: GOTO 12900
12510 IF T<>-102 THEN 12540: REM "the RND function"
12520 GOSUB 11000: GOSUB 12430
12530 GOSUB 13030: X=RND(X): GOSUB 13040: GOTO 12900
12540 IF T<>-101 THEN 12560: REM "the SIZE constant"
12550 X=42: GOSUB 13040: GOTO 12900
12560 IF T<>"@" THEN 12600: REM "the array"
12570 GOSUB 11000: GOSUB 12430
12580 GOSUB 13030: IF X<1 OR X>100 THEN PRINT "ARRAY ERROR": E=-1: GOTO 12900
12590 X=AR(X): GOSUB 13050
12600 REM "this is the place where one could add more functions and constants"
12900 REM "The debug and error hook of factor"
12910 IF DE<>2 THEN RETURN
12920 PRINT "** after factor, token", T, "stack is ": GOSUB 13060
12922 PRINT "**   cursor position is", I
12990 RETURN
13000 REM "*********** helpers of the parser and the interpreter ***************"
13020 REM "pop data from the stack"
13030 IF SI>0 THEN X=ST(SI): SI=SI-1: GOTO 13060
13035 PRINT "STACK ERROR": END
13040 REM "push data on the stack"
13050 IF SI<SS THEN SI=SI+1: ST(SI)=X: GOTO 13060
13055 PRINT "STACK ERROR": END
13060 REM "Stack debugging"
13065 IF NOT DE RETURN
13070 PRINT "Stack pointer =", SI : IF SI>0 THEN FOR K=1 TO SI: PRINT K, ST(K): NEXT
13080 RETURN
13100 REM "Find a line in line storage"
13110 F=0
13120 FOR K=1 TO NL
13140 IF LN(K)=X THEN F=K: GOTO 13160
13150 NEXT 
13160 RETURN
13200 REM "insert a line to the line storage, X is the line number"
13210 GOSUB 13100 
13220 IF F<>0 THEN 13400
13230 F=1
13240 IF LN(F)<X AND LN(F)<>0 AND F<NL THEN F=F+1: GOTO 13240
13250 FOR K=F TO NL
13260 IF LN(K)=0 THEN 13300
13270 NEXT
13300 IF F=K THEN 13400
13310 FOR J=K TO F+1 STEP -1
13330 LN(J)=LN(J-1): LS$()(J)=LS$()(J-1)
13340 NEXT
13400 LS$()(F)=MID$(A$, I): LN(F)=X
13420 RETURN
13500 REM "delete one line"
13510 GOSUB 13100 
13520 IF F=0 THEN PRINT "Unknown line": RETURN
13530 FOR K=F TO NL-1: LN(K)=LN(K+1): LS$()(K)=LS$()(K+1): NEXT
13540 LN(NL)=0: LS$()(NL)=""
13540 RETURN
13600 PRINT "** termsymbol": REM "Find a terminal symbol"
13610 GOSUB 11000: IF T<>0 AND T<>ASC(":") THEN 13610
13620 RETURN
13700 REM "check if FOR already exists, purge the FOR stack then"
13710 IF FS=1 THEN RETURN
13720 FOR F=FS-1 TO 1 STEP -1
13730 IF FV(FS)=FV(F) THEN 13760
13740 NEXT
13750 RETURN
13760 FD(F)=FD(FS): FE(F)=FE(FS): FH(F)=FH(FS): FI(F)=FI(FS)
13770 FOR K=FM TO FS+1: FV(K)=0: NEXT : REM "delete all inner loops"
13780 RETURN
14000 REM "*********** statements are parsed here ************************"
14010 REM
14015 REM "******** PRINT *********"
14020 IF T<>-118 THEN 14200
14030 GOSUB 11000: IF E<>0 THEN RETURN
14040 IF T=-125 THEN PRINT C$;: GOSUB 11000: GOTO 14070
14050 IF FNTE(T) THEN GOSUB 11000: GOTO 14080
14060 GOSUB 12000: GOSUB 13030: PRINT X;
14070 IF T=ASC(",") OR T=ASC(";") THEN 14030
14080 PRINT: RETURN 
14195 REM "********* LET **********"
14200 IF T<>-117 AND T<>ASC("@") AND T<>-124 THEN 14400
14210 IF T=-117 THEN GOSUB 11000: IF T<>ASC("@") AND T<>-124 THEN E=-1: RETURN
14220 T0=T: C0=C : REM "Remember if we deal with a variable or an array as left hand side"
14230 GOSUB 11000: IF T0<>ASC("@") THEN 14240
14235 IF T<>ASC("(") THEN PRINT "ERROR": E=-1: RETURN
14237 GOSUB 11000: GOSUB 12000: GOSUB 13030: X0=X: IF T<>ASC(")") THEN E=-1: RETURN 
14238 GOSUB 11000
14240 IF E OR T<>ASC("=") THEN E=-1: RETURN
14250 GOSUB 11000: GOSUB 12000: GOSUB 13030
14260 IF T0=-124 THEN VA(C0-64)=X: GOTO 14290
14270 IF X0<1 OR X0>AS THEN E=-1: RETURN
14280 AR(X0)=X
14290 GOSUB 11000: RETURN
14395 REM "********* INPUT ********"
14400 IF T<>-116 THEN 14600
14410 GOSUB 11000
14420 IF T=-125 THEN PRINT C$; : GOSUB 11000
14430 IF T=-124 THEN INPUT X: VA(C-64)=X: GOSUB 11000
14440 IF T=ASC(",") THEN 14410
14450 IF FNTE(T) THEN RETURN
14460 E=-1: RETURN
14595 REM "********* GOTO *********"
14600 IF T<>-115 THEN 14800
14610 GOSUB 11000: GOSUB 12000: IF E<>0 THEN RETURN
14620 GOSUB 13030: GOSUB 13100: REM "Try to find the line"
14630 IF F=0 THEN PRINT "Line not found in ", HE: E=-1: RETURN
14640 HE=F: I=1: A$=LS$()(HE): GOSUB 11000: RETURN
14795 REM "********* GOSUB *********"
14800 IF T<>-114 THEN 15000
14810 GOSUB 11000: GOSUB 12000: IF E<>0 THEN RETURN
14820 GOSUB 13030: GOSUB 13100: REM "Try to find the line"
14830 IF F=0 THEN PRINT "Line not found in ", HE: E=-1: RETURN
14840 IF GS=GD THEN PRINT "Gosub error": E=-1: RETURN
14850 GS=GS+1: GH(GS)=HE: GI(GS)=I: I=1: HE=F: A$=LS$()(HE): GOSUB 11000
14860 RETURN 
14995 REM "********* RETURN *********"
15000 IF T<>-113 THEN 15200
15010 IF GS=0 THEN PRINT "Return error": E=-1: RETURN
15015 PRINT GS
15020 HE=GH(GS): I=GI(GS): GS=GS-1: A$=LS$()(HE): GOSUB 11000
15030 RETURN
15195 REM "********* IF *********"
15200 IF T<>-112 THEN 15400
15210 GOSUB 11000: GOSUB 12000: IF E<>0 THEN RETURN
15220 GOSUB 13030 
15230 IF NOT X THEN T=0: I=LEN(A$): REM "if goes to the end of line"
15260 RETURN
15395 REM "********* FOR, TO, STEP *********"
15400 IF T<>-111 THEN 15800
15410 IF FS=FM THEN PRINT "For error": E=-1: RETURN
15420 FS=FS+1
15430 GOSUB 11000
15440 IF T<>-124 THEN E=-1: RETURN
15450 FV(FS)=C: CF=C: GOSUB 11000
15460 IF T<>ASC("=") THEN E=-1: RETURN
15470 GOSUB 11000: GOSUB 12000: IF E<>0 THEN RETURN
15480 GOSUB 13030: VA(CF-64)=X
15490 IF T<>-110 THEN E=-1: RETURN
15500 GOSUB 11000: GOSUB 12000: IF E<>0 THEN RETURN
15510 GOSUB 13030: FE(FS)=X
15520 X=1
15530 IF T<>-109 THEN 15550
15540 GOSUB 11000: GOSUB 12000: IF E<>0 THEN RETURN
15545 GOSUB 13030
15550 FD(FS)=X
15560 IF DE=4 THEN PRINT "** for complete", FV(FS), FD(FS), FE(FS)
15595 REM "All data for the FOR is complete here"
15600 GOSUB 13700 : REM "Order FOR stack correctly!!"
15610 REM "Condition fullfilled, skip to NEXT"
15620 IF FD(FS)>0 AND FE(FS)<VA(CF-64) THEN FS=FS-1: GOTO 15700
15630 IF FD(FS)<0 AND FE(FS)>VA(CF-64) THEN FS=FS-1: GOTO 15700
15640 REM "Remember this location"
15650 FH(FS)=HE: FI(FS)=I: IF DE=4 THEN PRINT "** for location", FI(FS), FH(FS)
15660 RETURN
15700 REM "Find the next"
15710 GOSUB 11000: IF T<>-108 AND T<>0 GOTO 15710
15720 IF T=-108 THEN GOSUB 11000: RETURN
15730 HE=HE+1: IF LN(HE)<>0 THEN A$=LS$()(HE): I=1: GOTO 15710
15740 PRINT "Next error": E=-1
15750 RETURN
15795 REM "********* NEXT *********"
15800 IF T<>-108 THEN 16000
15810 IF FS=0 THEN PRINT "Next error": E=-1: RETURN
15820 CF=FV(FS): VA(CF-64)=VA(CF-64)+FD(FS)
15825 IF DE=4 THEN PRINT "** next executed", FV(FS), FD(FS), FE(FS), FI(FS), FH(FS)
15830 IF FD(FS)>0 AND FE(FS)<VA(CF-64) THEN 15870
15840 IF FD(FS)<0 AND FE(FS)>VA(CF-64) THEN 15870
15850 I=FI(FS): HE=FH(FS): IF HE>0 THEN A$=LS$()(HE)
15860 GOSUB 11000: RETURN
15870 FV(FS)=0: FS=FS-1: GOSUB 11000: REM "Clean up the variable field!"
15880 RETURN 
15995 REM "********* STOP ********* (handled in loop)"
16000 IF T<>-107 THEN 16200
16010 RETURN
16195 REM "********* LIST *********"
16200 IF T<>-106 THEN 16400
16210 GOSUB 11000
16220 FOR K=1 TO NL
16230 IF LN(K)<>0 THEN PRINT LN(K); LS$()(K)
16240 NEXT
16250 RETURN
16395 REM "********* NEW *********"
16400 IF T<>-105 THEN 16600
16410 GOSUB 11000
16420 FOR K=1 TO NL: LN(K)=0: LS$()(K)="": NEXT
16430 RETURN
16595 REM "********* RUN ********* (handled in loop)"
16600 IF T<>-104 THEN 16800
16610 GOSUB 11000: RETURN
16795 REM "********* REM *********"
16800 IF T<>-100 THEN 17000
16810 T=0: I=LEN(A$): RETURN 
16695 REM "******** LOAD *********"
17000 IF T<>-99 THEN 17200
17010 GOSUB 11000: PRINT "not implemented": RETURN
17195 REM "******** SAVE *********"
17200 IF T<>-98 THEN 17400
17210 GOSUB 11000: PRINT "not implemented": RETURN
17395 REM "********* : ***********"
17400 IF T<>":" THEN 17500 
17410 GOSUB 11000: RETURN
17500 IF T=0 THEN RETURN
17510 REM "******** Syntax Error *********"
17520 PRINT "Syntax Error": E=-1: RETURN
19995 RETURN
30000 REM "********* the keyword storage with the token values *************"
30010 DATA 24
30020 DATA ">=",-121,"<=",-120,"<>",-119
30030 DATA "print",-118,"let",-117,"input",-116,"goto",-115
30040 DATA "gosub",-114, "return", -113, "if",-112
30050 DATA "for", -111, "to", -110, "step", -109, "next", -108
30060 DATA "stop", -107, "list", -106, "new", -105, "run", -104
30070 DATA "abs", -103, "rnd", -102, "size", -101, "rem", -100
30080 DATA "load", -99, "save", -98

