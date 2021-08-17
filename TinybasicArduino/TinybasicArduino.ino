// $Id: basic.c,v 1.69 2021/08/15 06:22:24 stefan Exp stefan $
/*
	Stefan's tiny basic interpreter 

	Playing around with frugal programming. See the licence file on 
	https://github.com/slviajero/tinybasic for copyright/left.
    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)

	Author: Stefan Lenz, sl001@serverfabrik.de

*/

/* 
	Defines the target - for any Arduino or a Mac nothing 
	has to be set here. Define ESP8266 for these systems.
*/
#undef ESP8266

/* 
	SMALLness - Memory footprint of extensions
	
	          FLASH	    RAM
	EEPROM    834  		0
	FOR       804  		34
	LCD       712  		37
	GOSUB     144  		10
	DUMP 	  130  		0
	PICO     -504 	-179

	The extension flags control features and code size

*/ 

#define ARDUINOLCD
#define ARDUINOPS2
#undef ARDUINOTFT
#undef ARDUINOEEPROM
#define HASFORNEXT
#define HASGOSUB
#define HASDUMP
#undef USESPICOSERIAL

#ifdef ARDUINOPS2
#include <PS2Keyboard.h>
#endif

/* 
 	if PROGMEM is defined we can asssume we compile on 
 	the Arduino IDE. Dun't change anything here.

*/
#ifdef PROGMEM
#define ARDUINO
#define ARDUINOPROGMEM
#else
#undef ARDUINO
#endif

#ifdef ESP8266
#define PROGMEM
#undef ARDUINOPROGMEM
#endif

#ifdef ARDUINOEEPROM
#include <EEPROM.h>
unsigned short elength() { return EEPROM.length(); }
void eupdate(unsigned short i, short c) { EEPROM.update(i, c); }
short eread(unsigned short i) { return EEPROM.read(i); }
#else
unsigned short elength() { return 0; }
void eupdate(unsigned short i, short c) { return; }
short eread(unsigned short i) { return 0; }
#endif


#ifdef ARDUINO
#include <avr/pgmspace.h>
#ifdef ARDUINOLCD
#include <LiquidCrystal.h>
#endif
#ifdef USESPICOSERIAL
#include <PicoSerial.h>
#endif
#else 
#define PROGMEM
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#endif

const int serial_baudrate = 9600;

// global variables for the keyboard

#ifdef ARDUINOPS2
const int PS2DataPin = 3;
const int PS2IRQpin =  2;
PS2Keyboard keyboard;
#endif

// global variables for the LCD

#ifdef ARDUINOLCD
const int lcd_rows = 4;
const int lcd_columns = 20;
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 10; 
const int pin_d5 = 11; 
const int pin_d6 = 12; 
const int pin_d7 = 13; 
//const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);

char lcdbuffer[lcd_rows][lcd_columns];
char lcdmyrow = 0;
char lcdmycol = 0;
#endif

// global variables for a TFT

#ifdef ARDUINOTFT
#endif


// general definitions

#define TRUE  1
#define FALSE 0

// debug mode switches 
#define DEBUG  0

// various buffer sizes
#define BUFSIZE 	92
#define SBUFSIZE	16
#define MEMSIZE  	4096
#define VARSIZE		26
#define STACKSIZE 	15
#define GOSUBDEPTH 	4
#define FORDEPTH 	4

// definitions on EEROM handling in the Arduino
#define EHEADERSIZE 3
#define EDUMMYSIZE 0

// not yet used - added to implement different number types
#define NUMSIZE 2

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
#define TSTOP    -107
#define TLIST	-106
#define TNEW    -105
#define TRUN    -104
#define TABS 	-103
#define TRND	-102
#define TSIZE   -101
#define TREM 	-100
// this is the Apple 1 language set in addition to Palo Alto (15)
#define TNOT    -99
#define TAND	-98
#define TOR  	-97
#define TLEN    -96
#define TSGN	-95	
#define TPEEK	-94
#define TDIM	-93
#define TCLR	-92
#define TSCR    -91
#define TLOMEM  -90
#define THIMEM  -89 
#define TTAB 	-88
#define TTHEN   -87
#define TEND    -86
#define TPOKE	-85
// Stefan's tinybasic additions (9)
#define TCONT   -84
#define TSQR	-83
#define TFRE	-82
#define TDUMP 	-81
#define TBREAK  -80
#define TSAVE   -79
#define TLOAD   -78
#define TGET    -77
#define TSET    -76
// Arduino functions (6)
#define TPINM	-75
#define TDWRITE	-74
#define TDREAD	-73
#define TAWRITE	-72
#define TAREAD  -71
#define TDELAY  -70
// currently unused constants
#define TERROR  -3
#define UNKNOWN -2
#define NEWLINE -1


// the number of keywords, and the base index of the keywords
#define NKEYWORDS	3+19+15+9+6 
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

#define OSERIAL 0
#define OLCD 1
#define OTFT 2

#define ISERIAL 0
#define IKEYBOAD 1

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
const char snot[]    PROGMEM = "NOT";
const char sand[]    PROGMEM = "AND";
const char sor[]     PROGMEM = "OR";
const char slen[]    PROGMEM = "LEN";
const char ssgn[]    PROGMEM = "SGN";
const char speek[]   PROGMEM = "PEEK";
const char sdim[]    PROGMEM = "DIM";
const char sclr[]    PROGMEM = "CLR";
const char sscr[]    PROGMEM = "SCR";
const char slomem[]  PROGMEM = "LOMEM";
const char shimem[]  PROGMEM = "HIMEM";
const char stab[]    PROGMEM = "TAB";
const char sthen[]   PROGMEM = "THEN";
const char send[]    PROGMEM = "END";
const char spoke[]   PROGMEM = "POKE";
// Stefan's tinybasic additions
const char scont[]   PROGMEM = "CONT";
const char ssqr[]    PROGMEM = "SQR";
const char sfre[]    PROGMEM = "FRE";
const char sdump[]   PROGMEM = "DUMP";
const char sbreak[]  PROGMEM = "BREAK";
const char ssave[]   PROGMEM = "SAVE";
const char sload[]   PROGMEM = "LOAD";
const char sget[]    PROGMEM = "GET";
const char sset[]    PROGMEM = "SET";
// Arduino functions
const char spinm[]   PROGMEM = "PINM";
const char sdwrite[] PROGMEM = "DWRITE";
const char sdread[]  PROGMEM = "DREAD";
const char sawrite[] PROGMEM = "AWRITE";
const char saread[]  PROGMEM = "AREAD";
const char sdelay[]  PROGMEM = "DELAY";


const char* const keyword[] PROGMEM = {
// Palo Alto BASIC
	sge, sle, sne, sprint, slet, sinput, 
	sgoto, sgosub, sreturn, sif, sfor, sto,
	sstep, snext, sstop, slist, snew, srun,
	sabs, srnd, ssize, srem,
// Apple 1 BASIC additions
	snot, sand, sor, slen, ssgn, speek, sdim,
	sclr, sscr, slomem, shimem, stab, sthen, 
	send, spoke,
// Stefan's additions
	scont, ssqr, sfre, sdump, sbreak, ssave,
	sload, sget, sset, 
// Arduino stuff
    spinm, sdwrite, sdread, sawrite, saread, 
    sdelay, 
// the end 
};

/*
	the message catalogue also moved to progmem
*/

// the messages and errors
#define MFILE        0
#define MPROMPT      1
#define MGREET 		 2
#define EGENERAL 	 3
#define EUNKNOWN	 4
#define ENUMBER      5 
#define EDIVIDE		 6
#define ELINE        7
#define ERETURN      8
#define ENEXT        9
#define EGOSUB       10 
#define EFOR         11
#define EOUTOFMEMORY 12
#define ESTACK 		 13
#define EDIM         14
#define ERANGE 		 15
#define ESTRING      16
#define EVARIABLE	 17
#define EFILE 		 18
#define EFUN 		 19
#define EARGS		 20
#define EEEPROM		 21

const char mfile[]    	PROGMEM = "file.bas";
const char mprompt[]	PROGMEM = "] ";
const char mgreet[]		PROGMEM = "Tinybasic 1.1";
const char egeneral[]  	PROGMEM = "Error";
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

const char* const message[] PROGMEM = {
	mfile, mprompt, mgreet,egeneral, eunknown,
	enumber, edivide, eline, ereturn, enext,
	egosub, efor, emem, estack, edim, erange,
	estring, evariable, efile, efun, eargs, 
	eeeprom
};


