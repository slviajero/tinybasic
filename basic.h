/*
 *
 *	$Id: basic.h,v 1.13 2023/07/16 14:17:08 stefan Exp stefan $
 *
 *	Stefan's basic interpreter 
 *
 *	Playing around with frugal programming. See the licence file on 
 *	https://github.com/slviajero/tinybasic for copyright/left.
 *    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 *	Author: Stefan Lenz, sl001@serverfabrik.de
 *
 *	basic.h are the core defintions and function protypes
 *
 */

/*
 *	if the PROGMEM macro is define we compile on the Arduino IDE
 *	we undef all hardware settings otherwise a little odd
 */
#ifdef ARDUINO_ARCH_MBED
#define PROGMEM
#endif

#ifdef PROGMEM
#define ARDUINOPROGMEM
#else
#undef ARDUINO
#undef ARDUINOSD
#undef ARDUINORF24
#undef ARDUINORTC
#undef ARDUINOEEPROM
#undef ARDUINOEEPROMI2C
#undef ARDUINOWIRE
#endif

/* 
 *	MSDOS, Mac, Linux and Windows 
 */
#ifndef ARDUINO
typedef unsigned char uint8_t;
#define PROGMEM
#include <stdio.h>
#include <stdlib.h>
#ifdef HASFLOAT
#include <math.h>
#include <float.h>
#endif
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>
#ifndef MSDOS
#include <dirent.h>
#include <unistd.h>
#else
#include <dir.h>
#include <dos.h>
#endif
#ifdef MINGW
#include <windows.h>
#endif
#endif


#if defined(ARDUINO_ARCH_AVR)
/* the small memory model with shallow stacks and small buffers */
#define BUFSIZE 		80
#define STACKSIZE		15
#define GOSUBDEPTH	4
#define FORDEPTH		4
#define LINECACHESIZE	4
#else 
/* the for larger microcontrollers */
#ifdef ARDUINO
#define BUFSIZE 		128
#define STACKSIZE		64
#define GOSUBDEPTH		8
#define FORDEPTH		8
#define LINECACHESIZE	16
#else 
/* for real computers */
#define BUFSIZE         256
#define STACKSIZE       256
#define GOSUBDEPTH      64
#define FORDEPTH        64
#define LINECACHESIZE   64
#endif
#endif

/* on the real small systems we remove the linecache and set a fixed memory size*/
#ifdef ARDUINO_AVR_DUEMILANOVE
#undef LINECACHESIZE
#if MEMSIZE == 0
#define MEMSIZE 512
#endif
#endif

/* more duffers and vars */
#define SBUFSIZE	32
#define VARSIZE		26
/* default sizes of arrays and strings if they are not DIMed */
#define ARRAYSIZEDEF    10
#define STRSIZEDEF  32

/*
 *	the time intervall in ms needed for 
 *	ESP8266 yields, network client loops 
 *	and other timing related functions 
 */
#define LONGYIELDINTERVAL 1000
#define YIELDINTERVAL 32
#define YIELDTIME 2

/* the default EEPROM dummy size */
#define EEPROMSIZE 1024

/* after run behaviour on POSIX systems, 1 to terminate if started
    on the command line with a file argument, 0 to stay active and 
    show a BASIC prompt*/
#define TERMINATEAFTERRUN 1

/*
 * The tokens for the BASIC keywords
 *
 *	All single character operators are their own tokens
 *	ASCII values above 0x7f are used for tokens of keywords.
 *	EOL is a token
 */
#define EOL			 0
#define NUMBER   	 -127
#define LINENUMBER   -126
#define STRING   	 -125
#define VARIABLE 	 -124
#define STRINGVAR 	 -123
#define ARRAYVAR     -122
/* multi character tokens - BASEKEYWORD (3) */
#define GREATEREQUAL -121
#define LESSEREQUAL  -120
#define NOTEQUAL	 -119
/* this is the Palo Alto Language Set (19) */
#define TPRINT  -118
#define TLET    -117
#define TINPUT  -116
#define TGOTO   -115
#define TGOSUB  -114
#define TRETURN -113
#define TIF     -112
#define TFOR    -111
#define TTO     -110
#define TSTEP   -109
#define TNEXT   -108
#define TSTOP   -107
#define TLIST	-106
#define TNEW    -105
#define TRUN    -104
#define TABS 	-103
#define TRND	-102
#define TSIZE   -101
#define TREM 	-100
/* this is the Apple 1 language set in addition to Palo Alto (14) */
#define TNOT    -99
#define TAND	-98
#define TOR 	-97
#define TLEN	-96
#define TSGN	-95	
#define TPEEK	-94
#define TDIM	-93
#define TCLR	-92
#define THIMEM  -91
#define TTAB 	-90
#define TTHEN   -89
#define TEND    -88
#define TPOKE	-87
/* Stefan's tinybasic additions (14) */
#define TCONT   -86
#define TSQR	-85
#define TPOW	-84
#define TMAP	-83
#define TDUMP 	-82
#define TBREAK  -81
#define TSAVE   -80
#define TLOAD   -79
#define TGET    -78
#define TPUT    -77
#define TSET    -76
#define TCLS    -75
#define TLOCATE -74
#define TELSE   -73
/* Arduino functions (10) */
#define TPINM	-72
#define TDWRITE	-71
#define TDREAD	-70
#define TAWRITE	-69
#define TAREAD	-68
#define TDELAY	-67
#define TMILLIS	-66
#define TTONE	-65
#define TPULSE	-64
#define TAZERO	-63
#define TLED	-62
/* the DOS functions (5) */
#define TCATALOG    -61
#define TDELETE -60
#define TOPEN 	-59
#define TCLOSE 	-58
#define TFDISK  -57
/* low level access of internal routines (2) */
#define TUSR	-56
#define TCALL 	-55
/* mathematical functions (7) */
#define TSIN 	-54
#define TCOS    -53
#define TTAN 	-52
#define TATAN   -51
#define TLOG    -50
#define TEXP    -49
#define TINT    -48
/* graphics - experimental - rudimentary (7) */
#define TCOLOR 	-47
#define TPLOT   -46
#define TLINE 	-45
#define TCIRCLE -44
#define TRECT   -43
#define TFCIRCLE -42
#define TFRECT   -41
/* the Dartmouth extensions (6) */
#define TDATA	-40
#define TREAD   -39
#define TRESTORE    -38
#define TDEF	-37
#define TFN 	-36
#define TON     -35
/* darkarts (3) */
#define TMALLOC -34
#define TFIND   -33
#define TEVAL   -32
/* iot extensions (9) */
#define TERROR	-31
#define TAVAIL	-30
#define TSTR	-29
#define TINSTR	-28
#define TVAL	-27
#define TNETSTAT    -26
#define TSENSOR	-25
#define TWIRE	-24
#define TSLEEP	-23
/* events and interrupts */
#define TAFTER -22
#define TEVERY -21
#define TEVENT -20
/* experimental structured commands, currently partially implemented */
#define TWHILE -19
#define TWEND   -18
#define TREPEAT -17
#define TUNTIL -16
#define TSWITCH -15
#define TCASE -14
#define TSWEND -13
#define TDO -12
#define TDEND -11
/* these are multibyte token extension, currently unused */
/* using them would allow over 1000 BASIC keywords */
#define TEXT1 -3
/* end of tokens */
/* constants used for some obscure purposes */
#define TBUFFER -2
/* UNKNOWN is not used in the current code, the 
 * lexer tokenizes everything blindly. There is a UNKNOWN hook 
 * in statement for a grammar aware lexer */
