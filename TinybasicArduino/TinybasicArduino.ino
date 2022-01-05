/*

	$Id: basic.c,v 1.121 2022/01/03 07:25:25 stefan Exp stefan $

	Stefan's tiny basic interpreter 

	Playing around with frugal programming. See the licence file on 
	https://github.com/slviajero/tinybasic for copyright/left.
    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)

	Author: Stefan Lenz, sl001@serverfabrik.de

	The first set of definions define the target.
	- for any Arduino ARCH or a Mac the default settings are correct
		and architectures #defines of the IDE are used.
	- MINGW switches on Windows calls. 
	- MSDOS for MSDOS file access.
	- ARDUINOLCD, ARDUINOTFT and LCDSHIELD active the LCD code, 
		LCDSHIELD automatically defines the right settings for 
		the classical shield modules
	- ARDUINOPS2 activates the PS2 code. Default pins are 2 and 3.
		If you use other pins the respective changes have to be made 
			below. 
	- _if_  and PS2 are both activated STANDALONE cause the Arduino
			to start with keyboard and lcd as standard devices.
	- ARDUINOEEPROM includes the EEPROM access code
	- ARDUINOSD and ESPSPIFFS activate filesystem code 
	- HAS* activates or deactives features of the interpreter
	- activating Picoserial is not compatible with keyboard code
		Picoserial doesn't work on MEGA

	The extension flags control features and code size

	MEMSIZE sets the BASIC main memory to a fixed value,
		if MEMSIZE=0 a heuristic is used stepping down 
		from 60000 to 128 bytes.

	Architectures and the definitions from the Arduino IDE

	 	ARDUINO_ARCH_SAM: no tone command, dtostrf
	 	ARDUINO_ARCH_RP2040: dtostrf (for ARDUINO_NANO_RP2040_CONNECT)
	 	ARDUINO_ARCH_SAMD: dtostrf (for ARDUINO_SAMD_MKRWIFI1010, ARDUINO_SEEED_XIAO_M0)
		ARDUINO_ARCH_ESP8266: SPIFFS, dtostrf (ESP8266)
 		ARDUINO_AVR_MEGA2560, ARDUARDUINO_SAM_DUE: second serial port is Serial1 - no software serial
 		ARDUARDUINO_SAM_DUE: hardware heuristics
 		ARDUINO_ARCH_AVR: nothing

 	The code still contains hardware hueristics from my own projects, 
 	will be removed in the future

*/

#undef MINGW
#undef MSDOS

/*
	interpreter features
*/
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#undef  HASFLOAT
#undef  HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT

/* hardcoded memory size set 0 for automatic malloc */
#define MEMSIZE 0
/* 
	Arduino hardware settings 

	input/output methods USERPICOSERIAL, ARDUINOPS2
		ARDUINOPRT, DISPLAYCANSCROLL, ARDUINOLCDI2C,
		ARDUINOTFT
	storage ARDUINOEEPROM, ARDUINOSD, ESPSPIFFS
	sensors ARDUINORTC, ARDUINOWIRE
	network ARDUINORF24

*/
#undef USESPICOSERIAL 
#undef ARDUINOPS2
#undef ARDUINOPRT
#undef DISPLAYCANSCROLL
#undef ARDUINOLCDI2C
#undef LCDSHIELD
#undef ARDUINOTFT
#define ARDUINOEEPROM
#undef ARDUINOSD
#undef ESPSPIFFS
#undef ARDUINORTC
#undef ARDUINOWIRE
#undef ARDUINORF24
#undef STANDALONE
/* 
	Don't change the definitions here unless you must

	If PROGMEM is defined we can asssume we compile on 
	the Arduino IDE. Don't change anything here. 
  	This is a little hack to detect where we compile
*/
#ifdef PROGMEM
#define ARDUINOPROGMEM
#else
#undef ARDUINO
#undef ARDUINOSD
#undef ARDUINORF24
#undef ARDUINORTC
#undef ARDUINOEEPROM
#undef ARDUINOWIRE
#endif
/* 
	the non AVR arcitectures 
*/
#if defined(ARDUINO_ARCH_ESP8266) || defined(ARDUINO_ARCH_SAM) || defined(ARDUINO_ARCH_SAMD) || defined(ARDUINO_ARCH_RP2040)
#include <avr/dtostrf.h>
#define ARDUINO 100
#undef ARDUINOEEPROM
#ifdef ESPSPIFFS
#include <SPI.h>
#include <FS.h>
#endif
#endif
/* 
	all microcontrollers and their hardware 
*/
#ifdef ARDUINO
#ifdef ARDUINOPS2
#include <PS2Keyboard.h>
#endif
#ifdef ARDUINOPROGMEM
#include <avr/pgmspace.h>
#endif
#ifdef ARDUINOEEPROM
#include <EEPROM.h>
#endif
#ifdef USESPICOSERIAL
#include <PicoSerial.h>
#endif
#ifdef ARDUINOSD
#include <SPI.h>
#include <SD.h>
#endif
/* 
	MSDOS, Mac, Linux and Windows 
*/
#else 
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
/* 
	Arduino default serial baudrate 
*/
#ifdef ARDUINO
const int serial_baudrate = 9600;
#else 
const int serial_baudrate = 0;
#endif
#ifdef ARDUINOPRT
const int serial1_baudrate = 9600;
char sendcr = 0;
short blockmode = 0;
#else
const int serial1_baudrate = 0;
char sendcr = 0;
short blockmode = 0;
#endif

// general definitions
#define TRUE  1
#define FALSE 0

// debug mode switches 
#define DEBUG  0

// various buffer sizes
#define BUFSIZE 	128
#define SBUFSIZE	32
#define VARSIZE		26
#define STACKSIZE 	15
#define GOSUBDEPTH 	4
#define FORDEPTH 	4
#define ARRAYSIZEDEF 10
#define STRSIZEDEF   32

/*

   	The tokens:

	All single character operators are their own tokens
	ASCII values above 0x7f are used for tokens of keywords.

*/

#define EOL			 0
#define NUMBER   	 -127
#define LINENUMBER   -126
#define STRING   	 -125
#define VARIABLE 	 -124
#define STRINGVAR 	 -123
#define ARRAYVAR     -122
// multi character tokens - BASEKEYWORD (3)
#define GREATEREQUAL -121
#define LESSEREQUAL  -120
#define NOTEQUAL	 -119
// this is the Palo Alto Language Set (19)
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
// this is the Apple 1 language set in addition to Palo Alto (14)
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
// Stefan's tinybasic additions (12)
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
// Arduino functions (10)
#define TPINM	-73
#define TDWRITE	-72
#define TDREAD	-71
#define TAWRITE	-70
#define TAREAD  -69
#define TDELAY  -68
#define TMILLIS  -67
#define TTONE   -66
#define TPULSEIN  -65
#define TAZERO	  -64
// the SD card DOS functions (4)
#define TCATALOG -63
#define TDELETE  -62
#define TOPEN 	-61
#define TCLOSE  -60
// low level access of internal routines
#define TUSR	-59
#define TCALL 	-58
// mathematical functions 
#define TSIN 	-57
#define TCOS    -56
#define TTAN 	-55
#define TATAN   -54
#define TLOG    -53
#define TEXP    -52
#define TINT    -51
// graphics - experimental - rudimentary
#define TCOLOR 	-50
#define TPLOT   -49
#define TLINE 	-48
#define TCIRCLE -47
#define TRECT   -46
#define TFCIRCLE -45
#define TFRECT   -44
// the dark arts and Dartmouth extensions
// not yet implemented only tokens reserverd
#define TDATA	-43
#define TREAD   -42
#define TRESTORE -41
#define TDEF      -40
#define TFN 	-39
#define TON     -38
// darkarts
#define TMALLOC -37
#define TFIND   -36
#define TEVAL   -35
// iot extensions
#define TITER	-34
#define TAVAIL	-33
// constants used for some obscure purposes 
#define TBUFFER -4
// unused right now from earlier code to be removed soon
#define TERROR  -3
#define UNKNOWN -2
#define NEWLINE -1

// the number of keywords, and the base index of the keywords
#define NKEYWORDS	3+19+14+12+10+4+2+7+7+6+5
#define BASEKEYWORD -121

/*
	Interpreter states 
	SRUN means running from a programm
	SINT means interactive mode
	SERUN means running directly from EEPROM
	(enum would be the right way of doing this.)
	BREAKCHAR is the character stopping the program on Ardunios
*/

#define SINT 0
#define SRUN 1
#define SERUN 2
#define BREAKCHAR '#'

/* 
	Arduino input and output models
*/

#define OSERIAL 1
#define ODSP 2
#define OPRT 4
#define OWIRE 7
#define ORADIO 8
#define OFILE 16

#define ISERIAL 1
#define IKEYBOARD 2
#define ISERIAL1 4
#define IWIRE 7
#define IRADIO 8
#define IFILE 16

/*
	All BASIC keywords
*/

const char sge[]   PROGMEM = "=>";
const char sle[]   PROGMEM = "<=";
const char sne[]   PROGMEM = "<>";
// Palo Alto language set
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
// Apple 1 language set
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
// Stefan's tinybasic additions
#ifdef HASSTEFANSEXT
const char scont[]   PROGMEM = "CONT";
const char ssqr[]    PROGMEM = "SQR";
const char spow[]    PROGMEM = "POW";
const char sfre[]    PROGMEM = "FRE";
const char sdump[]   PROGMEM = "DUMP";
const char sbreak[]  PROGMEM = "BREAK";
const char ssave[]   PROGMEM = "SAVE";
const char sload[]   PROGMEM = "LOAD";
const char sget[]    PROGMEM = "GET";
const char sput[]    PROGMEM = "PUT";
const char sset[]    PROGMEM = "SET";
const char scls[]    PROGMEM = "CLS";
#endif
// Arduino functions
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
const char stone[]    PROGMEM = "ATONE";
#endif
#ifdef HASPULSE
const char splusein[] PROGMEM = "PULSEIN";
#endif
// SD Card DOS functions
#ifdef HASFILEIO
const char scatalog[] PROGMEM = "CATALOG";
const char sdelete[]  PROGMEM = "DELETE";
const char sfopen[]   PROGMEM = "OPEN";
const char sfclose[]  PROGMEM = "CLOSE";
#endif
// low level access functions
#ifdef HASSTEFANSEXT
const char susr[]  PROGMEM = "USR";
const char scall[] PROGMEM = "CALL";
#endif
// mathematics
#ifdef HASFLOAT
const char ssin[]  PROGMEM = "SIN";
const char scos[]  PROGMEM = "COS";
const char stan[]  PROGMEM = "TAN";
const char satan[] PROGMEM = "ATAN";
const char slog[]  PROGMEM = "LOG";
const char sexp[]  PROGMEM = "EXP";
#endif
const char sint[]  PROGMEM = "INT"; // int is always needed 
// elemetars graphics for tft display
#ifdef HASGRAPH
const char scolor[]  PROGMEM  = "COLOR";
const char splot[]   PROGMEM  = "PLOT";
const char sline[]   PROGMEM  = "LINE";
const char scircle[] PROGMEM  = "CIRCLE";
const char srect[]   PROGMEM  = "RECT";
const char sfcircle[] PROGMEM  = "FCIRCLE";
const char sfrect[]   PROGMEM  = "FRECT";
#endif
// Dartmouth BASIC extensions 
#ifdef HASDARTMOUTH
const char sdata[]  	PROGMEM  = "DATA";
const char sread[]  	PROGMEM  = "READ";
const char srestore[]   PROGMEM  = "RESTORE";
const char sdef[] 	PROGMEM  = "DEF";
const char sfn[]   	PROGMEM  = "FN";
const char son[]   	PROGMEM  = "ON";
#endif
// The Darkarts commands unthinkable in Dartmouth
#ifdef HASDARKARTS
const char smalloc[]	PROGMEM  = "MALLOC";
const char sfind[]		PROGMEM  = "FIND";
const char seval[]		PROGMEM  = "EVAL";
#endif
// iot extensions
#ifdef HASIOT
const char siter[]		PROGMEM  = "ITER";
const char savail[]		PROGMEM  = "AVAIL";
#endif

// the keyword storage
const char* const keyword[] PROGMEM = {
// Palo Alto BASIC
	sge, sle, sne, sprint, slet, sinput, 
	sgoto, sgosub, sreturn, sif, sfor, sto,
	sstep, snext, sstop, slist, snew, srun,
	sabs, srnd, ssize, srem,
// Apple 1 BASIC additions
#ifdef HASAPPLE1
	snot, sand, sor, slen, ssgn, speek, sdim,
	sclr, slomem, shimem, stab, sthen, 
	sbend, spoke,
#endif
// Stefan's additions
#ifdef HASSTEFANSEXT
	scont, ssqr, spow, sfre, sdump, sbreak, 
	ssave, sload, sget, sput, sset, scls,
#endif
// Arduino stuff
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
// SD Card DOS
#ifdef HASFILEIO
    scatalog, sdelete, sfopen, sfclose,
#endif
// low level access
#ifdef HASSTEFANSEXT
    susr, scall,
#endif
// mathematical functions 
#ifdef HASFLOAT
    ssin, scos, stan, satan, slog, sexp,
#endif
    sint,
// graphics 
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
#ifdef HASIOT
	siter, savail,
#endif
// the end 
	0
};

// the token dictonary needed for scalability
const signed char tokens[] PROGMEM = {
// Palo Alto BASIC
	GREATEREQUAL, LESSEREQUAL, NOTEQUAL, TPRINT, TLET,    
    TINPUT, TGOTO, TGOSUB, TRETURN, TIF, TFOR, TTO, TSTEP,
    TNEXT, TSTOP, TLIST, TNEW, TRUN, TABS, TRND, TSIZE, TREM,
// this is the Apple 1 language set in addition to Palo Alto (14)
#ifdef HASAPPLE1
    TNOT, TAND, TOR, TLEN, TSGN, TPEEK, TDIM, TCLR, TLOMEM,
    THIMEM, TTAB, TTHEN, TEND, TPOKE,
#endif
// Stefan's tinybasic additions (11)
#ifdef HASSTEFANSEXT
	TCONT, TSQR, TPOW, TFRE, TDUMP, TBREAK, TSAVE, TLOAD, 
	TGET, TPUT, TSET, TCLS,
#endif
// Arduino functions (10)
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
// the SD card DOS functions (4)
#ifdef HASFILEIO
	TCATALOG, TDELETE, TOPEN, TCLOSE,
#endif
// low level access of internal routines
#ifdef HASSTEFANSEXT
	TUSR, TCALL,
#endif
// mathematical functions 
#ifdef HASFLOAT
	TSIN, TCOS, TTAN, TATAN, TLOG, TEXP,
#endif
	TINT,
// graphics - experimental - rudimentary
#ifdef HASGRAPH
	TCOLOR, TPLOT, TLINE, TCIRCLE, TRECT, 
	TFCIRCLE, TFRECT,
#endif
// Dartmouth BASIC extensions 
#ifdef HASDARTMOUTH
	TDATA, TREAD, TRESTORE, TDEF, TFN, TON,
#endif
// the Darkarts commands that shouldn't be there
#ifdef HASDARKARTS
	TMALLOC, TFIND, TEVAL, 
#endif
// IOT extensions
#ifdef HASIOT
	TITER, TAVAIL,
#endif
// the end
	0
};

/*
	the message catalogue also moved to progmem
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
#define ERANGE 		 21
#define ESTRING      22
#define EVARIABLE	 23
#define EFILE 		 24
#define EFUN 		 25
#define EARGS		 26
#define EEEPROM		 27
#define ESDCARD		 28

const char mfile[]    	PROGMEM = "file.bas";
const char mprompt[]	PROGMEM = "> ";
const char mgreet[]		PROGMEM = "Stefan's Basic 1.2";
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
	code for variable numbers and addresses sizes
	the original code was 16 bit but can be extended here
	works but with the tacit assumption that 
	sizeof(number_t) >= sizeof(address_t) 
	floating point here is under construction we always 
	assume that float >= 4 bytes in the following

	maxnum: the maximum accurate(!) integer of a 
		32 bit float 
	strindexsize: the index size of strings either 
		1 byte or 2 bytes - no other values supported

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

	id and od are the input and output model for an arduino
		they are set to serial by default

	fnc counts the depth of for - next loop nesting

	ifile, ofile are the filedescriptors for input/output

*/
static number_t stack[STACKSIZE];
static address_t sp=0; 

static char sbuffer[SBUFSIZE];

static char ibuffer[BUFSIZE] = "\0";
static char *bi;

static number_t vars[VARSIZE];

#if MEMSIZE != 0
static signed char mem[MEMSIZE];
static address_t himem = MEMSIZE-1;
static address_t memsize = MEMSIZE-1;
#else
static signed char* mem;
static address_t himem, memsize;
#endif

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

// this is unsigned hence address_t 
static address_t rd;

// output and input vector
static unsigned char id;
static unsigned char od;

#ifdef STANDALONE
static unsigned char idd = IKEYBOARD;
static unsigned char odd = ODSP;
#else
static unsigned char idd = ISERIAL;
static unsigned char odd = OSERIAL;
#endif

// the runtime debuglevel
char debuglevel = 0;

// data pointer
#ifdef HASDARTMOUTH
address_t data = 0;
#endif

#ifndef ARDUINO
FILE* ifile;
FILE* ofile;
#ifndef MSDOS
// POSIX like OSes
DIR* root;
struct dirent* file; 
#else
// MSDOS to be done 
void* root;
void* file;
#endif 
#else 
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
File ifile;
File ofile;
char tempname[SBUFSIZE];
#ifdef ARDUINOSD
File root;
File file;
#else
Dir root;
File file;
#endif
#ifndef ESP8266
#define FILE_OWRITE (O_READ | O_WRITE | O_CREAT | O_TRUNC)
#else
#define FILE_OWRITE (sdfat::O_READ | sdfat::O_WRITE | sdfat::O_CREAT | sdfat::O_TRUNC)
#endif
#endif
#endif

/* 
	Layer 0 functions 

	variable and memory handling - interface between memory 
 	and variable storage

*/

// heap management 
address_t bmalloc(signed char, char, char, short);
address_t bfind(signed char, char, char);
address_t blength (signed char, char, char);
void clrvars();

// normal variables
void  createvar(char, char);
number_t getvar(char, char);
void  setvar(char, char, number_t);

// low level memory access packing n*8bit bit into n 8 bit objects
// e* is for Arduino EEPROM
void  getnumber(address_t, short);
void  setnumber(address_t, short);
void  egetnumber(address_t, short);
void  esetnumber(address_t, short);

// array handling
address_t createarray(char, char, address_t);
void  array(char, char, char, address_t, number_t*);

// string handling 
address_t createstring(char, char, address_t);
char* getstring(char, char, address_t);
void  setstring(char, char, address_t, char *, address_t);

// access memory dimensions and for strings also the actual length
number_t arraydim(char, char);
number_t stringdim(char, char);
number_t lenstring(char, char);
void setstringlength(char, char, address_t);

// get keywords and tokens from PROGMEM
char* getkeyword(unsigned short);
char* getmessage(char);
signed char gettokenvalue(char);
void printmessage(char);

// error handling
void error(signed char);
void reseterror();
void debugtoken();
void debug(char*);

// stack stuff
void push(number_t);
number_t pop();
void drop();
void clearst();

// generic display code - used for Shield, I2C, and TFT
void dspwrite(char);
void dspbegin();
char dspwaitonscroll();
char dspactive();
void dspsetscrollmode(char, short);
void dspsetcursor(short, short);

// real time clock and wire code 
void rtcset(char, short);
short rtcget(char);
short rtcread(char);

// input output
// these are the platfrom depended lowlevel functions
void serialbegin();
void prtbegin();
void ioinit();
void iodefaults();
void picogetchar(int);
void outch(char);
char inch();
char checkch();
short availch();
void ins(char*, short); 
void inb(char*, short);

// RF24 radio input 
void iradioopen(char*);
void oradioopen(char*);
void radioins(char*, short);
void radioouts(char* , short);

// generic wire access
void wirebegin();
void wireopen(char* s);
void wireins(char*, uint8_t);
void wireouts(char*, uint8_t);

// from here on the functions only use the functions above
// there should be no platform depended code here
void outcr();
void outspc();
void outs(char*, short);
void outsc(char*);
void outscf(char *, short);
char innumber(number_t*);
short parsenumber(char*, number_t*);
short parsenumber2(char*, number_t*);
void outnumber(number_t);
short writenumber(char*, number_t);
short writenumber2(char*, number_t);

/*
	Arduino definitions and code
	wrapper functions for the Arduino libraries
	Arduino definitions and code
*/