/*
	The basic interpreter is implemented as a stack machine
	with global variable for the interpreter state, the memory
	and the arithmetic during run time.

	stack is the stack memory and sp controls the stack.

	ibuffer is an input buffer and *bi a pointer to it.

	sbuffer is a short buffer for arduino progmem access. 

	vars is a static array of 26 single character variables.

	mem is the working memory of the basic interperter.

	x, y, xc, yc are two 16 bit and two 8 bit accumulators.

	z is a mixed 16/8 bit accumulator

	ir, ir2 are general index registers for string processing.

	token contains the actually processes token.

	st, here and top are the interpreter runtime controls.

	nvars is the number of vars the interpreter has stored.

	form is used for number formation Palo Alto BASIC style.

	rd is the random number storage.

	id and od are the input and output model for an arduino
		they are set to serial by default

	fd is the filedescriptor for save/load

*/
static short stack[STACKSIZE];
static unsigned short sp=0; 

static char sbuffer[SBUFSIZE];

static char ibuffer[BUFSIZE] = "\0";
static char *bi;

static short vars[VARSIZE];

static signed char mem[MEMSIZE];
static unsigned short himem = MEMSIZE-1;

#ifdef HASFORNEXT
static struct {char varx; char vary; short here; short to; short step;} forstack[FORDEPTH];
static short forsp = 0;
#endif

#ifdef HASGOSUB
static short gosubstack[GOSUBDEPTH];
static short gosubsp = 0;
#endif

static short x, y;
static signed char xc, yc;

struct twobytes {signed char l; signed char h;};
static union accu168 { short i; struct twobytes b; } z;

static char *ir, *ir2;
static signed char token;
static signed char er;

static signed char st; 
static unsigned short here; 
static unsigned short top;

static unsigned short nvars = 0; 

static char form = 0;

static unsigned int rd;

static char id = ISERIAL;
static char od = OSERIAL;

static char fnc; 

#ifndef ARDUINO
FILE* fd;

#endif

/* 
	Layer 0 functions 

	variable handling - interface between memory 
 	and variable storage

*/
void  clrvars();

void  createvar(char, char);
short getvar(char, char);
void  setvar(char, char, short);

void  createarry(char, char, short);
short getarray(char, char, short);
void  setarray(char, char, short, short);

void  createstring(char, char, short);
char* getstring(char, char, short);
void  setstring(char, char, short, char *, short);

short getshort(short);
void  setshort(short, short);

void  parsesubstring();
short parsesubscripts();
void  parsenarguments(char);
short parsearguments();

// error handling
void error(signed char);
void reseterror();

// stack stuff
void push(short);
short pop();
void drop();
void clearst();

// get keyword from PROGMEM
char* getkeyword(signed char);

// mathematics and other functions
short rnd(short);
short sqr(short);
short fre(short);
short peek(short);
short xabs(short);
short xsgn(short);

// input output
// these are the platfrom depended lowlevel functions
void ioinit();
void picogetchar(int);
void outch(char);
char inch();
char checkch();
void ins(char*, short); 
// from here on the functions only use the functions above
// there should be no platform depended code here
void outcr();
void outspc();
void outs(char*, short);
void outsc(char*);
void outnumber(short);
char innumber(short*);
short getnumber(char*, short*);

/* 	
	Layer 1 function, provide data and do the heavy lifting 
	for layer 2.
*/

// lexical analysis
void debugtoken();
void whitespaces();
void nexttoken();

// storeing and retrieving programs
char nomemory(short);
void dumpmem(unsigned short, unsigned short);
void storetoken(); 
char memread(unsigned short);
void gettoken();
void firstline();
void nextline();
void findline(unsigned short);
unsigned short myline(unsigned short);
void moveblock(unsigned short, unsigned short, unsigned short);
void zeroblock(unsigned short, unsigned short);
void diag();
void storeline();

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
void xgoto();
void xif();
void xinput();
void xrem();
void xpoke();
void xget();
void xset();

// optional FOR NEXT loops
#ifdef HASFORNEXT
void xfor();
void xnext();
void xbreak();
void pushforstack();
void popforstack();
void dropforstack();
#endif
// optional GOSUB commands
#ifdef HASGOSUB
void xreturn();
void pushgosubstack();
void popgosubstack();
void dropgosubstack();
#endif

// Arduino IO control
void xdwrite();
void xawrite();
void xpinm();
void xdelay();
short dread(short);
short aread(short);

// control commands 
void xclr();
void xlist();
void xrun();
void xxnew();
void xsave();
void xload();
void outputtoken();

// the statement loop
void statement();

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


unsigned short bmalloc(signed char t, char c, char d, short l) {

	unsigned short vsize;     // the length of the header
	unsigned short b;


    if (DEBUG) { outsc("** bmalloc with token "); outnumber(t); outcr(); }

	// how much space is needed
	if ( t == VARIABLE ) vsize=2+3; 	
	else if ( t == ARRAYVAR ) vsize=2*l+2+3;
	else if ( t == STRINGVAR ) vsize=l+2+3;
	else { error(EUNKNOWN); return 0; }
	if ( (himem - top) < vsize) { error(EOUTOFMEMORY); return 0;}

	// here we would create the hash, currently simplified
	// the hash is the first digit of the variable plus the token

	// write the header - inefficient - 3 bytes for a hash
	b=himem;
	mem[b--]=c;
	mem[b--]=d;
	mem[b--]=t;

	// for strings and arrays write the length
	if (t == ARRAYVAR || t == STRINGVAR) {
		z.i=vsize-5;
		mem[b--]=z.b.h;
		mem[b--]=z.b.l;
	}

	// reserve space for the payload
	himem-=vsize;
	nvars++;

	return himem+1;
}


// bfind passes back the location of the object as result
// the length of the object is in z.i as a side effect 

unsigned short bfind(signed char t, char c, char d) {

	short unsigned b = MEMSIZE-1;
	signed char t1;
	char c1, d1;
	short i=0;

nextitem:
	if (i >= nvars) return 0;

	c1=mem[b--];
	d1=mem[b--];
	t1=mem[b--];

	if (t1 == STRINGVAR || t1 == ARRAYVAR) {
		z.b.h=mem[b--];
		z.b.l=mem[b--];
	} else 
		z.i=2; 

	b-=z.i;

	if (c1 == c && d1 == d && t1 == t) {
		return b+1;
	}

	i++;
	goto nextitem;
}

unsigned short blength (signed char t, char c, char d) {
	if (! bfind(t, c, d)) return 0;
	return z.i;
}


// ununsed so far 
void createvar(char c, char d){
	return;
}

short getvar(char c, char d){
	unsigned short a;

	if (DEBUG) { outsc("* getvar "); outch(c); outch(d); outspc(); outcr(); }

	if (c >= 65 && c<=91 && d == 0)
			return vars[c-65];

	a=bfind(VARIABLE, c, d);
	if ( a == 0) {
		a=bmalloc(VARIABLE, c, d, 0);
		if (er != 0) return 0;
	} 

	return getshort(a);

}

void setvar(char c, char d, short v){
	unsigned short a;

	if (DEBUG) { outsc("* setvar "); outch(c); outch(d); outspc(); outnumber(v); outcr(); }
	if (c >= 65 && c<=91 && d == 0) {
		vars[c-65]=v;
		return;
	}

	a=bfind(VARIABLE, c, d);
	if ( a == 0) {
		a=bmalloc(VARIABLE, c, d, 0);
		if (er != 0) return;
	} 

	setshort(a, v);
}


void clrvars() {
	for (char i=0; i<VARSIZE; i++) vars[i]=0;
	nvars=0;
	himem=MEMSIZE-1;
}

short getshort(short m){
	z.b.l=memread(m++);
	z.b.h=memread(m);
	return z.i;
}

void setshort(short m, short v){
	z.i=v;
	mem[m++]=z.b.l;
	mem[m]=z.b.h;
}


void createarry(char c, char d, short i) {

	if (bfind(ARRAYVAR, c, d)) { error(EVARIABLE); return; }
	(void) bmalloc(ARRAYVAR, c, d, i);
	if (er != 0) return;
	if (DEBUG) { outsc("* created array "); outch(c); outspc(); outnumber(nvars); outcr(); }

}