#define UNKNOWN -1

/* extension tokens can be in the range from -128 to -255 
 * one needs to set HASLONGTOKENS
 */
#undef HASLONGTOKEN
#define TTOKEN1 -128


/* the number of keywords, and the base index of the keywords 
 * the number is irrelevant but BASEKEYWORD is used */
#define NKEYWORDS	3+19+13+14+11+5+2+7+7+6+12+3+9
#define BASEKEYWORD -121

/*
 *	Interpreter states 
 *	SRUN means running from a programm
 *	SINT means interactive mode
 *	SERUN means running directly from EEPROM
 *		(enum would be the right way of doing this.)
 *	BREAKCHAR is the character stopping the program on Ardunios
 *  BREAKPIN can be set, it is a pin that needs to go to low to stop a BASIC program
 *    This should be done in hardware*.h
 *  BREAKSIGNAL can also be set, should be done in hardware*.h
 */
#define SINT 0
#define SRUN 1
#define SERUN 2
#define BREAKCHAR '#'

/* 
 *	Input and output channels
 */
#define OSERIAL 1
#define ODSP 2
#define OPRT 4
#define OWIRE 7
#define ORADIO 8
#define OMQTT  9
#define OFILE 16

#define ISERIAL 1
#define IKEYBOARD 2
#define ISERIAL1 4
#define IWIRE 7
#define IRADIO 8
#define IMQTT  9
#define IFILE 16

/*
 *	All BASIC keywords for the tokens
 */