/* Arduino Real Time clock code */
#ifdef ARDUINORTC
#include <uRTCLib.h>
uRTCLib rtc(0x68);
#ifdef ARDUINO_ARCH_ESP8266
// D3 and D4 on ESP8266 - taken from uRTCLIB examples, needs to be isolated
const char espwire_sda=0;
const char espwire_scl=2;
#endif
// commen DS3231 modules have a build in EEPROM addressable via I2C 0x57
// this is very raw - definitions set here by hand
#include <uEEPROMLib.h>
#define RTCEEPROMSIZE 4096 
uEEPROMLib c_eeprom(0x57);
#else
#define RTCEEPROMSIZE 0
#endif

/* the plain Wire library, there is a conflict here as some 
   other libraries also call Wire, currently unresolved */
#ifdef ARDUINOWIRE
#include <Wire.h>
#endif

/*
	 EEPROM handling 
*/
void ebegin(){}
void eflush(){}
// the DS3231 module EEPROM is the only one present
#if defined(ARDUINORTC) && ! defined(ARDUINOEEPROM)
address_t elength() { return RTCEEPROMSIZE; }
short eread(address_t i) { return (signed char) c_eeprom.eeprom_read(i); }
void eupdate(address_t i, short c) { 
	short b = eread(i);
	if (b != c) c_eeprom.eeprom_write(i, (signed char) c);
}
#endif
// only the internal Arduino EEPROM
#if defined(ARDUINOEEPROM) && ! defined(ARDUINORTC)
address_t elength() { return EEPROM.length(); }
void eupdate(address_t i, short c) { EEPROM.update(i, c); }
short eread(address_t i) { return (signed char) EEPROM.read(i); }
#endif
// the RTC EEPROM extends the internal EEPROM
#if defined(ARDUINOEEPROM) && defined(ARDUINORTC)
address_t elength() { return EEPROM.length()+RTCEEPROMSIZE; }
void eupdate(address_t i, short c) { 
	if (i<elength()) 
		EEPROM.update(i, c); 
	else {
		short b = eread(i);
		if (b != c) c_eeprom.eeprom_write(i, (signed char) c);
	}	
}
short eread(address_t i) { 
	if (i<elength())
		return (signed char) EEPROM.read(i); 
	else
		return (signed char) c_eeprom.eeprom_read(i); 
}
#endif
// no EEPROM present
#if ! defined(ARDUINOEEPROM) && !defined(ARDUINORTC)
address_t elength() { return 0; }
void eupdate(address_t i, short c) { return; }
short eread(address_t i) { return 0; }
#endif

// save a file to EEPROM
void esave() {
	address_t a=0;
	if (top+eheadersize < elength()) {
		a=0;
		eupdate(a++, 0); // EEPROM per default is 255, 0 indicates that there is a program

		// store the size of the program in byte 1,2 of the EEPROM	
		z.a=top;
		esetnumber(a, addrsize);
		a+=addrsize;

		while (a < top+eheadersize){
			eupdate(a, mem[a-eheadersize]);
			a++;
		}
		eupdate(a++,0);
	} else {
		error(EOUTOFMEMORY);
		er=0; //oh oh! check this.
	}
}

// load a file from EEPROM
void eload() {
	address_t a=0;
	if (elength()>0 && (eread(a) == 0 || eread(a) == 1)) { // have we stored a program
		a++;

		// how long is it?
		egetnumber(a, addrsize);
		top=z.a;
		a+=addrsize;

		while (a < top+eheadersize){
			mem[a-eheadersize]=eread(a);
			a++;
		}
	} else { // no valid program data is stored 
		error(EEEPROM);
	}
}

/*
	global variables for the keyboard
	heuristic here - with and without TFT shield 
	needs to be changed according to hw config
	ESP added as well making it even more complex
*/
#ifdef ARDUINOPS2
#ifdef ARDUINOTFT 
#ifdef ARDUINO_SAM_DUE
const int PS2DataPin = 9;
const int PS2IRQpin =  8;
PS2Keyboard keyboard;
#else
const int PS2DataPin = 18;
const int PS2IRQpin =  19;
PS2Keyboard keyboard;
#endif
#else
#ifdef ESP8266
const int PS2DataPin = 0;
const int PS2IRQpin =  2;
PS2Kbd keyboard(PS2DataPin, PS2IRQpin);
#else
const int PS2DataPin = 3;
const int PS2IRQpin =  2;
PS2Keyboard keyboard;
#endif
#endif
#endif

// global variables for a standard LCD shield.
#ifdef LCDSHIELD
#define DISPLAYDRIVER
#include <LiquidCrystal.h>
// LCD shield pins to Arduino
//  RS, EN, d4, d5, d6, d7; 
// backlight on pin 10;
const int dsp_rows=2;
const int dsp_columns=16;
LiquidCrystal lcd( 8,  9,  4,  5,  6,  7);
void dspbegin() { 	lcd.begin(dsp_columns, dsp_rows); dspsetscrollmode(1, 1);  }
void dspprintchar(char c, short col, short row) { lcd.setCursor(col, row); lcd.write(c);}
void dspclear() { lcd.clear(); }
#endif

// global variables for a LCD display connnected
// via i2c. 
#ifdef ARDUINOLCDI2C
#define DISPLAYDRIVER
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
const int dsp_rows=4;
const int dsp_columns=20;
LiquidCrystal_I2C lcd(0x27, dsp_columns, dsp_rows);
void dspbegin() {   lcd.init(); lcd.backlight(); dspsetscrollmode(1, 1); }
void dspprintchar(char c, short col, short row) { lcd.setCursor(col, row); lcd.write(c);}
void dspclear() { lcd.clear(); }
#endif
/*
	global variables for a TFT
	this is code for a SD1963 800*480 board using the UTFT library
	it is mainly intended for a MEGA or DUE as a all in one system
	this is for a MEGA shield and the CTE DUE shield, for the due 
	you need to read the comment in Arduino/libraries/UTFT/hardware/arm
	HW_ARM_defines.h -> uncomment the DUE shield
*/
#ifdef ARDUINOTFT
#include <memorysaver.h>
#include <UTFT.h>
#define DISPLAYDRIVER
extern uint8_t SmallFont[];
extern uint8_t BigFont[];
#ifdef ARDUINO_SAM_DUE
UTFT tft(CTE70,25,26,27,28);
#else 
UTFT tft(CTE70,38,39,40,41);
#endif
const int dsp_rows=30;
const int dsp_columns=50;
char dspfontsize = 16;
void dspbegin() { tft.InitLCD(); tft.setFont(BigFont); tft.clrScr(); dspsetscrollmode(1, 4); }
void dspprintchar(char c, short col, short row) { tft.printChar(c, col*dspfontsize, row*dspfontsize); }
void dspclear() { tft.clrScr(); }
//experimental graphics code 
#ifdef HASGRAPH
void color(int r, int g, int b) { tft.setColor(r,g,b); }
void plot(int x, int y) { tft.drawPixel(x, y); }
void line(int x0, int y0, int x1, int y1)   { tft.drawLine(x0, y0, x1, y1); }
void rect(int x0, int y0, int x1, int y1)   { tft.drawRect(x0, y0, x1, y1); }
void frect(int x0, int y0, int x1, int y1)   { tft.fillRect(x0, y0, x1, y1); }
void circle(int x0, int y0, int r) { tft.drawCircle(x0, y0, r); }
void fcircle(int x0, int y0, int r) { tft.fillCircle(x0, y0, r); }
#endif
#endif

// global variables for an Arduino SD card, chipselect 
// depends on the shield. 
#ifdef ARDUINOSD
// the SD chip select, set 4 for the Ethernet/SD shield
// and 53 for all configurations of a MEGA
#if defined(ARDUINOTFT) || defined(ARDUINO_AVR_MEGA2560)
const char sd_chipselect = 53;
#else
const char sd_chipselect = 4;
#endif
#endif

// definitions for the nearfield module, still very experimental
#if defined(ARDUINORF24) && defined(ARDUINO) 
const char rf24_ce = 8;
const char rf24_csn = 9;
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
rf24_pa_dbm_e rf24_pa = RF24_PA_MAX;
RF24 radio(rf24_ce, rf24_csn);
#endif

// the wrappers of the arduino io functions, to avoid 
// spreading arduino code in the interpreter code 
// also, this would be the place to insert the Wiring code
// for raspberry
#ifdef ARDUINO
void aread(){ push(analogRead(pop())); }
void dread(){ push(digitalRead(pop())); }
void awrite(number_t p, number_t v){
	if (v >= 0 && v<256) analogWrite(p, v);
	else error(ERANGE);
}
void dwrite(number_t p, number_t v){
	if (v == 0) digitalWrite(p, LOW);
	else if (v == 1) digitalWrite(p, HIGH);
	else error(ERANGE);
}
void pinm(number_t p, number_t m){
	if (m>=0 && m<=2)  pinMode(p, m);
	else error(ERANGE); 
}
void bmillis() {
	number_t m;
	// millis is processed as integer and is cyclic mod maxnumber and not cast to float!!
	m=(number_t) (millis()/(unsigned long)pop() % (unsigned long)maxnum);
	push(m); 
};
void bpulsein() { 
  unsigned long t, pt;
  t=((unsigned long) pop())*1000;
  y=pop(); 
  x=pop(); 
  pt=pulseIn(x, y, t)/10; 
  push(pt);
}
#else
void aread(){ return; }
void dread(){ return; }
void awrite(number_t p, number_t v){}
void dwrite(number_t p, number_t v){}
void pinm(number_t p, number_t m){}
#ifndef MSDOS
#ifndef MINGW
void delay(number_t t) {usleep(t*1000);}
#else
void delay(number_t t) {Sleep(t);}
#endif
#endif
struct timeb start_time;
void bmillis() {
	struct timeb thetime;
	time_t dt;
	number_t m;
	ftime(&thetime);
	dt=(thetime.time-start_time.time)*1000+(thetime.millitm-start_time.millitm);
	m=(number_t) ( dt/(time_t)pop() % (time_t)maxnum);
	push(m);
}
void bpulsein() { pop(); pop(); pop(); push(0); }
#endif

/* start a secondary serial port for printing and/or networking */
#ifdef ARDUINOPRT
#if !defined(ARDUINO_AVR_MEGA2560) && !defined(ARDUINO_SAM_DUE)
#include <SoftwareSerial.h>
/* definition of the serial port pins from "pretzel board"
for UNO 11 is not good for rx */
const int software_serial_rx = 11;
const int software_serial_tx = 12;
SoftwareSerial Serial1(software_serial_rx, software_serial_tx);
#endif
#endif


/* 	
	Layer 1 function, provide data and do the heavy lifting 
	for layer 2.
*/

// lexical analysis
void whitespaces();
void debugtoken();
void nexttoken();

// storeing and retrieving programs
char nomemory(number_t);
void dumpmem(address_t, address_t);
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

// read arguments from the token stream.
char  termsymbol();
void  parsesubstring();
short parsesubscripts();
void  parsenarguments(char);
short parsearguments();

// mathematics and other functions / int and float
void rnd();
void sqr();
void xpow();
void fre();
void xpeek();
void xabs();
void xsgn();

// string values 
char stringvalue();
void streval();

// expression evaluation 
void factor();
void term();
void addexpression();
void compexpression();
void notexpression();
void andexpression();
void expression();

/* 
	Layer 2 - statements call Layer 1 functions and 
	use the global variables 
*/

// basic commands of the core language set
void xprint();
void assignment();
void lefthandside(address_t*, char*);
void assignnumber(signed char, char, char, address_t, char);
void xinput();
void xgoto();
void xreturn();
void xif();

// optional FOR NEXT loops
void findnextcmd();
void xfor();
void xnext();
void xbreak();
void pushforstack();
void popforstack();
void dropforstack();
void clrforstack();

// GOSUB commands
void xreturn();
void pushgosubstack();
void popgosubstack();
void dropgosubstack();
void clrgosubstack();

// control commands and misc
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

// file access 
void stringtosbuffer();
void getfilename(char*, char);
void xsave();
void xload();
void xcatalog();
void xdelete();
void xopen();
void xclose();

// low level I/O in BASIC
void xget();
void xput();
void xset();

// Arduino IO control
void xdwrite();
void xawrite();
void xpinm();
void xdelay();
void xtone();

// low level access functions
void xcall();
void xusr();

// the dartmouth stuff
void xdata();
void xread();
void xrestore();
void xdef();
void xon();
void clrdata();

// the darkarts
void xmalloc();
void xfind();
void xeval();
void xiter();
void xavail();

// the statement loop
void statement();

/* 
 	Layer 0 - low level IO functions 
*/







/*

	this is a generic display code 
	it combines the functions of LCD and TFT drivers
	if this code is active 
	dspprintchar(char c, short col, short row)
	dspclear()
	dspbegin()
	dspclear()
	have to be defined before in a hardware dependent section
*/

#ifdef DISPLAYDRIVER
short dspmycol = 0;
short dspmyrow = 0;
char esc = 0;

void dspsetcursor(short c, short r) {
	dspmyrow=r;
	dspmycol=c;
}

char dspactive() {
	return od & ODSP;
}

#ifdef HASVT52
// nano vt52 state engine
char vt52s = 0;

void dspvt52(char* c){
  
	switch (vt52s) {
		case 'Y':
			if (esc == 2) { dspmyrow=(*c-31)%dsp_rows; esc=1; return;}
			if (esc == 1) { dspmycol=(*c-31)%dsp_columns; *c=0; }
      		vt52s=0; 
			break;
	}
 
	switch (*c) {
		case 'A': // cursor up
			if (dspmyrow>0) dspmyrow--;
			break;
		case 'B': // cursor down
		    dspmyrow=(dspmyrow++) % dsp_rows;
			break;
		case 'C': // cursor right
			dspmycol=(dspmycol++) % dsp_columns;
			break; 
		case 'D': // cursor left
			if (dspmycol>0) dspmycol--;
			break;
		case 'E': // GEMDOS / TOS extension clear screen
		    dspbufferclear();
    		dspclear();
			break;
		case 'H': // cursor home
		    dspmyrow=0;
    		dspmycol=0;
			break;	
		case 'Y': // Set cursor position
			vt52s='Y';
			esc=2;
      		*c=0;
			return;
		case 'F': // enter graphics mode
		case 'G': // exit graphics mode
		case 'I': // reverse line feed
		case 'J': // clear to end of screen
		case 'K': // clear to end of line
		case 'L': // Insert line
		case 'M': // Delete line
		case 'Z': // Ident
		case '=': // alternate keypad on
		case '>': // alternate keypad off
		case 'b': // GEMDOS / TOS extension text color
		case 'c': // GEMDOS / TOS extension background color
		case 'd': // GEMDOS / TOS extension clear to start of screen
		case 'e': // GEMDOS / TOS extension enable cursor
		case 'f': // GEMDOS / TOS extension disable cursor
		case 'j': // GEMDOS / TOS extension restore cursor
		case 'k': // GEMDOS / TOS extension save cursor
		case 'l': // GEMDOS / TOS extension clear line
		case 'o': // GEMDOS / TOS extension clear to start of line		
		case 'p': // GEMDOS / TOS extension reverse video
		case 'q': // GEMDOS / TOS extension normal video
		case 'v': // GEMDOS / TOS extension enable wrap
		case 'w': // GEMDOS / TOS extension disable wrap
		case '^': // Printer extensions - print on
		case '_': // Printer extensions - print off
		case 'W': // Printer extensions - print without display on
		case 'X': // Printer extensions - print without display off
		case 'V': // Printer extensions - print cursor line
		case ']': // Printer extension - print screen 
			break;
	}
      esc=0;
      *c=0;
}
#endif

#ifdef DISPLAYCANSCROLL
char dspbuffer[dsp_rows][dsp_columns];
char  dspscrollmode = 0;
short dsp_scroll_rows = 1; 

// 0 normal scroll
// 1 enable waitonscroll function
// ... more to come
void dspsetscrollmode(char c, short l) {
	dspscrollmode = c;
	dsp_scroll_rows = l;
}

void dspbufferclear() {
	short r,c;
	for (r=0; r<dsp_rows; r++)
		for (c=0; c<dsp_columns; c++)
      		dspbuffer[r][c]=0;
  	dspmyrow=0;
  	dspmycol=0;
}

void dspscroll(){
	short r,c;
	int i;
  	char a,b;

  	// shift dsp_scroll_rows up
  	for (r=0; r<dsp_rows-dsp_scroll_rows; r++) { 
    	for (c=0; c<dsp_columns; c++) {
        	a=dspbuffer[r][c];
        	b=dspbuffer[r+dsp_scroll_rows][c];
        	if ( a != b ) {
            	if (b >= 32) dspprintchar(b, c, r); else dspprintchar(' ', c, r);
        	}      
        	dspbuffer[r][c]=b;
    	} 
    }

    // delete the characters in the remaining lines
  	for (r=dsp_rows-dsp_scroll_rows; r<dsp_rows; r++) {
    	for (c=0; c<dsp_columns; c++) {
			if (dspbuffer[r][c] > 32) dspprintchar(' ', c, r); 
        	dspbuffer[r][c]=0;     
    	}
    }
  
	// set the cursor to the fist free line	    
  	dspmycol=0;
    dspmyrow=dsp_rows-dsp_scroll_rows;
}

void dspwrite(char c){

#ifdef HASVT52
    // escape character received - we switch to vt52 mode
    // the character is modified and then handed over to the
    // internal pipeline
    if (esc) dspvt52(&c);
#endif

    // will be reworked soon - not super good 
  	switch(c) {
  		case 10: // this is LF Unix style doing also a CR
    		dspmyrow=(dspmyrow + 1);
    		if (dspmyrow >= dsp_rows) dspscroll(); 
    		dspmycol=0;
    		return;
    	case 12: // form feed is clear screen - deprecated
    		dspbufferclear();
    		dspclear();
    		return;
    	case 13: // classical carriage return 
    		dspmycol=0;
    		return;
        case 27: // escape - initiate vtxxx mode
        	esc=1;
        	return;
    	case 127: // delete
    		if (dspmycol > 0) {
      			dspmycol--;
      			dspbuffer[dspmyrow][dspmycol]=0;
      			dspprintchar(' ', dspmycol, dspmyrow);
      			return;
    		}
  	}

	if (c < 32 ) return; 

	dspprintchar(c, dspmycol, dspmyrow);
	dspbuffer[dspmyrow][dspmycol++]=c;
	if (dspmycol == dsp_columns) {
		dspmycol=0;
		dspmyrow=(dspmyrow + 1);
    	if (dspmyrow >= dsp_rows) dspscroll(); 
	}
}

char dspwaitonscroll() {
  	char c;

	if ( dspscrollmode == 1 ) {
		if (dspmyrow == dsp_rows-1) {
        	c=inch();
        	if (c == ' ') outch(12);
		    return c;
		}
	}
	return 0;
}

// code for low memory simple display access - can still be simplified
#else 

void dspbufferclear() {}

char dspwaitonscroll() {
	return 0;
}

void dspwrite(char c){

#ifdef HASVT52
	// escape character received - we switch to vt52 mode
    // the character is modified and then handed over to the
    // internal pipeline
    if (esc) { dspvt52(&c); }
#endif


 	switch(c) {
  		case 12: // form feed is clear screen
    		dspclear();
    		return;
  		case 10: // this is LF Unix style doing also a CR
    		dspmyrow=(dspmyrow + 1)%dsp_rows;
    		dspmycol=0;
    		return;
    	case 11: // one char down 
    		dspmyrow=(dspmyrow+1) % dsp_rows;
    		return;
    	case 13: // classical carriage return 
    		dspmycol=0;
    		return;
        case 27: // escape - initiate vtxxx mode
        	esc=1;
        	return;
    	case 127: // delete
    		if (dspmycol > 0) {
      			dspmycol--;
      			dspprintchar(' ', dspmycol, dspmyrow);
    		}
    		return;
  	}

	if (c < 32 ) return; 

	dspprintchar(c, dspmycol++, dspmyrow);
	dspmycol=dspmycol%dsp_columns;
}

void dspsetscrollmode(char c, short l) {}
#endif
#else
const int dsp_rows=0;
const int dsp_columns=0;
void dspwrite(char c){};
void dspbegin() {};
char dspwaitonscroll() { return 0; };
char dspactive() {return FALSE; }
void dspsetscrollmode(char c, short l) {}
void dspsetcursor(short c, short r) {}
#endif


/*
	Layer 0 function - variable handling.

	These function access variables, 
	In this implementation variables are a 
	static array and we simply subtract 'A' 
	from the variable name to get the index
	any other way to generate an index from a 
	byte hash can be used just as well.

	delvar and createvar as stubs for further 
	use. They are not yet used consistenty in
	the code.
 */

