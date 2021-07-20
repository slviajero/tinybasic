// $Id: basic.c,v 1.41 2021/07/18 21:02:00 stefan Exp stefan $
/*
	Stefan's tiny basic interpreter 

	Playing around with frugal programming. See the licence file on 
	https://github.com/slviajero/tinybasic for copyright/left.

*/

/* 
	Defines the target - ARDUINO compiles for ARDUINO
*/

#define ARDUINO

#ifdef ARDUINO
#include <EEPROM.h>
#include <avr/pgmspace.h>
#else 
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#endif

#define TRUE  1
#define FALSE 0

#define DEBUG  0
#define DEBUG1 0
#define DEBUG2 0

#define BUFSIZE 	72
#define SBUFSIZE	8
#define MEMSIZE  	1024
#define VARSIZE		26
#define STACKSIZE 	15
#define GOSUBDEPTH 	4
#define FORDEPTH 	4

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
#define GREATEREQUAL -123
#define LESSEREQUAL  -122
#define NOTEQUAL	 -121
#define TIF     -120
#define TTO     -119
#define TNEW    -118
#define TRUN    -117
#define TLET    -116
#define TFOR    -115
#define TEND    -114
#define TTHEN   -113
#define TGOTO   -112
#define TCONT   -111
#define TNEXT   -110
#define TSTEP   -109
#define TPRINT  -108
#define TINPUT  -107
#define TGOSUB  -106
#define TRETURN -105
#define TLIST	-104
#define TCLR	-103
#define TNOT    -102
#define TAND	-101
#define TOR  	-100
#define TABS 	-99
#define TRND	-98
#define TSGN	-97	
#define TPEEK	-96
#define TSQR	-95
#define TFRE	-94
#define TDUMP 	-93
#define TBREAK  -92
#define TSAVE   -91
#define TLOAD   -90
#define TREM 	-89
#define TPINM	-88
#define TDWRITE	-87
#define TDREAD	-86
#define TAWRITE	-85
#define TAREAD   -84
#define TDELAY   -83
#define TPOKE	-82
#define TGET    -81
#define TERROR  -3
#define UNKNOWN -2
#define NEWLINE -1

// the errors
#define EOUTOFMEMORY 1
#define ESTACK 		 2
#define ELINE        3
#define EUNKNOWN	 4
#define EVARIABLE	 5
#define EDIVIDE		 6
#define ERANGE 		 7
#define EFILE 		 8
#define EFUN 		 9
#define EARGS		 10

// the number of keywords, and the base index of the keywords
#define NKEYWORDS	43
#define BASEKEYWORD -123

/*
	Interpreter states 
	SRUN means running from a programm
	SINT means interactive mode
	(enum would be the right way of doing this.)
*/

#define SINT 0
#define SRUN 1

/*
	All basic keywords
*/

#ifndef ARDUINO
static char* const keyword[] = {
	"=>", "<=", "<>", "IF", "TO", "NEW","RUN",
	"LET","FOR", "END", "THEN" , "GOTO" , "CONT", 
	"NEXT", "STEP", "PRINT", "INPUT", "GOSUB", 
	"RETURN", "LIST", "CLR", "NOT", "AND", "OR" , 
	"ABS", "RND", "SGN", "PEEK", "SQR", "FRE", "DUMP",
    "BREAK", "SAVE", "LOAD", "REM",
    "PINM", "DWRITE", "DREAD", "AWRITE", "AREAD", "DELAY", 
    "POKE", "GET"};
#else
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

const char* const keyword[] PROGMEM = {
	sge, sle, sne, sif, sto, snew, srun, slet,
 	sfor, send, sthen, sgoto, scont, snext,
 	sstep, sprint, sinput, sgosub, sreturn,
 	slist, sclr, snot, sand, sor, sabs, srnd,
 	ssgn, speek, ssqr, sfre, sdump, sbreak,
 	ssave, sload, srem, spinm, sdwrite, sdread,
    sawrite, saread, sdelay, spoke, sget	
};
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

	fd is the filedescriptor for save/load

