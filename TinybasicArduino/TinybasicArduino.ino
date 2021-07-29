// $Id: basic.c,v 1.46 2021/07/29 19:19:52 stefan Exp stefan $
/*
	Stefan's tiny basic interpreter 

	Playing around with frugal programming. See the licence file on 
	https://github.com/slviajero/tinybasic for copyright/left.
    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)

	Author: Stefan Lenz, sl001@serverfabrik.de

*/

/* 
	Defines the target - ARDUINO compiles for ARDUINO
	ARDINOLCD includes the lcd code 
*/


#undef ESP8266

// if PROGMEM is defined we can asssume we compile on 
// the Arduino IDE 
#ifdef PROGMEM
#define ARDUINO
#define ARDUINOPROGMEM
#else
#undef ARDUINO
#endif

// don's use PROGMEM on an ESP
#ifdef ESP8266
#define PROGMEM
#undef ARDUINOPROGMEM
#endif

// ARDUINO extensions
#undef ARDUINOLCD
#undef ARDUINOEEPROM

// SMALLness 
// GOSUB costs 100 bytes of program memory
// FORNEXTSTEP costs 850 bytes on an ARDUINO UNO
// FUNCTIONS SQR and RND cost 166 bytes
#define HASFORNEXT
#define HASGOSUB
#define HASFUNCTIONS
#define HASDUMP
#undef  USESPICOSERIAL


#ifdef ARDUINO
#include <avr/pgmspace.h>
#ifdef ARDUINOEEPROM
#include <EEPROM.h>
#endif
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

#define TRUE  1
#define FALSE 0

#define DEBUG  0

#define BUFSIZE 	72
#define SBUFSIZE	9
#define MEMSIZE  	1024
#define VARSIZE		26
#define STACKSIZE 	15
#define GOSUBDEPTH 	4
#define FORDEPTH 	4

// definitions on EEROM handling in the Arduino
#define EHEADERSIZE 3

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
#define GREATEREQUAL -121
#define LESSEREQUAL  -120
#define NOTEQUAL	 -119
#define TIF     -118
#define TTO     -117
#define TNEW    -116
#define TRUN    -115
#define TLET    -114
#define TFOR    -113
#define TEND    -112
#define TTHEN   -111
#define TGOTO   -110
#define TCONT   -109
#define TNEXT   -108
#define TSTEP   -107
#define TPRINT  -106
#define TINPUT  -105
#define TGOSUB  -104
#define TRETURN -103
#define TLIST	-102
#define TCLR	-101
#define TNOT    -100
#define TAND	-99
#define TOR  	-98
#define TABS 	-97
#define TRND	-96
#define TSGN	-95	
#define TPEEK	-94
#define TSQR	-93
#define TFRE	-92
#define TDUMP 	-91
#define TBREAK  -90
#define TSAVE   -89
#define TLOAD   -88
#define TREM 	-87
#define TPINM	-86
#define TDWRITE	-85
#define TDREAD	-84
#define TAWRITE	-83
#define TAREAD  -82
#define TDELAY  -81
#define TPOKE	-80
#define TGET    -79
#define TSET    -78
#define TDIM	-77
#define TERROR  -3
#define UNKNOWN -2
#define NEWLINE -1

// the messages and errors
#define MREADY       0
#define EGENERAL 	 1
#define EOUTOFMEMORY 2
#define ESTACK 		 3
#define ELINE        4
#define EUNKNOWN	 5
#define EVARIABLE	 6
#define EDIVIDE		 7
#define ERANGE 		 8
#define EFILE 		 9
#define EFUN 		 10
#define EARGS		 11
#define EEEPROM		 12


// the number of keywords, and the base index of the keywords
#define NKEYWORDS	45
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
	Arduino output models
*/

#define OSERIAL 0
#define OLCD 1

/*
	All BASIC keywords
*/

