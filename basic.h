/*
 *
 *	$Id: basic.h,v 1.11 2023/02/18 20:16:59 stefan Exp stefan $
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
#ifdef RASPPI
#include <wiringPi.h>
#endif
#endif


#if defined(ARDUINO_ARCH_AVR)
/* the small memory model with shallow stacks and small buffers */
#define BUFSIZE 		80
#define STACKSIZE		15
#define GOSUBDEPTH		4
#define FORDEPTH		4
#define LINECACHESIZE	4
#else 
/* the for larger microcontrollers and real computers */
#define BUFSIZE 		128
#define STACKSIZE		64
#define GOSUBDEPTH		8
#define FORDEPTH		8
#define LINECACHESIZE	16
#endif

/* on the real small systems we remove the linecache and set a fixed memory size*/
#ifdef ARDUINO_AVR_DUEMILANOVE
#undef LINECACHESIZE
#if MEMSIZE == 0
#define MEMSIZE 512
#endif
#endif

/* more duffers and vars */
#define SBUFSIZE		32
#define VARSIZE			26
/* default sizes of arrays and strings if they are not DIMed */
#define ARRAYSIZEDEF	10
#define STRSIZEDEF		32

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
/* Stefan's tinybasic additions (12) */
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
/* Arduino functions (10) */
#define TPINM	-74
#define TDWRITE	-73
#define TDREAD	-72
#define TAWRITE	-71
#define TAREAD	-70
#define TDELAY	-69
#define TMILLIS	-68
#define TTONE	-67
#define TPULSEIN	-66
#define TAZERO	-65
#define TLED	-64
/* the DOS functions (5) */
#define TCATALOG	-63
#define TDELETE	-62
#define TOPEN 	-61
#define TCLOSE 	-60
#define TFDISK  -59
/* low level access of internal routines (2) */
#define TUSR	-58
#define TCALL 	-57
/* mathematical functions (7) */
#define TSIN 	-56
#define TCOS    -55
#define TTAN 	-54
#define TATAN   -53
#define TLOG    -52
#define TEXP    -51
#define TINT    -50
/* graphics - experimental - rudimentary (7) */
#define TCOLOR 	-49
#define TPLOT   -48
#define TLINE 	-47
#define TCIRCLE -46
#define TRECT   -45
#define TFCIRCLE -44
#define TFRECT   -43
/* the Dartmouth extensions (6) */
#define TDATA	-42
#define TREAD   -41
#define TRESTORE -40
#define TDEF	-39
#define TFN 	-38
#define TON     -37
/* the latecomer ELSE */
#define TELSE 	-36
/* darkarts (3) */
#define TMALLOC -35
#define TFIND   -34
#define TEVAL   -33
/* iot extensions (9) */
#define TERROR	-32
#define TAVAIL	-31
#define TSTR	-30
#define TINSTR	-29
#define TVAL	-28
#define TNETSTAT	-27
#define TSENSOR	-26
#define TWIRE	-25
#define TSLEEP	-24
/* events and interrupts */
#define TAFTER -23
#define TEVERY -22
#define TEVENT -21
/* end of tokens */
/* constants used for some obscure purposes */
#define TBUFFER -2
/* UNKNOWN is not used in the current code, the 
 * lexer tokenizes everything blindly. There is a UNKNOWN hook 
 * in statement for a grammar aware lexer */
#define UNKNOWN -1

/* the number of keywords, and the base index of the keywords */
#define NKEYWORDS	3+19+13+12+11+5+2+7+7+7+12+3
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
 * 
 */
#define SINT 0
#define SRUN 1
#define SERUN 2
#define BREAKCHAR '#'

/* 
 *	Arduino input and output channels
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
const char splusein[] PROGMEM = "PULSEIN";
#endif
/* DOS functions */
#ifdef HASFILEIO
const char scatalog[] PROGMEM = "CATALOG";
const char sdelete[]  PROGMEM = "DELETE";
const char sfopen[]   PROGMEM = "OPEN";
const char sfclose[]  PROGMEM = "CLOSE";
const char sfdisk[]  PROGMEM = "FDISK";
#endif
/* low level access functions */
#ifdef HASSTEFANSEXT
const char susr[]  PROGMEM = "USR";
const char scall[] PROGMEM = "CALL";
#endif
/* mathematics */
#ifdef HASFLOAT
const char ssin[]  PROGMEM = "SIN";
const char scos[]  PROGMEM = "COS";
const char stan[]  PROGMEM = "TAN";
const char satan[] PROGMEM = "ATAN";
const char slog[]  PROGMEM = "LOG";
const char sexp[]  PROGMEM = "EXP";
#endif
/* INT is always needed to make float/int programs compatible */
const char sint[]  PROGMEM = "INT"; 
/* elemetars graphics */
#ifdef HASGRAPH
const char scolor[]  PROGMEM  = "COLOR";
const char splot[]   PROGMEM  = "PLOT";
const char sline[]   PROGMEM  = "LINE";
const char scircle[] PROGMEM  = "CIRCLE";
const char srect[]   PROGMEM  = "RECT";
const char sfcircle[] PROGMEM  = "FCIRCLE";
const char sfrect[]   PROGMEM  = "FRECT";
#endif
/* Dartmouth BASIC extensions */
#ifdef HASDARTMOUTH
const char sdata[]  	PROGMEM  = "DATA";
const char sread[]  	PROGMEM  = "READ";
const char srestore[]   PROGMEM  = "RESTORE";
const char sdef[] 	PROGMEM  = "DEF";
const char sfn[]   	PROGMEM  = "FN";
const char son[]   	PROGMEM  = "ON";
#endif
/* a latecomer the ELSE command */
#ifdef HASSTEFANSEXT
const char selse[]	PROGMEM  = "ELSE";
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
	sget, sput, sset, scls,