*/
static short stack[STACKSIZE];
static unsigned short sp=0; 

static char ibuffer[BUFSIZE] = "\0";
static char *bi;

static char sbuffer[SBUFSIZE];

static short vars[VARSIZE];

static char mem[MEMSIZE];
static short himem=MEMSIZE;

static struct {char var; short here; short to; short step;} forstack[FORDEPTH];
static short forsp = 0;

static struct {short here;} gosubstack[GOSUBDEPTH];
static short gosubsp = 0;

static short x, y;
static char xc, yc;

static char *ir;
static char token;
static char er;

static char st; 
static unsigned short here, here2, here3; 
static unsigned short top;

static unsigned int rd;

#ifndef ARDUINO
FILE* fd;
struct termios initialstate, newstate;
#endif

/* 
	Layer 0 functions 
*/

// variable handling - interface between memory 
// and variable storage
short getvar(char);
void setvar(char, short);
char varname(char);
void createvar(char);
void delvar(char);
void clrvars();

// error handling
void error(char);

// stack stuff
void push(short);
short pop();
void drop();
void clearst();

// mathematics
short rnd(short);
short sqr(short);

// input output
void outch(char);
char inch();
char checkch();
void outcr();
void outspc();
void outtab();
void outs(char*, short);
void outsc(char*);
void ins();
void outnumber(short);
char innumber(short*);

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

void print();
void assignment();
void xgoto();
void end();
void xif();
void input();

// helpers of gosub and for
void pushforstack();
void popforstack();
void dropforstack();
void pushgosubstack();
void popgosubstack();
void dropgosubstack();

// control commands LIST, RUN, NEW, CLR, SAVE, LOAD
void clr();
void outputtoken();
void list();
void run();
void xnew();
void save();
void load();

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

short getvar(char c){
	if (c >= 65 && c<=91)
		return vars[c-65];
	error(EVARIABLE);
	return 0;
}