short getarray(char c, char d, short i){

	unsigned short a;

	if (DEBUG) { outsc("* get array "); outch(c); outspc(); outnumber(i); outcr(); }

	// Dr. Wang's famous rest of memory array
	if (c == '@') {

		if ( i > 0 && i < (himem-top)/2 )
			return getshort(himem-2*i);
		else {
			error(ERANGE);
			return 0;
		}
	}

	// the EEPROM array in the same style;
#ifdef ARDUINOEEPROM
	if (c == '&') {
		if ( i > 0 && i <= elength()/2  ) {
			z.b.h=eread(elength() - i*2);
			z.b.l=eread(elength() - i*2 + 1);
			return z.i;		
		} else {
			error(ERANGE);
			return 0;	
		}
	}
#endif

	// dynamically allocated arrays
	a=bfind(ARRAYVAR, c, d);
	if (a == 0) {
		error(EVARIABLE);
		return 0;
	}

	if ( (i < 1) || (i > z.i/2) ) {
		error(ERANGE); return 0;
	}

	a=a+(i-1)*2;
	return getshort(a);
}

void setarray(char c, char d, short i, short v){

	unsigned short a;

		if (DEBUG) { outsc("* set array "); outch(c); outspc(); outnumber(i); outspc(); outnumber(v); outcr(); }

	// currently only Dr. Wang's @ array implemented 
	if (c == '@') {
		if ( i > 0 && i < (himem-top)/2 ) {
			setshort(himem-2*i, v);
			return;		
		} else {
			error(ERANGE);
			return;
		}
	}

	// Dr. Wang's EEPROM analogon
#ifdef ARDUINOEEPROM
	if (c == '&') {
		if ( i > 0 && i <= elength()/2 ) {
			z.i=v;
			eupdate(elength() - i*2, z.b.h);
			eupdate(elength() - i*2 + 1, z.b.l);
			return;
		} else {
			error(ERANGE);
			return;			
		}
	}
#endif

	// dynamically allocated arrays
	a=bfind(ARRAYVAR, c, d);
	if (a == 0) {
		error(EVARIABLE);
		return;
	}

	if ( (i < 1) || (i > z.i/2) ) {
		error(ERANGE); return;
	}

	a=a+(i-1)*2;
	setshort(a, v);
}

void createstring(char c, char d, short i) {

	if (bfind(STRINGVAR, c, d)) { error(EVARIABLE); return; }
	(void) bmalloc(STRINGVAR, c, d, i+1);
	if (er != 0) return;
	if (DEBUG) { outsc("Created string "); outch(c); outch(d); outspc(); outnumber(nvars); outcr(); }
}


char* getstring(char c, char d, short b) {	
	unsigned short a;

	if (DEBUG) { outsc("* get string var "); outch(c); outch(d); outspc(); outnumber(b); outcr(); }

	// direct access to the input buffer
	if ( c == '@')
			return ibuffer+b;

	// dynamically allocated strings
	a=bfind(STRINGVAR, c, d);
	if (er != 0) return 0;
	if (a == 0) {
		error(EVARIABLE);
		return 0;
	}

	if ( (b < 1) || (b > z.i) ) {
		error(ERANGE); return 0;
	}

	// outsc("a ="); outnumber(a); outcr();
	// outsc("b ="); outnumber(b); outcr();
	// x=mem[a]-b+1;
	// outsc("x ="); outnumber(x); outcr();
	a=a+b;
	return (char *)&mem[a];
}


// test code based on static strings 
short arraydim(char c) {
	if (c == '@')
		return (himem-top)/2;
	return blength(ARRAYVAR, c, '$')/2;
}

short stringdim(char c, char d) {
	if (c == '@')
		return BUFSIZE-1;
	return blength(STRINGVAR, c, d)-1;
}

short lenstring(char c, char d){
	char* b;
	if (c == '@')
		return ibuffer[0];
	
	b=getstring(c, d, 1);
	if (er != 0) return 0;
	return b[-1];
}

void setstringlength(char c, char d, short l) {

	unsigned short a; 

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

	if (l < z.i-1)
		mem[a]=l;
	else
		error(ERANGE);

}

void setstring(char c, char d, short w, char* s, short n) {
	char *b;
	unsigned short a;

	if (DEBUG) { outsc("* set string "); outch(c); outch(d); outspc(); outnumber(w); outcr(); }


	if ( c == '@') {
		b=ibuffer;
	} else {
		a=bfind(STRINGVAR, c, d);
		if (er != 0) return;
		if (a == 0) {
			error(EVARIABLE);
			return;
		}

		b=(char *)&mem[a+1];

	}

	if ( (w+n-1) <= stringdim(c, d) ) {
		for (int i=0; i<n; i++) { b[i+w]=s[i]; } 
		b[0]=w+n-1; 	
	}
	else 
		error(ERANGE);
}

/* 
	Layer 0 - keyword handling - PROGMEM logic goes here
		getkeyword is the only access to the keyword array
		in the code.  

		Same for messages and errors
*/ 

char* getkeyword(signed char t) {
	if (t < BASEKEYWORD || t > BASEKEYWORD+NKEYWORDS) {
		error(EUNKNOWN);
		return 0;
	} else 
#ifndef ARDUINOPROGMEM
	return (char *) keyword[t-BASEKEYWORD];
#else
	strcpy_P(sbuffer, (char*) pgm_read_word(&(keyword[t-BASEKEYWORD]))); 
	return sbuffer;
#endif
}

char * getmessage(char i) {

	if (i >= NKEYWORDS) return NULL;

#ifndef ARDUINOPROGMEM
	return (char *) message[i];
#else
	strcpy_P(sbuffer, (char*) pgm_read_word(&(message[i]))); 
	return sbuffer;
#endif
}

void printmessage(char i){
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
	if (st != SINT) {
		outnumber(myline(here));
		outch(':');
		outspc();
	}
	printmessage(e);
	outspc();
	printmessage(EGENERAL);
	outcr();
	clearst();
}

void reseterror() {
	er=0;
	clearst();
	fnc=0;
	st=SINT;
}

#ifdef DEBUG
void debugtoken(){
	outsc("* token: ");
	switch(token) {
		case LINENUMBER: 
			outsc("LINE ");
			outputtoken();
			outcr();
			break;
		case NUMBER:
			outsc("NUMBER ");
			outputtoken();
			outcr();
			break;
		case VARIABLE:
			outsc("VARIABLE ");
			outputtoken();
			outcr();
			break;	
		case ARRAYVAR:
			outsc("ARRAYVAR ");
			outputtoken();
			outcr();
			break;		
		case STRING:
			outsc("STRING ");
			outputtoken();
			outcr();
			break;
		case STRINGVAR:
			outsc("STRINGVAR ");
			outputtoken();
			outcr();
			break;
		case EOL: 
			outsc("EOL");
			outcr();
			break;	
		default:
			outputtoken();
			outcr();	
	}
}

void debug(char *c){
	outch('*');
	outspc();
	outsc(c); 
	debugtoken();
}


#endif

/*
	Arithmetic and runtime operations are mostly done
	on a stack of 16 bit objects.
*/

void push(short t){
	if (DEBUG) {outsc("** push sp= "); outnumber(sp); outcr(); }
	if (sp == STACKSIZE)
		error(ESTACK);
	else
		stack[sp++]=t;
}

short pop(){
	if (DEBUG) {outsc("** pop sp= "); outnumber(sp); outcr(); }
	if (sp == 0) {
		error(ESTACK);
		return 0;
	}
	else
		return stack[--sp];	
}

void drop() {
	if (DEBUG) {outsc("** drop sp= "); outnumber(sp); outcr(); }
	if (sp == 0)
		error(ESTACK);
	else
		--sp;	
}

void clearst(){
	sp=0;
}

/* 

	Stack handling for gosub and for

*/

#ifdef HASFORNEXT
void pushforstack(){
	if (forsp < FORDEPTH) {
		forstack[forsp].varx=xc;
		forstack[forsp].vary=yc;
		forstack[forsp].here=here;
		forstack[forsp].to=x;
		forstack[forsp].step=y;
		forsp++;	
		return;	
	} else 
		error(ESTACK);
}


void popforstack(){
	if (forsp>0) {
		forsp--;
	} else {
		error(ESTACK);
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
		error(ESTACK);
		return;
	} 
}
#endif


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

// device driver code -rudimentary
#ifdef ARDUINOLCD