const char sge[]   PROGMEM = "=>";
const char sle[]   PROGMEM = "<=";
const char sne[]   PROGMEM = "<>";
/* Palo Alto language set */
const char sprint[]  PROGMEM = "PRINT";
const char slet[]    PROGMEM = "LET";
const char sinput[]  PROGMEM = "INPUT";
const char sgoto[]   PROGMEM = "GOTO";
const char sgosub[]  PROGMEM = "GOSUB";
const char sreturn[] PROGMEM = "RETURN";
const char sif[]     PROGMEM = "IF";
const char sfor[]    PROGMEM = "FOR";
const char sto[]     PROGMEM = "TO";
const char sstep[]   PROGMEM = "STEP";
const char snext[]   PROGMEM = "NEXT";
const char sstop[]   PROGMEM = "STOP";
const char slist[]   PROGMEM = "LIST";
const char snew[]    PROGMEM = "NEW";
const char srun[]  	 PROGMEM = "RUN";
const char sabs[]    PROGMEM = "ABS";
const char srnd[]    PROGMEM = "RND";
const char ssize[]   PROGMEM = "SIZE";
const char srem[]    PROGMEM = "REM";
/* Apple 1 language set */
#ifdef HASAPPLE1
const char snot[]    PROGMEM = "NOT";
const char sand[]    PROGMEM = "AND";
const char sor[]     PROGMEM = "OR";
const char slen[]    PROGMEM = "LEN";
const char ssgn[]    PROGMEM = "SGN";
const char speek[]   PROGMEM = "PEEK";
const char sdim[]    PROGMEM = "DIM";
const char sclr[]    PROGMEM = "CLR";
const char shimem[]  PROGMEM = "HIMEM";
const char stab[]    PROGMEM = "TAB";
const char sthen[]   PROGMEM = "THEN";
const char sbend[]   PROGMEM = "END";
const char spoke[]   PROGMEM = "POKE";
#endif
/* Stefan's basic additions */
#ifdef HASSTEFANSEXT
const char scont[]   PROGMEM = "CONT";
const char ssqr[]    PROGMEM = "SQR";
const char spow[]    PROGMEM = "POW";
const char smap[]    PROGMEM = "MAP";
const char sdump[]   PROGMEM = "DUMP";
const char sbreak[]  PROGMEM = "BREAK";
#endif
/* LOAD and SAVE is always there */
const char ssave[]   PROGMEM = "SAVE";
const char sload[]   PROGMEM = "LOAD";
#ifdef HASSTEFANSEXT
const char sget[]    PROGMEM = "GET";
const char sput[]    PROGMEM = "PUT";
const char sset[]    PROGMEM = "SET";
const char scls[]    PROGMEM = "CLS";
const char slocate[]  PROGMEM  = "LOCATE";
const char selse[]  PROGMEM  = "ELSE";
#endif
/* Arduino functions */
#ifdef HASARDUINOIO
const char spinm[]    PROGMEM = "PINM";
const char sdwrite[]  PROGMEM = "DWRITE";
const char sdread[]   PROGMEM = "DREAD";
const char sawrite[]  PROGMEM = "AWRITE";
const char saread[]   PROGMEM = "AREAD";
const char sdelay[]   PROGMEM = "DELAY";
const char smillis[]	PROGMEM = "MILLIS";
const char sazero[]	PROGMEM = "AZERO";
const char sled[]	PROGMEM = "LED";
#endif
#ifdef HASTONE
const char stone[]    PROGMEM = "PLAY";
#endif
#ifdef HASPULSE
const char spulse[] PROGMEM = "PULSE";
#endif
/* DOS functions */
#ifdef HASFILEIO
const char scatalog[] PROGMEM = "CATALOG";
const char sdelete[]  PROGMEM = "DELETE";
const char sfopen[]   PROGMEM = "OPEN";
const char sfclose[]  PROGMEM = "CLOSE";
const char sfdisk[]   PROGMEM = "FDISK";
#endif
/* low level access functions */
#ifdef HASSTEFANSEXT
const char susr[]   PROGMEM = "USR";
const char scall[]  PROGMEM = "CALL";
#endif
/* mathematics */
#ifdef HASFLOAT
const char ssin[]   PROGMEM = "SIN";
const char scos[]   PROGMEM = "COS";
const char stan[]   PROGMEM = "TAN";
const char satan[]  PROGMEM = "ATAN";
const char slog[]   PROGMEM = "LOG";
const char sexp[]   PROGMEM = "EXP";
#endif
/* INT is always needed to make float/int programs compatible */
const char sint[]   PROGMEM = "INT"; 
/* elemetars graphics */
#ifdef HASGRAPH
const char scolor[]     PROGMEM  = "COLOR";
const char splot[]      PROGMEM  = "PLOT";
const char sline[]      PROGMEM  = "LINE";
const char scircle[]    PROGMEM  = "CIRCLE";
const char srect[]      PROGMEM  = "RECT";
const char sfcircle[]   PROGMEM  = "FCIRCLE";
const char sfrect[]     PROGMEM  = "FRECT";
#endif
/* Dartmouth BASIC extensions */
#ifdef HASDARTMOUTH
const char sdata[]      PROGMEM  = "DATA";
const char sread[]      PROGMEM  = "READ";
const char srestore[]   PROGMEM  = "RESTORE";
const char sdef[]       PROGMEM  = "DEF";
const char sfn[]        PROGMEM  = "FN";
const char son[]        PROGMEM  = "ON";
#endif
/* The Darkarts commands unthinkable in Dartmouth */
#ifdef HASDARKARTS
const char smalloc[]	PROGMEM  = "MALLOC";
const char sfind[]		PROGMEM  = "FIND";
const char seval[]		PROGMEM  = "EVAL";
#endif
/* complex error handling */
#ifdef HASERRORHANDLING
const char serror[]     PROGMEM  = "ERROR";
#endif
/* iot extensions */
#ifdef HASIOT
const char savail[]		PROGMEM  = "AVAIL";
const char sstr[]		PROGMEM  = "STR";
const char sinstr[]		PROGMEM  = "INSTR";
const char sval[]		PROGMEM  = "VAL";
const char snetstat[]	PROGMEM  = "NETSTAT";
const char ssensor[]	PROGMEM  = "SENSOR";
const char swire[]		PROGMEM  = "WIRE";
const char ssleep[]		PROGMEM  = "SLEEP";
#endif
/* events and interrupts */
#ifdef HASTIMER
const char safter[]     PROGMEM  = "AFTER";
const char severy[]     PROGMEM  = "EVERY";
#endif
#ifdef HASEVENTS
const char sevent[]     PROGMEM  = "EVENT";
#endif
#ifdef HASSTRUCT
const char swhile[]		PROGMEM	= "WHILE";
const char swend[]      PROGMEM = "WEND";
const char srepeat[]	PROGMEM	= "REPEAT";
const char suntil[]     PROGMEM	= "UNTIL";
const char sswitch[]	PROGMEM	= "SWITCH";
const char scase[]		PROGMEM	= "CASE";
const char sswend[]     PROGMEM = "SWEND";
const char sdo[]        PROGMEM = "DO";
const char sdend[]      PROGMEM = "DEND";
#endif