#endif
#ifdef HASARDUINOIO
    spinm, sdwrite, sdread, sawrite, saread, 
    sdelay, smillis, sazero, sled,
#endif
#ifdef HASTONE
	stone,
#endif
#ifdef HASPULSE
	splusein,
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
#ifdef HASSTEFANSEXT
	selse,
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
	TGET, TPUT, TSET, TCLS,
#endif
#ifdef HASARDUINOIO
	TPINM, TDWRITE, TDREAD, TAWRITE, TAREAD, TDELAY, TMILLIS,
	TAZERO, TLED,
#endif
#ifdef HASTONE
	TTONE, 
#endif
#ifdef HASPULSE
	TPULSEIN, 
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
#ifdef HASSTEFANSEXT
	TELSE,
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
#define ERETURN      14
#define ENEXT        15
#define EGOSUB       16 
#define EFOR         17
#define EOUTOFMEMORY 18
#define ESTACK 		 19
#define EDIM         20
#define EORANGE 	 21
#define ESTRING      22
#define EVARIABLE	 23
#define EFILE 		 24
#define EFUN 		 25
#define EARGS		 26
#define EEEPROM		 27
#define ESDCARD		 28

const char mfile[]    	PROGMEM = "file.bas";
const char mprompt[]	PROGMEM = "> ";
const char mgreet[]		PROGMEM = "Stefan's Basic 1.4b";
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
const char ereturn[]    PROGMEM = "Return";
const char enext[]		PROGMEM = "Next";
const char egosub[] 	PROGMEM = "GOSUB";
const char efor[]		PROGMEM = "FOR";
const char emem[]  	   	PROGMEM = "Memory";
const char estack[]    	PROGMEM = "Stack";
const char edim[]		PROGMEM = "DIM";
const char erange[]  	PROGMEM = "Range";
const char estring[]	PROGMEM = "String";
const char evariable[]  PROGMEM = "Variable";
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
	, eunknown, enumber, edivide, eline, ereturn, 
	enext, egosub, efor, emem, estack, edim, erange,
	estring, evariable, efile, efun, eargs, 
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
 *  address_t is an unsigned type adddressing memory 
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
 *
 *
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
typedef short index_t; /* this type counts at least 16 bit */

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

/*
 *	The basic interpreter is implemented as a stack machine
 *	with global variable for the interpreter state, the memory
 *	and the arithmetic during run time.
 *
 *	stack is the stack memory and sp controls the stack.
 *
 *	ibuffer is an input buffer and *bi a pointer to it.
 *
 *	sbuffer is a short buffer for arduino progmem access. 
 *
 *	vars is a static array of 26 single character variables.
 *
 *	mem is the working memory of the basic interperter.
 *
 *	x, y, xc, yc are two n*8 bit and two 8 bit accumulators.
 *
 *	ax, ax are address type accumulators.
 *
 *	z is a mixed n*8 bit accumulator
 *
 *	ir, ir2 are general index registers for string processing.
 *
 *	token contains the actually processes token.
 *
 *	er is the nontrapable error status
 *
 *	ert is the trapable error status 
 *
 *	st, here and top are the interpreter runtime controls.
 *
 *	nvars is the number of vars the interpreter has stored.
 *
 *	form is used for number formation Palo Alto BASIC style.
 *
 *	charcount counts the printed characters to create a real TAB
 *		only implemented on the serial stream
 *	reltab controls if the relative char mechanisms is active
 *
 *	rd is the random number storage.
 *
 *	fnc counts the depth of for - next loop nesting
 *
 *	args is the global arg count variable
 *
 *	id and od are the input and output model for an arduino
 *		they are set to serial by default
 *
 *	idd and odd are the default values of the above
 *
 *	debuglevel is the statement loop debug level
 *
 *	data is the data pointer of the READ/DATA mechanism
 *  datarc is the counter of the read data record
 *
 * static keyword here is obsolete on most platforms
 *
 */

static number_t stack[STACKSIZE];
static address_t sp=0; 