void lcdscroll(){

  short r,c;
  short i;

  for (r=1; r<lcd_rows; r++)
    for (c=0; c<lcd_columns; c++)
      lcdbuffer[r-1][c]=lcdbuffer[r][c];
   for (c=0; c<lcd_columns; c++) lcdbuffer[lcd_rows-1][c]=0;
   lcd.clear();
   lcd.home();
   for (r=0; r<lcd_rows-1; r++) {
    lcd.setCursor(0, r);
    for (c=0; c<lcd_columns; c++) {
      if (lcdbuffer[r][c] >= 32) {
        lcd.write(lcdbuffer[r][c]);
      }
    }
   }
   lcdmyrow=lcd_rows-1;
   return;
}

void lcdclear() {
  short r,c;
  for (r=0; r<lcd_rows; r++)
    for (c=0; c<lcd_columns; c++)
      lcdbuffer[r][c]=0;
  lcd.clear();
  lcd.home();
  lcdmyrow=0;
  lcdmycol=0;
  return;
}




void lcdwrite(char c) {
	short ic;
  	ic= (short) c;
  
  	if (c == 10) {
    	lcdmyrow=(lcdmyrow + 1);
    	if (lcdmyrow >= lcd_rows) {
      		lcdscroll(); 
    	}
    	lcdmycol=0;
    	lcd.setCursor(lcdmycol, lcdmyrow);
    	return;
  	}
  	if (c == 127) {
    	if (lcdmycol > 0) {
      		lcdmycol--;
      		lcdbuffer[lcdmyrow][lcdmycol]=0;
      		lcd.setCursor(lcdmycol, lcdmyrow);
      		lcd.write(" ");
      		lcd.setCursor(lcdmycol, lcdmyrow);
      		return;
    	}
  	}
	if (c < 32) return; 

	lcd.write(c);
	lcdbuffer[lcdmyrow][lcdmycol++]=c;
	if (lcdmycol == lcd_columns) {
		lcdmycol=0;
		lcdmyrow=(lcdmyrow + 1);
    	if (lcdmyrow >= lcd_rows) {
      		lcdscroll(); 
    	}
		lcd.setCursor(lcdmycol, lcdmyrow);
	}
}

void lcdbegin() {
	lcd.begin(lcd_columns, lcd_rows);
}
#else 
void lcdwrite(char c) {}
void lcdbegin() {}
#endif

#ifndef ARDUINO
/* 
	this is C standard library stuff, we branch to file input/output
	if there is a valid file descriptor in fd.
*/
void ioinit() {
	return;
}


void outch(char c) { 
	if (!fd)
		putchar(c);
	else 
		fputc(c, fd);
}

char inch(){
	char c;
	if (!fd) {
   		return getchar();; 
	} else 
		return fgetc(fd);
}

char checkch(){
	return 0;
}


void ins(char *b, short nb) {
	char c;
	short i = 1;
	while(i < nb-1) {
		c=inch();
		if (c == '\n' || c == '\r') {
			b[i]=0x00;
			b[0]=i-1;
			break;
		} else {
			b[i++]=c;
		} 

	}
}

#else 
#ifndef USESPICOSERIAL
/*

	This is standard Arduino code Serial code. 
	In inch() we wait for input by looping. 
	LCD output is controlled by the flag od.
	Keyboard code here is totally beta

*/ 

void ioinit() {
	Serial.begin(serial_baudrate);
   	lcdbegin();  // the dimension of the lcd shield - hardcoded, ugly
#ifdef ARDUINOPS2
	keyboard.begin(PS2DataPin, PS2IRQpin, PS2Keymap_German);
#endif
}


void outch(char c) {
	if (od == OLCD) {
		lcdwrite(c);
	} else 
		Serial.write(c);
}


char inch(){
	char c=0;
	if (id == ISERIAL) {
		do 
			if (Serial.available()) c=Serial.read();
		while(c == 0); 
		return c;
	}
#ifdef ARDUINOPS2	
	if (id == IKEYBOAD) {
		do 
			if (keyboard.available()) c=keyboard.read();
		while(c == 0);	
    if (c == 13) c=10;
		return c;
	}
#endif
}

char checkch(){
	if (Serial.available() && id == ISERIAL) return Serial.peek(); 
#ifdef ARDUINOPS2
	if (keyboard.available() && id == IKEYBOAD) return keyboard.read();
#endif
}

void ins(char *b, short nb) {
  	char c;
  	short i = 1;
  	while(i < nb-1) {
    	c=inch();
    	outch(c);
    	if (c == '\n' || c == '\r') {
      		b[i]=0x00;
      		b[0]=i-1;
      		break;
    	} else if (c == 127 && i>1) {
      		i--;
    	} else {
      		b[i++]=c;
    	} 
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

void ioinit() {
	(void) PicoSerial.begin(serial_baudrate, picogetchar);
   	lcdbegin();  // the dimension of the lcd shield - hardcoded, ugly
}

void picogetchar(int c){
	picochar=c;
	if (picob && (! picoa) ) {
		if (picochar != '\n' && picochar != '\r' && picoi<picobsize-1) {
			picob[picoi++]=picochar;
		} else {
			picoa = TRUE;
			picob[picoi]=0;
			picob[0]=picoi;
			picoi=1;
		}
		picochar=0; // every buffered byte is deleted
	}
}

void outch(char c) {
	if (od == OLCD) {
		lcdwrite(c);
	} else 
		PicoSerial.print(c);
}

char inch(){
	return picochar;
}

char checkch(){
    return picochar;
}

void ins(char *b, short nb) {
	picob=b;
	picobsize=nb;
	picoa=FALSE;
	while (! picoa);
	outsc(b+1); outcr();
}
#endif
#endif


/*


// PicoSerial Code, read and buffer one entire string that become deaf
// but still track 
#ifdef USESPICOSERIAL
volatile static char picochar;
volatile static char picoa = FALSE;
volatile static char* picob = NULL;
static short picobsize = 0;
volatile static short picoi = 1;

void picogetchar(int c){
	picochar=c;
	if (picob && (! picoa) ) {
		if (picochar != '\n' && picochar != '\r' && picoi<picobsize-1) {
			picob[picoi++]=picochar;
		} else {
			picoa = TRUE;
			picob[picoi]=0;
			picob[0]=picoi;
			picoi=1;
		}
		picochar=0; // every buffered byte is deleted
	}
}
#endif

void outch(char c) {
#ifdef ARDUINO
#ifdef ARDUINOLCD
	if (od == OLCD) {
		if (c > 31) lcd.write(c);
	} else 
#endif
#ifdef USESPICOSERIAL
		PicoSerial.print(c);
#else
		Serial.write(c);
#endif
#else 
	if (!fd)
		putchar(c);
	else 
		fputc(c, fd);
#endif
}

// blocking input - the loop around getchar is 
// unneccessary in the blocking I/O model
char inch(){
	char c=0;
#ifdef ARDUINO
#ifndef ARDUINOPS2
#ifdef USESPICOSERIAL
	return picochar;
#else
	do 
		if (Serial.available()) c=Serial.read();
	while(c == 0); 
	outch(c);
	return c;
#endif
#else 
	do 
		if (keyboard.available()) c=keyboard.read();
	while(c == 0); 
	outch(c);
	if (c == 13) outch('\n');
	return c;
#endif	
#else
	if (!fd) {
		do 
   			c=getchar();
  		while(c == 0); 
		return c;
	} else 
		return fgetc(fd);
#endif
}

// this is the non blocking io function needed to 
// interrupt a running program and for GET, 
// typically an ARDUINO code can add other keyboard 
// models here
char checkch(){
#ifdef ARDUINO
#ifndef ARDUINOPS2
#ifdef USESPICOSERIAL
    return picochar;
#else 
	if (Serial.available()) return Serial.peek(); 
#endif	
#else 
	if (keyboard.available()) return keyboard.read();
#endif
#else 
// code for non blocking I/O on non Arduino platforms
#endif
	return 0;
}


// reads a line from the keyboard to the input buffer
// the new ins - reads into a buffer the caller supplies
// nb is the max size of the buffer

void ins(char *b, short nb) {
#ifndef USESPICOSERIAL
	char c;
	short i = 1;
	while(i < nb-1) {
		c=inch();
		if (c == '\n' || c == '\r') {
			b[i]=0x00;
			b[0]=i-1;
			break;
		} else {
			b[i++]=c;
		} 
		if (c == 0x08 && i>1) {
			i--;
		}
	}
}
#else 
	picob=b;
	picobsize=nb;
	picoa=FALSE;
	while (! picoa);
	outsc(b+1); outcr();
}
#endif

*/


void outcr() {
	outch('\n');
} 

void outspc() {
	outch(' ');
}

// output a string of length x at index ir - basic style
void outs(char *ir, short l){
	for(int i=0; i<l; i++) outch(ir[i]);
}

// output a zero terminated string at ir - c style
void outsc(char *c){
	while (*c != 0) outch(*c++);
}


// reading a 16 bit number using the stack to collect bytes

short getnumber(char *c, short *r) {
	int nd = 0;
	*r=0;
	while (*c >= '0' && *c <= '9' && *c != 0) {
		*r=*r*10+*c++-'0';
		nd++;
		if (nd == 5) break;
	}
	return nd;
}

char innumber(short *r) {
	int i = 1;
	int s = 1;

again:
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
			(void) getnumber(&sbuffer[i], r);
			*r*=s;
			return 0;
		} else {
			printmessage(ENUMBER); 
			outspc(); 
			printmessage(EGENERAL);
			outcr();
			*r=0;
			s=1;
			i=1;
			goto again;
		}
	}
	return 0;
}