#if MEMSIZE == 0
// guess the possible basic memory size
void ballocmem() {
	signed char i = 0;
	// 									RP2040      ESP        MK       MEGA    UNO  168  FALLBACK
	const unsigned short memmodel[9] = {60000, 44000, 32000, 24000, 6000, 4096, 1024, 512, 128}; 

	if (sizeof(number_t) <= 2) i=3;
// this is tiny model MSDOS compile - dos chrashes on 
// file access with 60 k allocated in a tiny model
#ifdef MSDOS
	i=1;
#endif
	if (sizeof(number_t) <= 2) i=3;

	do {
		mem=(signed char*)malloc(memmodel[i]);
		if (mem != NULL) break;
		i++;
	} while (i<9);
	memsize=memmodel[i]-1;
}
#endif

#ifdef HASAPPLE1
/*
	allocate a junk of memory for a variable on the heap
	every objects is identified by name (c,d) and type t
	3 bytes are used here but 2 would be enough
*/
address_t bmalloc(signed char t, char c, char d, short l) {

	address_t vsize;     // the length of the header
	address_t b;

    if (DEBUG) { outsc("** bmalloc with token "); outnumber(t); outcr(); }

    /*
		check if the object already exists
    */
    if (bfind(t, c, d) != 0 ) { error(EVARIABLE); return 0; };

	/* 
		how much space is needed
			3 bytes for the token and the 2 name characters
			numsize for every number including array length
			one byte for every string character
	*/

    switch(t) {
    	case VARIABLE:
    		vsize=numsize+3;
    		break;
    	case ARRAYVAR:
    		vsize=numsize*l+addrsize+3;
    		break;
    	case TFN:
    		vsize=addrsize+2+3;
    		break;
    	default:
    		vsize=l+addrsize+3;
    }
	
    /* enough memory ? */ 
	if ( (himem - top) < vsize) { error(EOUTOFMEMORY); return 0;}

	/* here we could create a hash, currently simplified
	 the hash is the first digit of the variable plus the token */

	b=himem;
	mem[b--]=c;
	mem[b--]=d;
	mem[b--]=t;

	/* for strings, arrays and buffers write the (maximum) length 
	   directly after the header */
	if (t == ARRAYVAR || t == STRINGVAR || t == TBUFFER) {
		b=b-addrsize+1;
		z.a=vsize-(addrsize+3);
		setnumber(b, addrsize);

		b--;
	}

	/* reserve space for the payload */
	himem-=vsize;
	nvars++;

	return himem+1;
}

/*
	bfind passes back the location of the object as result
	the length of the object is in z.i as a side effect 
*/
address_t bfind(signed char t, char c, char d) {

	address_t b = memsize;
	signed char t1;
	char c1, d1;
	short i=0;

	while (i < nvars) { 

		c1=mem[b--];
		d1=mem[b--];
		t1=mem[b--];

		switch(t1) {
			case VARIABLE:
				z.a=numsize;
				break;
			case TFN:
				z.a=addrsize+2;
				break;
			default:
				b=b-addrsize+1;
				getnumber(b, addrsize);
				b--;
		}

		b-=z.a;

		if (c1 == c && d1 == d && t1 == t) return b+1;
		i++;
	}

	return 0;

}

// the length of an object
address_t blength (signed char t, char c, char d) {
	if (bfind(t, c, d)) return z.a; else return 0;
}
#endif


// ununsed so far, simple variables are created on the fly
void createvar(char c, char d){
	return;
}

// get and create a variable 
number_t getvar(char c, char d){
	address_t a;

	if (DEBUG) { outsc("* getvar "); outch(c); outch(d); outspc(); outcr(); }

	// the static variable array
	if (c >= 65 && c<=91 && d == 0) return vars[c-65];

	// the special variables 
	if ( c == '@' )
		switch (d) {
			case 'S': 
				return ert;
			case 'I':
				return id;
			case 'O':
				return od;
			case 'C':
				if (availch()) return inch(); else return 0;
			case 'E':
				return elength();
			case 'R':
				return rd;
#ifdef DISPLAYDRIVER
			case 'X':
				return dspmycol;
			case 'Y':
				return dspmyrow;
#endif
		}

#ifdef HASAPPLE1
	/* dynamically allocated vars, create them on the fly if needed */
	if (!(a=bfind(VARIABLE, c, d))) a=bmalloc(VARIABLE, c, d, 0);
	if (er != 0) return 0;
	
	/* retrieve the value */
	getnumber(a, numsize);
	return z.i;
#else
	error(EVARIABLE);
	return 0;
#endif
}

// set and create a variable 
void setvar(char c, char d, number_t v){
	address_t a;

	if (DEBUG) { outsc("* setvar "); outch(c); outch(d); outspc(); outnumber(v); outcr(); }

	// the static variable array
	if (c >= 65 && c<=91 && d == 0) {
		vars[c-65]=v;
		return;
	}

	// the special variables 
	if ( c == '@' )
		switch (d) {
			case 'S': 
				ert=v;
				return;
			case 'I':
				id=v;
				return;
			case 'O':
				od=v;
				return;
			case 'C':
				outch(v);
				return;
			case 'R':
				rd=v;
				return;
#ifdef DISPLAYDRIVER
			case 'X':
				dspmycol=(short)v%dsp_columns;
				return;
			case 'Y':
				dspmyrow=(short)v%dsp_rows;
				return;
#endif
		}

#ifdef HASAPPLE1
	/* dynamically allocated vars */
	if ( !(a=bfind(VARIABLE, c, d))) a=bmalloc(VARIABLE, c, d, 0);
	if (er != 0) return;

	/* set the valus */
	z.i=v;
	setnumber(a, numsize);
#else 	
	error(EVARIABLE);
#endif
}

// clr all variables 
void clrvars() {
	signed char i;
	for (i=0; i<VARSIZE; i++) vars[i]=0;
	nvars=0;
	himem=memsize;
}

/*  the program memory access */
void getnumber(address_t m, short n){
	signed char i;

	z.i=0;

	switch (n) {
		case 1:
			z.i=mem[m];
			break;
		case 2:
			z.b.l=mem[m++];
			z.b.h=mem[m];
			break;
		default:
			for (i=0; i<n; i++) z.c[i]=mem[m++];
	}

	// for (i=0; i<n; i++) z.c[i]=mem[m++];

}

/* the eeprom memory access */
void egetnumber(address_t m, short n){
	signed char i;

	z.i=0;

	switch (n) {
		case 1:
			z.i=eread(m);
			break;
		case 2:
			z.b.l=eread(m++);
			z.b.h=eread(m);
			break;
		default:
			for (i=0; i<n; i++) z.c[i]=eread(m++);
	}
}

/* set a number at a memory location */
void setnumber(address_t m, short n){
	signed char i;

	switch (n) {
		case 1:
			mem[m]=z.i;
			break;
		case 2: 
			mem[m++]=z.b.l;
			mem[m]=z.b.h;
			break;
		default:
 			for (i=0; i<n; i++) mem[m++]=z.c[i];
	}
}

/* set a number at a eepromlocation */
void esetnumber(address_t m, short n){
	signed char i; 

	switch (n) {
		case 1:
			eupdate(m, z.i);
			break;
		case 2: 
			eupdate(m++, z.b.l);
			eupdate(m, z.b.h);
			break;
		default:
 			for (i=0; i<n; i++) eupdate(m++, z.c[i]);
	}
}

/* create an array */
address_t createarray(char c, char d, address_t i) {
#ifdef HASAPPLE1
	if (DEBUG) { outsc("* create array "); outch(c); outch(d); outspc(); outnumber(i); outcr(); }
	if (bfind(ARRAYVAR, c, d)) error(EVARIABLE); else return bmalloc(ARRAYVAR, c, d, i);
#endif
	return 0;
}

/* generic array access function */
void array(char m, char c, char d, address_t i, number_t* v) {

	address_t a;
	address_t h;
	char e = FALSE;

	if (DEBUG) { outsc("* get/set array "); outch(c); outspc(); outnumber(i); outcr(); }

	/* special arrays EEPROM, Display and Wang */
	if (c == '@') {
		switch(d) {
			case 'E': {
				h=elength()/numsize;
				a=elength()-numsize*i;
				e=TRUE;
				break;
			}
			case 'D': {
#if defined(DISPLAYDRIVER) && defined(DISPLAYCANSCROLL)
        		if (dsp_rows == 0 || dsp_columns == 0) { return; }
        		if (i<1 || i>dsp_columns*dsp_columns ) { return; }
        		i--;
        		a=i%dsp_columns;
        		h=i/dsp_columns;
        		// this should be encapsulated later into the display object
        		if (m == 's') {
          			if (*v == 0) e=' '; else e=*v;
          			dspprintchar(e, a, h);
          			if (*v == 32) dspbuffer[h][a]=0; else dspbuffer[h][a]=*v;
        		} else {
          			*v=(number_t)dspbuffer[h][a];          			
        		}
#endif
				return;
			}
			case 'T':
				if (m == 'g') *v=rtcget(i); 
				else if (m == 's') rtcset(i, *v);
				return;
			case 0: 
			default: {
				h=(himem-top)/numsize;
				a=himem-numsize*(i)+1;
				break;
			}
		}
	} else {
#ifdef HASAPPLE1
		/* dynamically allocated arrays autocreated if needed */ 
		if ( !(a=bfind(ARRAYVAR, c, d)) ) a=createarray(c, d, ARRAYSIZEDEF);
		if (er != 0) return;
		h=z.a/numsize;
		a=a+(i-1)*numsize;
#else
		error(EVARIABLE); 
		return;
#endif
	}


	// is the index in range 
	if ( (i < 1) || (i > h) ) { error(ERANGE); return; }

	// set or get the array
	if (m == 'g') {
		if (! e) { getnumber(a, numsize); } else { egetnumber(a, numsize); }
		*v=z.i;
	} else if ( m == 's') {
		z.i=*v;
		if (! e) { setnumber(a, numsize); } else { esetnumber(a, numsize); }
	}
}

address_t createstring(char c, char d, address_t i) {
#ifdef HASAPPLE1
	address_t a;
	if (DEBUG) { outsc("Create string "); outch(c); outch(d); outspc(); outnumber(nvars); outcr(); }
	if (bfind(STRINGVAR, c, d)) error(EVARIABLE); else a=bmalloc(STRINGVAR, c, d, i+strindexsize);
	if (er != 0) return 0;
	return a;
#else 
	return 0;	
#endif
}

// the -1+stringdexsize is needed because a string index starts with 1
char* getstring(char c, char d, address_t b) {	
	address_t a;

	if (DEBUG) { outsc("* get string var "); outch(c); outch(d); outspc(); outnumber(b); outcr(); }

	// direct access to the input buffer
	if ( c == '@')
			return ibuffer+b;

#ifdef HASAPPLE1
	// dynamically allocated strings
	if (! (a=bfind(STRINGVAR, c, d)) ) a=createstring(c, d, STRSIZEDEF);

	if (DEBUG) { outsc("** heap address "); outnumber(a); outcr(); }
	if (DEBUG) { outsc("** byte length "); outnumber(z.a); outcr(); }

	if (er != 0) return 0;

	if ( (b < 1) || (b > z.a-strindexsize ) ) {
		error(ERANGE); return 0;
	}

	a=a+b-1+strindexsize;

	if (DEBUG) { outsc("** payload address address "); outnumber(a); outcr(); }

	return (char *)&mem[a];
#else 
	return 0;
#endif
}
 
 // this function is currently not used 
number_t arraydim(char c, char d) {
	if (c == '@')
		switch (d) {
			case 0:
				return (himem-top)/numsize;
			case 'E':
				return elength()/numsize;
		}
#ifdef HASAPPLE1
	return blength(ARRAYVAR, c, d)/numsize;
#else 
	return 0;
#endif
}

#ifdef HASAPPLE1
number_t stringdim(char c, char d) {
	if (c == '@')
		return BUFSIZE-1;

	return blength(STRINGVAR, c, d)-strindexsize;
}

number_t lenstring(char c, char d){
	char* b;
	number_t a;

	if (c == '@')
		return ibuffer[0];
	
	a=bfind(STRINGVAR, c, d);
	if (er != 0) return 0;

	getnumber(a, strindexsize);
	return z.a;
}

void setstringlength(char c, char d, address_t l) {
	address_t a; 

	if (c == '@') {
		*ibuffer=l;
		return;
	}

	a=bfind(STRINGVAR, c, d);
	if (er != 0) return;
	if (a == 0) {
		error(EVARIABLE);
		return;
	}

	if (l < z.a) {
		z.a=l;
		setnumber(a, strindexsize);
		//mem[a]=l;
	} else
		error(ERANGE);

}

void setstring(char c, char d, address_t w, char* s, address_t n) {
	char *b;
	address_t a;
	int i;

	if (DEBUG) { outsc("* set var string "); outch(c); outch(d); outspc(); outnumber(w); outcr(); }

	if ( c == '@') {
		b=ibuffer;
	} else {
		if ( !(a=bfind(STRINGVAR, c, d)) ) a=createstring(c, d, STRSIZEDEF);
		if (er != 0) return;
		b=(char *)&mem[a+strindexsize];
	}

	if ( (w+n-1) <= stringdim(c, d) ) {
		for (i=0; i<n; i++) { b[i+w]=s[i]; } 
		z.a=w+n-1;
		setnumber(a, strindexsize);
		//b[0]=w+n-1; 	
	}
	else 
		error(ERANGE);
}
#endif

/* 
	Layer 0 - keyword handling - PROGMEM logic goes here
		getkeyword is the only access to the keyword array
		in the code.  

		Same for messages and errors

		(here construction site right now)
*/ 

char* getkeyword(unsigned short i) {

	if (DEBUG) { outsc("** getkeyword from index "); outnumber(i); outcr(); }

#ifndef ARDUINOPROGMEM
	return (char *) keyword[i];
#else
	strcpy_P(sbuffer, (char*) pgm_read_ptr(&(keyword[i]))); 
	return sbuffer;
#endif
}

char* getmessage(char i) {
	if (i >= sizeof(message)) return 0;
#ifndef ARDUINOPROGMEM
	return (char *) message[i];
#else
	strcpy_P(sbuffer, (char*) pgm_read_ptr(&(message[i]))); 
	return sbuffer;
#endif
}

signed char gettokenvalue(char i) {
	if (i >= sizeof(tokens)) return 0;
#ifndef ARDUINOPROGMEM
	return tokens[i];
#else
	return (signed char) pgm_read_byte(&tokens[i]);
#endif
}


void printmessage(char i){
#ifndef HASERRORMSG
	if (i > EGENERAL) return;
#endif
	outsc((char *)getmessage(i));
}

/*
  Layer 0 - error handling

  The general error handler. The static variable er
  contains the error state. 

  Strategy: the error function writes the message and then 
  clears the stack. All calling functions must check er and 
  return after funtion calls with no further messages etc.
  reseterror sets the error state to normal and end the 
  run loop.

*/ 

void error(signed char e){
	er=e;
	// set input and output device back to default
	od=odd;
	id=idd;
	form=0;
	// find the line number
	if (st != SINT) {
		outnumber(myline(here));
		outch(':');
		outspc();
	}
#ifdef HASERRORMSG
	printmessage(e);
	outspc();
#endif
	printmessage(EGENERAL);
	outcr();
	clearst();
	clrforstack();
	clrgosubstack();
	iodefaults();
}

void reseterror() {
	er=0;
	here=0;
	st=SINT;
}

void debugtoken(){
	outsc("* ");

	if (token == EOL) {
		outsc("EOL\n");
		return;	
	}
	switch(token) {
		case LINENUMBER: 
			printmessage(MLINE);
			break;
		case NUMBER:
			printmessage(MNUMBER);
			break;
		case VARIABLE:
			printmessage(MVARIABLE);
			break;	
		case ARRAYVAR:
			printmessage(MARRAY);
			break;		
		case STRING:
			printmessage(MSTRING);
			break;
		case STRINGVAR:
			printmessage(MSTRINGVAR);
			break;
	}
	outspc();
	outputtoken();
}

void debug(char *c){
	outch('*');
	outspc();
	outsc(c); 
	debugtoken();
	outcr();
}

/*
	Arithmetic and runtime operations are mostly done
	on a stack of number_t.
*/

void push(number_t t){
	if (DEBUG) {outsc("** push sp= "); outnumber(sp); outcr(); }
	if (sp == STACKSIZE)
		error(ESTACK);
	else
		stack[sp++]=t;
}

number_t pop(){
	if (DEBUG) {outsc("** pop sp= "); outnumber(sp); outcr(); }
	if (sp == 0) {
		error(ESTACK);
		return 0;
	}
	else
		return stack[--sp];	
}

void clearst(){
	sp=0;
}

void clrdata() {
#ifdef HASDARTMOUTH
	data=0;
#endif
}

/* 

	Stack handling for gosub and for

*/

void pushforstack(){
	short i, j;
	if (DEBUG) { outsc("** forsp and here in pushforstack "); outnumber(forsp); outspc(); outnumber(here); outcr(); }
	// before pushing into the for stack we check is an
	// old for exists - this is on reentering a for loop
	for(i=0; i<forsp; i++) {
		if (forstack[i].varx == xc && forstack[i].vary == yc) {
			for(j=i; j<forsp-1; j++) {
				forstack[j].varx=forstack[j+1].varx;
				forstack[j].vary=forstack[j+1].vary;
				forstack[j].here=forstack[j+1].here;
				forstack[j].to=forstack[j+1].to;
				forstack[j].step=forstack[j+1].step;	
			}
			forsp--;
			break;
		}
	}

	if (forsp < FORDEPTH) {
		forstack[forsp].varx=xc;
		forstack[forsp].vary=yc;
		forstack[forsp].here=here;
		forstack[forsp].to=x;
		forstack[forsp].step=y;
		forsp++;	
		return;	
	} else 
		error(EFOR);
}


void popforstack(){
	if (forsp>0) {
		forsp--;
	} else {
		error(EFOR);
		return;
	} 
	xc=forstack[forsp].varx;
	yc=forstack[forsp].vary;
	here=forstack[forsp].here;
	x=forstack[forsp].to;
	y=forstack[forsp].step;
}

void dropforstack(){
	if (forsp>0) {
		forsp--;
	} else {
		error(EFOR);
		return;
	} 
}

void clrforstack() {
	forsp=0;
	fnc=0;
}

/* 

	Input and output functions.

	On Mac/Linux the interpreter is implemented to use only putchar 
	and getchar.

	These functions need to be provided by the stdio lib 
	or by any other routine like a serial terminal or keyboard 
	interface of an arduino.
 
 	ioinit(): called at setup to initialize what ever io is needed
 	outch(): prints one ascii character 
 	inch(): gets one character (and waits for it)
 	checkch(): checks for one character (non blocking)
 	ins(): reads an entire line (uses inch except for pioserial)

 	For picoserial blocking I/O makes little sense. We read directly
 	into the input buffer through the interrupt routine. 

 	In addition the this a few "device driver functions" are also 
 	included to simplify keyboard, LCD and TFT code 

*/

/* 
	 device driver code - emulates a lcd_columns * lcd_rows 
	 very dumb ascii terminal
*/

void ioinit() {
	serialbegin();
	wirebegin();
	dspbegin();
	prtbegin();
#ifdef ARDUINOPS2
#ifdef ESP8266
	keyboard.begin();
#else
	keyboard.begin(PS2DataPin, PS2IRQpin, PS2Keymap_German);
#endif
#endif
	ebegin();
	iodefaults();
}

void iodefaults() {
	od=odd;
	id=idd;
}

/* 

	Platform dependend IO functions, implemented models are
		- Standard C library
		- Arduino Serial 
		- Arduino Picoserial
		- SD filesystems
		- SPIFFS filesystems

	Wrappers around the OS functions 

*/

// wrapper around file access
void filewrite(char c) {
#ifndef ARDUINO
	if (ofile) fputc(c, ofile); else ert=1;
#else
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	if (ofile) ofile.write(c); else ert=1;
#endif
#endif
	return;
}

char fileread(){
	char c;
#ifndef ARDUINO
	if (ifile) c=fgetc(ifile); else { ert=1; return 0; }
#else
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	if (ifile) c=ifile.read(); else { ert=1; return 0; }
#endif
#endif
	if (c == -1 ) {
		ert=-1;
	}
	return c;
}

char ifileopen(char* filename){
#ifndef ARDUINO
	ifile=fopen(filename, "r");
	return (int) ifile;
#else
#ifdef ARDUINOSD
	ifile=SD.open(filename, FILE_READ);
	return (int) ifile;
#else
#ifdef ESPSPIFFS
	ifile=SPIFFS.open(filename, "r");
	return (int) ifile;
#endif
#endif
#endif
	return 0;
}