/* zero terminated keyword storage */
const char* const keyword[] PROGMEM = {
	sge, sle, sne, sprint, slet, sinput, 
	sgoto, sgosub, sreturn, sif, sfor, sto,
	sstep, snext, sstop, slist, snew, srun,
	sabs, srnd, ssize, srem,
#ifdef HASAPPLE1
	snot, sand, sor, slen, ssgn, speek, sdim,
	sclr, shimem, stab, sthen, 
	sbend, spoke,
#endif
#ifdef HASSTEFANSEXT
	scont, ssqr, spow, smap, sdump, sbreak, 
#endif
	ssave, sload, 
#ifdef HASSTEFANSEXT
	sget, sput, sset, scls, slocate, selse,
#endif
#ifdef HASARDUINOIO
    spinm, sdwrite, sdread, sawrite, saread, 
    sdelay, smillis, sazero, sled,
#endif
#ifdef HASTONE
	stone,
#endif
#ifdef HASPULSE
	spulse,
#endif
#ifdef HASFILEIO
    scatalog, sdelete, sfopen, sfclose, sfdisk,
#endif
#ifdef HASSTEFANSEXT
    susr, scall,
#endif
#ifdef HASFLOAT
    ssin, scos, stan, satan, slog, sexp,
#endif
    sint,
#ifdef HASGRAPH
    scolor, splot, sline, scircle, srect, 
    sfcircle, sfrect,
#endif
#ifdef HASDARTMOUTH
	sdata, sread, srestore, sdef, sfn, son,
#endif
#ifdef HASDARKARTS
	smalloc, sfind, seval, 
#endif
/* complex error handling */
#ifdef HASERRORHANDLING
    serror,
#endif
#ifdef HASIOT
	savail, sstr, sinstr, sval, 
	snetstat, ssensor, swire, ssleep, 
#endif
#ifdef HASTIMER
    safter, severy,
#endif
#ifdef HASEVENTS
    sevent,
#endif
#ifdef HASSTRUCT
	swhile, swend, srepeat, suntil, sswitch, scase, sswend,	
    sdo, sdend, 
#endif 
	0
};

/* the zero terminated token dictonary needed for scalability */
const signed char tokens[] PROGMEM = {
	GREATEREQUAL, LESSEREQUAL, NOTEQUAL, TPRINT, TLET,    
    TINPUT, TGOTO, TGOSUB, TRETURN, TIF, TFOR, TTO, TSTEP,
    TNEXT, TSTOP, TLIST, TNEW, TRUN, TABS, TRND, TSIZE, TREM,
#ifdef HASAPPLE1
    TNOT, TAND, TOR, TLEN, TSGN, TPEEK, TDIM, TCLR,
    THIMEM, TTAB, TTHEN, TEND, TPOKE,
#endif
#ifdef HASSTEFANSEXT
	TCONT, TSQR, TPOW, TMAP, TDUMP, TBREAK, 
#endif
	TSAVE, TLOAD, 
#ifdef HASSTEFANSEXT	
	TGET, TPUT, TSET, TCLS, TLOCATE, TELSE,
#endif
#ifdef HASARDUINOIO
	TPINM, TDWRITE, TDREAD, TAWRITE, TAREAD, TDELAY, TMILLIS,
	TAZERO, TLED,
#endif
#ifdef HASTONE
	TTONE, 
#endif
#ifdef HASPULSE
	TPULSE, 
#endif
#ifdef HASFILEIO
	TCATALOG, TDELETE, TOPEN, TCLOSE, TFDISK,
#endif
#ifdef HASSTEFANSEXT
	TUSR, TCALL,
#endif
#ifdef HASFLOAT
	TSIN, TCOS, TTAN, TATAN, TLOG, TEXP,
#endif
	TINT,
#ifdef HASGRAPH
	TCOLOR, TPLOT, TLINE, TCIRCLE, TRECT, 
	TFCIRCLE, TFRECT,
#endif
#ifdef HASDARTMOUTH
	TDATA, TREAD, TRESTORE, TDEF, TFN, TON, 
#endif
#ifdef HASDARKARTS
	TMALLOC, TFIND, TEVAL, 
#endif
#ifdef HASERRORHANDLING
    TERROR, 
#endif
#ifdef HASIOT
	TAVAIL, TSTR, TINSTR, TVAL, TNETSTAT,
	TSENSOR, TWIRE, TSLEEP,
#endif
#ifdef HASTIMER
    TAFTER, TEVERY,
#endif
#ifdef HASEVENTS
    TEVENT, 
#endif
#ifdef HASSTRUCT
	TWHILE, TWEND, TREPEAT, TUNTIL, TSWITCH, TCASE, TSWEND,
    TDO, TDEND, 
#endif
	0
};

/*
 *	the message catalog
 */
#define MFILE		0
#define MPROMPT		1
#define MGREET		2
#define MLINE		3
#define MNUMBER		4
#define MVARIABLE	5
#define MARRAY		6
#define MSTRING		7
#define MSTRINGVAR	8
#define EGENERAL 	 9
#define EUNKNOWN	 10
#define ENUMBER      11
#define EDIVIDE		 12
#define ELINE        13
#define EOUTOFMEMORY 14
#define ESTACK 		 15
#define EORANGE 	 16
#define ESTRING      17
#define EVARIABLE	 18
#define ELOOP        19
#define EFILE 		 20
#define EFUN 		 21
#define EARGS       22
#define EEEPROM	    23
#define ESDCARD     24