// prints a 16 bit number
void outnumber(short x){
	char c, co;
	short d;
	char i=1;
	if (x<0) {
		outch('-');
		i++;
		x=-x;
	}
	d=10000;
	c=FALSE; // surpress leading 0s
	while (d > 0){
		co=x/d;
		x=x%d;
		if (co != 0 || d == 1 || c) {
			co=co+48;
			outch(co);
			i++;
			c=TRUE;
		}
		d=d/10;
	}

	// number formats in Palo Alto style
	while (i < form) {outspc(); i++; };

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
	if (*bi <='9' && *bi>= '0'){
		bi+=getnumber(bi, &x);
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

	// Dr. Wangs BASIC had this array we keep it for sentimental reasons
	if (*bi == '@') {
		token=ARRAYVAR;
		xc=*bi++;
		if (*bi == '$') {
			token=STRINGVAR;
			bi++;
		}
		if (DEBUG) debugtoken();
		return;
	}

#ifdef ARDUINOEEPROM 
	if (*bi == '&') {
		token=ARRAYVAR;
		xc=*bi++;
		if (DEBUG) debugtoken();
		return;
	}
#endif

	// relations
	// single character relations are their own token
	// >=, =<, =<, =>, <> ore tokenized
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

	// keyworks and variables
	// isolate a word, bi points to the beginning, x is the length of the word
	// ir points to the end of the word after isolating

	// if (DEBUG) printmessage(EVARIABLE);
	x=0;
	ir=bi;
	while (-1) {
		// toupper 
		if (*ir >= 'a' && *ir <= 'z') {
			*ir-=32;
			ir++;
			x++;
		} else if (*ir >= 'A' && *ir <= 'Z'){
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

	keywords are an arry of null terminated strings.

*/

	token=BASEKEYWORD;
	while (token < NKEYWORDS+BASEKEYWORD){
		ir=getkeyword(token);
		xc=0;
		while (*(ir+xc) != 0) {
			if (*(ir+xc) != *(bi+xc)){
				token++;
				xc=0;
				break;
			} else 
				xc++;
		}
		if (xc == 0)
			continue;
		if ( *(bi+xc) < 'A' || *(bi+xc) > 'Z' ) {
			bi+=xc;
			if (DEBUG) debugtoken();
			return;
		} else {
			bi+=xc;
			token=UNKNOWN;
			return;
		}
	}

/*
	a variable has length 1 in the first version
	 its literal value is returned in xc
	in addition to this, a number or $ is accepted as 
	second character like in Apple 1 BASIC

*/
	if ( x == 1 ){
		token=VARIABLE;
		xc=*bi;
		yc=0;
		bi++;
		whitespaces();
		if (*bi >= '0' && *bi <= '9') { 
			yc=*bi;
			bi++;
		} 
		whitespaces();
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


// single letters are parsed and stored - not really good

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
	we pack into 2 bytes, this is clumsy but portable
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

char nomemory(short b){
	if (top >= himem-b) return TRUE; else return FALSE;
}

void storetoken() {
	short i=x;
	switch (token) {
		case NUMBER:
		case LINENUMBER:
			if ( nomemory(3) ) goto memoryerror;
			mem[top++]=token;	
			z.i=x;
			mem[top++]=z.b.l;
			mem[top++]=z.b.h;
			return;
		case ARRAYVAR:
		case VARIABLE:
		case STRINGVAR:
			if ( nomemory(3) ) goto memoryerror;
			mem[top++]=token;
			mem[top++]=xc;
			mem[top++]=yc;
			return;
		case STRING:
			if ( nomemory(x+2) ) goto memoryerror;
			mem[top++]=token;
			mem[top++]=i;
			while (i > 0) {
				mem[top++] = *ir++;
				i--;
			}
			return;
		default:
			if ( nomemory(1) ) goto memoryerror;
			mem[top++]=token;
			return;
	}
memoryerror:
	er=EOUTOFMEMORY;
} 


char memread(unsigned short i){
#ifndef ARDUINOEEPROM
	return mem[i];
#else 
	if (st != SERUN) {
		return mem[i];
	} else {
		return eread(i+EHEADERSIZE);
	}
#endif
}


void gettoken() {

	// if we have reached the end of the program, EOL is always returned
	// we don't rely on mem having a trailing EOL
	if (here >= top) {
		token=EOL;
		return;
	}

	token=memread(here++);
	switch (token) {
		case NUMBER:
		case LINENUMBER:		
			z.b.l=memread(here++);
			z.b.h=memread(here++);
			x=z.i;
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
			if (st == SERUN) { // we run from the EEROM and cannot simply pass a point
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

void firstline() {
	if (top == 0) {
		x=0;
		return;
	}
	here=0;
	gettoken();
}

void nextline() {
	while (here < top) {
		gettoken();
		if (token == LINENUMBER)
			return;
		if (here >= top) {
			here=top;
			x=0;
			return;
		}
	}
}

// find a line
void findline(unsigned short l) {
	here=0;
	while (here < top) {
		gettoken();
		if (token == LINENUMBER && x == l ) return;
	}
	error(ELINE);
}

// finds the line of a location
unsigned short myline(unsigned short h) {
	unsigned short l=0; 
	unsigned short l1=0;
	unsigned short here2;
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

void moveblock(unsigned short b, unsigned short l, unsigned short d){
	short i;

	if (d+l > himem) {
		error(EOUTOFMEMORY);
		return;
	}
	
	if (l<1) 
		return;

	if (b < d)
		for (i=l-1; i>=0; i--)
			mem[d+i]=mem[b+i]; 
	else 
		for (i=0; i<l; i++) 
			mem[d+i]=mem[b+i]; 
}

void zeroblock(unsigned short b, unsigned short l){
	short i;

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
	stage 3: caculate lengthes and free memory and make room at the 
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
	short linelength;
	short newline; 
	unsigned short here2, here3; 

	// zero is an illegal line number
	if (x == 0) {
		error(ELINE);
		return;
	}

/*
	stage 1: append the line at the end of the memory,
	remember the line number on the stack and the old top in here

*/
    push(x);			
    here=top;		
    newline=here;	 
	token=LINENUMBER;
	do {
		storetoken();
		if (er == EOUTOFMEMORY) {
			printmessage(EOUTOFMEMORY); outcr();
			drop();
			top=newline;
			here=0;
			//zeroblock(top, himem-top);
			return;
		}
		nexttoken();
	} while (token != EOL);

	x=pop();				// recall the line number
	linelength=top-here;	// calculate the number of stored bytes

/* 
	stage 2: check if only a linenumber stored - then delete this line
	
*/

	if (linelength == 3) {  		
		//zeroblock(here, linelength);
		top-=3;
		y=x;					
		findline(y);
		if (er == ELINE) {
			printmessage(ELINE); outcr();
			return;
		}		
		y=here-3;							
		nextline();			
		here-=3;
		if (x != 0) {						
			moveblock(here, top-here, y);	
			top=top-(here-y);
			//zeroblock(top, here-y);
		} else {
			//zeroblock(y, top-y);
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
		here=3;
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
			here=here3-3;
			gettoken();
			if (token == LINENUMBER && x == y) { // we have a double line at the end
				here2-=3;
				here-=3;
				moveblock(here2, linelength, here);
				//zeroblock(here+linelength, top-here-linelength);
				top=here+linelength;
			}
			return;
		}
		here-=3;
		push(here);
		here=here2-3;
		push(here);
		gettoken();
		if (x == y) {     // the line already exists and has to be replaced
			here2=pop();  // this is the line we are dealing with
			here=pop();   // this is the next line
			y=here-here2; // the length of the line as it is 
			if (linelength == y) {     // no change in line legth
				moveblock(top-linelength, linelength, here2);
				top=top-linelength;
				//zeroblock(top, linelength);
			} else if (linelength > y) { // the new line is longer than the old one
				moveblock(here, top-here, here+linelength-y);
				here=here+linelength-y;
				top=top+linelength-y;
				moveblock(top-linelength, linelength, here2);
				//zeroblock(top-linelength, linelength);
				top=top-linelength;
			} else {					// the new line is short than the old one
				moveblock(top-linelength, linelength, here2);
				//zeroblock(top-linelength, linelength);
				top=top-linelength;
				moveblock(here, top-here, here2+linelength);
				top=top-y+linelength;
			}
		} else {         // the line has to be inserted in between
			drop();
			here=pop();
			moveblock(here, top-here, here+linelength);
			moveblock(top, linelength, here);
			//zeroblock(top, linelength);
		}
	}
}

/* 
	Intermezzo the wrappers of the arduino io functions
*/ 

#ifdef ARDUINO
short aread(short p){ return analogRead(p); }
short dread(short p){ return digitalRead(p); }
void awrite(short p, short v){
	if (v >= 0 && v<256) analogWrite(p, v);
	else error(ERANGE);
}
void dwrite(short p, short v){
	if (v == 0) digitalWrite(p, LOW);
	else if (v == 1) digitalWrite(p, HIGH);
	else error(ERANGE);
}
void pinm(short p, short m){
	if ( m == 0 ) pinMode(p, OUTPUT);
	else if (m == 1) pinMode(p, INPUT);
	else if (m == 2) pinMode(p, INPUT_PULLUP);
	else error(ERANGE); 
}
#else
short aread(short p){ return 0; }
short dread(short p){ return 0; }
void awrite(short p, short v){}
void dwrite(short p, short v){}
void pinm(short p, short m){}
void delay(short t) {}
#endif

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

// parses a list of expression
short parsearguments() {
	char args=0;
	if (termsymbol()) return args;

nextexpression:
	expression();
	if (er != 0) { clearst(); return 0; }

	args++;

	if (token == ',') {
		nexttoken();
		goto nextexpression;
	} else 
		return args;
}

void parsenarguments(char n) {
	char args=0;
	nexttoken();

	args=parsearguments();
	if (er != 0 ) return;

	if (args == n) {
		return;
	}

	error(EARGS);
	clearst();
}


void parsefunction(short (*f)(short)){
	char args;

	nexttoken();
	args=parsesubscripts();
	if (er != 0) return;

	if (args == 1) {
		x=pop();
		x=f(x);
		push(x);
	} else {
		error(EARGS);
		return;
	}

}


void parseoperator(void (*f)()) {
	nexttoken();
	f();
	y=pop();
	x=pop();
}

short parsesubscripts() {
	char args = 0;

	if (token != '(') {return 0; }
	nexttoken();
	args=parsearguments();
	if (er != 0) {return 0; }
	if (token != ')') {error(EARGS); return 0; }
	return args;
}

void parsesubstring() {
	char xc1, yc1; 
	short args;
	short t1,t2;
	short unsigned h1; // remember the here
	char * bi1;
    xc1=xc;
    yc1=yc;

    if (st == SINT) // this is a hack - we rewind a token !
    	bi1=bi;
    else 
    	h1=here; 
    nexttoken();
    args=parsesubscripts();

    if (er != 0) {return; }
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

short xabs(short x){
	if (x<0) x=-x;
	return x;
}

short xsgn(short x){
	if (x>0) x=1;
	if (x<0) x=-1;
	return x;
}

// on an arduino, negative values of peek lead to the EEPROM
short peek(short x){
	if (x >= 0 && x<MEMSIZE) 
		return mem[x];
	else if (x < 0 && -x < elength())
		return eread(-x);
	else {
		error(ERANGE);
		return 0;
	}
}

// the fre function 
short xfre(short x) {
	if (x >=0 )
		return himem-top;
	return elength();
}


// very basic random number generator with constant seed.
short rnd(short r) {
	rd = (31421*rd + 6927) % 0x10000;
	if (r>=0) 
		return (rd*r)/0x10000;
	else 
		return (rd*r)/0x10000+1;
}

// a very simple approximate square root formula. 
short sqr(short r){
	short t=r;
	short l=0;
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
	return t;
}


char stringvalue() {
	char xcl;
	char ycl;
	if (token == STRING) {
		ir2=ir;
		push(x);
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
	} else {
		return FALSE;
	}
	return TRUE;
}

/* 

	numerical evaluation of a string expression

*/

void streval(){
	char *irl;
	short xl;
	char xcl;
	signed char t;
	unsigned short h1;

	if ( ! stringvalue()) {
		error(EUNKNOWN);
		return;
	} 
	if (er != 0) return;
	irl=ir2;
	xl=pop();

	h1=here;
	nexttoken();
	if (token != '=' && token != NOTEQUAL) {
		here=h1;
		push(irl[1]);
		return; 
	}

	t=token;
	nexttoken();

	if (! stringvalue() ){
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


void factor(){
	short args;
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
			push(getarray(xc, yc, x)); 
			break;
		case '(':
			nexttoken();
			expression();
			if (er != 0 ) return;
			if (token != ')') { error(EARGS); return; }
			break;

// Palo Alto BASIC functions
		case TABS: 
			parsefunction(xabs);
			break;
		case TRND: 
			parsefunction(rnd);
			break;
		case TSIZE:
			push(himem-top);
			break;
// Apple 1 BASIC functions
		case TSGN: 
			parsefunction(xsgn);
			break;
		case TPEEK: 
			parsefunction(peek);
			break;
		case TLEN: 
			nexttoken();
			if ( token != '(') {
				error(EARGS);
				return;
			}
			nexttoken();
			if (! stringvalue()) {
				error(EUNKNOWN);
				return;
			}
			if (er != 0) return;
			nexttoken();
			if (token != ')') {
				error(EARGS);
				return;	
			}
			break;
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
			//debugtoken(); outsc("after streval in factor"); outcr();
			if (er != 0 ) return;
			break;
//  Stefan's tinybasic additions
		case TSQR: 
			parsefunction(sqr);
			break;
		case TFRE: 
			parsefunction(xfre);
			break;
// Arduino I/O
		case TAREAD: 
			parsefunction(aread);
			break;
		case TDREAD: 
			parsefunction(dread);
			break;
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
			push(x%y);
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
	form=0;

	nexttoken();

processsymbol:

	if (termsymbol()) {
		if (! semicolon) outcr();
		nexttoken();
		return;
	}
	semicolon=FALSE;

	if (stringvalue()) {
		if (er != 0) return;
 		outs(ir2, pop());
 		nexttoken();
		goto separators;
	}

	// Palo Alto BASIC formatting stuff - tolerant code
	if (token == '#') {
		nexttoken();
		if (token == NUMBER) {
			form=x;
			nexttoken();
		}
	}

	if (token != ',' && token != ';' ) {
		expression();
		if (er != 0) return;
		outnumber(pop());
	}

separators:
	if (token == ',') {
		outspc();
		nexttoken();
	}
	if (token == ';'){
		semicolon=TRUE;
		nexttoken();
	}
	goto processsymbol;
}

/* 
	
	assigment code for various lefthand and righthand side. 

*/

void assignment() {
	signed char t=token;  // remember the left hand side token until the end of the statement, type of the lhs
	char ps=TRUE;  // also remember if the left hand side is a pure string of something with an index 
	char xcl, ycl; // to preserve the left hand side variable names
	short i=1;      // and the beginning of the destination string  
	short lensource;
	short args;

	// this code evaluates the left hand side
	ycl=yc;
	xcl=xc;
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
				clearst();
				return;
			}
			i=pop();
			break;
		case STRINGVAR:
			nexttoken();
			args=parsesubscripts();
			if (er != 0) return;
			switch(args) {
				case 0:
					i=1;
					break;
				case 1:
					nexttoken();
					i=pop();
					break;
				default:
					error(EARGS);
					clearst();
					return;
			}
			break;
		default:
			error(EUNKNOWN);
			return;
	}

	
	// the assignment part
	if (token != '=') {
		error(EUNKNOWN);
		return;
	}
	nexttoken();

	// here comes the code for the right hand side
	// rewritten 

	// stringvalue is a nasty function with many side effects
	// in ir2 and pop() we have the the adress and length of the source string, 
	// xc is the name, y contains the end and x the beginning index 

	if (! stringvalue () ) {
		if (er != 0 ) return;

		expression();
		if (er != 0 ) return;

		switch (t) {
			case VARIABLE:
				x=pop();
				setvar(xcl, ycl , x);
				break;
			case ARRAYVAR: 
				x=pop();	
				setarray(xcl, ycl, i, x);
				break;
			case STRINGVAR:
				ir=getstring(xcl, ycl, i);
				if (er != 0) return;
				ir[0]=pop();
				if (lenstring(xcl, ycl) < i && i < stringdim(xcl, ycl)) setstringlength(xcl, ycl, i);
				break;
		}		
	} else {

		switch (t) {
			case VARIABLE: // a scalar variable gets assigned the first string character 
				setvar(xcl, ycl , ir2[0]);
				drop();
				break;
			case ARRAYVAR: 	
				setarray(xcl, ycl, i, ir2[0]);
				drop();
				break;
			case STRINGVAR: // a string gets assigned a substring - copy algorithm
				lensource=pop();
				if ((lenstring(xcl, ycl)+lensource-1) > stringdim(xcl, ycl)) { error(ERANGE); return; }

				// the destination adress
				ir=getstring(xcl, ycl, i);

				// this code is needed to make sure we can copy one string to the same string 
				// without overwriting stuff, we go either left to right or backwards

				if (x > i) 
					for (int j=0; j<lensource; j++) { ir[j]=ir2[j];}
				else
					for (int j=lensource-1; j>=0; j--) ir[j]=ir2[j]; 
				setstringlength(xcl, ycl, i+lensource-1);
		}

		nexttoken();
	} 

}

/*
	input ["string",] variable [,["string",] variable]* 

*/

void xinput(){
	short args;

	nexttoken();

nextstring:
	if (token == STRING) {   
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
		outsc("? ");
		if (innumber(&x) == BREAKCHAR) {
			setvar(xc, yc, 0);
			st=SINT;
			nexttoken();
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

		outsc("? ");
		if (innumber(&x) == BREAKCHAR) {
			setarray(xc, yc, pop(), 0);
			st=SINT;
			nexttoken();
			return;
		} else {
			setarray(xc, yc, pop(), x);
		}
	}

	if (token == STRINGVAR) {
		ir=getstring(xc, yc, 1); 
		outsc("? ");
		ins(ir-1, stringdim(xc, yc));
 	}

	nexttoken();
	if (token == ',' || token == ';') {
		nexttoken();
		goto nextstring;
	}
}

/*

	goto, gosub, return and its helpers

*/

#ifdef HASGOSUB
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
		error(ESTACK);
	} 
}
#endif

void xgoto() {
	int t=token;

	nexttoken();
	expression();
	if (er != 0) return;

#ifdef HASGOSUB
	if (t == TGOSUB) pushgosubstack();
	if (er != 0) return;
#endif

	x=pop();
	findline(x);
	if ( er != 0 ) {
		error(ELINE);
#ifdef HASGOSUB
		if (t == TGOSUB) dropgosubstack();
#endif
		return;
	}
	if (st == SINT) st=SRUN;
	nexttoken();
}

void xreturn(){ 
#ifdef HASGOSUB
	popgosubstack();
	if (er != 0) return;
#endif
	nexttoken();
}


/* 

	if and then

*/


void xif() {
	
	nexttoken();
	expression();
	if (er != 0 ) return;

	x=pop();
	if (DEBUG) { outnumber(x); outcr(); } 
	if (! x) // on condition false skip the entire line
		do nexttoken();	while (token != LINENUMBER && token !=EOL && here <= top);
		
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

#ifdef HASFORNEXT

// find the NEXT token or the end of the program
void findnext(){
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
	
	nexttoken();
	if (token != VARIABLE) {
		error(EUNKNOWN);
		return;
	}
	push(yc);
	push(xc);
	nexttoken();
	if (token != '=') { 
		error(EUNKNOWN); 
		return; 
	}
	nexttoken();
	expression();
	if (er != 0) return;
	x=pop();	
	xc=pop();
	yc=pop();
	setvar(xc, yc, x);
	if (DEBUG) { outch(xc); outspc(); outnumber(x); outcr(); }
	push(xc);
	if (token != TTO){
		error(EUNKNOWN);
		return;
	}
	nexttoken();
	expression();
	if (er != 0) return;

	if (token == TSTEP) {
		nexttoken();
		expression();
		if (er != 0) return;
		y=pop();
	} else 
		y=1;
	if (DEBUG) { debugtoken(); outnumber(y); outcr(); }
	if (! termsymbol()) {
		error(UNKNOWN);
		return;
	}
	x=pop();
	xc=pop();
	if (st == SINT)
		here=bi-ibuffer;
	pushforstack();
	if (er != 0) return;

/*
	this tests the condition and stops if it is fulfilled already from start 
	there is an apocryphal feature here: STEP 0 is legal triggers an infinite loop
*/
	if ( (y > 0 && getvar(xc, yc)>x) || (y < 0 && getvar(xc, yc)<x ) ) { 
		dropforstack();
		findnext();
		nexttoken();
		return;
	}
}

/*
	an apocryphal feature here is the BREAK command ending a look
	doesn't work well for nested loops - to be tested carefully
*/
void xbreak(){
	dropforstack();
	findnext();
	nexttoken();
}

void xnext(){
	char xcl=0;
	char ycl;
	short t;

	nexttoken();
	if (termsymbol()) goto plainnext;
	if (token == VARIABLE) {
		xcl=xc;
		ycl=yc;
		nexttoken();
		if (! termsymbol()) {
			error(EUNKNOWN);
			return;
		}
	}

plainnext:
	push(here);
	popforstack();
	if (xcl) {
		if (xcl != xc || ycl != yc ) {
			error(EUNKNOWN);
			return;
		} 
	}
	if (y == 0) goto backtofor;
	t=getvar(xc, yc)+y;
	setvar(xc, yc, t);
	if (y > 0 && t <= x) goto backtofor;
	if (y < 0 && t >= x) goto backtofor;

	// last iteration completed
	here=pop();
	nexttoken();
	return;

	// next iteration
backtofor:
	pushforstack();
	if (st == SINT)
		bi=ibuffer+here;
	drop();
	nexttoken();
	return;

}

#else
void xfor(){
	nexttoken();
}
void xbreak(){
	nexttoken();
}
void xnext(){
	nexttoken();
}
#endif

/* 
	
	list - this is also used in save

*/

void outputtoken() {

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
				if (token == TTHEN || token == TTO || token == TSTEP) outspc();
				outsc(getkeyword(token)); 
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
	short b, e;
	char oflag = FALSE;

	nexttoken();
	y=parsearguments();
	if (er != 0) return;
	switch (y) {
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

	here=0;
	gettoken();
	while (here < top) {
		if (token == LINENUMBER && x >= b) oflag=TRUE;
		if (token == LINENUMBER && x >  e) oflag=FALSE;
		if (oflag) outputtoken();
		gettoken();
		if (token == LINENUMBER && oflag) outcr();
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
	if ( er != 0 ) { error(ELINE); return; }
	if (st == SINT) st=SRUN;

	xclr();

statementloop:
	while (here < top && ( st == SRUN || st == SERUN)) {	
		statement();
	}
	st=SINT;
}



void xnew(){ // the general cleanup function
	clearst();
	himem=MEMSIZE-1;
	top=0;
	zeroblock(top,himem);
	reseterror();
	st=SINT;
	nvars=0;
	fnc=0;
#ifdef HASGOSUB
	gosubsp=0;
#endif
#ifdef HASFORNEXT
	forsp=0;
#endif
}


void xrem() {
	
	while (token != LINENUMBER && token != EOL && here <= top)
		nexttoken(); 
}

/* 

	The Apple 1 BASIC additions

*/


/* 

	clearing variable space

*/


void xclr() {
	clrvars();
#ifdef HASGOSUB
	gosubsp=0;
#endif
#ifdef HASFORNEXT
	forsp=0;
#endif
	nexttoken();
}

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
			createstring(xcl, ycl, x);
		} else {
			createarry(xcl, ycl, x);
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
	
	low level poke to the basic memory

*/

void xpoke(){
	parsenarguments(2);
	if (er != 0) return;
	y=pop();
	x=pop();
	if (x >= 0 && x<MEMSIZE) 
		mem[x]=y;
	else if (x < 0 && -x < elength())
			eupdate(-x, y);
	else {
		error(ERANGE);
	}
}

/*

	the TAB spaces command of Apple 1 BASIC

*/

void xtab(){
	parsenarguments(1);
	if (er != 0) return;
	x=pop();
	while (x-- > 0) outspc();	
}

/*

	Stefan's additions 

*/

#ifdef HASDUMP
void xdump() {
	
	nexttoken();
	y=parsearguments();
	if (er != 0) return;
	switch (y) {
		case 0: 
			x=0;
			y=MEMSIZE-1;
			break;
		case 1: 
			x=pop();
			y=MEMSIZE-1;
			break;
		case 2: 
			y=pop();
			x=pop();
			break;
		default:
			error(EARGS);
			return;
	}

	form=6;
	dumpmem(y/8+1, x);
	form=0;
	nexttoken();
}

void dumpmem(unsigned short r, unsigned short b) {
	unsigned short j, i;	
	unsigned short k;

	k=b;
	i=r;
	while (i>0) {
		outnumber(k); outspc();
		for (j=0; j<8; j++) {
			outnumber(mem[k++]); outspc();
			delay(1); // slow down a little here for low serial baudrates
			if (k > MEMSIZE-1) break;
		}
		outcr();
		i--;
		if (k > MEMSIZE-1) break;
	}
#ifdef ARDUINOEEPROM
	printmessage(EEEPROM); outcr();
	i=r;
	k=0;
	while (i>0) {
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
#endif

char * getfilename() {
	nexttoken();
	if (token == STRING && x > 0 && x < SBUFSIZE) {
		sbuffer[x--]=0;
		while (x >= 0) { sbuffer[x]=ir[x]; x--; }
		return sbuffer;
	} else if (termsymbol()) {
		return getmessage(MFILE);
	} else {
		error(EUNKNOWN);
		return NULL;
	}
}


void xsave() {
	char * filename;

#ifdef ARDUINO
#ifdef ARDUINOEEPROM
	if (top+EHEADERSIZE < elength()) {
		x=0;
		eupdate(x++, 0); // EEPROM per default is 255, 0 indicates that there is a program
		z.i=top;
		eupdate(x++, z.b.l);
		eupdate(x++, z.b.h);
		while (x < top+EHEADERSIZE){
			eupdate(x, mem[x-EHEADERSIZE]);
			x++;
		}
		eupdate(x++,0);
		nexttoken();
	} else {
		error(EOUTOFMEMORY);
		er=0;
		nexttoken();
		return;
	}
#else 
	nexttoken();
#endif
#else 

	filename=getfilename();
	if (er != 0 || filename == NULL) return; 

	fd=fopen(filename, "w");
	if (!fd) {
		error(EFILE);
		nexttoken();
		return;
	} 
	xlist();
	fclose(fd);
	fd=0;
	// no nexttoken here because list has already done this
#endif
}

void xload() {
	char * filename;

#ifdef ARDUINO
#ifdef ARDUINOEEPROM
	x=0;
	if (eread(x) == 0 || eread(x) == 1) { // have we stored a program
		x++;
		z.b.l=eread(x++);
		z.b.h=eread(x++);
		top=z.i;
		while (x < top+EHEADERSIZE){
			mem[x-EHEADERSIZE]=eread(x);
			x++;
		}
	} else { // no valid program data is stored 
		error(EEEPROM);
	}
	nexttoken();
#else 
	nexttoken();
#endif
#else 
/*
	Load code on the Mac
*/

	filename=getfilename();
	if (er != 0 || filename == NULL) return; 

	fd=fopen(filename, "r");
	if (!fd) {
		error(EFILE);
		nexttoken();
		return;
	}

	while (fgets(ibuffer+1, BUFSIZE, fd)) {
		bi=ibuffer;
		while(*bi != 0) { if (*bi == '\n' || *bi == '\r') *bi=' '; bi++; };
		bi=ibuffer;
		nexttoken();
		if (token == NUMBER) storeline();
		if (er != 0 ) break;
	}
	fclose(fd);	
	fd=0;

	// nexttoken();

#endif
}

/*

	get just one character from input

*/


void xget(){
	nexttoken();
	if (token == VARIABLE) {
		checkch();
		setvar(xc, yc, inch());
		nexttoken();
	} else if (token == STRINGVAR) {
		// need to implement string left value
		error(EUNKNOWN);
	} else {
		error(EUNKNOWN);
	}
}


/* 
	the set command itself is also apocryphal it is a low level
	control command setting certain properties
	syntax, currently it is only 

	set expression
*/

void xset(){
	parsenarguments(2);
	if (er != 0) return;
	x=pop();
	y=pop();
	switch (y) {
#ifdef ARDUINOEEPROM
			// change the autorun/run flag of the EEPROM
			// 255 for clear, 0 for prog, 1 for autorun
		case 1: 
			eupdate(0, x);
			break;
#endif
#ifdef ARDUINOLCD
		case 2:
			switch (x) {
				case 0:
					od=OSERIAL;
					break;
				case 1: 
					od=OLCD;
					break;
				case 2:
					lcd.clear();
          			lcd.home();
          			lcdmycol=0;
          			lcdmyrow=0;
					break;
				case 3:
					lcd.home();
          			lcdmycol=0;
          			lcdmyrow=0;
					break;
				case 4:
					lcd.blink();
					break;
				case 5: 
					lcd.noBlink();
					break;
			}
			break;
		case 3:
			lcd.setCursor(0, x);
			break;
#endif
#ifdef ARDUINOPS2
		case 4:
			switch(x) {
				case 0: 
					id=ISERIAL;
					break;
				case 1:
					id=IKEYBOAD;
					break;
			}
			break;

#endif
	}
}



/*

	The arduino io functions.

*/

void xdwrite(){
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	dwrite(y, x);	
}

void xawrite(){
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	awrite(y, x);
}

void xpinm(){
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	pinm(y, x);	
}

void xdelay(){
	parsenarguments(1);
	if (er != 0) return;
	x=pop();
	delay(x);	
}

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
#ifdef HASGOSUB
			case TRETURN:
				xreturn();
				break;
			case TGOSUB:
#endif
			case TGOTO:
				xgoto();	
				break;
			case TIF:
				xif();
				break;
#ifdef HASFORNEXT
			case TFOR:
				xfor();
				break;		
			case TNEXT:
				xnext();
				break;
			case TBREAK:
				xbreak();
				break;
#endif
			case TSTOP:
			case TEND: // return here because new input is needed
				st=SINT;
				return;
			case TLIST:
				xlist();
				break;
			case TSCR:
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
// Stefan's tinybasic additions
#ifdef HASDUMP
			case TDUMP:
				xdump();
				break;
#endif
			case TSAVE:
				xsave();
				break;
			case TLOAD: // return here because new input is needed
				xload();
				return;
			case TGET:
				xget();
				break;
			case TSET:
				xset();
				break;
// Arduino IO
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
			case UNKNOWN:
				error(EUNKNOWN);
				return;
			case ':':
				nexttoken();
				break;
			default: // very tolerant - tokens are just skipped 
				nexttoken();
		}
		// clearst(); // brutalinsky - fix potential memory leaks
#ifdef ARDUINO
		if (checkch() == BREAKCHAR) {st=SINT; xc=inch(); return;};  // on an Arduino entering "#" at runtime stops the program
#endif
		if (er) { reseterror(); return;}
	}
}

// the setup routine - Arduino style
void setup() {

	ioinit();
	printmessage(MGREET); outcr();
 	xnew();		

#ifdef ARDUINOEEPROM
  	if (eread(0) == 1){ // autorun from the EEPROM
		top=(unsigned char) eread(1);
		top+=((unsigned char) eread(2))*256;
  		st=SERUN;
  	} 
#endif
}

// the loop routine for interactive input 
void loop() {

	if (st != SERUN) {
		// printmessage(MREADY); outcr();
		printmessage(MPROMPT);
    	ins(ibuffer, BUFSIZE);
        
        bi=ibuffer;
		nexttoken();

    	if (token == NUMBER)
      		storeline();
    	else {
      		st=SINT;
      		statement();   
    	}
	} else {
		xrun();
		// cleanup needed after autorun, top is the EEPROM top
    	top=0;
	}
}


#ifndef ARDUINO
int main(){
	setup();
	while (TRUE)
		loop();
}
#endif