static char sbuffer[SBUFSIZE];

static char ibuffer[BUFSIZE] = "\0";
static char *bi;

static number_t vars[VARSIZE];

#if MEMSIZE != 0
static mem_t mem[MEMSIZE];
#else
static mem_t* mem;
#endif
static address_t himem, memsize;

static struct {mem_t varx; mem_t vary; address_t here; number_t to; number_t step;} forstack[FORDEPTH];
static index_t forsp = 0;
static mem_t fnc; 

static address_t gosubstack[GOSUBDEPTH];
static index_t gosubsp = 0;

static number_t x, y;
static mem_t xc, yc;

static address_t ax, ay;

/* this union is used to store larger objects into byte oriented memory */
struct twobytes {mem_t l; mem_t h;};
static union accunumber { number_t i; address_t a; struct twobytes b; mem_t c[sizeof(number_t)]; } z;

static char *ir, *ir2;
static mem_t token;
static mem_t er;
static mem_t ert;

static mem_t st; 
static address_t here; 
static address_t top;

static address_t nvars = 0; 

static mem_t form = 0;

#ifdef HASMSTAB
static mem_t charcount = 0;
static mem_t reltab = 0;
#endif

#ifdef HASARRAYLIMIT
static address_t arraylimit = 1;
#else 
const static address_t arraylimit = 1;
#endif

static mem_t args;

/* this is unsigned hence address_t */
static address_t rd;

/* output and input vector */
static mem_t id;
static mem_t od;

/* default IO - not constant, can be changed at runtime 
	through a user call */
static mem_t idd = ISERIAL;
static mem_t odd = OSERIAL;

/* the runtime debuglevel */
static mem_t debuglevel = 0;

/* data pointer */
#ifdef HASDARTMOUTH
static address_t data = 0;
static address_t datarc = 1;
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
static long lastyield=0;
static long lastlongyield=0;

/* formaters lastouttoken and spaceafterkeyword to make a nice LIST */
static mem_t lastouttoken;
static mem_t spaceafterkeyword;

/* 
 * the cache for the heap search - helps the string code 
 * the last found object on the heap is remembered. This is needed
 * because the string code sometime searches the heap twice during the 
 * same operation. 
 */
#ifdef HASAPPLE1
static mem_t bfindc, bfindd, bfindt;
static address_t bfinda, bfindz;
#endif

/*
 * a variable for some string operations 
 */
#ifdef HASIOT
static int vlength;
#endif

/* the timer code - very simple needs to to to a struct */
#ifdef HASTIMER

/* timer type */
typedef struct {
    mem_t enabled;
    unsigned long last;
    unsigned long interval; 
    mem_t type;
    address_t linenumber;
} btimer_t;

static btimer_t after_timer = {0, 0, 0, 0, 0};
static btimer_t every_timer = {0, 0, 0, 0, 0};
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
static int nevents = 0;
static int ievent = 0;
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

static berrorh_t berrorh = {0 , 0};
static mem_t erh = 0;
#endif

/* the string for real time clocks */
char rtcstring[20] = { 0 }; 

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
short eread(address_t);
void eupdate(address_t, short);

/* arduino io functions */
void aread();
void dread();
void awrite(number_t, number_t);
void dwrite(number_t, number_t);
void pinm(number_t, number_t);
void bmillis();
void bpulsein();
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
#if!defined(MSDOS) || defined(MINGW)
typedef unsigned int uint32_t;
#endif
void picogetchar(char);
void picowrite(char);
void picobegin(uint32_t);
void picoins(char, short);
void serialbegin();
int serialstat(char);
char serialread();
void serialwrite(char);
short serialcheckch();
short serialavailable();
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
void error(mem_t);
void reseterror();
void debugtoken();
void bdebug(const char*);

/* the arithemtic stack */
void push(number_t);
number_t pop();
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

/* character and string I/O functions */
/* input */
char inch();
char checkch();
short availch();
void inb(char*, short);
void ins(char*, address_t); 

/* output */
void outch(char);
void outcr();
void outspc();
void outs(char*, address_t);
void outsc(const char*);
void outscf(const char *, short);

/* I/O of number_t - floats and integers */
address_t parsenumber(char*, number_t*);
address_t parsenumber2(char*, number_t*);
address_t writenumber(char*, wnumber_t); 
address_t writenumber2(char*, number_t);
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
char expect(mem_t, mem_t);
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

/* optional FOR NEXT loops */
void findnextcmd();
void xfor();
void xbreak();
void xcont();
void xnext();

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

/* file access and other i/o */
void stringtobuffer();
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
void xfind2();
void xeval();

/* IoT commands */
void xassign();
void xavail();
void xfsensor();
void xsleep();
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
void resettimer();

/* the emulation of tone using the byield loop */
void toggletone();
void playtone(int, int, int);

/* the statement loop */
void statement();

/* the extension functions */
void bsetup();
void bloop();