const char sge[]   PROGMEM = "=>";
const char sle[]   PROGMEM = "<=";
const char sne[]   PROGMEM = "<>";
const char sif[]   PROGMEM = "IF";
const char sto[]   PROGMEM = "TO";
const char snew[]  PROGMEM = "NEW";
const char srun[]  PROGMEM = "RUN";
const char slet[]  PROGMEM = "LET";
const char sfor[]  PROGMEM = "FOR";
const char send[]  PROGMEM = "END";
const char sthen[] PROGMEM = "THEN";
const char sgoto[] PROGMEM = "GOTO";
const char scont[] PROGMEM = "CONT";
const char snext[] PROGMEM = "NEXT";
const char sstep[]   PROGMEM = "STEP";
const char sprint[]  PROGMEM = "PRINT";
const char sinput[]  PROGMEM = "INPUT";
const char sgosub[]  PROGMEM = "GOSUB";
const char sreturn[] PROGMEM = "RETURN";
const char slist[]   PROGMEM = "LIST";
const char sclr[]    PROGMEM = "CLR";
const char snot[]    PROGMEM = "NOT";
const char sand[]    PROGMEM = "AND";
const char sor[]     PROGMEM = "OR";
const char sabs[]    PROGMEM = "ABS";
const char srnd[]    PROGMEM = "RND";
const char ssgn[]    PROGMEM = "SGN";
const char speek[]   PROGMEM = "PEEK";
const char ssqr[]    PROGMEM = "SQR";
const char sfre[]    PROGMEM = "FRE";
const char sdump[]   PROGMEM = "DUMP";
const char sbreak[]  PROGMEM = "BREAK";
const char ssave[]   PROGMEM = "SAVE";
const char sload[]   PROGMEM = "LOAD";
const char srem[]    PROGMEM = "REM";
const char spinm[]   PROGMEM = "PIMN";
const char sdwrite[] PROGMEM = "DWRITE";
const char sdread[]  PROGMEM = "DREAD";
const char sawrite[] PROGMEM = "AWRITE";
const char saread[]  PROGMEM = "AREAD";
const char sdelay[]  PROGMEM = "DELAY";
const char spoke[]   PROGMEM = "POKE";
const char sget[]    PROGMEM = "GET";
const char sset[]    PROGMEM = "SET";
const char sdim[]    PROGMEM = "DIM";

const char* const keyword[] PROGMEM = {
	sge, sle, sne, sif, sto, snew, srun, slet,
 	sfor, send, sthen, sgoto, scont, snext,
 	sstep, sprint, sinput, sgosub, sreturn,
 	slist, sclr, snot, sand, sor, sabs, srnd,
 	ssgn, speek, ssqr, sfre, sdump, sbreak,
 	ssave, sload, srem, spinm, sdwrite, sdread,
    sawrite, saread, sdelay, spoke, sget, sset,
    sdim
};

/*
	the message catalogue also moved to progmem
*/

// the messages and errors
#define MREADY       0
#define EGENERAL 	 1
#define EOUTOFMEMORY 2
#define ESTACK 		 3
#define ELINE        4
#define EUNKNOWN	 5
#define EVARIABLE	 6
#define EDIVIDE		 7
#define ERANGE 		 8
#define EFILE 		 9
#define EFUN 		 10
#define EARGS		 11
#define EEEPROM		 12

const char mready[]    	PROGMEM = "Ready";
const char egeneral[]  	PROGMEM = "Error";
const char emem[]  	   	PROGMEM = "Memory";
const char estack[]    	PROGMEM = "Stack";
const char eline[]  	PROGMEM = "Line";
const char eunknown[]  	PROGMEM = "Syntax";
const char evariable[]  PROGMEM = "Variable";
const char edivide[]  	PROGMEM = "Divide";
const char erange[]  	PROGMEM = "Range";
const char efile[]  	PROGMEM = "File";
const char efun[] 	 	PROGMEM = "Function";
const char eargs[]  	PROGMEM = "Args";
const char eeeprom[]	PROGMEM = "EEPROM";

const char* const message[] PROGMEM = {
	mready, egeneral, emem, estack, eline,
	eunknown, evariable, edivide, erange,
	efile, efun, eargs, eeeprom
};


/* 
	The Arduino LCD shield code
*/ 

#ifdef ARDUINOLCD
const int pin_RS = 8; 
const int pin_EN = 9; 
const int pin_d4 = 4; 
const int pin_d5 = 5; 
const int pin_d6 = 6; 
const int pin_d7 = 7; 
const int pin_BL = 10; 
LiquidCrystal lcd( pin_RS,  pin_EN,  pin_d4,  pin_d5,  pin_d6,  pin_d7);
#endif

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

	ir is a general index egister for string processing.

	token contains the actually processes token.

	st, here and top are the interpreter runtime controls.
	here2 and here3 are aux variable to make walking through
	lines more efficient.

	rd is the random number storage.

	od is the output model for an arduino

	fd is the filedescriptor for save/load

*/
static short stack[STACKSIZE];
static unsigned short sp=0; 

static char ibuffer[BUFSIZE] = "\0";
static char *bi;

static char sbuffer[SBUFSIZE];

static short vars[VARSIZE];

static signed char mem[MEMSIZE];
static unsigned short himem=MEMSIZE;

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

static char *ir;
static signed char token;
static signed char er;

