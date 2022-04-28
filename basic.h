/*

	$Id: basic.h,v 1.7 2022/04/10 06:25:05 stefan Exp stefan $

	Stefan's basic interpreter 

	Playing around with frugal programming. See the licence file on 
	https://github.com/slviajero/tinybasic for copyright/left.
    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)

	Author: Stefan Lenz, sl001@serverfabrik.de

	basic.h are the core defintions and function protypes

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

/* general definitions, needed on some old compilers */
#define TRUE  1
#define FALSE 0


#if defined(ARDUINO_ARCH_AVR)
/* the small memory model with shallow stacks and small buffers */
#define BUFSIZE 		80
#define STACKSIZE		15
#define GOSUBDEPTH		4
#define FORDEPTH		4
#else 
/* the for larger microcontrollers and real computers */
#define BUFSIZE 		128
#define STACKSIZE		64
#define GOSUBDEPTH		8
#define FORDEPTH		8
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
#define TOR  	-97
#define TLEN    -96
#define TSGN	-95	
#define TPEEK	-94
#define TDIM	-93
#define TCLR	-92
#define TLOMEM  -91
#define THIMEM  -90 
#define TTAB 	-89
#define TTHEN   -88
#define TEND    -87
#define TPOKE	-86
/* Stefan's tinybasic additions (12) */
#define TCONT   -85
#define TSQR	-84
#define TPOW	-83
#define TFRE	-82
#define TDUMP 	-81
#define TBREAK  -80
#define TSAVE   -79
#define TLOAD   -78
#define TGET    -77
#define TPUT    -76
#define TSET    -75
#define TCLS    -74
/* Arduino functions (10) */
#define TPINM	-73
#define TDWRITE	-72
#define TDREAD	-71
#define TAWRITE	-70
#define TAREAD	-69
#define TDELAY	-68
#define TMILLIS	-67
#define TTONE	-66
#define TPULSEIN	-65
#define TAZERO		-64
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
/* the dark arts and Dartmouth extensions (6) */
#define TDATA	-42
#define TREAD   -41
#define TRESTORE -40
#define TDEF     -39
#define TFN 	-38
#define TON     -37
#define TELSE 	-36
/* darkarts (3) */
#define TMALLOC -35
#define TFIND   -34
#define TEVAL   -33
/* iot extensions (6) */
#define TITER	-32
#define TAVAIL	-31
#define TSTR    -30
#define TINSTR  -29
#define TVAL 	-28
#define TNETSTAT -27
#define TSENSOR  -26
#define TWIRE 	-25
/* constants used for some obscure purposes */
#define TBUFFER -4
/* unused right now from earlier code to be removed soon */
#define TERROR  -3
#define UNKNOWN -2
#define NEWLINE -1

/* the number of keywords, and the base index of the keywords */
#define NKEYWORDS	3+19+14+12+10+5+2+7+7+7+11
#define BASEKEYWORD -121