const char mfile[]    	PROGMEM = "file.bas";
const char mprompt[]	PROGMEM = "> ";
const char mgreet[]		PROGMEM = "Stefan's Basic 1.4";
const char mline[]		PROGMEM = "LINE";
const char mnumber[]	PROGMEM = "NUMBER";
const char mvariable[]	PROGMEM = "VARIABLE";
const char marray[]		PROGMEM = "ARRAY";
const char mstring[]	PROGMEM = "STRING";
const char mstringv[]	PROGMEM = "STRINGVAR";
const char egeneral[]  	PROGMEM = "Error";
#ifdef HASERRORMSG
const char eunknown[]  	PROGMEM = "Syntax";
const char enumber[]	PROGMEM = "Number";
const char edivide[]  	PROGMEM = "Div by 0";
const char eline[]  	PROGMEM = "Unknown Line";
const char emem[]  	   	PROGMEM = "Memory";
const char estack[]    	PROGMEM = "Stack";
const char erange[]  	PROGMEM = "Range";
const char estring[]	PROGMEM = "String";
const char evariable[]  PROGMEM = "Variable";
const char eloop[]      PROGMEM = "Loop";
const char efile[]  	PROGMEM = "File";
const char efun[] 	 	PROGMEM = "Function";
const char eargs[]  	PROGMEM = "Args";
const char eeeprom[]	PROGMEM = "EEPROM";
const char esdcard[]	PROGMEM = "SD card";
#endif

const char* const message[] PROGMEM = {
	mfile, mprompt, mgreet, 
	mline, mnumber, mvariable, marray, 
	mstring, mstringv,
	egeneral
#ifdef HASERRORMSG
	, eunknown, enumber, edivide, eline, 
	emem, estack, erange,
	estring, evariable, eloop, efile, efun, eargs, 
	eeeprom, esdcard
#endif
};

/*
 *	code for variable numbers and addresses sizes
 *	the original code was 16 bit but can be extended here
 * 	to arbitrary types 
 *
 *	number_t is the type for numerical work - either float or int
 *  wnumber_t is the type containing the largest printable integer, 
 *    for float keep this int on 32 bit and long on 8 bit unless you 
 *    want to use very long integers, like 64 or 128 bit types. 
 *  address_t is an unsigned type adddressing memory, default 16bit 
 *  mem_t is a SIGNED 8bit character type.
 *	index_t is a SIGNED minimum 16 bit integer type
 *
 *	works with the tacit assumption that 
 *	sizeof(number_t) >= sizeof(address_t) 
 *	and that the entire memory is smaller than the positive
 * 	part of number type (!!)
 *
 *	we assume that float >= 4 bytes in the following
 *
 *	maxnum: the maximum accurate(!) integer of a 
 *		32 bit float 
 *	strindexsize: the index size of strings either 
 *		1 byte or 2 bytes - no other values supported
 */
#ifdef HASFLOAT
typedef float number_t;
const number_t maxnum=16777216; 
typedef long wnumber_t;
#else
typedef int number_t;
typedef int wnumber_t;
const number_t maxnum=(number_t)~((number_t)1<<(sizeof(number_t)*8-1));
#endif
typedef unsigned short address_t; /* this type addresses memory */
const int numsize=sizeof(number_t);
const int addrsize=sizeof(address_t);
const int eheadersize=sizeof(address_t)+1;
const int strindexsize=2; /* default in the meantime, strings up to unsigned 16 bit length */
const address_t maxaddr=(address_t)(~0); 
typedef signed char mem_t; /* a signed 8 bit type for the memory */
typedef int index_t; /* this type counts at least 16 bit */
#ifndef HASLONGTOKENS
typedef signed char token_t; /* the type of tokens, normally mem_t with a maximum of 127 commands and data types */
#else
typedef short token_t; /* token type extension, allows an extra of 127 commands and symbols */
#endif

/* this type maps numbers to bytes */
typedef struct {mem_t l; mem_t h;} twobytes_t;
typedef union { number_t i; address_t a; twobytes_t b; mem_t c[sizeof(number_t)]; } accu_t;

/* the memreader function type */
typedef mem_t (*memreader_t)(address_t);

/* 
 * system type identifiers
 */

#define SYSTYPE_UNKNOWN	0
#define SYSTYPE_AVR 	1
#define SYSTYPE_ESP8266 2
#define SYSTYPE_ESP32	3
#define SYSTYPE_RP2040  4
#define SYSTYPE_SAM     5
#define SYSTYPE_XMC		6
#define SYSTYPE_SMT32	7
#define SYSTYPE_POSIX	32
#define SYSTYPE_MSDOS	33
#define SYSTYPE_MINGW   34
#define SYSTYPE_RASPPI  35

/*
 *	The basic interpreter is implemented as a stack machine
 *	with global variable for the interpreter state, the memory
 *	and the arithmetic during run time.
 */

/* the stack, all BASIC arithmetic is done here */
number_t stack[STACKSIZE];
address_t sp=0; 

/* a small buffer to process string arguments, mostly used for Arduino PROGMEM */
char sbuffer[SBUFSIZE];

/* the input buffer, the lexer can tokenize this and run from it, bi is an index to this.
   bi must be global as it is the program cursor in interactive mode */
char ibuffer[BUFSIZE] = "\0";
char *bi;

/* a static array of variables A-Z for the small systems that have no heap */
number_t vars[VARSIZE];

/* the BASIC working memory, either malloced or allocated as a global array */
#if MEMSIZE != 0
mem_t mem[MEMSIZE];
#else
mem_t* mem;
#endif
address_t himem, memsize;

/* the for stack - remembers the variable, indices, and optionally a type for stuctured BASIC */
struct forstackitem {mem_t varx; mem_t vary; address_t here; number_t to; number_t step; 
#ifdef HASSTRUCT
mem_t type;
#endif
} forstack[FORDEPTH];
index_t forsp = 0;
 
/* the GOSUB stack remembers an address to jump to */
address_t gosubstack[GOSUBDEPTH];
index_t gosubsp = 0;

/* this variable stores the location in pushlocation() and poplocation(), used to rewind the program cursor */
address_t slocation;