void setvar(char c, short v){
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
*/ 

char* getkeyword(char t) {
#ifndef ARDUINO
	if (t < BASEKEYWORD || t > BASEKEYWORD+NKEYWORDS) {
		error(EUNKNOWN);
		return 0;
	} 
	return keyword[t-BASEKEYWORD];
#else
	strcpy_P(sbuffer, (char*)pgm_read_word(&(keyword[t-BASEKEYWORD]))); 
	return sbuffer;
#endif
}

/*
  Layer 0 - error handling

  The general error handler. The static variable er
  contains the error state. 

*/ 

void error(char e){
	er=e;
	if (e < 0) {
		outsc("Error in: \n");
		debugtoken();
		return;
	}
	switch (e) {
		case EVARIABLE:
			outsc("Var");
		case EUNKNOWN: 
			outsc("Syntax");
			return;
		case ELINE:
			outsc("Line");
			return;
		case ESTACK: 
			outsc("Stack");
			return;
		case EOUTOFMEMORY: 
			outsc("Memory");
			return;
		case ERANGE:
			outsc("Range");
			return;
		case EFILE:
			outsc("File");
			return;
		case EFUN:
			outsc("Function");
			return;		
		case EARGS:
			outsc("Arg");	
	}
	outsc(" error\n");
	return;
}

void debug(char *c){
	outsc(c); 
	debugtoken();
}

void debugn(char t){
	outsc(getkeyword(t)); outcr();
}

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

/* 

	Input and output functions.

	The interpreter is implemented to use only putchar 
	and getchar.

	These functions need to be provided by the stdio lib 
	or by any other routine like a serial terminal or keyboard 
	interface of an arduino.

*/

void outch(char c) {
#ifdef ARDUINO
	Serial.write(c);
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
	do 
		if (Serial.available()) c=Serial.read();
	while(c == 0); 
	outch(c);
	return c;
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
	if (Serial.available()) return Serial.peek(); 
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

void outtab() {
	outch('\t');
}

// output a string of length x at index ir - basic style
void outs(char *ir, short x){
	int i;
	for(i=0; i<x; i++) outch(ir[i]);
}

// output a zero terminated string at ir - c style
void outsc(char *ir){
	while (*ir != 0) outch(*ir++);
}

// reads a line from the keybord to the input buffer
void ins(){
	char c;
	bi=ibuffer;
	while(TRUE) {
		c=inch();
		if (bi-ibuffer < BUFSIZE-1) {
			if (c == '\n' || c == '\r') {
				*bi=0x00;
				break;
			} else 
				*bi++=c;
		} 
		if (c == 0x08 && bi>ibuffer) {
			bi--;
		}
	}
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

// reading a 16 bit number using the stack to collect bytes
char innumber(short *r){
	char c;
	char nd;
	short s;
	s=1;
	nd=0;
	c=inch();
	if (c == '#') {
		*r=0;
		return c;
	}	
	if (c == '-') {
		s=-1;
		c=inch();
	}
	while (TRUE){
		if (c <= '9' && c >='0' && nd<5) {
			x=c-'0';
			push(x);
			nd++;
		} 
		if (c == 0x08 && nd>0) {
			x=pop();
			nd--;
		}
		if (c == '\n' || c == '\r')
			break;
		c=inch();
	}
	*r=0;
	while(nd>0){
		*r+=pop()*s;
		s=s*10;
		nd--;
	}
	return 1;
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
	if (st == SRUN) {
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
		x=*bi-'0';
		bi++;
		while(*bi <='9' && *bi>= '0'){
			x=x*10+*bi-'0';
			bi++;
		}
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

	if (DEBUG) outsc("Scanning vars");
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

	// a variable have length 1 in this first version
	// its literal value is returned in xc
	if ( x == 1 ){
		token=VARIABLE;
		xc=*bi;
		bi++;
		if (DEBUG) debugtoken();
		return;
	}

	token=UNKNOWN;
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

void dumpmem(int i) {
	int j;
	int k=0;
	while (i>0) {
		for (j=0; j<8; j++) {
			outnumber(mem[k++]); outspc();
		}
		outcr();
		i--;
	}
	outsc("top= "); outnumber(top); outcr();
}

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

void gettoken() {
	token=mem[here++];
	switch (token) {
		case NUMBER:
		case LINENUMBER:		
			x=(unsigned char)mem[here++];
			x+=mem[here++]*256; 
			break;
		case VARIABLE:
			xc=mem[here++];
			break;
		case STRING:
			x=(unsigned char)mem[here++];
			ir=&mem[here];
			here+=x;
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
			outsc("Line ignored.\n");
			drop();
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
			outsc("Line not found \n");
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
#ifndef ARDUINO
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
	return MEMSIZE-top;
}

void factor(){
	if (DEBUG1) debug("factor");
	switch (token) {
		case NUMBER: 
			push(x);
			break;
		case VARIABLE: 
			push(getvar(xc));
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
		case TRND: 
			parsefunction(rnd);
			break;
		case TSGN: 
			parsefunction(xsgn);
			break;
		case TPEEK: 
			parsefunction(peek);
			break;
		case TSQR: 
			parsefunction(sqr);
			break;
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
	if (DEBUG2) debug("factor"); 
}

void term(){
	if (DEBUG1) debug("term"); 
	factor();

nextfactor:
	nexttoken();
	if (DEBUG) debug("nexttoken");
	if (token == '*'){
		nexttoken();
		factor();
		y=pop();
		x=pop();
		push(x*y);
		goto nextfactor;
	} else if (token == '/'){
		nexttoken();
		factor();
		y=pop();
		x=pop();
		if (y != 0)
			push(x/y);
		else {
			error(EDIVIDE);
			push(0);	
		}
		goto nextfactor;
	} else if (token == '%'){
		nexttoken();
		factor();
		y=pop();
		x=pop();
		if (y != 0)
			push(x%y);
		else {
			error(EDIVIDE);
			push(0);	
		}
		goto nextfactor;
	} 
	if (DEBUG2) debug("term"); 
}

void addexpression(){
	if (DEBUG1) debug("addexp");
	if (token != '+' && token != '-') {
		term();
	} else {
		push(0);
	}

nextterm:
	if (token == '+' ) { 
		nexttoken();
		term();
		y=pop();
		x=pop();
		push(x+y);
		goto nextterm;
	} else if (token == '-'){
		nexttoken();
		term();
		y=pop();
		x=pop();
		push(x-y);
		goto nextterm;
	}
	if (DEBUG2) debug("addexp");
}

void compexpression() {
	if (DEBUG1) debug("compexp"); 
	addexpression();
	switch (token){
		case '=':
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x == y);
			break;
		case NOTEQUAL:
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x != y);
			break;
		case '>':
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x > y);
			break;
		case '<':
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x < y);
			break;
		case LESSEREQUAL:
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x <= y);
			break;
		case GREATEREQUAL:
			nexttoken();
			compexpression();
			y=pop();
			x=pop();
			push(x >= y);
			break;
	}
	if (DEBUG2) debug("compexp");
}

void notexpression() {
	if (DEBUG1) debug("notexp");
	if (token == TNOT) {
		nexttoken();
		compexpression();
		x=pop();
		push(!x);
	} else 
		compexpression();
	if (DEBUG2) debug("notexp"); 
}

void andexpression() {
	if (DEBUG1) debug("andexp");
	notexpression();
	if (token == TAND) {
		nexttoken();
		expression();
		y=pop(); 
		x=pop(); 
		push(x && y);
	}
	if (DEBUG2) debug("andexp"); 
}

void expression(){
	if (DEBUG1) debug("exp"); 
	andexpression();
	if (token == TOR) {
		nexttoken();
		expression();
		y=pop(); 
		x=pop(); 
		push(x || y);
	} 
	if (DEBUG2) debug("exp");
}

/*

   the print command 
   PRINT expressionlist ( EOL | : )
   expressionlist :== ( string | expression ) (, ( string | expression ) )* 

*/ 
void print(){
	if (DEBUG1) debugn(TPRINT); 
	while (TRUE) {
		nexttoken();
		if (token == STRING) {
			outs(ir, x);	
			nexttoken();
		} else if (token != ',') {
			expression();
			if (er == 0) {
				outnumber(pop());
			} else {
				clearst();
				er=0;
				return ;
			}
		}
		if (token == EOL || token ==':' || token == LINENUMBER) {
			outcr();
			break;
		}
	}
}

void assignment() {
	if (DEBUG1) debugn(TLET); 
	push(xc);
	nexttoken();
	if ( token == '=') {
		nexttoken();
		expression();
		if (er == 0) {
			x=pop(); 
			y=pop();
			setvar(y,x);
		} else {
			clearst();
			er=0;
			return;
		}
	} else 
		error(token);
}

void xgoto() {
	if (DEBUG1) debugn(TGOTO); 
	int t=token;
	nexttoken();
	expression();
	if (er == 0) {
		if (t == TGOSUB) {
			if (DEBUG1) debugn(TGOSUB); 
			pushgosubstack();
		}
		x=pop();
		findline(x);
		if ( er != 0 && t == TGOSUB) {
			dropgosubstack();
		}
		nexttoken();
	} else {
		clearst();
		er=0;
		return;
	} 
}

void end(){
	st=SINT;
}

void cont(){
	st=SRUN;
	nexttoken();
}

void xif(){
	if (DEBUG1) debugn(TIF); 
	nexttoken();
	expression();
	if (er == 0) {
		x=pop();
		if (DEBUG1) { debug("boolean"); outnumber(x); outcr(); } 
		if (! x) {// on condition false skip the entire line
			do
				nexttoken();
			while (token != LINENUMBER && token !=EOL && here <= top);
		}
		if (token == TTHEN) {
			nexttoken();
			if (token == NUMBER) 
				findline(x);
		}
	} else {
		outsc("Error in if \n");
		clearst();
		er=0;
		return;
	}
}

void input(){
	if (DEBUG1) debugn(TINPUT); 
	nexttoken();
	if (token == STRING) {
		outs(ir, x);
		nexttoken();
		if (token != ',') {
			error(TINPUT);
			return;
		} else 
			nexttoken();
	}
	do {
		if (token == VARIABLE) {
			outsc("? ");
			if (innumber(&x) == '#') {
				setvar(xc, 0);
				st=SINT;
			} else {
				setvar(xc, x);
			}
		}
		nexttoken();
	} while (token == ',');
}


void pushforstack(){
	if (forsp < FORDEPTH) {
		forstack[forsp].var=xc;
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
	xc=forstack[forsp].var;
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

void pushgosubstack(){
	if (gosubsp < GOSUBDEPTH) {
		gosubstack[gosubsp].here=here;
		gosubsp++;	
		return;	
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
	here=gosubstack[gosubsp].here;
}

void dropgosubstack(){
	if (gosubsp>0) {
		gosubsp--;
	} else {
		error(ESTACK);
		return;
	} 
}


void findnext(){
	while (token != TNEXT && here < top) 
		nexttoken(); 
}

// for is implemented only in program mode not in interactive mode
void xfor(){
	if (DEBUG1) debugn(TFOR);
	nexttoken();
	if (token != VARIABLE) {
		error(token);
		return;
	}
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
	setvar(xc,x);
	if (DEBUG1) { outsc("In for: \n"); 
		outsc("Variable is : "); outch(xc); outcr();
		outsc("Value is    : "); outnumber(x); outcr();
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
	if (DEBUG1) { 
		outsc("In for: \n"); 
		outsc("Step is    : "); outnumber(y); outcr();
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
		here=ibuffer-bi;
	pushforstack();
	if (er != 0){
		return;
	}
/*
	this tests the condition and stops if it is fulfilled already from start 
	there is an apocryphal feature her STEP 0 is legal triggers an infinite loop
*/
	if ( (y > 0 && getvar(xc)>x) || (y < 0 && getvar(xc)<x ) ) { 
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
	if (DEBUG1) debugn(TBREAK);
	dropforstack();
	findnext();
	nexttoken();
}

// next is implemented in program mode and not in interactive mode
void xnext(){
	if (DEBUG1) debugn(TNEXT); 
	push(here);
	popforstack();
	// at this point xc is the variable, x the stop value and y the step
	// here is the point tp jump back
	if (y == 0) {				// next loop - we branch to <here>
		pushforstack();
		drop();
		nexttoken();
		return;
	} 
	int t=getvar(xc)+y;
	setvar(xc, t);
	if (y > 0 && t <= x) {		// next loop - we branch to <here>
		pushforstack();
		drop();
		nexttoken();
		return;
	}
	if (y < 0 && t >= x) {		// next loop - we branch to <here>
		pushforstack();
		drop();
		nexttoken();
		return;
	} 
	// last iteration completed
	here=pop();
	nexttoken();
}

void xreturn(){
	if (DEBUG1) debugn(TRETURN); 
	popgosubstack();
	nexttoken();
}

void load() {
	if (DEBUG1) debugn(TLOAD);
#ifdef ARDUINO
	x=0;
	// here comes the autorun flag code 
	x++;
	top=(unsigned char) EEPROM.read(x++);
	top+=((unsigned char) EEPROM.read(x++))*256;
	while (x < top+3){
		mem[x-3]=EEPROM.read(x);
		x++;
	}
	nexttoken();
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

void save() {
	if (DEBUG1) debugn(TSAVE); 
#ifdef ARDUINO
	if (top+3 < EEPROM.length()) {
		x=0;
		EEPROM.write(x++, 0); // currently always zero - this will be the outrun flag
		EEPROM.write(x++, top%256);
		EEPROM.write(x++, top/256);
		while (x < top+3){
			EEPROM.update(x, mem[x-3]);
			x++;
		}
		nexttoken();
	} else {
		error(EOUTOFMEMORY);
		er=0;
		nexttoken();
		return;
	}
#else 
	fd=fopen("file.bas", "w");
	if (!fd) {
		error(EFILE);
		nexttoken();
		return;
	} 
	list();
	fclose(fd);
	fd=0;
	// no nexttoken here because list has already done this
#endif
}

void rem() {
	if (DEBUG1) debugn(TREM); 
	while (token != LINENUMBER && token != EOL && here <= top)
		nexttoken(); 
}
/* 

	control commands LIST, RUN, NEW, CLR

*/

void clr() {
	clrvars();
	nexttoken();
}

void outputtoken() {
	if (token == LINENUMBER) {
		outcr(); outnumber(x); outspc();			
	} else if (token == NUMBER ){
		outnumber(x); outspc();
	} else if (token == VARIABLE){
		outch(xc); outspc();
	} else if (token == STRING) {
		outch('"'); outs(ir, x); outch('"');
	} else if (token < -3) {
		outsc(getkeyword(token)); outspc();
	} else if (token >= 32) {
		outch(token); outspc();
	} else{
		outch(token); outspc(); outnumber(token);
	} 
}

void list(){
	if (DEBUG1) debugn(TLIST);
	here=0;
	gettoken();
	while (here <= top) {
		outputtoken();
		gettoken();
	}
	outcr();
	nexttoken();
 }


void run(){
	char c;
	if (DEBUG1) debugn(TRUN);
	here=0;
	st=SRUN;
	nexttoken();
	while (here < top && st == SRUN) {	
		statement();
	}
	st=SINT;
}

void xnew(){
	top=0;
	zeroblock(0,himem);
	clrvars();
}

char termsymbol() {
	return (token == ':' || token != LINENUMBER || token != EOL );
}


/* 
	The arduino io functions.
*/

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

void poke(){
	parsetwoarguments();
	y=pop();
	x=pop();
	if (x >= 0 && x<MEMSIZE) 
		mem[x]=y;
	else {
#ifndef ARDUINO
		error(ERANGE);
#else
		if (x < 0 && -x < EEPROM.length())
			EEPROM.update(-x, y);
		else {
			error(ERANGE);
		}
#endif	
	}
}

void xget(){
	if (DEBUG1) debugn(TGET); 
	nexttoken();
	if (token != VARIABLE) {
		error(TGET);
	} else {
		if (checkch())
			setvar(xc, inch());
		nexttoken();
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
	if (DEBUG1) debug("statement\n"); 
	while (token != EOL) {
		switch(token){
			case TLIST:
				list();
				break;
			case TNEW:
				xnew();
				return;
			case TCLR:
				clr();
				break;
			case TRUN:
				run();
				return;		
			case TPRINT:
				print();
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
			case TGOSUB:
			case TGOTO:
				xgoto();	
				break;
			case TEND:
				end();
				return;
			case TCONT:
				cont();
				break;
			case TREM:
				rem();
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
			case TRETURN:
				xreturn();
				break;
			case TINPUT:
				input();
				break;
			case TDUMP:
				dumpmem(5);
				nexttoken();
				break;
			case TSAVE:
				save();
				break;
			case TLOAD:
				load();
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
				poke();
				break;
			case TGET:
				xget();
				break;
			case UNKNOWN:
				error(EUNKNOWN);
				return;
			default:
				nexttoken();
		}
#ifdef ARDUINO
		if (checkch() == '#') {st=SINT; xc=inch(); return;};  // on an Arduino entering "#" at runtime stops the program
#endif
	}
}

// the setup routine - Arduino style
void setup() {
#ifdef ARDUINO
  Serial.begin(9600); 
#else 
// to be done
#endif
  xnew();
}

// the loop routine for interactive input 
void loop() {

    outsc("Ready \n");
    ins();
    
    bi=ibuffer;
    nexttoken();

    if (token == NUMBER)
      storeline();
    else {
      st=SINT;
      statement();   
    }
}


#ifndef ARDUINO
int main(){
	setup();
	while (TRUE)
		loop();
}
#endif