/*
 *	Interpreter states 
 *	SRUN means running from a programm
 *	SINT means interactive mode
 *	SERUN means running directly from EEPROM
 *		(enum would be the right way of doing this.)
 *	BREAKCHAR is the character stopping the program on Ardunios
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
const char slomem[]  PROGMEM = "LOMEM";
const char shimem[]  PROGMEM = "HIMEM";
const char stab[]    PROGMEM = "TAB";
const char sthen[]   PROGMEM = "THEN";
const char sbend[]    PROGMEM = "END";
const char spoke[]   PROGMEM = "POKE";
#endif
/* Stefan's tinybasic additions */
#ifdef HASSTEFANSEXT
const char scont[]   PROGMEM = "CONT";
const char ssqr[]    PROGMEM = "SQR";
const char spow[]    PROGMEM = "POW";
const char sfre[]    PROGMEM = "FRM";
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
const char smillis[]  PROGMEM = "MILLIS";
const char sazero[]   PROGMEM = "AZERO";
#endif
#ifdef HASTONE
const char stone[]    PROGMEM = "PLAYTONE";
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
const char sfrect[]   PROGMEM  = "FRCT";
#endif
/* Dartmouth BASIC extensions */
#ifdef HASDARTMOUTH
const char sdata[]  	PROGMEM  = "DATA";
const char sread[]  	PROGMEM  = "READ";
const char srestore[]   PROGMEM  = "RESTORE";
const char sdef[] 	PROGMEM  = "DEF";
const char sfn[]   	PROGMEM  = "FN";
const char son[]   	PROGMEM  = "ON";
const char selse[]	PROGMEM  = "ELSE";
#endif
/* The Darkarts commands unthinkable in Dartmouth */
#ifdef HASDARKARTS
const char smalloc[]	PROGMEM  = "MALLOC";
const char sfind[]		PROGMEM  = "FIND";
const char seval[]		PROGMEM  = "EVAL";
#endif
/* iot extensions */
#ifdef HASIOT
const char siter[]		PROGMEM  = "ITER";
const char savail[]		PROGMEM  = "AVAIL";
const char sstr[]		PROGMEM  = "STR";
const char sinstr[]		PROGMEM  = "INSTR";
const char sval[]		PROGMEM  = "VAL";
const char snetstat[]	PROGMEM  = "NETSTAT";
const char ssensor[]	PROGMEM  = "SENSOR";
const char swire[]		PROGMEM  = "WIRE";
#endif

/* zero terminated keyword storage */
const char* const keyword[] PROGMEM = {
	sge, sle, sne, sprint, slet, sinput, 
	sgoto, sgosub, sreturn, sif, sfor, sto,
	sstep, snext, sstop, slist, snew, srun,
	sabs, srnd, ssize, srem,
#ifdef HASAPPLE1
	snot, sand, sor, slen, ssgn, speek, sdim,
	sclr, slomem, shimem, stab, sthen, 
	sbend, spoke,
#endif
#ifdef HASSTEFANSEXT
	scont, ssqr, spow, sfre, sdump, sbreak, 
#endif
	ssave, sload, 
#ifdef HASSTEFANSEXT
	sget, sput, sset, scls,
#endif
#ifdef HASARDUINOIO
    spinm, sdwrite, sdread, sawrite, saread, 
    sdelay, smillis, sazero,  
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
	selse,
#endif
#ifdef HASDARKARTS
	smalloc, sfind, seval, 
#endif
#ifdef HASIOT
	siter, savail, sstr, sinstr, sval, 
	snetstat, ssensor, swire,
#endif
	0
};