/* arithmetic accumulators - used by many statements, y may be obsolete in future*/
number_t x, y;

/* the names of a variable and small integer accumulator */
mem_t xc, yc;

/* an address accumulator, used a lot in string operations */
address_t ax;

/* z is another accumulator used to convert numbers and addressed to bytes and vice versa */
/* this union is used to store larger objects into byte oriented memory */
accu_t z;

/* string index registers */
char *ir, *ir2;

/* the active token */
token_t token;

/* the curent error, can be a token, hance token type */
token_t er;

/* a trapable error */
mem_t ert;

/* the interpreter state, interactive, run or run from EEPROM */
mem_t st; 

/* the current program location or "cursor" */
address_t here; 

/* the topmost byte of a program in memory, beginning of free BASIC RAM */
address_t top;

/* the number of variables on the heap */
address_t nvars = 0; 

/* used to format output with # */
mem_t form = 0;

/* counts the outputed characters on streams 0-3, used to emulate a real tab */
#ifdef HASMSTAB
mem_t charcount[3]; /* devices 1-4 support tabing */
mem_t reltab = 0;
#endif

/* the lower limit of the array is one by default, can be a variable */
#ifdef HASARRAYLIMIT
address_t arraylimit = 1;
#else 
const address_t arraylimit = 1;
#endif

/* the number of arguments parsed from a command */
mem_t args;

/* the random number seed, this is unsigned hence address_t */
#ifndef HASFLOAT
address_t rd;
#else 
unsigned long rd;
#endif

/* output and input channels, used to direct output to various devices */
mem_t id;
mem_t od;

/* default IO - not constant, can be changed at runtime through a user call */
mem_t idd = ISERIAL;
mem_t odd = OSERIAL;

/* the runtime debuglevel */
mem_t debuglevel = 0;

/* DATA pointer, where is the current READ statement  */
#ifdef HASDARTMOUTH
address_t data = 0;
address_t datarc = 1;
#endif
    
/* 
 * process command line arguments in the POSIX world 
 * bnointafterrun is a flag to remember if called as command
 * line argument, in this case we don't return to interactive 
 */
#ifndef ARDUINO
int bargc;
char** bargv;
mem_t bnointafterrun = 0;
#endif

/*
 * Yield counter, we count when we did yield the last time
 *	lastyield controlls the client loops of network functions 
 *	like mqtt, scanned keyboard, and USB.
 *
 *	lastlongyield controls longterm functions like DHCP lease 
 *	renewal in Ethernet 
 * 
 * there variables are only needed if the platform has background 
 *  tasks
 */
long lastyield=0;
long lastlongyield=0;

/* formaters lastouttoken and spaceafterkeyword to make a nice LIST */
mem_t lastouttoken;
mem_t spaceafterkeyword;
mem_t outliteral = 0;
mem_t lexliteral = 0; 

/* 
 * the cache for the heap search - helps the string code 
 * the last found object on the heap is remembered. This is needed
 * because the string code sometime searches the heap twice during the 
 * same operation. 
 */
#ifdef HASAPPLE1
mem_t bfindc, bfindd, bfindt;
address_t bfinda, bfindz;
#endif

/*
 * a variable for some string operations 
 */
int vlength;

/* the timer code - very simple needs to to to a struct */
/* timer type */
typedef struct {
    mem_t enabled;
    unsigned long last;
    unsigned long interval; 
    mem_t type;
    address_t linenumber;
} btimer_t;

#ifdef HASTIMER
btimer_t after_timer = {0, 0, 0, 0, 0};
btimer_t every_timer = {0, 0, 0, 0, 0};
#endif

/* the event code */
#ifdef HASEVENTS

#define EVENTLISTSIZE 4

/* event type */
typedef struct {
    mem_t enabled;
    mem_t pin;
    mem_t mode; 
    mem_t type;
    address_t linenumber;
    mem_t active;
} bevent_t;

/* the event list */
int nevents = 0;
int ievent = 0;
static mem_t events_enabled = 1;
static volatile bevent_t eventlist[EVENTLISTSIZE];

/* the extension of the GOSUB stack */
static mem_t gosubarg[GOSUBDEPTH];

/* handle the event list */
mem_t addevent(mem_t, mem_t, mem_t, address_t);
void deleteevent(mem_t);
volatile bevent_t* findevent(mem_t);
mem_t eventindex(mem_t);
#endif

#ifdef HASERRORHANDLING
/* the error handler type, very simple for now */
typedef struct {
    mem_t type;
    address_t linenumber;
} berrorh_t;

berrorh_t berrorh = {0 , 0};
mem_t erh = 0;
#endif

/* the string for real time clocks */
char rtcstring[20] = { 0 }; 

/* the units pulse operates on, in microseconds*/
short bpulseunit = 10; 

/* only needed for POSIXNONBLOCKING */
mem_t breakcondition = 0;

/* 
 * Function prototypes, ordered by layers
 * HAL - hardware abstraction
 * Layer 0 - memory and I/O
 * Layer 1 - Program storage and control
 * Layer 2 - Where stuff happens
 */

/*
 *	HAL - see hardware-*.h
 *  This is the hardware abstraction layer of the BASIC
 *	interpreter 
 */

/* setup codes */
void timeinit();
void wiringbegin();

/* low level mem and hardware features */
long freeRam();
long freememorysize();
void restartsystem();
void activatesleep(long t);

/* start the spi bus */
void spibegin();

/* 
 * the hardware interface display driver functions, need to be 
 * 	implemented for the display driver to work 
 *	dspupdate() only for display like Epapers
 */