void ifileclose(){
#ifndef ARDUINO
	if (ifile) fclose(ifile);
	ifile=NULL;
#else
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	ifile.close();
#endif
#endif	
}

char ofileopen(char* filename){
#ifndef ARDUINO
	ofile=fopen(filename, "w");
	return (int) ofile;
#else
#ifdef ARDUINOSD
	ofile=SD.open(filename, FILE_OWRITE);
	return (int) ofile;
#else
#ifdef ESPSPIFFS
	ofile=SPIFFS.open(filename, "w");
	return (int) ofile;
#endif
#endif
#endif
	return 0;
}

void ofileclose(){
#ifndef ARDUINO
	if (ofile) fclose(ofile);
#else
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
    ofile.close();
#endif
#endif
}

int fileavailable(){
#ifndef ARDUINO
	return !feof(ifile);
#else
#if defined(ARDUINOSD) || defined(ESPSPIFFS)
	return ifile.available();
#endif
#endif
}

void rootopen() {
#ifndef ARDUINO	
#ifndef MSDOS
	root=opendir ("./");
#endif
#else 
#ifdef ARDUINOSD
	root=SD.open("/");
#else 
#ifdef ESPSPIFFS
	root=SPIFFS.openDir("/");
#endif
#endif
#endif
}

int rootnextfile() {
#ifndef ARDUINO	
#ifndef MSDOS
  file = readdir(root);
  return (file != 0);
#else 
  return FALSE;
#endif
#else 
#ifdef ARDUINOSD
  file=root.openNextFile();
  return (file != 0);
#else 
#ifdef ESPSPIFFS
  if (root.next()) {
    file=root.openFile("r");
    return 1;
  } else {
    return 0;
  }
#endif
#endif
#endif
}

int rootisfile() {
#ifndef ARDUINO	
#ifndef MSDOS
  return (file->d_type == DT_REG);
#else
  return FALSE;
#endif
#else 
#ifdef ARDUINOSD
  return (! file.isDirectory());
#else 
#ifdef ESPSPIFFS
  return TRUE;
#endif
#endif
#endif
}


char* rootfilename() {
#ifndef ARDUINO 
#ifndef MSDOS
  return (file->d_name);
#else
  return 0;
#endif
#else 
#ifdef ARDUINOSD
  return (char*) file.name();
#else 
#ifdef ESPSPIFFS
   	// c_str() sometimes broken
  	int i=0;
	String s=root.fileName();
 	for (i=0; i<s.length(); i++) { tempname[i]=s[i]; }
 	tempname[i]=0;
	return tempname;
#endif
#endif
#endif  
}

int rootfilesize() {
#ifndef ARDUINO 
  return 0;
#else 
#ifdef ARDUINOSD
  return file.size();
#else 
#ifdef ESPSPIFFS
  return file.size();
#endif
#endif
#endif  
}


void rootfileclose() {
#ifndef ARDUINO 
  return;
#else 
#ifdef ARDUINOSD
  return file.close();
#else 
#ifdef ESPSPIFFS
  return file.close();
#endif
#endif
#endif  
}

void rootclose(){
#ifndef ARDUINO 
#ifndef MSDOS
  (void) closedir(root);
#endif
#else 
#ifdef ARDUINOSD
  root.close();
#else 
#ifdef ESPSPIFFS
  return;
#endif
#endif
#endif  
}

// wrapper around console output
void serialwrite(char c) {
#ifndef ARDUINO
	putchar(c);
#else 
#ifndef USESPICOSERIAL
	Serial.write(c);
#else
	PicoSerial.print(c);
#endif
#endif
	return;	
}


// printer wrappers
void prtbegin() {
#ifdef ARDUINOPRT
	Serial1.begin(serial1_baudrate);
#endif
}

void prtwrite(char c) {
#ifdef ARDUINOPRT
	Serial1.write(c);
#endif
}


/* 
	read from the radio interface, radio is always block 
	oriented. This function is called from ins for an entire 
	line.

	In blockmode the entire message is returned in the 
	receiving string while in line mode the length of the 
	string is adapted. Blockmode can be used to transfer
	binary data.

*/
void radioins(char *b, short nb) {
#ifdef ARDUINORF24
    if (radio.available()) {
    	radio.read(b+1, nb);
    	if (!blockmode) {
        	for (z.a=0; z.a<nb; z.a++) if (b[z.a+1]==0) break;		
    	} else {
    		z.a=radio.getPayloadSize();
      		if (z.a > nb) z.a=nb;
    	}
      	b[0]=z.a;
	} else {
      	b[0]=0; 
      	b[1]=0;
      	z.a=0;
	}
#else 
      b[0]=0; 
      b[1]=0;
      z.a=0;
#endif
}

/* write to radio, no character mode here */
void radioouts(char *b, short l) {
#ifdef ARDUINORF24
	radio.stopListening();
	if (radio.write(b, l)) ert=0; else ert=1;
	radio.startListening();
#endif
}


/* this can result in multiple wire begin calls
 no protection here for user errors from BASIC */ 
#ifdef ARDUINOWIRE
uint8_t wire_slaveid = 0;
uint8_t wire_myid = 0;
#endif

// default begin is as a master
void wirebegin() {
#ifdef ARDUINOWIRE
	Wire.begin();
#endif
}

// as a master open sets the slave id for the communication
// no extra begin while we stay master
void wireopen(char* s) {
#ifdef ARDUINOWIRE
	if (s[0] == 'm') {
		wire_slaveid=s[1];
		wire_myid=0;
	} else if ( s[0] == 's' ) { 
		wire_myid=s[1];
		wire_slaveid=0;
		Wire.begin(wire_myid);
		// here the slave code if this Arduino is a slave
		// to be done
	} else 
		error(ERANGE);
#endif
}

void wireins(char *b, uint8_t l) {
#ifdef ARDUINOWIRE
	z.a=0;
	Wire.requestFrom(wire_slaveid, l);
	while (Wire.available() && z.a<l) b[++z.a]=Wire.read();
	b[0]=z.a;
#else 
	b[0]=0;
	z.a=0;
#endif
}

void wireouts(char *b, uint8_t l) {
#ifdef ARDUINOWIRE
	  Wire.beginTransmission(wire_slaveid); 
	  for(z.a=0; z.a<l; z.a++) Wire.write(b[z.a]);     
	  ert=Wire.endTransmission(); 
#endif
}

/* 
	we always read from pipe 1 and use pipe 0 for writing, 
	the filename is the pipe address, by default the radio 
	goes to reading mode after open and is only stopped for 
	write
*/

void iradioopen(char *filename) {
#ifdef ARDUINORF24
	radio.begin();
	radio.openReadingPipe(1, pipeaddr(filename));
	radio.startListening();
#endif
}

void oradioopen(char *filename) {
#ifdef ARDUINORF24
	radio.begin();
	radio.openWritingPipe(pipeaddr(filename));
#endif
}

/*
	real time clock with EEPROM stuff based on uRTC and uEEPROM
	this is a minimalistic library 

*/ 

void rtcbegin() {
#ifdef ARDUINORTC
#ifdef ARDUINO_ARCH_ESP8266
Wire.begin(0, 2); // D3 and D4 on ESP8266
#else
Wire.begin();
#endif
#endif
}

short rtcread(char i) {
#ifdef ARDUINORTC
	switch (i) {
		case 0: 
			return rtc.second();
		case 1:
			return rtc.minute();
		case 2:
			return rtc.hour();
		case 3:
			return rtc.day();
		case 4:
			return rtc.month();
		case 5:
			return rtc.year();
		case 6:
			return rtc.dayOfWeek();
		case 7:
			return rtc.temp();
	}
#endif
	return 0;
}

short rtcget(char i) {
#ifdef ARDUINORTC
	rtc.refresh();
	return rtcread(i);
#else
	return 0;
#endif
}

void rtcset(char i, short v) {
#ifdef ARDUINORTC
	uint8_t tv[7];
	char j;
	rtc.refresh();
	for (j=0; j<7; j++) tv[j]=rtcread(j);
	tv[i]=v;
	rtc.set(tv[0], tv[1], tv[2], tv[6], tv[3], tv[4], tv[5]);
#endif
}


#ifndef ARDUINO
/* 
	this is C standard library stuff, we branch to file input/output
	if there is a valid file descriptor in fd.
*/

void serialbegin(){}

char inch(){
	char c;
	if (id == ISERIAL)
		return getchar(); 
	if (id == IFILE) 
		return fileread();
	return 0;
}

char checkch(){
	return TRUE;
}

short availch(){
	if (id == ISERIAL)
		return 1; 
	if (id == IFILE) 
		return fileavailable();
	return 0;
}

void ins(char *b, short nb) {
	char c;
	short i = 1;
	while(i < nb) {
		c=inch();
		if (c == '\r') c=inch();
		if (c == '\n' ) {
			break;
		} else {
			b[i++]=c;
		} 
	}
	b[i]=0x00;
	b[0]=(unsigned char)i-1;
	z.a=i-1;
}

#else 
#ifdef LCDSHIELD
short keypadread(){
	short a=analogRead(A0);
	if (a > 850) return 0;
	else if (a>600 && a<800) return 's';
	else if (a>400 && a<600) return 'l';
	else if (a>200 && a<400) return 'd';
	else if (a>60  && a<200) return 'u';
	else if (a<60)           return 'r';
	return 0;
}
#endif
#ifndef USESPICOSERIAL
/*

	This is standard Arduino code Serial code. 
	In inch() we wait for input by looping. 
	output is controlled by the flag od.

*/ 

void serialbegin() {
	Serial.begin(serial_baudrate);
}


char inch(){
	char c=0;

	switch(id) {
		case ISERIAL:
			while (!Serial.available()) yield();
			return Serial.read();		
		case IFILE:
			return fileread();
#ifdef ARDUINORF24
		// radio is not character oriented, this is only added to make GET work
		// for single byte payloads, radio, like file is treated nonblocking here
		case IRADIO:
			radioins(sbuffer, SBUFSIZE-1);
			if (sbuffer[0]>0) return sbuffer[1]; else return 0;
#endif
#ifdef ARDUINOPRT
		case ISERIAL1:
			while (!Serial1.available()) yield();
			return Serial1.read();
#endif				
		case IKEYBOARD:
#ifdef ARDUINOPS2		
			do {
				if (keyboard.available()) c=keyboard.read();
				delay(1); // this seems to be needed on an ESP, probably rather yield()
			} while(c == 0);	
    		if (c == 13) c=10;
			return c;
#endif
#ifdef LCDSHIELD
			return keypadread();
#endif
			break;
	}

	return 0;
}

char checkch(){
	switch (id) {
		case ISERIAL:
			if (Serial.available()) return Serial.peek(); 
			break;
		case IFILE:
			return fileavailable();
#ifdef ARDUINORF24
    	case IRADIO:
    		return radio.available();
#endif
#ifdef ARDUINOWIRE
    	case IWIRE:
    		return Wire.available();
#endif
#ifdef ARDUINOPRT
		case ISERIAL1:
			if (Serial1.available()) return Serial1.peek();
			break; 
#endif
		case IKEYBOARD:
#ifdef ARDUINOPS2		
			if (keyboard.available()) return keyboard.read();
#endif
#ifdef LCDSHIELD
			return keypadread();
#endif
			break;
	}
	return 0;
}

short availch(){
	switch (id) {
    	case ISERIAL:
      		return Serial.available(); 
    	case IFILE:
    		return fileavailable();
#ifdef ARDUINORF24
    	case IRADIO:
    		return radio.available();
#endif
#ifdef ARDUINOWIRE
    	case IWIRE:
    		return Wire.available();
#endif
#ifdef ARDUINOPRT
    	case ISERIAL1:
      		return Serial1.available();
#endif
    	case IKEYBOARD:
#ifdef ARDUINOPS2   
      		return keyboard.available();
#endif
#ifdef LCDSHIELD
      		return (keypadread() != 0);
#endif
	    	break;
	}
	return 0;
}

/* 
	ins is the generic reader into a string, by default 
	it works in line mode and ends reading after newline

  the first element of the buffer is the lower byte of the length

  this is corrected later in xinput, z.a has to be set as 
  a side effect
  
*/

void ins(char *b, short nb) {
  	char c;
  	short i = 1;
    // only ISERIAL 1 can be switched to block mode right now
    if (blockmode > 0 && id == ISERIAL1 ) {
    	inb(b, nb);
    } else if (id == IRADIO) {
    	radioins(b, nb);
    } else if (id == IWIRE) {
    	wireins(b, nb);
    } else {
  		while(i < nb) {
    		c=inch();
    		if (id == ISERIAL || id == IKEYBOARD) outch(c);
    		if (c == '\r') c=inch(); /* skip carriage return */
    	 	if (c == '\n') {
      			break;
    	  	} else if ( (c == 127 || c == 8) && i>1) {
      		  	i--;
    	  	} else {
      		  	b[i++]=c;
    	  	} 
  	  }
  	  b[i]=0;
      b[0]=(unsigned char)i-1;
      z.a=i-1; 
    }  
}

/* 
	the block mode reader for esp and sensor modules 
 	on a serial interface, it tries to read as many 
 	characters as possible into a buffer
 	blockmode = 1 reads once availch() bytes
 	blockmode > 1 implements a timeout mechanism and tries 
 		to read until blockmode milliseconds have expired
 		this is needed for esps and other sensors without
 		flow control and volatile timing to receive more 
 		then 64 bytes 
*/
void inb(char *b, short nb) {
	long m;
	short i = 0;
	if (blockmode == 1 ) {
	    i=availch();
    	if (i>nb-1) i=nb-1;
    	b[0]=(unsigned char)i;
    	z.a=i;
    	b[i+1]=0;
    	b++;
    	while (i--) {*b++=inch();} 	
	} else if (blockmode > 1) {
		m=millis();
		while (i < nb-1) {
			if (availch()) b[++i]=inch();
			if (millis() > m+blockmode) break;
		}
		b[0]=(unsigned char)i;
		z.a=i;
		b[i+1]=0;
	} else {
		b[0]=0;
		z.a=0;
		b[1]=0;
	}
	
}

#else 

/*

	Picoserial allows to define an own input buffer and an 
	interrupt function. This is used to fill the input buffer 
	directly on read. Write is standard like in the Serial code.

*/ 

volatile static char picochar;
volatile static char picoa = FALSE;
volatile static char* picob = NULL;
static short picobsize = 0;
volatile static short picoi = 1;

void serialbegin() {
	(void) PicoSerial.begin(serial_baudrate, picogetchar); 
}

void picogetchar(int c){
	if (picob && (! picoa) ) {
    	picochar=c;
		if (picochar != '\n' && picochar != '\r' && picoi<picobsize-1) {
			picob[picoi++]=picochar;
			outch(picochar);
		} else {
			picoa = TRUE;
			picob[picoi]=0;
			picob[0]=picoi;
			z.a=picoi;
			picoi=1;
		}
		picochar=0; // every buffered byte is deleted
	} else {
    	if (c != 10) picochar=c;
	}
}


char inch(){
	char c;
	if (id == ISERIAL) {
		c=picochar;
		picochar=0;
		return c;	
	}
#ifdef ARDUINORF24
	// radio is not character oriented, this is only added to make GET work
	// for single byte payloads, radio is treated nonblocking here
	if (id == IRADIO) {
			radioins(sbuffer, SBUFSIZE-1);
			if (sbuffer[0]>0) return sbuffer[1]; else return 0;
	}
#endif
#ifdef LCDSHIELD
	if (id == IKEYBOARD) {
		return keypadread();	
	}
#endif
}

char checkch(){
    if (id == ISERIAL) return picochar;
#ifdef ARDUINORF24
    if (id == IRADIO) return radio.available();
#endif
#ifdef ARDUINOWIRE
    if (id == IWIRE) return Wire.available();
#endif
#ifdef LCDSHIELD
	if (id =IKEYBOARD) return (keypadread()!=0);
#endif

}
short availch(){
	if (id == ISERIAL) return picoi;
#ifdef ARDUINORF24
   	if (id == IRADIO) return radio.available();
#endif
 #ifdef ARDUINOWIRE
    if (id == IWIRE) return Wire.available();
#endif  	
#ifdef LCDSHIELD
	if (id =IKEYBOARD) return (keypadread()!=0);
#endif
}

/* serial pico code only implements serial input and no input channels */
void ins(char *b, short nb) {
	if (id == ISERIAL) {
		picob=b;
		picobsize=nb;
		picoa=FALSE;
		while (! picoa);
		//outsc(b+1); 
		outcr();
		return;
	}
#ifdef ARDUINORF24
	if (id == IRADIO) {
		radioins(b, nb);
		return;
	}
#endif
#ifdef ARDUINOWIRE
	if (id == IWIRE) {
		wireins(b, nb);
		return;
	}
#endif
}
#endif
#endif

/* 

	The generic IO code 

*/ 

// output one character to a stream
// block oriented i/o like in radio not implemented here
void outch(char c) {
	if (od == OSERIAL)
		serialwrite(c);
	if (od == OPRT) 
		prtwrite(c);
	if (od == OFILE) 
		filewrite(c); 
	if (od == ODSP)
		dspwrite(c);
}

// send a newline
void outcr() {
#ifdef ARDUINOPRT
	if (sendcr) outch('\r');
#endif
	outch('\n');
} 

// send a space
void outspc() {
	outch(' ');
}

/*
	output a string of length x at index ir - basic style
	default is a character by character operation, block 
	oriented write need special functions

*/
void outs(char *ir, short l){
	int i;
	switch (od) {
		case ORADIO:
			radioouts(ir, l);
			break;
		case OWIRE:
			wireouts(ir, l);
			break;
		default:
			for(i=0; i<l; i++) outch(ir[i]);
	}

}


// output a zero terminated string at ir - c style
void outsc(char *c){
	while (*c != 0) outch(*c++);
}

// output a zero terminated string in a formated box
void outscf(char *c, short f){
  short i = 0;
  while (*c != 0) { outch(*c++); i++; }
  if (f > i) {
    f=f-i;
    while (f--) outspc();
  }
}


// reading a number from a char buffer 
// maximum number of digits is adjusted to 16
// ugly here, testcode when introducting 
// number_t was strictly 16 bit before
short parsenumber(char *c, number_t *r) {
	short nd = 0;
	*r=0;
	while (*c >= '0' && *c <= '9' && *c != 0) {
		*r=*r*10+*c++-'0';
		nd++;
		if (nd == SBUFSIZE) break;
	}
	return nd;
}
#ifdef HASFLOAT
// a poor man's atof implementation with character count
short parsenumber2(char *c, number_t *r) {
	short nd = 0;
	short i;
	number_t fraction = 0;
	number_t exponent = 0;
	char nexp = FALSE;

	*r=0;

	i=parsenumber(c, r);
	c+=i;
	nd+=i;

	if (*c == '.') {
		c++; 
		nd++;

		i=parsenumber(c, &fraction);
		c+=i;
		nd+=i;

		if (i > 0) {
			while ((--i)>=0) fraction=fraction/10;
			*r+=fraction;
		} 
	}

	if (*c == 'E' || *c == 'e') {
		c++;
		nd++;
		if (*c == '-') {c++; nd++; nexp=TRUE;};
		i=parsenumber(c, &exponent);
		nd+=i;
		while ((--exponent)>=0) if (nexp) *r=*r/10; else *r=*r*10;		
	}

	return nd;
}
#endif

// convert a number to a string
// the use of long v here is a hack related to HASFLOAT 
// unfinished here and need to be improved
short writenumber(char *c, number_t vi){
	short nd = 0;	
	long v;
	short i,j;
	short s = 1;
	char c1;

	// not really needed any more
	v=(long)vi;

	if (v<0) {
		s=-1; 
		v=-v;
	}
	do {
		c[nd++]=v%10+'0';
		v=v/10;
	} while (v > 0);

	if (s < 0 ) c[nd]='-'; else nd--;

	i=0; 
	j=nd;
	while (j>i) {
		c1=c[i];
		c[i]=c[j];
		c[j]=c1;
		i++;
		j--;
	}

	nd++;
	c[nd]=0;
	return nd;
} 