static signed char st; 
static unsigned short here, here2, here3; 
static unsigned short top;

#ifdef HASFUNCTIONS
static unsigned int rd;
#endif

static char od;

#ifndef ARDUINO
FILE* fd;
struct termios initialstate, newstate;
#endif

/* 
	Layer 0 functions 
*/

// variable handling - interface between memory 
// and variable storage
short getvar(char, char);
void setvar(char, char, short);
char varname(char);
void createvar(char);
void delvar(char);
void clrvars();

// error handling
void error(signed char);
void resetinterpreter();

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
void picogetchar(int);
void outch(char);
char inch();
char checkch();
void outcr();
void outspc();
void outs(char*, short);
void outsc(char*);
void ins(char*, short); 
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
void storetoken(); 
void gettoken();
void firstline();
void nextline();
void findline(short);
void moveblock(short, short, short);
void zeroblock(short, short);
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
void xend();
void xcont();
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

short getvar(char c, char d){
	if (DEBUG) { outsc("getvar "); outch(c); outch(d); outspc(); outnumber(vars[c-65]); outcr(); }
	if (c >= 65 && c<=91)
		return vars[c-65];
	error(EVARIABLE);
	return 0;
}

void setvar(char c, char d, short v){
	if (DEBUG) { outsc("setvar "); outch(c); outch(d); outspc(); outnumber(v); outcr(); }
	if (c >= 65 && c<=91)
		vars[c-65]=v;
	else 
		error(EVARIABLE);
}

char varname(char c){
	return c;
}

void createvar(char c){
	return;
}

void delvar(char c){
	return;
}