void dspbegin();
void dspprintchar(char, mem_t, mem_t);
void dspclear();
void dspupdate();

/* keyboard code */
void kbdbegin();
int kbdstat(char);
char kbdavailable();
char kbdread();
char kbdcheckch();

/* graphics functions */
void rgbcolor(int, int, int);
void vgacolor(short c);
void vgascale(int*, int*);
void plot(int, int);
void line(int, int, int, int);  
void rect(int, int, int, int);
void frect(int, int, int, int);
void circle(int, int, int);
void fcircle(int, int, int);

/* text output to a VGA display */
void vgabegin();
int vgastat(char);
void vgawrite(char);

/* generic display code */
void dspwrite(char);
void dspbegin();
int dspstat(char);
char dspwaitonscroll();
char dspactive();
void dspsetupdatemode(char);
char dspgetupdatemode();
void dspgraphupdate();
void dspsetscrollmode(char, short);
void dspsetcursor(short, short);
void dspbufferclear();
void dspscroll(mem_t, mem_t);
void dspreversescroll(mem_t);
void dspvt52(char *);

/* real time clock */
char* rtcmkstr();
void rtcset(uint8_t, short);
short rtcget(short); 

/* network and mqtt functions */
void netbegin();
char netconnected();
void mqttsetname();
void mqttbegin();
int mqttstat(char);
int  mqttstate();
void mqttsubscribe(char*);
void mqttsettopic(char*);
void mqttouts(char *, short);
void mqttins(char *, short);
char mqttinch();

/* low level EEPROM handling */
void ebegin();
void eflush();
address_t elength();
mem_t eread(address_t);
void eupdate(address_t, mem_t);

/* arduino io functions */
void aread();
void dread();
void awrite(address_t, address_t);
void dwrite(address_t, address_t);
void pinm(address_t, address_t);
void bmillis();
void bpulsein();
void xpulse();
void bpulseout(short);
void btone(short);

/* timing control for ESP and network */
void byield();
void bdelay(unsigned long);
void fastticker();
void yieldfunction();
void longyieldfunction();

/* the file interface */
char* mkfilename(const char*);
const char* rmrootfsprefix(const char*);
void fsbegin(char);
int fsstat(char);
void filewrite(char);
char fileread();
char ifileopen(const char*);
void ifileclose();
char ofileopen(char*, const char*);
void ofileclose();
int fileavailable();
void rootopen();
int rootnextfile();
int rootisfile();
const char* rootfilename();
long rootfilesize();
void rootfileclose();
void rootclose();
void removefile(char*);
void formatdisk(short);

/* low level serial code */
void picogetchar(char);
void picowrite(char);
void picobegin(unsigned long);
void picoins(char, short);
void serialbegin();
int serialstat(char);
char serialread();
void serialwrite(char);
short serialcheckch();
short serialavailable();
void serialflush();
void consins(char*, short);
void prtbegin();
int prtstat(char);
void prtset(int);
char prtopen(char *, int);
void prtclose();
char prtread();
void prtwrite(char);
short prtcheckch();
short prtavailable();

/* generic wire access */
void wirebegin();
int wirestat(char);
void wireopen(char, char);
void wireins(char*, uint8_t);
void wireouts(char*, uint8_t);
short wireavailable();
short wirereadbyte(short);
void  wirewritebyte(short, short);

/* RF24 radio input */
int radiostat(char);
void radioset(int);
#ifdef ARDUINO 
uint64_t pipeaddr(char*);
#else
long pipeaddr(char*);
#endif
void iradioopen(char*);
void oradioopen(char*);
void radioins(char*, short);
void radioouts(char* , short);
short radioavailable();

/* sensor control */
void sensorbegin();
number_t sensorread(short, short);

/* SPI RAM code */
address_t spirambegin();
void spiramrawwrite(address_t, mem_t);
mem_t spiramrawread(address_t );

/*
 * Layer 0 functions - I/O and memory management 
 */

/* make room for BASIC */
address_t ballocmem(); 

/* handle files im EEPROM */
void eload();
void esave();
char autorun();

/* the variable heap from Apple 1 BASIC */
address_t bmalloc(mem_t, mem_t, mem_t, address_t);
address_t bfind(mem_t, mem_t, mem_t);
address_t bfree(mem_t, mem_t, mem_t);
address_t blength (mem_t, mem_t, mem_t);

/* normal variables of number_t */
number_t getvar(mem_t, mem_t);
void setvar(mem_t, mem_t, number_t);
void clrvars();

/*	low level memory access packing n*8bit bit into n 8 bit objects
	e* is for Arduino EEPROM */
void getnumber(address_t, mem_t);
void setnumber(address_t, mem_t);
void egetnumber(address_t, mem_t);
void esetnumber(address_t, mem_t);
void pgetnumber(address_t, mem_t);

/* array and string handling */
/* the multidim extension is experimental, here only 2 array dimensions implemented as test */
address_t createarray(mem_t, mem_t, address_t, address_t);
void array(mem_t, mem_t, mem_t, address_t, address_t, number_t*);
address_t createstring(char, char, address_t, address_t);
char* getstring(char, char, address_t, address_t);
number_t arraydim(char, char);
address_t stringdim(char, char);
address_t lenstring(char, char, address_t);
void setstringlength(char, char, address_t, address_t);
void setstring(char, char, address_t, char *, address_t, address_t);

/* the user defined extension functions */
number_t getusrvar();
void setusrvar(number_t);
number_t getusrarray(address_t);
void setusrarray(address_t, number_t);
void makeusrstring();
number_t usrfunction(address_t, number_t);
void usrcall(address_t);