#ifdef HASFLOAT
// this is for floats, going back to library functions
// for a starter.
short writenumber2(char *c, number_t vi) {

	short i;
  	number_t f;
  	short exponent = 0; 
  	char eflag=0;
	// pseudo integers are displayed as integer
	// zero trapped here
	f=floor(vi);
	if (f == vi && fabs(vi) < maxnum) {
		return writenumber(c, vi);
	}

	// floats are displayed using the libraries
#ifndef ARDUINO
	return sprintf(c, "%g", vi);
#else
  	f=vi;
    while (fabs(f)<1.0)   { f=f*10; exponent--; }
  	while (fabs(f)>=10.0-0.00001) { f=f/10; exponent++; }

  	// small numbers
  	if (exponent > -2 && exponent < 7) { 
    	dtostrf(vi, 0, 5, c);
  	} else {
    	dtostrf(f, 0, 5, c);
    	eflag=TRUE;
  	}
	
  	// remove trailing zeros
  	for (i=0; (i < SBUFSIZE && c[i] !=0 ); i++);
  	i--;
	while (c[i] == '0' && i>1) {i--;}
	i++;

  	// add the exponent
  	if (eflag) {
    	c[i++]='E';
    	i+=writenumber(c+i, exponent);
  	}

  	c[i]=0;
	return i;

#endif
}
#endif

// use sbuffer as a char buffer to get a number input 
// still not float ready
char innumber(number_t *r) {
	short i = 1;
	short s = 1;

again:
	*r=0;
	ins(sbuffer, SBUFSIZE);
	while (i < SBUFSIZE) {
		if (sbuffer[i] == ' ' || sbuffer[i] == '\t') i++;
		if (sbuffer[i] == BREAKCHAR) return BREAKCHAR;
		if (sbuffer[i] == 0) return 1;
		if (sbuffer[i] == '-') {
			s=-1;
			i++;
		}
		if (sbuffer[i] >= '0' && sbuffer[i] <= '9') {
#ifndef HASFLOAT
			(void) parsenumber(&sbuffer[i], r);
#else
			(void) parsenumber2(&sbuffer[i], r);
#endif
			*r*=s;
			return 0;
		} else {
			if (id == ISERIAL || id == IKEYBOARD) {
				printmessage(ENUMBER); 
				outspc(); 
				printmessage(EGENERAL);
				outcr();
				*r=0;
				s=1;
				i=1;
				goto again; 
			} else { 
				ert=1; 
				return 1; 
			}
		}
	}
	return 0;
}

// prints a number
void outnumber(number_t n){
	short nd;

#ifndef HASFLOAT
	nd=writenumber(sbuffer, n);
#else
	nd=writenumber2(sbuffer, n);
#endif

	//outsc(sbuffer); 
	outs(sbuffer, nd);

	// number formats in Palo Alto style
	while (nd < form) {outspc(); nd++; };

}

/* 

	Layer 1 functions - providing data into the global variable and 
	changing the interpreter state

*/


/*

	Lexical analyser - tokenizes the input line.

	nexttoken() increments the input buffer index bi and delivers values in the global 
	variable token, with arguments in the accumulator x and the index register ir
	xc is used in the routine. 

	xc, ir and x change values in nexttoken and deliver the result to the calling
	function.

	bi and ibuffer should not be changed or used for any other function in 
	interactive node as they contain the state of nexttoken(). In run mode 
	bi and ibuffer are not used as the program is fully tokenized in mem.

	debugtoken() is the debug function for the tokenizer
	whitespaces() skips whitespaces

*/ 


void whitespaces(){
	while (*bi == ' ' || *bi == '\t') bi++;
}

void nexttoken() {

	// RUN mode vs. INT mode
	if (st == SRUN || st == SERUN) {
		gettoken();
		if (debuglevel>1) debugtoken();
		return;
	}

	// after change in buffer logic the first byte is reserved for the length
	if (bi == ibuffer) bi++;

	// remove whitespaces outside strings
	whitespaces();

	// end of line token 
	if (*bi == '\0') { 
		token=EOL; 
		if (DEBUG) debugtoken();
		return; 
	}

	// unsigned numbers, value returned in x
	if (*bi <='9' && *bi >= '0'){
#ifndef HASFLOAT
		bi+=parsenumber(bi, &x);
#else
		bi+=parsenumber2(bi, &x);
#endif
		token=NUMBER;
		if (DEBUG) debugtoken();
		return;
	}

	// strings between " " or " EOL, value returned in ir
	if (*bi == '"'){
		x=0;
		bi++;
		ir=bi;
		while(*bi != '"' && *bi !='\0') {
			x++;
			bi++;
		} 
		bi++;
		token=STRING;
		if (DEBUG) debugtoken();
		return;
	}

	// single character operators are their own tokens
	if (*bi == '+' || *bi == '-' || *bi == '*' || *bi == '/' || *bi == '%'  ||
		*bi == '\\' || *bi == ':' || *bi == ',' || *bi == '(' || *bi == ')' ) { 
			token=*bi; 
			bi++; 
			if (DEBUG) debugtoken();
			return; 
	}  

	// relations
	// single character relations are their own token
	// >=, =<, =<, =>, <> are tokenized
	if (*bi == '=') {
		bi++;
		whitespaces();
		if (*bi == '>') {
			token=GREATEREQUAL;
			bi++;
		} else if (*bi == '<'){
			token=LESSEREQUAL;
			bi++;
		} else {
			token='=';
		}
		if (DEBUG) debugtoken();
		return;
	}

	if (*bi == '>'){
		bi++;
		whitespaces();
		if (*bi == '=') {
			token=GREATEREQUAL;
			bi++;
		} else  {
			token='>';
		}
		if (DEBUG) debugtoken();
		return;
	}

	if (*bi == '<'){
		bi++;
		whitespaces();
		if (*bi == '=') {
			token=LESSEREQUAL;
			bi++;
		} else  if (*bi == '>') {
			token=NOTEQUAL;
			bi++;
		} else {
			token='<';
		}
		if (DEBUG) debugtoken();
		return;
	}

	/* 
		keyworks and variables
		isolate a word, bi points to the beginning, x is the length of the word
	 	ir points to the end of the word after isolating.
	 	@ is a letter here to make the special @ arrays possible 
	 */

	x=0;
	ir=bi;
	while (-1) {
		// toupper 
		if (*ir >= 'a' && *ir <= 'z') {
			*ir-=32;
			ir++;
			x++;
		} else if (*ir >= '@' && *ir <= 'Z'){
			ir++;
			x++;
		} else {
			break;
		}
	}

/* 

	ir is reused here to implement string compares
	scanning the keyword array. 
	Once a keyword is detected the input buffer is advanced 
	by its length, and the token value is returned. 

	keywords are an array of null terminated strings.

*/

	yc=0;
	while (gettokenvalue(yc) != 0){
		ir=getkeyword(yc);
		xc=0;
		while (*(ir+xc) != 0) {
			if (*(ir+xc) != *(bi+xc)){
				yc++;
				xc=0;
				break;
			} else 
				xc++;
		}
		if (xc == 0) continue;
		bi+=xc;
		token=gettokenvalue(yc);
		if (DEBUG) debugtoken();
		return;
	}

/*
	a variable has length 1 in the first version
	 its literal value is returned in xc
	in addition to this, a number or $ is accepted as 
	second character like in Apple 1 BASIC

*/
	if ( x == 1 || (x == 2 && *bi == '@') ){
		token=VARIABLE;
		xc=*bi;
		yc=0;
		bi++;
		//whitespaces();
		if (*bi >= '0' && *bi <= '9') { 
			yc=*bi;
			bi++;
		} 
		if (xc == '@' && x == 2) {
			yc=*bi;
			bi++;
		}
		//whitespaces();
		if (*bi == '$') {
			token=STRINGVAR;
			bi++;
		}
		whitespaces();
		if (token == VARIABLE && *bi == '(' ) { 
			token=ARRAYVAR;
		}	
		if (DEBUG) debugtoken();
		return;
	}


/* single letters are parsed and stored */

	token=*bi;
	bi++;
	if (DEBUG) debugtoken();
	return;
}

/* 
	Layer 1 - program editor 

	Editing the program, the structure of a line is 
	LINENUMBER linenumber(2bytes) token(n bytes)

	store* stores something to memory 
	get* retrieves information

	No matter how long int is in the C implementation
	we pack into bytes, this is clumsy but portable
	the store and get commands all operate on here 
	and advance it

	Storetoken operates on the variable top. 
	We always append at the end and the sort. 

	Gettoken operate on the variable here 
	which will also be the key variable in run mode.

	Tokens are stored including their payload.

	This group of functions changes global states and
	cannot be called at program runtime with saving
	the relevant global variable to the stack.

	Error handling is still incomplete.
*/

char nomemory(number_t b){
	if (top >= himem-b) return TRUE; else return FALSE;
}

// store a token - check free memory before changing anything
void storetoken() {
	short i=x;
	switch (token) {
		case LINENUMBER:
			if ( nomemory(addrsize+1) ) break;
			mem[top++]=token;	
			z.a=x;
			setnumber(top, addrsize);
			top+=addrsize;
			return;	
		case NUMBER:
			if ( nomemory(numsize+1) ) break;
			mem[top++]=token;	
			z.i=x;
			setnumber(top, numsize);
			top+=numsize;
			return;
		case ARRAYVAR:
		case VARIABLE:
		case STRINGVAR:
			if ( nomemory(3) ) break;
			mem[top++]=token;
			mem[top++]=xc;
			mem[top++]=yc;
			return;
		case STRING:
			if ( nomemory(x+2) ) break;
			mem[top++]=token;
			mem[top++]=i;
			while (i > 0) {
				mem[top++] = *ir++;
				i--;
			}
			return;
		default:
			if ( nomemory(1) ) break;
			mem[top++]=token;
			return;
	}
	error(EOUTOFMEMORY);
} 


// wrapper around mem access for eeprom autorun on small Arduinos
char memread(address_t i){
#ifndef ARDUINOEEPROM
	return mem[i];
#else 
	if (st != SERUN) {
		return mem[i];
	} else {
		return eread(i+eheadersize);
	}
#endif
}


// get a token from memory
void gettoken() {

	// if we have reached the end of the program, EOL is always returned
	// we don't rely on mem having a trailing EOL
	if (here >= top) {
		token=EOL;
		return;
	}

	token=memread(here++);
	switch (token) {
		case LINENUMBER:
			if (st != SERUN) getnumber(here, addrsize); else egetnumber(here+eheadersize, addrsize);
			x=z.a;
			here+=addrsize;
			break;
		case NUMBER:	
			if (st !=SERUN) getnumber(here, numsize); else egetnumber(here+eheadersize, numsize);
			x=z.i;
			here+=numsize;	
			break;
		case ARRAYVAR:
		case VARIABLE:
		case STRINGVAR:
			xc=memread(here++);
			yc=memread(here++);
			break;
		case STRING:
			x=(unsigned char)memread(here++);  // if we run interactive or from mem, pass back the mem location
#ifndef ARDUINO
			ir=(char *)&mem[here];
			here+=x;
#else 
			if (st == SERUN) { // we run from EEPROM and cannot simply pass a pointer
				for(int i=0; i<x; i++) {
					ibuffer[i]=memread(here+i);   // we (ab)use the input buffer which is not needed here
				}
				ir=ibuffer;
			} else {
				ir=(char *)&mem[here];
			}
			here+=x;	
#endif
		}
}

// goto the first line of a program
void firstline() {
	if (top == 0) {
		x=0;
		return;
	}
	here=0;
	gettoken();
}

// goto the next line, search forward
void nextline() {
	while (here < top) {
		gettoken();
		if (token == LINENUMBER) return;
		if (here >= top) {
			here=top;
			x=0;
			return;
		}
	}
}

// find a line, search from the beginning
void findline(address_t l) {
	here=0;
	while (here < top) {
		gettoken();
		if (token == LINENUMBER && x == l ) return;
	}
	error(ELINE);
}

// finds the line of a location
address_t myline(address_t h) {
	address_t l=0; 
	address_t l1=0;
	address_t here2;

	here2=here;
	here=0;
	gettoken();
	while (here < top) {
		if (token == LINENUMBER) {
			l1=l;
			l=x;
		}
		if (here >= h) { break; }
		gettoken();
	}
	here=here2;
	if (token == LINENUMBER)
		return l1;
	else 
		return l;
}

/*

   	Move a block of storage beginng at b ending at e
  	to destination d. No error handling here!!

*/

void moveblock(address_t b, address_t l, address_t d){
	address_t i;

	//outsc("** Moving block: "); outnumber(b); outspc(); outnumber(l); outspc(); outnumber(d); outcr();
	if (d+l > himem) {
		error(EOUTOFMEMORY);
		return;
	}
	
	if (l<1) 
		return;

	if (b < d)
		for (i=l; i>0; i--)
			mem[d+i-1]=mem[b+i-1]; 
	else 
		for (i=0; i<l; i++) 
			mem[d+i]=mem[b+i]; 

	//outsc("** Done moving /n");
}

void zeroblock(address_t b, address_t l){
	address_t i;

	if (b+l > himem) {
		error(EOUTOFMEMORY);
		return;
	}
	if (l<1) 
		return;
	for (i=0; i<l+1; i++) mem[b+i]=0;
}

/*

	Line editor: 

	stage 1: no matter what the line number is - store at the top
   				remember the location in here.
	stage 2: see if it is only an empty line - try to delete this line
	stage 3: calculate lengthes and free memory and make room at the 
				appropriate place
	stage 4: copy to the right place 

	Very fragile code. 

	zeroblock statements commented out after EOL code was fixed
	
*/

#ifdef DEBUG
void diag(){
	outsc("top, here, y and x\n");
	outnumber(top); outspc();
	outnumber(here); outspc();
	outnumber(y); outspc();	
	outnumber(x); outspc();		
	outcr();
}
#endif

void storeline() {
	const short lnlength=addrsize+1;
	short linelength;
	number_t newline; 
	address_t here2, here3; 
	address_t t1, t2;

	// zero is an illegal line number
	if (x == 0) {
		error(ELINE);
		return;
	}

// the data pointer becomes invalid once the code has been changed
	clrdata();

/*
	stage 1: append the line at the end of the memory,
	remember the line number on the stack and the old top in here

*/
    t1=x;			
    here=top;		
    newline=here;	 
	token=LINENUMBER;
	do {
		storetoken();
		if (er != 0 ) {
			top=newline;
			here=0;
			return;
		}
		nexttoken();
	} while (token != EOL);

	x=t1;					// recall the line number
	linelength=top-here;	// calculate the number of stored bytes

/* 
	stage 2: check if only a linenumber stored - then delete this line
	
*/

	if (linelength == (lnlength)) {  		
		top-=(lnlength);
		y=x;					
		findline(y);
		if (er != 0) return;	
		y=here-lnlength;							
		nextline();			
		here-=lnlength;
		if (x != 0) {						
			moveblock(here, top-here, y);	
			top=top-(here-y);
		} else {
			top=y;
		}
		return;
	}

/* 
	stage 3, a nontrivial line with linenumber x is to be stored
	try to find it first by walking through all lines 
*/
	else {	
		y=x;
		here2=here;
		here=lnlength;
		nextline();
		if (x == 0) { // there is no nextline after the first line, we are done
			return;
		}
		here=0;       // go back to the beginning
		here2=0;
		while (here < top) {
			here3=here2;
			here2=here;
			nextline();
			if (x > y) break;
		}

		// at this point y contains the number of the line to be inserted
		// x contains the number of the first line with a higher line number
		// or 0 if the line is to be inserted at the end
		// here points to the following line and here2 points to the previous line

		if (x == 0) { 
			here=here3-lnlength;
			gettoken();
			if (token == LINENUMBER && x == y) { // we have a double line at the end
				here2-=lnlength;
				here-=lnlength;
				moveblock(here2, linelength, here);
				top=here+linelength;
			}
			return;
		}
		here-=lnlength;
		t1=here;
		here=here2-lnlength;
		t2=here;
		gettoken();
		if (x == y) {     // the line already exists and has to be replaced
			here2=t2;  // this is the line we are dealing with
			here=t1;   // this is the next line
			y=here-here2; // the length of the line as it is 
			if (linelength == y) {     // no change in line length
				moveblock(top-linelength, linelength, here2);
				top=top-linelength;
			} else if (linelength > y) { // the new line is longer than the old one
				moveblock(here, top-here, here+linelength-y);
				here=here+linelength-y;
				top=top+linelength-y;
				moveblock(top-linelength, linelength, here2);
				top=top-linelength;
			} else {					// the new line is short than the old one
				moveblock(top-linelength, linelength, here2);
				top=top-linelength;
				moveblock(here, top-here, here2+linelength);
				top=top-y+linelength;
			}
		} else {         // the line has to be inserted in between
			here=t1;
			moveblock(here, top-here, here+linelength);
			moveblock(top, linelength, here);
		}
	}
}

/* 
 
 calculates an expression, with a recursive descent algorithm
 using the functions term, factor and expression
 all function use the stack to pass values back. We use the 
 Backus-Naur form of basic from here https://rosettacode.org/wiki/BNF_Grammar
 implementing a C style logical expression model

*/

char termsymbol() {
	return ( token == LINENUMBER ||  token == ':' || token == EOL);
}

// a little helpers - one token expect 
char expect(signed char t, char e) {
	nexttoken();
	if (token != t) {error(e); return FALSE; } else return TRUE;
}

// a little helpers - expression expect
char expectexpr() {
	nexttoken();
	expression();
	if (er != 0) return FALSE; else return TRUE;
}


// parses a list of expression
short parsearguments() {
	char args=0;

	// having 0 args at the end of a command is legal
	if (termsymbol()) return args;

	// list of expressions separated by commas
	do {
		expression();
		if (er != 0) break;
		args++;
		if (token != ',') break;
		nexttoken();
	} while (TRUE);

	return args;
}


// expect exactly n arguments
void parsenarguments(char n) {
	if (parsearguments() != n ) error(EARGS);
}

// counts and parses the number of arguments given in brakets
short parsesubscripts() {
	char args = 0;
	if (token != '(') {return 0; } // zero arguments is legal here
	nexttoken();
	args=parsearguments();
	if (er != 0) return 0; 
	if (token != ')') {error(EARGS); return 0; }
	return args;
}


// parse a function argument ae is the number of 
// expected expressions in the argument list
void parsefunction(void (*f)(), short ae){
	char args;
	nexttoken();
	args=parsesubscripts();
	if (er != 0) return;
	if (args == ae) f(); else error(EARGS);
}

// helper function in the recursive decent parser
void parseoperator(void (*f)()) {
	nexttoken();
	f();
	if (er !=0 ) return;
	y=pop();
	x=pop();
}

// substring evaluation, mind the rewinding here - a bit of a hack
#ifdef HASAPPLE1
void parsesubstring() {
	char xc1, yc1; 
	char args;
	address_t h1; // remember the here
	char * bi1;

	// remember the string name
    xc1=xc;
    yc1=yc;

    if (st == SINT) // this is a hack - we rewind a token !
    	bi1=bi;
    else 
    	h1=here; 

    nexttoken();
    args=parsesubscripts();
    if (er != 0) return; 

    switch(args) {
    	case 2: 
    		break;
		case 1:
			push(lenstring(xc1, yc1));
			break;
		case 0: 
			if ( st == SINT) // this is a hack - we rewind a token !
				bi=bi1;
			else 
				here=h1; 
			push(1);
			push(lenstring(xc1, yc1));	
			break;
    }
}
#endif

/*
// absolute value, 
number_t babs(number_t n) {
	if (n<0) return -n; else return n;
}
*/

void xabs(){
	if ((x=pop())<0) { x=-x; }
	push(x);
}

// sign
void xsgn(){
	number_t n;
	n=pop();
	if (n>0) n=1;
	if (n<0) n=-1;
	push(n);
}

// on an arduino, negative values of peek address 
// the EEPROM range -1 .. -1024 on an UNO
void xpeek(){
	address_t amax;
	address_t a;
	a=pop();

	// this is a hack again, 16 bit numbers can't peek big addresses
	if ((long) memsize > (long) maxnum) amax=(address_t) maxnum; else amax=memsize;

	if (a >= 0 && a<amax) 
		push(mem[a]);
	else if (a < 0 && -a < elength())
		push(eread(-a-1));
	else {
		error(ERANGE);
		return;
	}
}

// the fre function 
void xfre() {
	if (pop() >=0 )
		push(himem-top);
	else 
		push(elength());
}


// very basic random number generator with constant seed.
void rnd() {
	number_t r;
	r=pop();
	rd = (31421*rd + 6927) % 0x10000;
	if (r>=0) 
		push((long)rd*r/0x10000);
	else 
		push((long)rd*r/0x10000+1);
}


#ifndef HASFLOAT
// a very simple approximate square root formula. 
void sqr(){
	number_t t,r;
	number_t l=0;
	r=pop();
	t=r;
	while (t > 0) {
		t>>=1;
		l++;
	}
	l=l/2;
	t=1;
	t<<=l;
	do {
		l=t;
		t=(t+r/t)/2;
	} while (abs(t-l)>1);
	push(t);
}
#else
void sqr(){
	push(sqrt(pop()));
}
#endif