void clrvars() {
	for (char i=0; i<VARSIZE; i++) vars[i]=0;
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

void printmessage(char i){
#ifndef ARDUINOPROGMEM
	outsc((char *)message[i]);
#else
	strcpy_P(sbuffer, (char*) pgm_read_word(&(message[i]))); 
	outsc(sbuffer);
#endif
}

/*
  Layer 0 - error handling

  The general error handler. The static variable er
  contains the error state. 

*/ 

void error(signed char e){
	er=e;
	if (e < 0) 
		outputtoken();
	else  
		printmessage(e);
	outspc();
	printmessage(EGENERAL);
	outcr();
}


#ifdef DEBUG
void debugtoken(){
	outsc("Token: ");
	if (token != EOL) {
		outputtoken();
		outcr();
	} else {
		outsc("EOL");
		outcr();
	}
}

void debug(char *c){
	outsc(c); 
	debugtoken();
}

void debugn(signed char t){
	outsc(getkeyword(t)); outcr();
}
#endif

void resetinterpreter(){ // the general cleanup function
	clearst();
#ifdef HASGOSUB
	gosubsp=0;
#endif
#ifdef HASFORNEXT
	forsp=0;
#endif
}


/*
	Arithmetic and runtime operations are mostly done
	on a stack of 16 bit objects.
*/

void push(short t){
	if (sp == STACKSIZE)
		error(EOUTOFMEMORY);
	else
		stack[sp++]=t;
}

short pop(){
	if (sp == 0) {
		error(ESTACK);
		return 0;
	}
	else
		return stack[--sp];	
}

void drop() {
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

#ifdef HASGOSUB
void pushgosubstack(){
	if (gosubsp < GOSUBDEPTH) {
		gosubstack[gosubsp]=here;
		gosubsp++;	
	} else 
		error(ESTACK);
}

void popgosubstack(){
	if (gosubsp>0) {
		gosubsp--;
	} else {
		error(ESTACK);
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

/* 

	Input and output functions.

	The interpreter is implemented to use only putchar 
	and getchar.

	These functions need to be provided by the stdio lib 
	or by any other routine like a serial terminal or keyboard 
	interface of an arduino.

*/

// PicoSerial Code, read and buffer one entire string that become deaf
// but still track 
#ifdef USESPICOSERIAL
volatile static char picochar;
volatile static char picoa = FALSE;
volatile static char* picob = NULL;
static short picobsize = 0;
volatile static short picoi = 0;

void picogetchar(int c){
	picochar=c;
	if (picob && (! picoa) ) {
		if (picochar != '\n' && picochar != '\r' && picoi<picobsize-1) {
			picob[picoi++]=picochar;
		} else {
			picoa = TRUE;
			picob[picoi]=0;
			picoi=0;
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
#ifdef USESPICOSERIAL
    return picochar;
#else 
	if (Serial.available()) return Serial.peek(); 
#endif	
#else 
	/*
	struct termios orgt, newt;
	char ic;
	tcgetattr(0,&orgt);
  	newt = orgt;
  	newt.c_lflag &= ~ICANON;
  	newt.c_lflag &= ~ECHO;
  	newt.c_lflag &= ~ISIG;
  	newt.c_cc[VMIN] = 0;
  	newt.c_cc[VTIME] = 0;
	tcsetattr(0, TCSANOW, &newt);
	ic=getchar();
	tcsetattr(0, TCSANOW, &orgt);
	if (ic != -1) { outsc("Got character "); outnumber(ic); outcr(); }
	return (ic == c);
	*/
#endif
	return 0;
}

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

// reads a line from the keybord to the input buffer
// the new ins - reads into a buffer the caller supplies
// nb is the max size of the buffer

void ins(char *b, short nb) {
#ifndef USESPICOSERIAL
	char c;
	short i = 0;
	while(i < nb-1) {
		c=inch();
		if (c == '\n' || c == '\r') {
			b[i]=0x00;
			break;
		} else {
			b[i++]=c;
		} 
		if (c == 0x08 && i>0) {
			i--;
		}
	}
}
#else 
	picob=b;
	picobsize=nb;
	picoa=FALSE;
	while (! picoa);
	outsc(b); outcr();
}
#endif

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
	int i = 0;
	int s = 1;
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
		}
	}
	return 0;
}

// prints a 16 bit number
void outnumber(short x){
	char c, co;
	short d;
	if (x<0) {
		outch('-');
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
			c=TRUE;
		}
		d=d/10;
	}
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

	if (DEBUG) printmessage(EVARIABLE);
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
		if ( *(bi+xc) < 'A' || *(bi+x) > 'Z' ) {
			bi+=xc;
			if (DEBUG) debugtoken();
			return;
		} else {
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
		if (*bi >= '0' && *bi <= '9') { 
			yc=*bi;
			bi++;
		} else if (*bi == '$') {
			token=STRINGVAR;
			bi++;
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

#ifdef HASDUMP
void dumpmem(int r) {
	int j, i;	
	int k=0;
	i=r;
	while (i>0) {
		for (j=0; j<8; j++) {
			outnumber(mem[k++]); outspc();
		}
		outcr();
		i--;
	}
#ifdef ARDUINOEEPROM
	printmessage(EEEPROM); outcr();
	i=r;
	k=0;
	while (i>0) {
		for (j=0; j<8; j++) {
			outnumber(EEPROM[k++]); outspc();
		}
		outcr();
		i--;
	}
#endif
	outnumber(top); outcr();
}
#endif

void storetoken() {
	short i=x;
	switch (token) {
		case NUMBER:
		case LINENUMBER:
			if ( nomemory(3) ) goto memoryerror;
			mem[top++]=token;	
			mem[top++]=x%256;
			mem[top++]=x/256;
			return;
		case VARIABLE:
			if ( nomemory(3) ) goto memoryerror;
			mem[top++]=token;
			mem[top++]=xc;
			mem[top++]=yc;
			return;
		case STRINGVAR:
			if ( nomemory(2) ) goto memoryerror;
			mem[top++]=token;
			mem[top++]=xc;
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


char memread(short i){
#ifndef ARDUINOEEPROM
	return mem[i];
#else 
	if (st != SERUN) {
		return mem[i];
	} else {
		return EEPROM[i+EHEADERSIZE];
	}
#endif
}


void gettoken() {
	token=memread(here++);
	switch (token) {
		case NUMBER:
		case LINENUMBER:		
			x=(unsigned char)memread(here++);
			x+=memread(here++)*256; 
			break;
		case VARIABLE:
			xc=memread(here++);
			yc=memread(here++);
			break;
		case STRINGVAR:
			xc=memread(here++);
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

void findline(short l) {
	here=0;
	while (here < top) {
		gettoken();
		if (token == LINENUMBER && x == l) {
			return;
		}
	}
	error(ELINE);
}

/*

   	Move a block of storage beginng at b ending at e
  	to destination d. No error handling here!!

*/

void moveblock(short b, short l, short d){
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

void zeroblock(short b, short l){
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
	
*/

#ifdef DEBUG
void diag(){
	outsc("top, here, here2, here3, y and x\n");
	outnumber(top); outspc();
	outnumber(here); outspc();
	outnumber(here2); outspc();
	outnumber(here3); outspc();
	outnumber(y); outspc();	
	outnumber(x); outspc();		
	outcr();
}
#endif

void storeline() {
	short linelength;
	short newline; 

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
			zeroblock(top, himem-top);
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
		zeroblock(here, linelength);
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
			zeroblock(top, here-y);
		} else {
			zeroblock(y, top-y);
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
				zeroblock(here+linelength, top-here-linelength);
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
				zeroblock(top, linelength);
			} else if (linelength > y) { // the new line is longer than the old one
				moveblock(here, top-here, here+linelength-y);
				here=here+linelength-y;
				top=top+linelength-y;
				moveblock(top-linelength, linelength, here2);
				zeroblock(top-linelength, linelength);
				top=top-linelength;
			} else {					// the new line is short than the old one
				moveblock(top-linelength, linelength, here2);
				zeroblock(top-linelength, linelength);
				top=top-linelength;
				moveblock(here, top-here, here2+linelength);
				top=top-y+linelength;
			}
		} else {         // the line has to be inserted in between
			drop();
			here=pop();
			moveblock(here, top-here, here+linelength);
			moveblock(top, linelength, here);
			zeroblock(top, linelength);
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

void parsefunction(short (*f)(short)){
	nexttoken();
	if ( token != '(') {
		error('(');
		return;
	}
	nexttoken();
	expression();
	if (er == 0 && token == ')') {
		x=pop();
		x=f(x);
		push(x);
	} else {
		error(')');
		return;
	}
	nexttoken();
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
	else {
#ifndef ARDUINOEEPROM
		error(ERANGE);
		return 0;
#else
		if (x < 0 && -x < EEPROM.length())
			return EEPROM.read(-x);
		else {
			error(ERANGE);
			return 0;
		}
#endif
	}
}

short xfre(short x) {
	return himem-top;
}


#ifdef HASFUNCTIONS
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
#endif

void factor(){
	if (DEBUG) debug("factor");
	switch (token) {
		case NUMBER: 
			push(x);
			break;
		case VARIABLE: 
			if (DEBUG) { outsc("factor: "); outch(xc); outch(yc); outcr(); }
			push(getvar(xc, yc));
			break;
		case '(':
			nexttoken();
			expression();
			if (token != ')')
				error(token);
			break;
		case TABS: 
			parsefunction(xabs);
			break;
		case TSGN: 
			parsefunction(xsgn);
			break;
		case TPEEK: 
			parsefunction(peek);
			break;
#ifdef HASFUNCTIONS
		case TRND: 
			parsefunction(rnd);
			break;	
		case TSQR: 
			parsefunction(sqr);
			break;
#endif
		case TFRE: 
			parsefunction(xfre);
			break;
		case TAREAD: 
			parsefunction(aread);
			break;
		case TDREAD: 
			parsefunction(dread);
			break;
		default:
			error(token);
	}
}

void parseoperator(void (*f)()) {
	nexttoken();
	f();
	y=pop();
	x=pop();
}

void term(){
	if (DEBUG) debug("term"); 
	factor();

nextfactor:
	nexttoken();
	if (DEBUG) debug("nexttoken");
	if (token == '*'){
		parseoperator(factor);
		push(x*y);
		goto nextfactor;
	} else if (token == '/'){
		parseoperator(factor);
		if (y != 0)
			push(x/y);
		else {
			error(EDIVIDE);
			push(0);	
		}
		goto nextfactor;
	} else if (token == '%'){
		parseoperator(factor);
		if (y != 0)
			push(x%y);
		else {
			error(EDIVIDE);
			push(0);	
		}
		goto nextfactor;
	} 
}

void addexpression(){
	if (DEBUG) debug("addexp");
	if (token != '+' && token != '-') {
		term();
	} else {
		push(0);
	}

nextterm:
	if (token == '+' ) { 
		parseoperator(term);
		push(x+y);
		goto nextterm;
	} else if (token == '-'){
		parseoperator(term);
		push(x-y);
		goto nextterm;
	}
}

void compexpression() {
	if (DEBUG) debug("compexp"); 
	addexpression();
	switch (token){
		case '=':
			parseoperator(compexpression);
			push(x == y);
			break;
		case NOTEQUAL:
			parseoperator(compexpression);
			push(x != y);
			break;
		case '>':
			parseoperator(compexpression);
			push(x > y);
			break;
		case '<':
			parseoperator(compexpression);
			push(x < y);
			break;
		case LESSEREQUAL:
			parseoperator(compexpression);
			push(x <= y);
			break;
		case GREATEREQUAL:
			parseoperator(compexpression);
			push(x >= y);
			break;
	}
}

void notexpression() {
	if (DEBUG) debug("notexp");
	if (token == TNOT) {
		nexttoken();
		compexpression();
		x=pop();
		push(!x);
	} else 
		compexpression();
}

void andexpression() {
	if (DEBUG) debug("andexp");
	notexpression();
	if (token == TAND) {
		parseoperator(expression);
		push(x && y);
	} 
}

void expression(){
	if (DEBUG) debug("exp"); 
	andexpression();
	if (token == TOR) {
		parseoperator(expression);
		push(x || y);
	} 
}

/* 

	The commands and their helpers
		termsymbol and two argument parsers
   
*/

char termsymbol() {
	return (token == ':' || token == LINENUMBER || token == EOL);
}

void parsetwoarguments() {
	nexttoken();
	expression();
	if (token != ',' ) {
		error(EARGS);
		return;
	} 
	nexttoken();
	expression();
	if (!termsymbol()) {
		error(EARGS);
		return;
	} 
	
}

void parseoneargument() {
	nexttoken();
	expression();
	if (!termsymbol()) {
		error(EARGS);
		return;
	} 
}

/*
   print 
*/ 
void xprint(){
	if (DEBUG) debugn(TPRINT); 

nextsymbol:
	nexttoken();

processsymbol:
	if (termsymbol()) {
		outcr();
		nexttoken();
		return;
	}
	if (token == STRING) {
		outs(ir, x);	
		goto nextsymbol;
	} 
	if (token == STRINGVAR) {
		goto nextsymbol;
	}
	if (token != ',' && token != ';' ) {
		expression();
		if (er == 0) {
			outnumber(pop());
		} else {
			clearst();
			er=0;
			nexttoken();
			return ;
		}
		goto processsymbol;
	}
	if (token == ',') {
		outspc();
	}
	goto nextsymbol;
}

void xget(){
	if (DEBUG) debugn(TGET); 

	nexttoken();
	if (token == VARIABLE) {
		checkch();
		setvar(xc, yc, inch());
		nexttoken();
	} else 
		error(TGET);
}

void assignment() {
	if (DEBUG) debugn(TLET); 
	push(xc);
	push(yc);
	nexttoken();
	if ( token == '=') {
		nexttoken();
		expression();
		if (er == 0) {
			x=pop();	
			yc=pop();
			xc=pop(); 
			setvar(xc, yc ,x);
		} else {
			clearst();
			er=0;
			return;
		}
	} else 
		error(token);
}

void xgoto() {
	if (DEBUG) debugn(TGOTO); 
	int t=token;

	nexttoken();
	expression();
	if (er == 0) {
#ifdef HASGOSUB
		if (t == TGOSUB) {
			if (DEBUG) debugn(TGOSUB); 
			pushgosubstack();
		}
#endif
		x=pop();
		findline(x);
#ifdef HASGOSUB
		if ( er != 0 && t == TGOSUB) {
			dropgosubstack();
		}
#endif
		nexttoken();
	} else {
		clearst();
		er=0;
		return;
	} 
}

void xend(){
	st=SINT;
}

void xcont(){
	st=SRUN;
	nexttoken();
}

void xif(){
	if (DEBUG) debugn(TIF); 
	nexttoken();
	expression();
	if (er == 0) {
		x=pop();
		if (DEBUG) { outnumber(x); outcr(); } 
		if (! x) {// on condition false skip the entire line
			do {
				nexttoken();	
			}
			while (token != LINENUMBER && token !=EOL && here <= top);
		}
		if (token == TTHEN) {
			nexttoken();
			if (token == NUMBER) 
				findline(x);
		}
	} else {
		error(TIF);
		clearst();
		er=0;
		return;
	}
}

// input ["string",] variable [,variable]* 
void xinput(){
	if (DEBUG) debugn(TINPUT); 
	nexttoken();

nextstring:
	if (token == STRING) {   		// [ "string" ,]
		outs(ir, x);
		nexttoken();
		if (token != ',') {
			error(TINPUT);
			return;
		} else 
			nexttoken();
	}

nextvariable:
	if (token == VARIABLE) {   // variable [, variable]
		outsc("? ");
		if (innumber(&x) == '#') {
			setvar(xc, yc, 0);
			st=SINT;
			nexttoken();
			return;
		} else {
			setvar(xc, yc, x);
		}
	} 
	nexttoken();
	if (token == ',') {
		nexttoken();
		goto nextstring;
	}
}


#ifdef HASFORNEXT
// find the NEXT token
void findnext(){
	while (token != TNEXT && here < top) 
		nexttoken(); 
}

// for is implemented only in program mode not in interactive mode
// for variable = expression to expression [STEP expression]
void xfor(){
	if (DEBUG) debugn(TFOR);
	nexttoken();
	if (token != VARIABLE) {
		error(token);
		return;
	}
	push(yc);
	push(xc);
	nexttoken();
	if (token != '=') {
		error(token);
		drop();
		return;
	}
	nexttoken();
	expression();
	if (er != 0) {
		drop();
		return;
	}
	x=pop();	
	xc=pop();
	yc=pop();
	setvar(xc, yc, x);
	if (DEBUG) {  outch(xc); outspc(); outnumber(x); outcr();
	}
	push(xc);
	if (token != TTO){
		drop();
		error(token);
		return;
	}
	nexttoken();
	expression();
	if (er != 0) {
		drop();
		return;
	}
	if (token == TSTEP) {
		nexttoken();
		expression();
		if (er != 0) {
			drop(); 
			drop();
			return;
		}
		y=pop();
	} else 
		y=1;
	if (DEBUG) { debugtoken(); outnumber(y); outcr();
	}
	if (token != LINENUMBER && token != ':') {
		error(token);
		drop();
		drop();
		return;
	}
	x=pop();
	xc=pop();
	if (st == SINT)
		here=bi-ibuffer;
	pushforstack();
	if (er != 0){
		return;
	}
/*
	this tests the condition and stops if it is fulfilled already from start 
	there is an apocryphal feature her STEP 0 is legal triggers an infinite loop
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
*/
void xbreak(){
	if (DEBUG) debugn(TBREAK);
	dropforstack();
	findnext();
	nexttoken();
}

// next is implemented in program mode and not in interactive mode
void xnext(){
	if (DEBUG) debugn(TNEXT); 
	push(here);
	popforstack();
	// at this point xc is the variable, x the stop value and y the step
	// here is the point tp jump back
	if (y == 0) goto backtofor;
	short t=getvar(xc, yc)+y;
	setvar(xc, yc, t);
	if (y > 0 && t <= x) goto backtofor;
	if (y < 0 && t >= x) goto backtofor;

	// last iteration completed
	here=pop();
	nexttoken();
	return;

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

#ifdef HASGOSUB
void xreturn(){
	if (DEBUG) debugn(TRETURN); 
	popgosubstack();
	nexttoken();
}
#else
void xreturn(){
	nexttoken();
}
#endif

void xload() {
	if (DEBUG) debugn(TLOAD);
#ifdef ARDUINO
#ifdef ARDUINOEEPROM
	x=0;
	if (EEPROM.read(x) == 0 || EEPROM.read(x) == 1) { // have we stored a program
		x++;
		top=(unsigned char) EEPROM.read(x++);
		top+=((unsigned char) EEPROM.read(x++))*256;
		while (x < top+EHEADERSIZE){
			mem[x-EHEADERSIZE]=EEPROM.read(x);
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
	fd=fopen("file.bas", "r");
	if (!fd) {
		error(EFILE);
		nexttoken();
		return;
	}
	while (fgets(ibuffer, BUFSIZE, fd)) {
		bi=ibuffer;
		while(*bi != 0) { if (*bi == '\n') *bi=' '; bi++; };
		bi=ibuffer;
		nexttoken();
		if (token == NUMBER) storeline();
	}
	fclose(fd);
	fd=0;
#endif
}

void xsave() {
	if (DEBUG) debugn(TSAVE); 
#ifdef ARDUINO
#ifdef ARDUINOEEPROM
	if (top+EHEADERSIZE < EEPROM.length()) {
		x=0;
		EEPROM.write(x++, 0); // EEROM per default is 255, 0 indicates that there is a program
		EEPROM.write(x++, top%256);
		EEPROM.write(x++, top/256);
		while (x < top+EHEADERSIZE){
			EEPROM.update(x, mem[x-EHEADERSIZE]);
			x++;
		}
		EEPROM.write(x++,0);
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
	fd=fopen("file.bas", "w");
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

void xrem() {
	if (DEBUG) debugn(TREM); 
	while (token != LINENUMBER && token != EOL && here <= top)
		nexttoken(); 
}
/* 
	control commands LIST, RUN, NEW, CLR
*/

void xclr() {
	clrvars();
	nexttoken();
}

void outputtoken() {
	if (token == LINENUMBER) {
		outnumber(x); outspc();			
	} else if (token == NUMBER ){
		outnumber(x); outspc();
	} else if (token == VARIABLE){
		outch(xc); 
		if (yc != 0) outch(yc);
	} else if (token == STRINGVAR) {
		outch(xc); outch('$');
	} else if (token == STRING) {
		outch('"'); outs(ir, x); outch('"');
	} else if (token < -3) {
		outsc(getkeyword(token)); outspc();
	} else if (token >= 32) {
		outch(token);
	} else{
		outch(token); outspc(); outnumber(token);
	} 
}

void xlist(){
	if (DEBUG) debugn(TLIST);
	nexttoken();
	if (token == NUMBER) {
		findline(x);
		if (er != 0) {
			nexttoken();
			return;
		} 
		push(here-3);
		nextline();
		here2=here-3;
		here=pop();
	} else {
		here=0;	
		here2=top;
	}
	gettoken();
	while (here <= here2) {
		outputtoken();
		gettoken();
		if (token == LINENUMBER) outcr();
	}
	if (here2 == top) outcr(); // supress newlines in "list 50" - a little hack
	nexttoken();
 }


void xrun(){
	char c;
	if (DEBUG) debugn(TRUN);
	here=0;
	if (st == SINT) st=SRUN;
	nexttoken();
	while (here < top && ( st == SRUN || st == SERUN)) {	
		statement();
	}
	st=SINT;
}

void xnew(){
	top=0;
	zeroblock(0,himem);
	resetinterpreter();
	st=SINT;
}

/* 
	The arduino io functions.
*/

void xdwrite(){
	parsetwoarguments();
	if (er == 0) {
		x=pop();
		y=pop();
		dwrite(y, x);	
	}
}

void xawrite(){
	parsetwoarguments();
	if (er == 0) {
		x=pop();
		y=pop();
		awrite(y, x);
	}
}

void xpinm(){
	parsetwoarguments();
	if (er == 0) {
		x=pop();
		y=pop();
		pinm(y, x);
	}
}

void xdelay(){
	parseoneargument();
	if (er == 0) {
		x=pop();
		delay(x);	
	}
}

/* 
	low level poke to the basic memory
*/

void xpoke(){
	parsetwoarguments();
	y=pop();
	x=pop();
	if (x >= 0 && x<MEMSIZE) 
		mem[x]=y;
	else {
#ifndef ARDUINO
		error(ERANGE);
#endif
#ifdef ARDUINOEEPROM
		if (x < 0 && -x < EEPROM.length())
			EEPROM.update(-x, y);
		else {
			error(ERANGE);
		}
#endif	
	}
}

/* 
	the set command itself is also apocryphal it is a low level
	control command setting certain properties
	syntax, currently it is only 

	set expression
*/

void xset(){
	parsetwoarguments();
	if (er == 0) {
		x=pop();
		y=pop();
		switch (y) {
#ifdef ARDUINOEEPROM
			// change the autorun/run flag of the EEPROM
			// 255 for clear, 0 for prog, 1 for autorun
			case 1: 
				EEPROM[0]=x;
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
						break;
					case 3:
						lcd.home();
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
		}
	}
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
	if (DEBUG) debug("statement\n"); 
	while (token != EOL) {
		switch(token){
			case TLIST:
				xlist();
				break;
			case TNEW:
				xnew();
				return;
			case TCLR:
				xclr();
				break;
			case TRUN:
				xrun();
				return;		
			case TPRINT:
				xprint();
				break;
			case TLET:
				nexttoken();
				if ( token != VARIABLE ) {
					error(token);
					break;
				}
			case VARIABLE:
				assignment();
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
			case TEND:
				xend();
				return;
			case TCONT:
				xcont();
				break;
			case TREM:
				xrem();
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
			case TINPUT:
				xinput();
				break;
#ifdef HASDUMP
			case TDUMP:
				dumpmem(5);
				nexttoken();
				break;
#endif
			case TSAVE:
				xsave();
				break;
			case TLOAD:
				xload();
				break;
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
			case TPOKE:
				xpoke();
				break;
			case TGET:
				xget();
				break;
			case TSET:
				xset();
				break;
			case UNKNOWN:
				error(EUNKNOWN);
				return;
			default:
				nexttoken();
		}
#ifdef ARDUINO
		if (checkch() == BREAKCHAR) {st=SINT; xc=inch(); return;};  // on an Arduino entering "#" at runtime stops the program
#endif
	}
}

// the setup routine - Arduino style
void setup() {

 	xnew();		
#ifdef ARDUINO
#ifdef USESPICOSERIAL
 	(void) PicoSerial.begin(9600, picogetchar);
#else 
  	Serial.begin(9600); 
#endif
#endif
#ifdef ARDUINOLCD
   	lcd.begin(16, 2);  // the dimension of the lcd shield - hardcoded, ugly
#endif
#ifdef ARDUINOEEPROM
  	if (EEPROM.read(0) == 1){ // autorun from the EEPROM
		top=(unsigned char) EEPROM.read(1);
		top+=((unsigned char) EEPROM.read(2))*256;
  		st=SERUN;
  	} 
#endif
}

// the loop routine for interactive input 
void loop() {

	if (st != SERUN) {
		printmessage(MREADY); outcr();
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