/* get keywords and tokens from PROGMEM */
char* getkeyword(unsigned short);
char* getmessage(char);
signed char gettokenvalue(char);
void printmessage(char);

/* error handling */
void error(token_t);
void reseterror();
void debugtoken();
void bdebug(const char*);

/* the arithemtic stack */
void push(number_t);
number_t pop();
address_t popaddress();
void drop();
void clearst();

/* READ DATA handling */
void clrdata();

/* FOR NEXT GOSUB stacks */
void pushforstack();
void popforstack();
void dropforstack();
void clrforstack();
void pushgosubstack(mem_t);
void popgosubstack();
void dropgosubstack();
void clrgosubstack();

/* general I/O initialisation */
void ioinit();
void iodefaults();

/* signal handling */
void signalon();
void signaloff();
void signalhandler(int);

/* character and string I/O functions */
/* we live in world where char may be signed or unsigned and keep it 
    that way on the lowest level, hence this function, fully defined here, 
    mostly inlined anyway*/
int cheof(int c) { if ((c == -1) || (c == 255)) return 1; else return 0; }
/* input */
char inch();
char checkch();
short availch();
void inb(char*, index_t);
void ins(char*, address_t); 

/* output */
void outch(char);
void outcr();
void outspc();
void outs(char*, address_t);
void outsc(const char*);
void outscf(const char *, index_t);

/* I/O of number_t - floats and integers */
address_t parsenumber(char*, number_t*);
address_t parsenumber2(char*, number_t*);
address_t writenumber(char*, wnumber_t); 
address_t writenumber2(char*, number_t);
address_t tinydtostrf(number_t, index_t, char*);
char innumber(number_t*);
void outnumber(number_t);

/* 	
 * Layer 1 functions, provide data and do the heavy lifting 
 * for layer 2 including lexical analysis, storing programs 
 * and expression evaluation 
 */

/* lexical analysis */
void whitespaces();
void nexttoken();

/* storing and retrieving programs */
char nomemory(number_t);
void storetoken(); 
mem_t memread(address_t);
mem_t memread2(address_t);
void memwrite2(address_t, mem_t);
void gettoken();
void firstline();
void nextline();

void clrlinecache();
void addlinecache(address_t, address_t);
address_t findinlinecache(address_t);
void findline(address_t);
address_t myline(address_t);
void moveblock(address_t, address_t, address_t);
void zeroblock(address_t, address_t);
void diag();
void storeline();

/* read arguments from the token stream and process them */
char termsymbol();
char expect(token_t, mem_t);
char expectexpr();
void parsearguments();
void parsenarguments(char);
void parsesubscripts();
void parsefunction(void (*)(), short);
void parseoperator(void (*)());
void parsesubstring();

/* mathematics and other functions for int and float */
void xabs();
void xsgn();
void xpeek();
void xmap();
void rnd();
void sqr();
void xpow();

/* string values and string evaluation */
char stringvalue();
void streval();

/* floating point functions */
void xsin();
void xcos();
void xtan();
void xatan();
void xlog();
void xexp();
void xint();

/* expression evaluation */
void factor();
void term();
void addexpression();
void compexpression();
void notexpression();
void andexpression();
void expression();

/* 
 * Layer 2 - statements and functions 
 * use the global variables 
 */

/* basic commands of the core language set */
void xprint();
void lefthandside(address_t*, address_t*, address_t*, mem_t*);
void assignnumber(signed char, char, char, address_t, address_t, char);
void assignment();
void showprompt();
void xinput();
void xgoto();
void xreturn();
void xif();

/* FOR NEXT loops */
void findnextcmd();
void findbraket(token_t, token_t);
void xfor();
void xbreak();
void xcont();
void xnext();

/* WHILE WEND*/
void xwhile();
void xwend();

/* REPEAT UNTIL */
void xrepeat();
void xuntil();

/* control commands and misc */
void outputtoken();
void xlist();
void xrun();
void xnew();
void xrem();
void xclr();
void xdim();
void xpoke();
void xtab();
void xdump();
void dumpmem(address_t, address_t, char);
void xlocate();

/* file access and other i/o */
void stringtobuffer(char*);
void getfilename(char*, char);
void xsave();
void xload(const char*);
void xget();
void xput();
void xset();
void xnetstat();

/* Arduino IO control interface */
void xdwrite();
void xawrite();
void xpinm();
void xdelay();
void xtone();

/* graphics commands */
void xcolor();
void xplot();
void xline();
void xrect();
void xcircle();
void xfrect();
void xfcircle();

/* the darkarts */
void xmalloc();
void xfind();
void xeval();

/* IoT commands */
void xassign();
void xavail();
void xfsensor();
void xsleep();
void xwire();
void xfwire();

/* timers */
void xafter();
void xevent();

/* File I/O functions */
char streq(const char*, char*);
void xcatalog();
void xdelete();
void xopen();
void xfopen();
void xclose();
void xfdisk();

/* low level access functions */
void xcall();
void xusr();

/* the dartmouth stuff */
void xdata();
void nextdatarecord();
void xread();
void xrestore();
void xdef();
void xfn();
void xon();

/* timers and interrupts */
void xtimer();
void resettimer(btimer_t*);

/* structured BASIC extensions */
void xwhile();
void xwend(); 
void xrepeat();
void xuntil();
void xswitch();
void xcase();
void xendswitch();

/* the emulation of tone using the byield loop */
void toggletone();
void playtone(int, int, int);

/* the statement loop */
void statement();

/* the extension functions */
void bsetup();
void bloop();