#ifndef HASFLOAT
// powers 
void xpow(){
	number_t n;
	number_t a;
	int i;

	n=pop();
	a=pop();

	x=1;
	if (n>=0) 
		for(i=0; i<n; i++) x*=a; 
	else 
		x=0;
	
	push(x);
}
#else
void xpow(){
	number_t n;
	n=pop();
	push(pow(pop(),n));
}
#endif

// evaluates a string value, FALSE if there is no string
// ir2 has the string location, the stack has the length
char stringvalue() {
	char xcl;
	char ycl;
	if (token == STRING) {
		ir2=ir;
		push(x);
#ifdef HASAPPLE1
	} else if (token == STRINGVAR) {
		xcl=xc;
		ycl=yc;
		parsesubstring();
		if (er != 0) return FALSE;
		y=pop();
		x=pop();
		ir2=getstring(xcl, ycl, x);
		push(y-x+1);
		xc=xcl;
		yc=ycl;
#endif
	} else {
		return FALSE;
	}
	return TRUE;
}

// (numerical) evaluation of a string expression used for 
// comparison and for string rightvalues as numbers
// the token rewind here is needed as streval is called in 
// factor - no factor function can nexttoken
void streval(){
	char *irl;
	address_t xl, x;
	char xcl;
	signed char t;
	address_t h1;

	if ( ! stringvalue()) {
		error(EUNKNOWN);
		return;
	} 
	if (er != 0) return;
	irl=ir2;
	xl=pop();

	h1=here; // get ready for rewind.
	t=token;

	nexttoken();

	if (token != '=' && token != NOTEQUAL) {
		here=h1; // rewind one token if not comparison
		token=t;
		if (xl == 0) push(0); else push(irl[0]); // a zero string length evaluate to 0
		return; 
	}
	t=token;

	nexttoken(); 
	//debugtoken();

	if (!stringvalue() ){
		error(EUNKNOWN);
		return;
	} 
	x=pop();
	if (er != 0) return;

	if (x != xl) goto neq;
	for (x=0; x < xl; x++) if ( irl[x] != ir2[x] ) goto neq;

	if (t == '=') push(1); else push(0);
	return;
neq:
	if (t == '=') push(0); else push(1);
	return;
}

#ifdef HASFLOAT
void xsin() {
	push(sin(pop()));
}

void xcos() {
	push(cos(pop()));
}

void xtan() {
	push(tan(pop()));
}

void xatan() {
	push(atan(pop()));
}

void xlog() {
	push(log(pop()));
}

void xexp() {
	push(exp(pop()));
}

void xint() {
	push(floor(pop()));
}
#else 
void xint() {}
#endif

#ifdef HASDARKARTS
// allocate a chunk of memory, currently limited to 8 bits
void xmalloc() {
	address_t h; 
	address_t s;
	s=pop();
	if (s<1) {error(ERANGE); return; };
	h=pop();
	push(bmalloc(TBUFFER, h%256, 0, s));
}

// find an object on the heap
void xfind() {
	address_t h;
	h=pop();
	push(bfind(TBUFFER, h%256, 0));
}
#endif

#ifdef HASIOT
// NEXT can be a function in the context of iterators
void xinext() {
	push(pop());
}
#endif

#ifdef HASDARTMOUTH
void xfn() {
	char fxc, fyc;
	char vxc, vyc;
	address_t a;
	address_t h1, h2;
	number_t xt;

	// the name of the function

	if (!expect(ARRAYVAR, EUNKNOWN)) return;
	fxc=xc;
	fyc=yc;

	// and the argument
	nexttoken();
	if (token != '(') {error(EUNKNOWN); return; }

	nexttoken();
	expression();
	if (er != 0) return;

	if (token != ')') {error(EUNKNOWN); return; }

	// find the function structure and retrieve the payload
	if ( (a=bfind(TFN, fxc, fyc)) == 0 ) {error(EUNKNOWN); return; }
	getnumber(a, addrsize);
	h1=z.a;
	vxc=mem[a+addrsize];
	vyc=mem[a+addrsize+1];

	// remember the original value of the variable and set it
	xt=getvar(vxc, vyc);
	if (DEBUG) {outsc("** saving the original running var "); outch(vxc); outch(vyc); outspc(); outnumber(xt); outcr();}


	setvar(vxc, vyc, pop());

	// store here and then evaluate the function
	h2=here;
	here=h1;
	if (DEBUG) {outsc("** evaluating expressing at "); outnumber(here); outcr(); }

	if (!expectexpr()) return;

	// restore everything
	here=h2;
	setvar(vxc, vyc, xt);

	// no nexttoken as this is called in factor
}
#endif

#ifdef HASIOT
void xavail() {
	unsigned char oid=id;
	id=pop();
	push(availch());
	id=oid;
}
#endif


// the factor function - contrary to all other function
// nothing here should end with a new token - this is handled 
// in factors calling function
void factor(){
	char args;
	if (DEBUG) debug("factor\n");
	switch (token) {
		case NUMBER: 
			push(x);
			break;
		case VARIABLE: 
			push(getvar(xc, yc));	
			break;
		case ARRAYVAR:
			push(yc);
			push(xc);
			nexttoken();
			args=parsesubscripts();
			if (er != 0 ) return;
			if (args != 1) { error(EARGS); return; }	
			x=pop();
			xc=pop();
			yc=pop();
			array('g', xc, yc, x, &y);
			push(y); 
			break;
		case '(':
			nexttoken();
			expression();
			if (er != 0 ) return;
			if (token != ')') { error(EARGS); return; }
			break;
// Palo Alto BASIC functions
		case TABS: 
			parsefunction(xabs, 1);
			break;
		case TRND: 
			parsefunction(rnd, 1);
			break;
		case TSIZE:
			push(himem-top);
			break;
// Apple 1 BASIC functions
#ifdef HASAPPLE1
		case TSGN: 
			parsefunction(xsgn, 1);
			break;
		case TPEEK: 
			parsefunction(xpeek, 1);
			break;
		case TLEN: 
			nexttoken();
			if ( token != '(') {
				error(EARGS);
				return;
			}
			nexttoken();
			if (! stringvalue()) {
#ifdef HASDARKARTS
				expression();
				if (er != 0) return;
				push(blength(TBUFFER, ((address_t) pop())%256, 0));
				return;
#else 
				error(EUNKNOWN);
				return;
#endif
			}
			if (er != 0) return;
			nexttoken();
			if (token != ')') {
				error(EARGS);
				return;	
			}
			break;
#ifdef HASIOT
		case TAVAIL:
			parsefunction(xavail, 1);
			break;	
#endif
		case TLOMEM:
			push(0);
			break;
		case THIMEM:
			push(himem);
			break;
// Apple 1 string compare code
		case STRING:
		case STRINGVAR:
			streval();
			if (er != 0 ) return;
			break;
#endif
//  Stefan's tinybasic additions
#ifdef HASSTEFANSEXT
		case TSQR: 
			parsefunction(sqr, 1);
			break;
		case TFRE: 
			parsefunction(xfre, 1);
			break;
		case TUSR:
			parsefunction(xusr, 2);
			break;
		case TPOW:
			parsefunction(xpow, 2);
			break;
#endif
// Arduino I/O
#ifdef HASARDUINOIO
		case TAREAD: 
			parsefunction(aread, 1);
			break;
		case TDREAD: 
			parsefunction(dread, 1);
			break;
		case TMILLIS: 
			parsefunction(bmillis, 1);
			break;	
#ifdef HASPULSE
		case TPULSEIN:
			parsefunction(bpulsein, 3);
			break;
#endif
		case TAZERO:
#ifdef ARDUINO
			push(A0);
#else 
			push(0);
#endif			
			break;
#endif
// mathematical functions
#ifdef HASFLOAT
		case TSIN:
			parsefunction(xsin, 1);
			break;
		case TCOS:
			parsefunction(xcos, 1);
			break;
		case TTAN:
			parsefunction(xtan, 1);
			break;		
		case TATAN:
			parsefunction(xatan, 1);
			break;
		case TLOG:
			parsefunction(xlog, 1);
			break;
		case TEXP:
			parsefunction(xexp, 1);
			break;
#endif
		// int is always present to make programs compatible
		case TINT:
			parsefunction(xint, 1);
			break;
#ifdef HASDARTMOUTH
		case TFN:
			xfn();
			break;
#endif
#ifdef HASDARKARTS
		case TMALLOC:
			parsefunction(xmalloc, 2);
			break;	
		case TFIND:
			parsefunction(xfind, 1);
			break;
#endif
#ifdef HASIOT
		case TNEXT:
			parsefunction(xinext, 1);
			break;
#endif

// unknown function
		default:
			error(EUNKNOWN);
			return;
	}
}

void term(){
	if (DEBUG) debug("term\n"); 
	factor();
	if (er != 0) return;

nextfactor:
	nexttoken();
	if (DEBUG) debug("in term\n");
	if (token == '*'){
		parseoperator(factor);
		if (er != 0) return;
		push(x*y);
		goto nextfactor;
	} else if (token == '/'){
		parseoperator(factor);
		if (er != 0) return;
		if (y != 0)
			push(x/y);
		else {
			error(EDIVIDE);
			return;	
		}
		goto nextfactor;
	} else if (token == '%'){
		parseoperator(factor);
		if (er != 0) return;
		if (y != 0)
#ifndef HASFLOAT
			push(x%y);
#else 
			push((int)x%(int)y);
#endif
		else {
			error(EDIVIDE);
			return;	
		}
		goto nextfactor;
	} 
}

void addexpression(){
	if (DEBUG) debug("addexp\n");
	if (token != '+' && token != '-') {
		term();
		if (er != 0) return;
	} else {
		push(0);
	}

nextterm:
	if (token == '+' ) { 
		parseoperator(term);
		if (er != 0) return;
		push(x+y);
		goto nextterm;
	} else if (token == '-'){
		parseoperator(term);
		if (er != 0) return;
		push(x-y);
		goto nextterm;
	}
}

void compexpression() {
	if (DEBUG) debug("compexp\n"); 
	addexpression();
	if (er != 0) return;
	switch (token){
		case '=':
			parseoperator(compexpression);
			if (er != 0) return;
			push(x == y);
			break;
		case NOTEQUAL:
			parseoperator(compexpression);
			if (er != 0) return;
			push(x != y);
			break;
		case '>':
			parseoperator(compexpression);
			if (er != 0) return;
			push(x > y);
			break;
		case '<':
			parseoperator(compexpression);
			if (er != 0) return;
			push(x < y);
			break;
		case LESSEREQUAL:
			parseoperator(compexpression);
			if (er != 0) return;
			push(x <= y);
			break;
		case GREATEREQUAL:
			parseoperator(compexpression);
			if (er != 0) return;
			push(x >= y);
			break;
	}
}

void notexpression() {
	if (DEBUG) debug("notexp\n");
	if (token == TNOT) {
		nexttoken();
		compexpression();
		if (er != 0) return;
		x=pop();
		push(!x);
	} else 
		compexpression();
}

void andexpression() {
	if (DEBUG) debug("andexp\n");
	notexpression();
	if (er != 0) return;
	if (token == TAND) {
		parseoperator(expression);
		if (er != 0) return;
		push(x && y);
	} 
}

void expression(){
	if (DEBUG) debug("exp\n"); 
	andexpression();
	if (er != 0) return;
	if (token == TOR) {
		parseoperator(expression);
		if (er != 0) return;
		push(x || y);
	}  
}

/* 
	The commands and their helpers
    
    Palo Alto BASIC languge set - print, let, input, goto, gosub, return,
    	if, for, to, next, step, (break), stop (end), list, new, run, rem
    	break is not Palo ALto but fits here, end is identical to stop.

*/



/*
   print 
*/ 

void xprint(){
	char semicolon = FALSE;
	char oldod;
	char modifier = 0;

	form=0;
	oldod=od;

	nexttoken();

processsymbol:

	if (termsymbol()) {
		if (! semicolon) outcr();
		nexttoken();
		od=oldod;
		form=0;
		return;
	}
	semicolon=FALSE;

	if (stringvalue()) {
		if (er != 0) return;
 		outs(ir2, pop());
 		nexttoken();
		goto separators;
	}

	// modifiers of the print statement
	if (token == '#' || token == '&') {
		modifier=token;


		nexttoken();
		expression();
		if (er != 0) return;


		switch(modifier) {
			case '#':
				form=pop();
				break;
			case '&':
				od=pop();
				break;
		}
		goto processsymbol;
	}

	if (token != ',' && token != ';' ) {
		expression();
		if (er != 0) return;
		outnumber(pop());
	}

separators:
	if (token == ',')  {
		if (! modifier ) outspc(); 
		nexttoken();	
	}
	if (token == ';'){
		semicolon=TRUE;
		nexttoken();
	}
	modifier=0;

	goto processsymbol;
}

/* 
	
	assigment code for various lefthand and righthand side. 

	lefthandside is a helper function for reuse in other 
	commands. It determines the address the value is to be 
	assigned to and whether the assignment target is a 
	"pure" i.e. subscriptless string expression

	assignnumber assigns a number to a given lefthandside

*/

void lefthandside(address_t* i, char* ps) {
	char args;

	switch (token) {
		case VARIABLE:
			nexttoken();
			break;
		case ARRAYVAR:
			nexttoken();
			args=parsesubscripts();
			nexttoken();
			if (er != 0) return;
			if (args != 1) {
				error(EARGS);
				return;
			}
			*i=pop();
			break;
#ifdef HASAPPLE1
		case STRINGVAR:
			nexttoken();
			args=parsesubscripts();
			if (er != 0) return;
			switch(args) {
				case 0:
					*i=1;
					*ps=TRUE;
					break;
				case 1:
					*ps=FALSE;
					nexttoken();
					*i=pop();
					break;
				default:
					error(EARGS);
					return;
			}
			break;
#endif
		default:
			error(EUNKNOWN);
			return;
	}
}

void assignnumber(signed char t, char xcl, char ycl, address_t i, char ps) {

		switch (t) {
			case VARIABLE:
				x=pop();
				setvar(xcl, ycl , x);
				break;
			case ARRAYVAR: 
				x=pop();	
				array('s', xcl, ycl, i, &x);
				break;
#ifdef HASAPPLE1
			case STRINGVAR:
				ir=getstring(xcl, ycl, i);
				if (er != 0) return;
				ir[0]=pop();
				if (ps)
					setstringlength(xcl, ycl, 1);
				else 
					if (lenstring(xcl, ycl) < i && i < stringdim(xcl, ycl)) setstringlength(xcl, ycl, i);
				break;
#endif
		}
}


// the core assigment function
void assignment() {
	signed char t;  // remember the left hand side token until the end of the statement, type of the lhs
	char ps=TRUE;  // also remember if the left hand side is a pure string of something with an index 
	char xcl, ycl; // to preserve the left hand side variable names
	address_t i=1;      // and the beginning of the destination string  
	address_t lensource, lendest, newlength;
	char s;
	int j;

	// this code evaluates the left hand side
	ycl=yc;
	xcl=xc;
	t=token;

	lefthandside(&i, &ps);
	if (er != 0) return;

	// the assignment part
	if (token != '=') {
		error(EUNKNOWN);
		return;
	}
	nexttoken();

	// here comes the code for the right hand side
	// rewritten 
	switch (t) {
		case VARIABLE:
		case ARRAYVAR: // the lefthandside is a scalar, evaluate the righthandside as a number
			expression();
			if (er != 0 ) return;
			assignnumber(t, xcl, ycl, i, ps);
			break;
#ifdef HASAPPLE1
		case STRINGVAR: // the lefthandside is a string 
			// we try to evaluate the righthandside as a stringvalue
			s=stringvalue();
			if (er != 0 ) return;

			// and then as an expression
			if (!s) {
				expression();
				if (er != 0 ) return;
				assignnumber(t, xcl, ycl, i, ps);
				break;
			}

			// this is the string righthandside code - how long is the rightandside
			lensource=pop();

			// the destination address of the lefthandside
			ir=getstring(xcl, ycl, i);
			if (er != 0) return;

			// the length of the original string
			lendest=lenstring(xcl, ycl);

			if (DEBUG) {
				outsc("* assigment stringcode "); outch(xcl); outch(ycl); outcr();
				outsc("** assignment source string length "); outnumber(lensource); outcr();
				outsc("** assignment dest string length "); outnumber(lendest); outcr();
				outsc("** assignment old string length "); outnumber(lenstring(xcl, ycl)); outcr();
				outsc("** assignment dest string dimension "); outnumber(stringdim(xcl, ycl)); outcr();
			};

			// does the source string fit into the destination
			if ((i+lensource-1) > stringdim(xcl, ycl)) { error(ERANGE); return; }

			// this code is needed to make sure we can copy one string to the same string 
			// without overwriting stuff, we go either left to right or backwards
			if (x > i) 
				for (j=0; j<lensource; j++) { ir[j]=ir2[j];}
			else
				for (j=lensource-1; j>=0; j--) ir[j]=ir2[j]; 

			// classical Apple 1 behaviour is string truncation in substring logic
			newlength = i+lensource-1;	
		
			setstringlength(xcl, ycl, newlength);
			break;
#endif
	}

	nexttoken();
}

/*
	input ["string",] variable [,["string",] variable]* 

*/

void xinput(){
	char args;
	char oldid = 0;

	nexttoken();

	// modifiers of the input statement
	if (token == '&') {

		if(!expectexpr()) return;

		oldid=id;
		id=pop();
		if ( token != ',') {
			error(EUNKNOWN);
			return;
		} else 
			nexttoken();
	}


nextstring:
	if (token == STRING && id != IFILE) {   
		outs(ir, x);
		nexttoken();
		if (token != ',' && token != ';') {
			error(EUNKNOWN);
			return;
		} else 
			nexttoken();
	}

nextvariable:
	if (token == VARIABLE) {   
		if (id == ISERIAL || id == IKEYBOARD) outsc("? ");
		if (innumber(&x) == BREAKCHAR) {
			setvar(xc, yc, 0);
			st=SINT;
			nexttoken();
			id=oldid;
			ert=1;
			return;
		} else {
			setvar(xc, yc, x);
		}
	} 

	if (token == ARRAYVAR) {

		nexttoken();
		args=parsesubscripts();
		if (er != 0 ) return;
		if (args != 1) {
			error(EARGS);
			return;
		}

		if (id == ISERIAL || id == IKEYBOARD) outsc("? ");
		if (innumber(&x) == BREAKCHAR) {
			x=0;
			array('s', xc, yc, pop(), &x);
			st=SINT;
			nexttoken();
			id=oldid;
			ert=1;
			return;
		} else {
			array('s', xc, yc, pop(), &x);
		}
	}

#ifdef HASAPPLE1
	/* strings are not passed through the input buffer but inputed directly 
	   in the string memory location */
	if (token == STRINGVAR) {
		ir=getstring(xc, yc, 1); 
		if (id == ISERIAL || id == IKEYBOARD) outsc("? ");
		ins(ir-1, stringdim(xc, yc));
		/* this is the length information correction for large strings, ins
			stored the string length in z.a as a side effect */
		if (xc != '@' && strindexsize == 2) { 
			*(ir-2)=z.b.l;
			*(ir-1)=z.b.h;
		}
 	}
#endif

	nexttoken();
	if (token == ',' || token == ';') {
		nexttoken();
		goto nextstring;
	}

	if (oldid != 0) id=oldid;

}

/*

	goto, gosub, return and its helpers

*/


void pushgosubstack(){
	if (gosubsp < GOSUBDEPTH) {
		gosubstack[gosubsp]=here;
		gosubsp++;	
	} else 
		error(EGOSUB);
}

void popgosubstack(){
	if (gosubsp>0) {
		gosubsp--;
	} else {
		error(ERETURN);
		return;
	} 
	here=gosubstack[gosubsp];
}

void dropgosubstack(){
	if (gosubsp>0) {
		gosubsp--;
	} else {
		error(EGOSUB);
	} 
}

void clrgosubstack() {
	gosubsp=0;
}

// goto and gosub function for a simple one statement goto
void xgoto() {
	signed char t=token;

	if (!expectexpr()) return;
	if (t == TGOSUB) pushgosubstack();
	if (er != 0) return;

	x=pop();

	if (DEBUG) { outsc("** goto/gosub evaluated line number "); outnumber(x); outcr(); }
	findline(x);
	if (er != 0) return;

	// goto in interactive mode switched to RUN mode
	// no clearing of variables and stacks
	if (st == SINT) st=SRUN;

	nexttoken();
}

// return retrieves here from the gosub stack
void xreturn(){ 
	popgosubstack();
	if (er != 0) return;
	nexttoken();
}


/* 

	if and then

*/