/* the zero terminated token dictonary needed for scalability */
const signed char tokens[] PROGMEM = {
	GREATEREQUAL, LESSEREQUAL, NOTEQUAL, TPRINT, TLET,    
    TINPUT, TGOTO, TGOSUB, TRETURN, TIF, TFOR, TTO, TSTEP,
    TNEXT, TSTOP, TLIST, TNEW, TRUN, TABS, TRND, TSIZE, TREM,
#ifdef HASAPPLE1
    TNOT, TAND, TOR, TLEN, TSGN, TPEEK, TDIM, TCLR, TLOMEM,
    THIMEM, TTAB, TTHEN, TEND, TPOKE,
#endif
#ifdef HASSTEFANSEXT
	TCONT, TSQR, TPOW, TFRE, TDUMP, TBREAK, 
#endif
	TSAVE, TLOAD, 
#ifdef HASSTEFANSEXT	
	TGET, TPUT, TSET, TCLS,
#endif
#ifdef HASARDUINOIO
	TPINM, TDWRITE, TDREAD, TAWRITE, TAREAD, TDELAY, TMILLIS,
	TAZERO, 
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
	TDATA, TREAD, TRESTORE, TDEF, TFN, TON, TELSE,
#endif
#ifdef HASDARKARTS
	TMALLOC, TFIND, TEVAL, 
#endif
#ifdef HASIOT
	TITER, TAVAIL, TSTR, TINSTR, TVAL, TNETSTAT,
	TSENSOR, TWIRE,
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
const char mgreet[]		PROGMEM = "Stefan's Basic 1.3b";
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
 *	works with the tacit assumption that 
 *	sizeof(number_t) >= sizeof(address_t) 
 *	floating point here is under construction we always 
 *	assume that float >= 4 bytes in the following
 *
 *	maxnum: the maximum accurate(!) integer of a 
 *		32 bit float 
 *	strindexsize: the index size of strings either 
 *		1 byte or 2 bytes - no other values supported
 */
#ifdef HASFLOAT
typedef float number_t;
const number_t maxnum=16777216; 
#else
typedef int number_t;
const number_t maxnum=(number_t)~((number_t)1<<(sizeof(number_t)*8-1));
#endif
typedef unsigned short address_t;
const int numsize=sizeof(number_t);
const int addrsize=sizeof(address_t);
const int eheadersize=sizeof(address_t)+1;
const int strindexsize=2; // 
const address_t maxaddr=(address_t)(~0); 

/*
	The basic interpreter is implemented as a stack machine
	with global variable for the interpreter state, the memory
	and the arithmetic during run time.

	stack is the stack memory and sp controls the stack.

	ibuffer is an input buffer and *bi a pointer to it.

	sbuffer is a short buffer for arduino progmem access. 

	vars is a static array of 26 single character variables.

	mem is the working memory of the basic interperter.

	x, y, xc, yc are two n*8 bit and two 8 bit accumulators.

	z is a mixed n*8 bit accumulator

	ir, ir2 are general index registers for string processing.

	token contains the actually processes token.

	er is the nontrapable error status

	ert is the trapable error status 

	st, here and top are the interpreter runtime controls.

	nvars is the number of vars the interpreter has stored.

	form is used for number formation Palo Alto BASIC style.

	rd is the random number storage.

	fnc counts the depth of for - next loop nesting

	args is the global arg count variable

	id and od are the input and output model for an arduino
		they are set to serial by default

	idd and odd are the default values of the above

	debuglevel is the statement loop debug level

	data is the data pointer of the READ/DATA mechanism

*/
static number_t stack[STACKSIZE];
static address_t sp=0; 

static char sbuffer[SBUFSIZE];

static char ibuffer[BUFSIZE] = "\0";
static char *bi;

static number_t vars[VARSIZE];

#if MEMSIZE != 0
static signed char mem[MEMSIZE];
#else
static signed char* mem;
#endif
static address_t himem, memsize;

static struct {char varx; char vary; address_t here; number_t to; number_t step;} forstack[FORDEPTH];
static short forsp = 0;
static char fnc; 

static address_t gosubstack[GOSUBDEPTH];
static short gosubsp = 0;

static number_t x, y;
static signed char xc, yc;

struct twobytes {signed char l; signed char h;};
static union accunumber { number_t i; address_t a; struct twobytes b; signed char c[sizeof(number_t)]; } z;

static char *ir, *ir2;
static signed char token;
static signed char er;
static signed char ert;

static signed char st; 
static address_t here; 
static address_t top;

static address_t nvars = 0; 

static char form = 0;

static signed char args;

/* this is unsigned hence address_t */
static address_t rd;

/* output and input vector */
static unsigned char id;
static unsigned char od;

/* default IO - not constant, can be changed at runtime 
	through a user call */
static unsigned char idd = ISERIAL;
static unsigned char odd = OSERIAL;

/* the runtime debuglevel */
char debuglevel = 0;

/* data pointer */
#ifdef HASDARTMOUTH
address_t data = 0;
#endif

/*
 * IoT yield counter, we count when we did yield the last time
 *	lastyield controlls the client loops of network functions 
 *	like mqtt
 *	lastlongyield controls longterm functions like DHCP lease 
 *	renewal in Ethernet 
 */
static long lastyield=0;
static long lastlongyield=0;

/* formaters lastouttoken and spaceafterkeyword to make a nice LIST */
signed char lastouttoken;
signed char spaceafterkeyword;

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
int freeRam();
long freememorysize();
void restartsystem();
void activatesleep();

/* start the spi bus */
void spibegin();

/* the hardware interface display driver functions, need to be 
	implemented for the display driver to work */
void dspbegin();
void dspprintchar(char, short, short);
void dspclear();

/* keyboard code */
void kbdbegin();
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
void vgawrite(char);

/* generic display code */
void dspwrite(char);
void dspbegin();
char dspwaitonscroll();
char dspactive();
void dspsetscrollmode(char, short);
void dspsetcursor(short, short);
void dspbufferclear();

/* real time clock */
char* rtcmkstr();
void rtcset(char, short);
short rtcget(char);
short rtcread(char);

/* network and mqtt functions */
void netbegin();
char netconnected();
void mqttsetname();
void mqttbegin();
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
void yieldfunction();
void longyieldfunction();

/* the file interface */
char* mkfilename(const char*);
const char* rmrootfsprefix(const char*);
void fsbegin(char);
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
int rootfilesize();
void rootfileclose();
void rootclose();
void formatdisk(short i);

/* low level serial code */
void serialbegin();
void picogetchar(int);
char serialread();
void serialwrite(char);
short serialcheckch();
short serialavailable();
void consins(char*, short);
void prtbegin();
char prtread();
void prtwrite(char);
short prtcheckch();
short prtavailable();

/* generic wire access */
void wirebegin();
void wireopen(char*);
void wireins(char*, uint8_t);
void wireouts(char*, uint8_t);

/* RF24 radio input */
void iradioopen(char*);
void oradioopen(char*);
void radioins(char*, short);
void radioouts(char* , short);

/* sensor control */
void sensorbegin();
number_t sensorread(short, short);

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
address_t bmalloc(signed char, char, char, address_t);
address_t bfind(signed char, char, char);
address_t blength (signed char, char, char);

/* normal variables of number_t */
void reatevar(char, char);
number_t getvar(char, char);
void setvar(char, char, number_t);
void clrvars();

/*	low level memory access packing n*8bit bit into n 8 bit objects
	e* is for Arduino EEPROM */
void getnumber(address_t, short);
void setnumber(address_t, short);
void egetnumber(address_t, short);
void esetnumber(address_t, short);

/* array and string handling */
address_t createarray(char, char, address_t);
void array(char, char, char, address_t, number_t*);
address_t createstring(char, char, address_t);
char* getstring(char, char, address_t);
number_t arraydim(char, char);
number_t stringdim(char, char);
number_t lenstring(char, char);
void setstringlength(char, char, address_t);
void setstring(char, char, address_t, char *, address_t);

/* get keywords and tokens from PROGMEM */
char* getkeyword(unsigned short);
char* getmessage(char);
signed char gettokenvalue(char);
void printmessage(char);

/* error handling */
void error(signed char);
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
void pushgosubstack();
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
void ins(char*, short); 

/* output */
void outch(char);
void outcr();
void outspc();
void outs(char*, short);
void outsc(const char*);
void outscf(const char *, short);

/* I/O of number_t - floats and integers */
short parsenumber(char*, number_t*);
short parsenumber2(char*, number_t*);
short writenumber(char*, number_t);
short writenumber2(char*, number_t);
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
char memread(address_t);
void gettoken();
void firstline();
void nextline();
void findline(address_t);
address_t myline(address_t);
void moveblock(address_t, address_t, address_t);
void zeroblock(address_t, address_t);
void diag();
void storeline();

/* read arguments from the token stream and process them */
char termsymbol();
char expect(signed char, char);
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
void fre();
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
void lefthandside(address_t*, char*);
void assignnumber(signed char, char, char, address_t, char);
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
void dumpmem(address_t, address_t);

/* file access and other i/o */
void stringtobuffer();
void getfilename(char*, char);
void xsave();
void xload(const char* f);
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
void xinext();
void xiter();
void xavail();
void xfsensor();

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

/* the statement loop */
void statement();
