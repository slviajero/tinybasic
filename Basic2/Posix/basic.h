/*
 *
 *	$Id: basic.h,v 1.1 2023/08/26 08:23:23 stefan Exp stefan $
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
 * a few of the buffers and vars come from hardware.h. 
 * These are 
 * for real computers 
 * #define BUFSIZE         
 * #define STACKSIZE      
 * #define GOSUBDEPTH      
 * #define FORDEPTH        
 * #define LINECACHESIZE
 * 
 * They depend on the hardware architecture and are configured there 
 */

/* more duffers and vars */
#define SBUFSIZE        32
#define VARSIZE         26

/* default sizes of arrays and strings if they are not DIMed */
#define ARRAYSIZEDEF    10
#define STRSIZEDEF      32

/*
 *	the time intervall in ms needed for 
 *	ESP8266 yields, network client loops 
 *	and other timing related functions 
 */
#define LONGYIELDINTERVAL 1000
#define YIELDINTERVAL 32

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
typedef long wnumber_t;
#else
typedef int number_t;
typedef int wnumber_t;
#endif
typedef uint16_t address_t; /* this type addresses memory */
typedef int8_t mem_t; /* a signed 8 bit type for the memory */
typedef int index_t; /* this type counts at least 16 bit */
#ifndef HASLONGTOKENS
typedef int8_t token_t; /* the type of tokens, normally mem_t with a maximum of 127 commands and data types */
#else
typedef int16_t token_t; /* token type extension, allows an extra of 127 commands and symbols */
#endif

/* this type maps numbers to bytes */
typedef struct {mem_t l; mem_t h;} twobytes_t;
typedef union { number_t i; address_t a; twobytes_t b; mem_t c[sizeof(number_t)]; } accu_t;

/* the memreader function type */
typedef mem_t (*memreader_t)(address_t);

typedef struct {
    mem_t enabled;
    unsigned long last;
    unsigned long interval; 
    mem_t type;
    address_t linenumber;
} btimer_t;


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
#define SYSTYPE_NRENESA 8
#define SYSTYPE_POSIX	32
#define SYSTYPE_MSDOS	33
#define SYSTYPE_MINGW   34
#define SYSTYPE_RASPPI  35

/* 
 * the events API for Arduino with interrupt service routines
 * analogous to the timer API
 * 
 * we use raw modes here 
 *
 * #define CHANGE 1
 * #define FALLING 2
 * #define RISING 3
 *
 */

/* event type */
typedef struct {
    mem_t enabled;
    mem_t pin;
    mem_t mode;
    mem_t type;
    address_t linenumber;
    mem_t active;
} bevent_t;

/* interrupts in BASIC fire once and then disable themselves, BASIC reenables them */
/* event type */

void bintroutine0();
void bintroutine1();
void bintroutine2();
void bintroutine3();
mem_t enableevent(mem_t);
void disableevent(mem_t);
mem_t eventindex(mem_t);

/* handle the event list */
mem_t addevent(mem_t, mem_t, mem_t, address_t);
void deleteevent(mem_t);
volatile bevent_t* findevent(mem_t);

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

/* done elsewhere */

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

/* signal handling */
void signalon();
void signaloff();
void signalhandler(int);


/* output */
void outcr();
void outspc();
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

/* real time clock string stuff */
char* rtcmkstr();

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
void xaread();
void xdread();
void xdwrite();
void xawrite();
void xpinm();
void xdelay();
void xtone();
void btone(int);
void bpulsein();

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

/* the statement loop */
void statement();

/* the extension functions */
void bsetup();
void bloop();