void xif() {
	
	if (!expectexpr()) return;

	x=pop();
	if (DEBUG) { outnumber(x); outcr(); } 

	// on condition false skip the entire line 
	if (!x) while(token != LINENUMBER && token != EOL) nexttoken();
	
	// a then token is interpreted as simple one statement goto	
	if (token == TTHEN) {
		nexttoken();
		if (token == NUMBER) {
			findline(x);
			if (er != 0) return;		
		}
	} 
}

/* 

	for, next and the apocryphal break

*/ 

// find the NEXT token or the end of the program
void findnextcmd(){
	while (TRUE) {
	    if (token == TNEXT) {
	    	if (fnc == 0) return;
	    	else fnc--;
	    }
	    if (token == TFOR) fnc++;
	    if (here >= top) {
	    	error(TFOR);
	    	return;
	    }
		nexttoken(); 
	}
}


/*
	for variable = expression to expression [STEP expression]
	for stores the variable, the increment and the boudary on the 
	for stack. Changing steps and boundaries during the execution 
	of a loop has no effect 
*/

void xfor(){
	char xcl, ycl;
	number_t b=1;
	number_t e=maxnum;
	number_t s=1;
	
	// there has to be a variable
	if (!expect(VARIABLE, EUNKNOWN)) return;
	xcl=xc;
	ycl=yc;

	// this is not standard BASIC all combinations of 
	// FOR TO STEP are allowed
	nexttoken();
	if (token == '=') { 
		if (!expectexpr()) return;
		b=pop();
	}

	if (token == TTO) {
		if (!expectexpr()) return;
		e=pop();
	}

	if (token == TSTEP) {
		if (!expectexpr()) return;
		s=pop();
	} 

	if (! termsymbol()) {
		error(UNKNOWN);
		return;
	}

	if (st == SINT)
		here=bi-ibuffer;

	// here we know everything to set up the loop	
	setvar(xcl, ycl, b);
	if (DEBUG) { 
		outsc("** for loop with parameters var begin end step : ");
		outch(xcl); outch(ycl); outspc(); outnumber(b); outspc(); outnumber(e); outspc(); outnumber(s); outcr(); }
	xc=xcl;
	yc=ycl;
	x=e;
	y=s;
	pushforstack();
	if (er != 0) return;

/*
	this tests the condition and stops if it is fulfilled already from start 
	there is an apocryphal feature here: STEP 0 is legal triggers an infinite loop
*/
	if ( (y > 0 && getvar(xc, yc)>x) || (y < 0 && getvar(xc, yc)<x ) ) { 
		dropforstack();
		findnextcmd();
		nexttoken();
		if (token == VARIABLE) nexttoken(); /* more evil - this should really check */
		return;
	}
}

/*
	an apocryphal feature here is the BREAK command ending a loop
	doesn't work well for nested loops - to be tested carefully
*/
void xbreak(){
	dropforstack();
	findnextcmd();
	nexttoken();
}

/* next variable statement */
void xnext(){
	char xcl=0;
	char ycl;
	address_t h;
	number_t t;

	nexttoken();

	// one variable is accepted as an argument, no list
	if (token == VARIABLE) {
		if (DEBUG) { outsc("** variable argument "); outch(xc); outch(yc); outcr(); }
		xcl=xc;
		ycl=yc;
		nexttoken();
		if (!termsymbol()) {
			error(EUNKNOWN);
			return;
		}
	}

	// remember the current position
	h=here;
	popforstack();
	if (er != 0) return;
	// a variable argument in next clears the for stack 
	// down as BASIC programs can and do jump out to a outer 
	// next
	if (xcl) {
		while (xcl != xc || ycl != yc ) {
			popforstack();
			if (er != 0) return;
		} 
	}

	// y=0 an infinite loop with step 0
	t=getvar(xc, yc)+y;
	setvar(xc, yc, t);

	// do we need another iteration, STEP 0 always triggers an infinite loop
	if ( (y==0) || (y > 0 && t <= x) || (y < 0 && t >= x) ) {
		// push the loop with the new values back to the for stack
		pushforstack();
		if (st == SINT) bi=ibuffer+here;
	} else {
		// last iteration completed we stay here after the next
		here=h;
	}
	nexttoken();
	if (DEBUG) {outsc("** after next found token "); debugtoken(); }
}

/* 
	
	list - this is also used in save

*/

void outputtoken() {
	unsigned short i;

	switch (token) {
		case NUMBER:
			outnumber(x);
			return;
		case LINENUMBER: 
			outnumber(x);
			outspc();
			return;
		case ARRAYVAR:
		case STRINGVAR:
		case VARIABLE:
			outch(xc); 
			if (yc != 0) outch(yc);
			if (token == STRINGVAR) outch('$');
			return;
		case STRING:
			outch('"'); 
			outs(ir, x); 
			outch('"');
			return;
		default:
			if (token < -3) {
				if (token == TTHEN || token == TTO || token == TSTEP || token == TGOTO || token == TGOSUB) outspc(); 
				for(i=0; gettokenvalue(i)!=0 && gettokenvalue(i)!=token; i++);
				outsc(getkeyword(i)); 
				if (token != GREATEREQUAL && token != NOTEQUAL && token != LESSEREQUAL) outspc();
				return;
			}	
			if (token >= 32) {
				outch(token);
				return;
			} 
			outch(token); outspc(); outnumber(token);
	}

}


void xlist(){
	number_t b, e;
	address_t arg;
	char oflag = FALSE;

	nexttoken();
 	arg=parsearguments();
	if (er != 0) return;
	switch (arg) {
		case 0: 
			b=0;
			e=32767;
			break;
		case 1: 
			b=pop();
			e=b;
			break;
		case 2: 
			e=pop();
			b=pop();
			break;
		default:
			error(EARGS);
			return;
	}

	if ( top == 0 ) { nexttoken(); return; }

	here=0;
	gettoken();
	while (here < top) {
		if (token == LINENUMBER && x >= b) oflag=TRUE;
		if (token == LINENUMBER && x >  e) oflag=FALSE;
		if (oflag) outputtoken();
		gettoken();
		if (token == LINENUMBER && oflag) {
			outcr();
			// wait after every line on small displays
			// if ( dspactive() && (dsp_rows < 10) ){ if ( inch() == 27 ) break;}
			if (dspactive()) 
				if ( dspwaitonscroll() == 27 ) break;
		}
	}
	if (here == top && oflag) outputtoken();
    if (e == 32767 || b != e) outcr(); // supress newlines in "list 50" - a little hack

	nexttoken();
 }


void xrun(){
	if (token == TCONT) {
		st=SRUN;
		nexttoken();
		goto statementloop;
	} 

	nexttoken();
	y=parsearguments();
	if (er != 0 ) return;
	if (y > 1) { error(EARGS); return; }
	if (y == 0) 
		here=0;
	else
		findline(pop());
	if ( er != 0 ) return;
	if (st == SINT) st=SRUN;

	xclr();

statementloop:
	while ( (here < top) && (st == SRUN || st == SERUN) && ! er) {	
		statement();
	}
	st=SINT;
}


void xnew(){ // the general cleanup function
	clearst();
	himem=memsize;
	top=0;
	zeroblock(top,himem);
	reseterror();
	st=SINT;
	nvars=0;
	clrgosubstack();
	clrforstack();

}


void xrem() {
	while (token != LINENUMBER && token != EOL && here <= top) nexttoken(); 
}

/* 

	The Apple 1 BASIC additions

*/


/* 

	clearing variable space

*/

void xclr() {
	clrvars();
	clrgosubstack();
	clrforstack();
	clrdata();
	nexttoken();
}

#ifdef HASAPPLE1
/* 

	the dimensioning of arrays and strings from Apple 1 BASIC

*/

void xdim(){
	char args, xcl, ycl; 
	signed char t;

	nexttoken();

nextvariable:
	if (token == ARRAYVAR || token == STRINGVAR ){
		
		t=token;
		xcl=xc;
		ycl=yc;

		nexttoken();
		args=parsesubscripts();
		if (er != 0) return;

		if (args != 1) {error(EARGS); return; }
		x=pop();
		if (x<=0) {error(ERANGE); return; }
		if (t == STRINGVAR) {
			if ( (x>255) && (strindexsize==1) ) {error(ERANGE); return; }
			(void) createstring(xcl, ycl, x);
		} else {
			(void) createarray(xcl, ycl, x);
		}	
	} else {
		error(EUNKNOWN);
		return;
	}


	nexttoken();
	if (token == ',') {	
		nexttoken();
		goto nextvariable;
	}

	nexttoken();
}


/* 
	
	low level poke to the basic memory, works only up to 32767

*/

void xpoke(){
	address_t amax;
	address_t a;

	// like in peek
	// this is a hack again, 16 bit numbers can't peek big addresses
	if ( (long) memsize > (long) maxnum) amax=(address_t) maxnum; else amax=memsize;

	nexttoken();
	parsenarguments(2);
	if (er != 0) return;

	y=pop();
	a=pop();
	if (a >= 0 && a<amax) 
		mem[a]=y;
	else if (a < 0 && a >= -elength())
			eupdate(-a-1, y);
	else {
		error(ERANGE);
	}
}

/*

	the TAB spaces command of Apple 1 BASIC

*/

void xtab(){

	nexttoken();
	parsenarguments(1);
	if (er != 0) return;

	x=pop();
	while (x-- > 0) outspc();	
}

#endif

/*

	Stefan's additions 

*/

void xdump() {
	address_t a;
	char arg;
	
	nexttoken();
	arg=parsearguments();
	if (er != 0) return;


	switch (arg) {
		case 0: 
			x=0;
			a=memsize;
			break;
		case 1: 
			x=pop();
			a=memsize;
			break;
		case 2: 
			a=pop();
			x=pop();
			break;
		default:
			error(EARGS);
			return;
	}

	form=6;
	dumpmem(a/8+1, x);
	form=0;
	nexttoken();
}

void dumpmem(address_t r, address_t b) {
	address_t j, i;	
	address_t k;

	k=b;
	i=r;
	while (i>0) {
		outnumber(k); outspc();
		for (j=0; j<8; j++) {
			outnumber(mem[k++]); outspc();
			delay(1); // slow down a little here for low serial baudrates
			if (k > memsize) break;
		}
		outcr();
		i--;
		if (k > memsize) break;
	}
#ifdef ARDUINOEEPROM
	printmessage(EEEPROM); outcr();
	i=r;
	k=0;
	while (i>0) {
		outnumber(k); outspc();
		for (j=0; j<8; j++) {
			outnumber(eread(k++)); outspc();
			if (k > elength()) break;
		}
		outcr();
		i--;
		if (k > elength()) break;	
	}
#endif
	outsc("top: "); outnumber(top); outcr();
	outsc("himem: "); outnumber(himem); outcr();
}

/*
	creates a C string from a BASIC string
	after reading a BASIC string ir2 contains a pointer
	to the data and x the string length
 */
void stringtobuffer(char *buffer) {
	short i;
	i=x;
	if (i >= SBUFSIZE) i=SBUFSIZE-1;
	buffer[i--]=0;
	while (i >= 0) { buffer[i]=ir2[i]; i--; }
}

/* get a file argument */
void getfilename(char *buffer, char d) {
	char s;
	char *sbuffer;

	s=stringvalue();
	if (er != 0) return;
	if (DEBUG) {outsc("** in getfilename2 stringvalue delivered "); outnumber(s); outcr(); }

	if (s) {
		x=pop();
		if (DEBUG) {outsc("** in getfilename2 copying string of length "); outnumber(x); outcr(); }
		stringtobuffer(buffer);
		if (DEBUG) {outsc("** in getfilename2 stringvalue string "); outsc(buffer); outcr(); }
	} else if (termsymbol()) {
		if (d) {
			sbuffer=getmessage(MFILE);
			s=0;
			while ( (sbuffer[s] != 0) && (s<SBUFSIZE-1)) { buffer[s]=sbuffer[s]; s++; }
			buffer[s]=0;
			x=s;
		} else 
			buffer[0]=0;
			x=0;
	} else {
		error(EUNKNOWN);
	}
}

// save a file either to disk or to EEPROM
void xsave() {
	char filename[SBUFSIZE];
	address_t here2;

#if !defined(ARDUINO) || defined(ARDUINOSD) || defined(ESPSPIFFS)
	nexttoken();
	getfilename(filename, 1);
	if (er != 0) return;
#else 
	filename[0]='!';
#endif

	if (filename[0] == '!') {
		esave();
	} else {		
		if (DEBUG) { outsc("** Opening the file "); outsc(filename); outcr(); };
	 	
		if (!ofileopen(filename)) {
			error(EFILE);
			nexttoken();
			return;
		} 

		// save the output mode and then save
		push(od);
		od=OFILE;
		
		// the core list function
		// we step away from list 
		here2=here;
		here=0;
		gettoken();
		while (here < top) {
			outputtoken();
			gettoken();
			if (token == LINENUMBER) outcr();
		}
		if (here == top) outputtoken();
   		outcr(); 
   		here=here2;

   		// clean up
		ofileclose();

		// restore the output mode
		od=pop();
	}

	// and continue
	nexttoken();
	return;
}

// loading a file 
void xload() {
	char filename[SBUFSIZE];
	char ch;
	address_t here2;
	char chain = FALSE;

#if !defined(ARDUINO) || defined(ARDUINOSD) || defined(ESPSPIFFS)
	nexttoken();
	getfilename(filename, 1);
	if (er != 0) return;
#else 
	filename[0]='!';
#endif

	if (filename[0] == '!') {
		eload();
		nexttoken();
	} else {

		// if load is called during runtime it chains
		// load the program as new but perserve the variables
		// gosub and for stacks are cleared 
		if ( st == SRUN ) { 
			chain=TRUE; 
			st=SINT; 
			top=0;
			clrgosubstack();
			clrforstack();
		}

		if (!ifileopen(filename)) {
			error(EFILE);
			nexttoken();
			return;
		} 

    	bi=ibuffer+1;
		while (fileavailable()) {
      		ch=fileread();
      		if (ch == '\n' || ch == '\r') {
        		*bi=0;
        		bi=ibuffer+1;
        		nexttoken();
        		if (token == NUMBER) storeline();
        		if (er != 0 ) break;
        		bi=ibuffer+1;
      		} else {
        		*bi++=ch;
      		}
      		if ( (bi-ibuffer) > BUFSIZE ) {
        		error(EOUTOFMEMORY);
        		break;
      		}
		}   	
		ifileclose();

		// go back to run mode and start from the first line
		if (chain) {
			st=SRUN;
			here=0;
		}
		nexttoken();
	}
}

/*
	get just one character from input or send one 
*/

void xget(){
	signed char t;  // remember the left hand side token until the end of the statement, type of the lhs
	char ps=TRUE;  // also remember if the left hand side is a pure string of something with an index 
	char xcl, ycl; // to preserve the left hand side variable names
	address_t i=1;      // and the beginning of the destination string  
	unsigned char oid=id;

	nexttoken();

	// modifiers of the get statement
	if (token == '&') {

		if (!expectexpr()) return;
		id=pop();		
		if (token != ',') {
			error(EUNKNOWN);
			return;
		}
		nexttoken();
	}

	// this code evaluates the left hand side - remember type and name
	ycl=yc;
	xcl=xc;
	t=token;

	// find the indices 
	lefthandside(&i, &ps);
	if (er != 0) return;

	// get the data
	if (availch()) push(inch()); else push(0);

	// store the data element as a number
	assignnumber(t, xcl, ycl, i, ps);

	nexttoken();

	id=oid;
}

/*
	PUT writes one character to the default output stream
*/

void xput(){
	unsigned char ood=od;
	char args;
	short i;

	nexttoken();

	// modifiers of the put statement
	if (token == '&') {

		if(!expectexpr()) return;
		od=pop();
		if (token != ',') {
			error(EUNKNOWN);
			return;
		}
		nexttoken();
	}

	args=parsearguments();
	if (er != 0) return;

	for (i=args-1; i>=0; i--) sbuffer[i]=pop();
	outs(sbuffer, args);

	od=ood;
}

/* 
	the set command itself is also apocryphal it is a low level
	control command setting certain properties
	syntax, currently it is only 

	set expression
*/

void xset(){
	address_t fn, arg;

	nexttoken();
	parsenarguments(2);
	if (er != 0) return;

	arg=pop();
	fn=pop();
	switch (fn) {	
		case 0:
			debuglevel=arg;
			break;	
		case 1: // autorun/run flag of the EEPROM 255 for clear, 0 for prog, 1 for autorun
			eupdate(0, arg);
			break;
		case 2: // change the output device 
			switch (arg) {
				case 0:
					od=OSERIAL;
					break;
				case 1:
					od=ODSP;
					break;
			}		
			break;
		case 3: // change the default output device
			switch (arg) {
				case 0:
					od=(odd=OSERIAL);
					break;
				case 1:
					od=(odd=ODSP);
					break;
			}		
			break;
		case 4: // change the input device (deprectated use @i instead)
			switch (arg) {
				case 0:
					id=ISERIAL;
					break;
				case 1:
					id=IKEYBOARD;
					break;
			}		
			break;		
		case 5: // change the default input device 
			switch (arg) {
				case 0:
					idd=(id=ISERIAL);
					break;
				case 1:
					idd=(id=IKEYBOARD);
					break;
			}		
			break;	
#ifdef ARDUINOPRT
		case 6: // set the cr behaviour
			sendcr=(char)arg;
			break;
		case 7: // set the blockmode behaviour
      		blockmode=arg;
      		break;
#endif
#ifdef ARDUINORF24
      	case 8: // set the power amplifier level of the module
      		if ((arg<0) && (arg>3)) {error(ERANGE); return; } 
      		rf24_pa=arg;
      		radio.setPALevel(rf24_pa);
      		break;
#endif
	}
}


/*

	The arduino io functions.

*/

void xdwrite(){
	nexttoken();
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	dwrite(y, x);	
}

void xawrite(){
	nexttoken();
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	awrite(y, x);
}

void xpinm(){
	nexttoken();
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	pinm(y, x);	
}

void xdelay(){
	nexttoken();
	parsenarguments(1);
	if (er != 0) return;

	x=pop();
	delay(x);	
}

#ifdef HASGRAPH
void xcolor() {
	short r, g, b;
	nexttoken();
	parsenarguments(3);
	if (er != 0) return; 
	b=pop();
	g=pop();
	r=pop();
	color(r, g, b);
}

void xplot() {
	short x0, y0;
	nexttoken();
	parsenarguments(2);
	if (er != 0) return; 
	y0=pop();
	x0=pop();
	plot(x0, y0);
}


void xline() {
	short x0, y0, x1, y1;
	nexttoken();
	parsenarguments(4);
	if (er != 0) return; 
	y1=pop(); 
	x1=pop();
	y0=pop();
	x0=pop();
	line(x0, y0, x1, y1);
}

void xrect() {
	short x0, y0, x1, y1;
	nexttoken();
	parsenarguments(4);
	if (er != 0) return; 
	y1=pop(); 
	x1=pop();
	y0=pop();
	x0=pop();
	rect(x0, y0, x1, y1);
}

void xcircle() {
	short x0, y0, r;
	nexttoken();
	parsenarguments(3);
	if (er != 0) return;  
	r=pop();
	y0=pop();
	x0=pop();
	circle(x0, y0, r);
}

void xfrect() {
	short x0, y0, x1, y1;
	nexttoken();
	parsenarguments(4);
	if (er != 0) return; 
	y1=pop(); 
	x1=pop();
	y0=pop();
	x0=pop();
	frect(x0, y0, x1, y1);
}

void xfcircle() {
	short x0, y0, r;
	nexttoken();
	parsenarguments(3);
	if (er != 0) return;  
	r=pop();
	y0=pop();
	x0=pop();
	fcircle(x0, y0, r);
}
#endif


#ifdef HASTONE
void xtone(){
	char args;

	nexttoken();
	args=parsearguments();
	if (er != 0) return;
	if (args>3 || args<2) {
		error(EARGS);
		return;
	}

#if !defined(ARDUINO) || defined(ARDUINO_ARCH_SAM)
	clearst();
	return;
#else 
	x=pop();
	y=pop();
	if (args=2) {
		tone(y,x);
	} else {
		z.i=pop();
		tone(z.i, y, x);
	}
#endif		
}
#endif

/*

  	SD card DOS - basics to access an SD card as mass storage from BASIC

*/

#if defined(HASFILEIO) || defined(ARDUINORF24)

// string equal helper in catalog 
char streq(char *s, char *m){
	short i=0;
	while (m[i]!=0 && s[i]!=0 && i < SBUFSIZE){
		if (s[i] != m[i]) return 0;
		i++;
	}	
	return 1;
}

// basic directory function
void xcatalog() {

	char filename[SBUFSIZE];
	char *name;

	nexttoken();
	getfilename(filename, 0);
	if (er != 0) return; 

	rootopen();
	while (rootnextfile()) {
		if( rootisfile()) {
			name=rootfilename();
			if (*name != '_' && *name !='.' && streq(name, filename)){
				outscf(name, 14); outspc();
				if (rootfilesize()>0) outnumber(rootfilesize()); 
				outcr();
				if ( dspwaitonscroll() == 27 ) break;
			}
		}
		rootfileclose();
	}
	rootclose();
	nexttoken();
}

void xdelete() {
	char filename[SBUFSIZE];

	nexttoken();
	getfilename(filename, 0);
	if (er != 0) return; 

#ifndef ARDUINO
	remove(filename);
#else 
#ifdef ARDUINOSD	
	SD.remove(filename);
#else 
#ifdef ESPSPIFFS
	SPIFFS.remove(filename);
#endif
#endif
#endif
	nexttoken();
}


#ifdef ARDUINORF24
/*
	generate a uint64_t pipe address from the filename string
	for RF64, horner schema to be on the save side
*/
uint64_t pipeaddr(char * f){
	uint64_t t = 0;
	t=(uint8_t)f[0];
	for(short i=1; i<=4; i++) t=t*256+(uint8_t)f[i];
	return t;
} 
#endif

void xopen() {
	char stream = IFILE; // default is file operation
	char filename[SBUFSIZE];
	char args=0;
	char mode;
	char nlen;

	// which stream do we open? default is FILE
	nexttoken();
	if (token == '&') {
		if (!expectexpr()) return;
		stream=pop();
		if (token != ',') {error(EUNKNOWN); return; }
		nexttoken();
	}
	
	// the filename and its length
	getfilename(filename, 0);
	if (er != 0) return; 
	nlen=x;

	// and the arguments
	nexttoken();
	if (token == ',') { 
		nexttoken(); 
		args=parsearguments();
	}

	if (args == 0 ) { 
		mode=0; 
	} else if (args == 1) {
		mode=pop();
	} else {
		error(EARGS);
		return;
	}
	switch(stream) {
		case IFILE:
			if (mode == 1) {
				ofileclose();
				if (ofileopen(filename)) ert=0; else ert=1;
			} else if (mode == 0) {
				ifileclose();
				if (ifileopen(filename)) ert=0; else ert=1;
			}
			break;
		case IRADIO:
			if (mode == 0) {
				iradioopen(filename);
			} else if (mode == 1) {
				oradioopen(filename);
			}
			break;
		case IWIRE:
			if (nlen == 2)
				wireopen(filename);
			else {
				error(ERANGE);
				return;
			}
			break;
		default:
			error(ERANGE);
			return;
	}
	nexttoken();
}

void xclose() {
	char stream = IFILE;
	char mode;
	char args=0;

	nexttoken();
	if (token == '&') {
		if (!expectexpr()) return;
		stream=pop();
		if (token != ',' && ! termsymbol()) {error(EUNKNOWN); return; }
		nexttoken();
	}

	args=parsearguments();

	if (args == 0 ) { 
		mode=0; 
	} else if (args == 1) {
		mode=pop();
	} else {
		error(EARGS);
		return;
	}

	switch(stream) {
		case IFILE:
			if (mode == 1) ofileclose(); else if (mode == 0) ifileclose();
			break;
	}

	nexttoken();
}

#endif

#ifdef HASSTEFANSEXT
/*
	low level function access of the interpreter
	for each group of functions there is a call vector
	and and argument.

	Implemented call vectors
		1: Serial code

*/

void xusr() {
	address_t a;
	number_t n;
	address_t fn;
	int arg;
	char* instr=ibuffer;

	arg=pop();
	fn=pop();
	switch(fn) {
		case 0: // USR(0,y) delivers all the internal constants
			switch(arg) {
				case 0: push(numsize); break;
				case 1: push(maxnum); break;
				case 2: push(addrsize); break;
				case 3: push(maxaddr); break;
				case 4: push(strindexsize); break;
				case 5: push(memsize+1); break;
				case 6: push(elength()); break;
				case 7: push(GOSUBDEPTH); break;
				case 8: push(FORDEPTH); break;
				case 9: push(STACKSIZE); break;
				case 10: push(BUFSIZE); break;
				case 11: push(SBUFSIZE); break;
				case 12: push(serial_baudrate); break;
				case 13: push(serial1_baudrate); break;
				case 14: push(dsp_rows); break;
				case 15: push(dsp_columns); break;
				default: push(0);
			}
			break;	
		case 1: // access to the variables of the interpreter
			switch(arg) {
				case 0: push(top); break;
				case 1: push(here); break;
				case 2: push(himem); break;
				case 3: push(nvars); break;
				case 4: push(0); break;
				case 5: push(0); break;
				case 6: push(0); break;
				case 7: push(gosubsp); break;
				case 8: push(fnc); break;
				case 9: push(sp); break;
				default: push(0);
			}
			break;
		case 2: // io definitions, somewhat redundant to @ 
			switch(arg) {
				case 0: push(id); break;
				case 1: push(idd); break;
				case 2: push(od); break;
				case 3: push(odd); break;
				case 4: push(0); break;
				default: push(0);
			}
			break;
		case 6: // parse a number in the input buffer - less evil
			(void) parsenumber(ibuffer+1, &x);
			push(x);
			break;
		case 7: // write a number to the input buffer
			push(*ibuffer=writenumber(ibuffer+1, arg));
			break;
	}
}

#endif

void xcall() {
	int r;

	if (!expectexpr()) return;
	r=pop();
	switch(r) {
		case 0: 
#ifndef ARDUINO
			exit(0);
#endif
		default:
			error(UNKNOWN);
			return;
	}
}

// the dartmouth stuff
#ifdef HASDARTMOUTH

// data is simply skipped 
void xdata() {
	while (!termsymbol()) nexttoken();
}

// function to find the next data record
void nextdatarecord() {
	address_t h;
	signed char s=1;

	// save the location of the interpreter
	h=here;

	// data at zero means we need to init it, by searching
	// the first data record
	if (data == 0) {
		here=0;
		while (here<top && token!=TDATA) gettoken();
		data=here;
	} 

processdata:
	// data at top means we have exhausted all data 
	// nothing more to be done here, however we simulate
	// a number value of 0 here
	if (data == top) { 
		token=NUMBER;
		x=0;
		ert=1;
		here=h;
		return;
	}
	
	// we process the data record
	here=data;
	gettoken();
	if (token == '-') {s=-1; gettoken();}
	if (token == NUMBER || token == STRING) goto enddatarecord;
	if (token == ',') {
		gettoken();
		if (token == '-') {s=-1; gettoken();}
		if (token!=NUMBER && token!=STRING) {
			error(EUNKNOWN);  
			here=h;
			return;
		}
		goto enddatarecord;
	}

	if (termsymbol()) {
		while (here<top && token!=TDATA) gettoken();
		data=here;
		goto processdata;
	}

	if (DEBUG) { outsc("** error in nextdata after termsymbol "); outnumber(data); outcr(); }
	error(EUNKNOWN);

enddatarecord:
	if (token == NUMBER && s == -1) {x=-x; s=1; }
	data=here;
	here=h;

	if (DEBUG) { 
		outsc("** leaving nextdata with data and here "); 
		outnumber(data); outspc(); 
		outnumber(here); outcr(); 
	}

}


// this code resembles get - generic stream read code needed later
void xread(){
	signed char t, t0;  // remember the left hand side token until the end of the statement, type of the lhs
	char ps=TRUE;  	// also remember if the left hand side is a pure string of something with an index 
	char xcl, ycl; 	// to preserve the left hand side variable names
	address_t i=1;  // and the beginning of the destination string 
	signed char datat; // the type of the data element
	address_t lendest, lensource, newlength;
	int j;
	
	nexttoken();

	// this code evaluates the left hand side - remember type and name
	ycl=yc;
	xcl=xc;
	t=token;

	if (DEBUG) {outsc("assigning to variable "); outch(xcl); outch(ycl); outsc(" type "); outnumber(t); outcr();}

	// find the indices and draw the next token of read
	lefthandside(&i, &ps);
	if (er != 0) return;

	// if the token after lhs is not a termsymbol, something is wrong
	if (! termsymbol()) {error(EUNKNOWN); return; }
	t0=token;

	nextdatarecord();
	if (er!=0) return;

	// assigne the value to the lhs - redundant code to assignment
	switch (token) {
		case NUMBER:
			// store the number on the stack
			push(x);
			assignnumber(t, xcl, ycl, i, ps);
			break;
		case STRING:	
			// store the source string data
			ir2=ir;
			lensource=x;

			// the destination address of the lefthandside, on the fly create
			// included
			ir=getstring(xcl, ycl, i);
			if (er != 0) return;

			// the length of the lefthandsie string
			lendest=lenstring(xcl, ycl);

			if (DEBUG) {
				outsc("* read stringcode "); outch(xcl); outch(ycl); outcr();
				outsc("** read source string length "); outnumber(lensource); outcr();
				outsc("** read dest string length "); outnumber(lendest); outcr();
				outsc("** read dest string dimension "); outnumber(stringdim(xcl, ycl)); outcr();
			};

			// does the source string fit into the destination
			if ((i+lensource-1) > stringdim(xcl, ycl)) { error(ERANGE); return; }

			// this code is needed to make sure we can copy one string to the same string 
			// without overwriting stuff, we go either left to right or backwards
			if (x > i) 
				for (j=0; j<lensource; j++) { ir[j]=ir2[j];}
			else
				for (j=lensource-1; j>=0; j--) ir[j]=ir2[j]; 

			// classical Apple 1 behaviour is string truncation in substring logic
			newlength = i+lensource-1;	
		
			setstringlength(xcl, ycl, newlength);
			break;
		default:
			error(EUNKNOWN);
			return;
	}

	// no nexttoken here as we have already a termsymbol
	if (DEBUG) {
		outsc("** leaving xread with "); outnumber(token); outcr();
		outsc("** at here "); outnumber(here); outcr();
		outsc("** and data pointer "); outnumber(data); outcr();
	}
	token=t0;
}

// restore sets the data pointer to zero right now 
void xrestore(){
	data=0;
	nexttoken();
}


// def simply skips to the end of line right now
void xdef(){
	char xcl1, ycl1, xcl2, ycl2;
	address_t a;

	// do we define a function
	if (!expect(TFN, EUNKNOWN)) return;

	// the name of the function, it is tokenized as an array
	if (!expect(ARRAYVAR, EUNKNOWN)) return;
	xcl1=xc;
	ycl1=yc;

	// the argument 
	if (!expect('(', EUNKNOWN)) return;
	if (!expect(VARIABLE, EUNKNOWN)) return;
	xcl2=xc;
	ycl2=yc;
	if (!expect(')', EUNKNOWN)) return;

	// the assignment
	if (!expect('=', EUNKNOWN)) return;

	// ready to store the function
	if (DEBUG) {
		outsc("** DEF FN with function "); 
		outch(xcl1); outch(ycl1); 
		outsc(" and argument ");
		outch(xcl2); outch(ycl2);
		outsc(" at here "); 
		outnumber(here);
		outcr();
	}

	// find the function
	if ( (a=bfind(TFN, xcl1, ycl1)) == 0 ) a=bmalloc(TFN, xcl1, ycl1, 0);
	if (DEBUG) {outsc("** found function structure at "); outnumber(a); outcr(); }

	// store the payload - the here address - and the name of the variable
	z.a=here;
	setnumber(a, addrsize);
	mem[a+addrsize]=xcl2;
	mem[a+addrsize+1]=ycl2;

	// skip whatever comes next
	while (!termsymbol()) nexttoken();
}

// on is a bit like if 
void xon(){
	number_t cr;
	int ci;
	short args;
	signed char t;
	address_t tmp, line = 0;
	
	if(!expectexpr()) return;

	// the result of the condition, can be any number
	// even large
	cr=pop();
	if (DEBUG) { outsc("** in on condition found "); outnumber(cr); outcr(); } 

	// is there a goto or gosub
	if ( token != TGOSUB && token != TGOTO)  {
		error(UNKNOWN);
		return;
	}

	// remember if we do gosub or goto
	t=token;

	// how many arguments have we got here
	nexttoken();
	args=parsearguments();
	if (er != 0) return;
	if (args == 0) { error(EARGS); return; }
	
	// do we have more arguments then the condition?
	if (cr > args && cr <= 0) ci=0; else ci=(int)cr;

	// now find the line to jump to and clean the stack
	while (args) {
		tmp=pop();
		if (args == ci) line=tmp;
		args--;
	}
	
	if (DEBUG) { outsc("** in on found line as target "); outnumber(line); outcr(); }
	// no line found to jump to
	if (line == 0) {
		nexttoken();
		return;
	}

	// prepare for the jump	
	if (t == TGOSUB) pushgosubstack();
	if (er != 0) return;

	findline(line);
	if (er != 0) return;

	// goto in interactive mode switched to RUN mode
	// no clearing of variables and stacks
	if (st == SINT) st=SRUN;

	nexttoken();
}
#endif

// the darkarts
#ifdef HASDARKARTS
void xeval(){
	short i, l;
	address_t mline, line;


	// get the line number to store
	if (!expectexpr()) return;
	line=pop();

	if (token != ',') {
		error(EUNKNOWN);
		return;
	}

	// the line to be stored
	nexttoken();
	if (! stringvalue()) {
		error(EARGS); return; 
	}

	// here we have the string to evaluate in ir2 and copy it to the ibuffer
	// only one line allowed, BUFSIZE is the limit
	l=pop();
	if (l>BUFSIZE-1) {error(ERANGE); return; }
	for (i=0; i<l; i++) ibuffer[i+1]=ir2[i];
	ibuffer[l+1]=0;
	if (DEBUG) {outsc("** Preparing to store line "); outnumber(line); outspc(); outsc(ibuffer+1); outcr(); }

	// we find the line we are currently at
	if (st != SINT) {
		mline=myline(here);
		if (DEBUG) {outsc("** myline is "); outnumber(mline); outcr(); }
	}

	
	// go to interactive mode and try to store the line
	x=line;             // the linennumber
	push(st); st=SINT;  // go to (fake) interactive mode
	bi=ibuffer; 		// go to the beginning of the line
	storeline();  		// try to store it
	st=pop();			// go back to run mode

	// find my line - side effects not checked here
	if (st != SINT) {
		findline(mline);
		nextline();
	}
}
#endif

#ifdef HASIOT
void xiter(){
	nexttoken();
}
#endif


/* 

	statement processes an entire basic statement until the end 
	of the line. 

	The statement loop is a bit odd and requires some explanation.
	A statement function called in the central switch here must either
	call nexttoken as its last action to feed the loop with a new token 
	and then break or it must return which means that the rest of the 
	line is ignored. A function that doesn't call nexttoken and just 
	breaks causes an infinite loop.

*/

void statement(){
	if (DEBUG) debug("statement \n"); 
	while (token != EOL) {
		if (debuglevel == 1) debugtoken();
		if (debuglevel) outcr();
		switch(token){
			case LINENUMBER:
				nexttoken();
				break;
// Palo Alto BASIC language set + BREAK
			case TPRINT:
				xprint();
				break;
			case TLET:
				nexttoken();
				if ((token != ARRAYVAR) && (token != STRINGVAR) && (token != VARIABLE)){
					error(EUNKNOWN);
					break;
				}
			case STRINGVAR:
			case ARRAYVAR:
			case VARIABLE:		
				assignment();
				break;
			case TINPUT:
				xinput();
				break;
			case TRETURN:
				xreturn();
				break;
			case TGOSUB:
			case TGOTO:
				xgoto();	
				break;
			case TIF:
				xif();
				break;
			case TFOR:
				xfor();
				break;		
			case TNEXT:
				xnext();
				break;
			case TBREAK:
				xbreak();
				break;
			case TSTOP:
			case TEND: // return here because new input is needed
				*ibuffer=0; // clear ibuffer - this is a hack
				st=SINT;
				return;
			case TLIST:
				xlist();
				break;
			case TNEW: // return here because new input is needed
				xnew();
				return;
			case TCONT:
			case TRUN:
				xrun();
				return;	
			case TREM:
				xrem();
				break;
// Apple 1 language set 
#ifdef HASAPPLE1
			case TDIM:
				xdim();
				break;
			case TCLR:
				xclr();
				break;
			case TTAB:
				xtab();
				break;	
			case TPOKE:
				xpoke();
				break;
#endif
// Stefan's tinybasic additions
			case TDUMP:
				xdump();
				break;
			case TSAVE:
				xsave();
				break;
			case TLOAD: 
				xload();
				return; // load doesn't like break as the ibuffer is messed up;
			case TGET:
				xget();
				break;
			case TPUT:
				xput();
				break;			
			case TSET:
				xset();
				break;
			case TCLS:
				outch(12);
				nexttoken();
				break;
// Arduino IO
#ifdef HASARDUINOIO
			case TDWRITE:
				xdwrite();
				break;	
			case TAWRITE:
				xawrite();
				break;
			case TPINM:
				xpinm();
				break;
			case TDELAY:
				xdelay();
				break;
#ifdef HASTONE
			case TTONE:
				xtone();
				break;	
#endif
#endif
// SD card DOS function 
#ifdef HASFILEIO
			case TCATALOG:
				xcatalog();
				break;
			case TDELETE:
				xdelete();
				break;
			case TOPEN:
				xopen();
				break;
			case TCLOSE:
				xclose();
				break;
#endif
// low level functions 
			case TCALL:
				xcall();
				break;	
// graphics 
#ifdef HASGRAPH
			case TCOLOR:
				xcolor();
				break;
			case TPLOT:
				xplot();
				break;
			case TLINE:
				xline();
				break;
			case TRECT:
				xrect();
				break;
			case TCIRCLE:
				xcircle();
				break;
			case TFRECT:
				xfrect();
				break;
			case TFCIRCLE:
				xfcircle();
				break;
#endif
#ifdef HASDARTMOUTH
			case TDATA:
				xdata();
				break;
			case TREAD:
				xread();
				break;
			case TRESTORE:
				xrestore();
				break;
			case TDEF:
				xdef();
				break;
			case TON:
				xon();
				break;
#endif
#ifdef HASDARKARTS
			case TEVAL:
				xeval();
				break;
#endif
#ifdef HASIOT
			case TITER:
				xiter();
				break;	
#endif
// and all the rest
			case UNKNOWN:
				error(EUNKNOWN);
				return;
			case ':':
				nexttoken();
				break;
			default: // very tolerant - tokens are just skipped 
				if (DEBUG) { outsc("** hoppla - unexpected token, skipped "); debugtoken(); }
				nexttoken();
		}
#ifdef ARDUINO
		if (checkch() == BREAKCHAR) {st=SINT; xc=inch(); return;};  // on an Arduino entering "#" at runtime stops the program
		yield();
#endif
		if (er) return;
	}
}

// the setup routine - Arduino style
void setup() {
#if MEMSIZE == 0
	ballocmem();
	himem=memsize;
#endif
// needed for the millis routine
#ifndef ARDUINO
	ftime(&start_time);
#endif
	ioinit();
	printmessage(MGREET); outspc();
	printmessage(EOUTOFMEMORY); outspc(); 
	outnumber(memsize+1); outspc();
	outnumber(elength()); outcr();

 	xnew();	
#ifdef ARDUINOSD
 	SD.begin(sd_chipselect);
#endif	
#if defined(ESPSPIFFS) && defined(ESP8266)
 	SPI.begin();
 	if (SPIFFS.begin()) {
		outsc("SPIFFS opened successfully \n");
		FSInfo fs_info;
		SPIFFS.info(fs_info);
		outsc("File system size "); outnumber(fs_info.totalBytes); outcr();
		outsc("File system used "); outnumber(fs_info.usedBytes); outcr();
 	}
#endif
#ifdef ARDUINOEEPROM
  	if (eread(0) == 1){ // autorun from the EEPROM
  		egetnumber(1, addrsize);
  		top=z.a;
  		st=SERUN;
  	} 
#endif
}

// the loop routine for interactive input 
void loop() {

	// autorun code is run once and then tries to return to interactive
	// all autorun code must loop in itself
	if (st == SERUN) {
		xrun();
    	top=0;
    	st=SINT;
	}

	iodefaults();

	printmessage(MPROMPT);
    ins(ibuffer, BUFSIZE-2);
        
	bi=ibuffer;
	nexttoken();

    if (token == NUMBER) {
         storeline();		
    } else {
      	st=SINT;
      	statement();   
    }

    // here, at last, all errors need to be catched
    if (er) reseterror();
}


#ifndef ARDUINO
int main(){
	setup();
	while (TRUE)
		loop();
}
#endif
