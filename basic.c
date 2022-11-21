/*
 *
 *	$Id: basic.c,v 1.139 2022/11/19 16:43:24 stefan Exp stefan $ 
 *
 *	Stefan's IoT BASIC interpreter 
 *
 * 	See the licence file on 
 *	https://github.com/slviajero/tinybasic for copyright/left.
 *    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 *	Author: Stefan Lenz, sl001@serverfabrik.de
 *
 *	The first set of definions define the target.
 *	- MINGW switches on Windows calls. 
 *	- MSDOS for MSDOS file access.
 *	- MAC doesn't need more settings here
 *	- RASPPI activates wiring code
 *	- Review hardware-*.h for settings specific Arduino hardware settings
 *	- HAS* activates or deactives features of the interpreter
 *	- the extension flags control features and code size
 *
 *	MEMSIZE sets the BASIC main memory to a fixed value,
 *		if MEMSIZE=0 a heuristic is used based on free heap
 *		size and architecture parameters
 *
 *	USEMEMINTERFACE controls the way memory is accessed. Don't change 
 *	this here. It is a parameter set by hardware-arduino.h. 
 *	This feature is experimental.
 *
 */
#undef MINGW
#undef MSDOS
#undef RASPPI

/*
	interpreter feature sets, choose one of the predefines 
  or undefine all predefines and set the features in custom settings
*/

/*
 * BASICFUL: full language set
 * BASICINTEGER: integer BASIC with full language 
 * BASICTINYWITHFLOAT: a floating point tinybasic
 * BASICMINIMAL: minimal language
 */
#define	BASICFULL
#undef	BASICINTEGER
#undef	BASICSIMPLE
#undef	BASICMINIMAL
#undef	BASICTINYWITHFLOAT

/*
 * custom settings undef all the the language sets 
 * when you def here
 */ 
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#define HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define  HASSTRINGARRAYS

/* Palo Alto plus Arduino functions */
#ifdef BASICMINIMAL
#undef HASAPPLE1
#define HASARDUINOIO
#undef HASFILEIO
#undef HASTONE
#undef HASPULSE
#undef HASSTEFANSEXT
#undef HASERRORMSG
#undef HASVT52
#undef HASFLOAT
#undef HASGRAPH
#undef HASDARTMOUTH
#undef HASDARKARTS
#undef HASIOT
#undef HASMULTIDIM
#undef HASSTRINGARRAYS
#endif

/* all features minus float and tone */
#ifdef  BASICINTEGER
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#undef  HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define HASSTRINGARRAYS
#endif

/* a simple integer basic for small systems (UNO etc) */
#ifdef  BASICSIMPLE
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#undef 	HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#undef  HASFLOAT
#undef  HASGRAPH
#define HASDARTMOUTH
#undef  HASDARKARTS
#define HASIOT
#undef  HASMULTIDIM
#undef  HASSTRINGARRAYS
#endif

/* all features activated */
#ifdef BASICFULL
#define HASAPPLE1
#define HASARDUINOIO
#define HASFILEIO
#define HASTONE
#define HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#define HASVT52
#define HASFLOAT
#define HASGRAPH
#define HASDARTMOUTH
#define HASDARKARTS
#define HASIOT
#define HASMULTIDIM
#define HASSTRINGARRAYS
#endif

/* a Tinybasic with float support */
#ifdef BASICTINYWITHFLOAT
#undef HASAPPLE1
#define HASARDUINOIO
#undef HASFILEIO
#undef HASTONE
#undef HASPULSE
#define HASSTEFANSEXT
#define HASERRORMSG
#undef HASVT52
#define HASFLOAT
#undef HASGRAPH
#undef HASDARTMOUTH
#undef HASDARKARTS
#undef HASIOT
#undef HASMULTIDIM
#undef HASSTRINGARRAYS
#endif

/*
 * Experimental features, to be tested and not sure that the add
 * real value 
 *
 * HASMSTAB: make tab more like MS TAB then Apple 1 TAB
 * HASARRAYLIMIT: make the lower limit of an variable
 */
#define HASMSTAB
#define HASARRAYLIMIT

/* 
 *	Language feature dependencies
 * 
 * Dartmouth and darkarts needs the heap which is in Apple 1
 * IoT needs strings and the heap, also Apple 1
 */
#if defined(HASDARTMOUTH) || defined(HASDARKARTS) || defined(HASIOT)
#define HASAPPLE1
#endif

#if defined(HASSTRINGARRAYS)
#define HASMULTIDIM
#endif

/* hardcoded memory size, set 0 for automatic malloc, don't redefine this beyond this point */
#define MEMSIZE 0

/* debug mode switch */
#define DEBUG 0

/*
 * the core basic language headers including some Arduino device stuff
 */
#include "basic.h"

/* 
 *	Hardware dependend definitions code are isolated in hardware-*.h
 *	Currently there are two versions
 *		hardware-arduino.h contains all platforms compiled in the Arduino IDE
 *		(ESP8266, ESP32, AVR, MEGAAVR, SAM*, RP2040)
 *		hardware-posix.h contains all platforms compiled in gcc with a POSIX OS
 *		(Mac, Raspberry, Windows/MINGW) plus rudimentary MSDOS with tc2.0. The 
 *		latter will be removed soon.
 */
#ifdef ARDUINO
#include "hardware-arduino.h"
#else 
#include "hardware-posix.h"
#endif

/* 
 *	Code hardware dependencies
 *	Removed
 */


/*
 *  Determine the possible basic memory size.
 *	using malloc causes some overhead which can be relevant on the smaller  
 *	boards. set MEMSIZE instead to a static value. In this case ballocmem 
 *	just returns the static MEMSIZE.
 * 
 * if SPIRAMINTERFACE is defined, we use the memory from a serial RAM and dont allocate 
 * it here.
 *
 */
#if MEMSIZE == 0 && !defined(SPIRAMINTERFACE)
address_t ballocmem() { 
	mem_t i = 0;

	const unsigned short memmodel[] = {
		60000,  // DUE systems, RP2040 and ESP32, all POSIX systems - set to fit in one 16 bit page
		48000,  // DUE with a bit of additional stuff,
		40000, 	// simple ESP8266 systems, MSDOS small model 
		24000,  // Arduino MK boards, SAMD, Seeduino
		12000,  // ESP systems with a lot of subsystems and VGA
		4096, 	// Arduino Nano Every, MEGA with a lot of stuff 
		2048,   // AVR with a lot of additional stuff on them
		1024, 	// UNO
		128,	// fallback, something has gone wrong
		0      
	}; 

/*  if the number type is only 2 bytes max memory is 32000
	if (sizeof(number_t) <= 2) i=2; */

/* on some platforms we know the free memory for BASIC */
	long m=freememorysize();
	if (m>maxaddr) m=maxaddr;
	if (m>0) {
		mem=(signed char*)malloc(m);
		if (mem != NULL) return m-1;
	}

/*
 * if there is no valid freememorysize() result we need 
 * to guess the size by doing malloc, MEMMODEL can be used
 * to limit the max size.
 *
 * tiny model MSDOS needs more buffers 
 * don't use the 60k 
 */
#ifdef MSDOS
	i=1;
#endif
#ifdef MEMMODEL
	i=MEMMODEL;
#endif

	do {
		mem=(mem_t*)malloc(memmodel[i]);
		if (mem != NULL) break;
		i++;
	} while (memmodel[i] != 0);
	return memmodel[i]-1;
}
#else 
address_t ballocmem(){ return MEMSIZE-1; };
#endif

/*
 *	Layer 0 function - variable handling.
 *
 *	These function access variables, 
 *	In this implementation variables are a 
 *	static array and we simply subtract 'A' 
 *	from the variable name to get the index
 *	any other way to generate an index from a 
 *	byte hash can be used just as well.
 *
 *	delvar and createvar as stubs for further 
 *	use. They are not yet used consistenty in
 *	the code.
 */

/*
 * eeprom load / save / autorun functions 
 * needed for SAVE and LOAD to an EEPROM
 * autorun is generic
 */

/* save a file to EEPROM, disabled if we use the EEPROM directly */
void esave() {
#ifndef EEPROMMEMINTERFACE
	address_t a=0;
	if (top+eheadersize < elength()) {
		a=0;
		/* EEPROM per default is 255, 0 indicates that there is a program */
		eupdate(a++, 0); 

		/* store the size of the program in byte 1,2 of the EEPROM*/
		z.a=top;
		esetnumber(a, addrsize);
		a+=addrsize;

		while (a < top+eheadersize){
			/* eupdate(a, mem[a-eheadersize]); */
			eupdate(a, memread2(a-eheadersize));
			a++;
		}
		eupdate(a++,0);
	} else {
		error(EOUTOFMEMORY);
		er=0; //oh oh! check this.
	}
/* needed on I2C EEPROM and other platforms where we buffer */
  eflush();
#endif
}

/* load a file from EEPROM, disabled if the use the EEPROM directly */
void eload() {
#ifndef EEPROMMEMINTERFACE
	address_t a=0;
	if (elength()>0 && (eread(a) == 0 || eread(a) == 1)) { // have we stored a program
		a++;

		/* how long is it? */
		egetnumber(a, addrsize);
		top=z.a;
		a+=addrsize;

		while (a < top+eheadersize){
			/* mem[a-eheadersize]=eread(a); */
			memwrite2(a-eheadersize, eread(a));
			a++;
		}
	} else { 
		/* no valid program data is stored */
		error(EEEPROM);
	}
#endif
}

/* autorun something from EEPROM or a filesystem */
char autorun() {
#if defined(ARDUINOEEPROM) || defined(ARDUINOI2CEEPROM) || ! defined(ARDUINO) 
	if (eread(0) == 1){ /* autorun from the EEPROM */
  		egetnumber(1, addrsize);
  		top=z.a;
  		st=SERUN;
  		return 1; /* EEPROM autorun overrules filesystem autorun */
	} 
#endif
#if defined(FILESYSTEMDRIVER) || ! defined(ARDUINO)
/* on a POSIX or DOS platform, we check the command line first and the autoexec */
#ifndef ARDUINO
    if (bargc > 0) {
        if (ifileopen(bargv[1])) {
          	xload(bargv[1]);
  		    st=SRUN;
            ifileclose();
            bnointafterrun=1;
            return 1;
        }
    }
#endif
	if (ifileopen("autoexec.bas")) {
  		xload("autoexec.bas");
  		st=SRUN;
		ifileclose();
		return 1;
	}
#endif
	return 0;
}

#ifdef HASAPPLE1
/*
 *	bmalloc() allocates a junk of memory for a variable on the 
 *		heap every objects is identified by name (c,d) and type t
 *		3 bytes are used here.
 */
address_t bmalloc(mem_t t, mem_t c, mem_t d, address_t l) {
	address_t vsize;     /* the length of the header */
	address_t b;

	if (DEBUG) { outsc("** bmalloc with token "); outnumber(t); outcr(); }

	/* check if the object already exists */
	if (bfind(t, c, d) != 0 ) { error(EVARIABLE); return 0; };

/* 
 *	how much space is needed
 *		3 bytes for the token and the 2 name characters
 *		numsize for every number including array length
 *		one byte for every string character
 */
	switch(t) {
  		case VARIABLE:
    		vsize=numsize+3;
    		break;
#ifndef HASMULTIDIM
			case ARRAYVAR:
				vsize=numsize*l+addrsize+3;
				break;
#else
/* test multidim implementation for n=2 */
			case ARRAYVAR:
				vsize=numsize*l+addrsize*2+3;
				break;
#endif
			case TFN:
				vsize=addrsize+2+3;
				break;
			default:
				vsize=l+addrsize+3;
	}
	
/* enough memory ?, on an EEPROM system we limit the heap to the RAM */ 
#ifndef EEPROMMEMINTERFACE
	if ((himem-top) < vsize) { error(EOUTOFMEMORY); return 0;}
#else
  if (himem-(elength()-eheadersize) < vsize) { error(EOUTOFMEMORY); return 0;}
#endif 

/* here we could create a hash, currently simplified
	 the hash is the first digit of the variable plus the token */
	b=himem;

/*
	mem[b--]=c;
	mem[b--]=d;
	mem[b--]=t;
*/
	memwrite2(b--, c);
	memwrite2(b--, d);
	memwrite2(b--, t);


/* for strings, arrays and buffers write the (maximum) length 
	  directly after the header */
	if (t == ARRAYVAR || t == STRINGVAR || t == TBUFFER) {
		b=b-addrsize+1;
		z.a=vsize-(addrsize+3);
		setnumber(b, addrsize);
		b--;
	}

/* remark on multidim, the byte length is after the header and the following 
	address_t bytes are then reserved for the first dimension */

/* reserve space for the payload */
	himem-=vsize;
	nvars++;

	return himem+1;
}

/*
 * bfind() passes back the location of the object as result
 *	the length of the object is in z.a as a side effect 
 *	rememers the last search
 *
 */
address_t bfind(mem_t t, mem_t c, mem_t d) {
	address_t b = memsize;
	mem_t t1;
	mem_t c1, d1;
	address_t i=0;

/* the bfind cache, did we ask for that object before? - 
	needed to make the string code efficient */
	if (t == bfindt && c == bfindc && d == bfindd) {
		z.a=bfindz;
		return bfinda;
	}

	while (i < nvars) { 

/*
		c1=mem[b--];
		d1=mem[b--];
		t1=mem[b--];
*/

		c1=memread2(b--);
		d1=memread2(b--);
		t1=memread2(b--);

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

		if (c1 == c && d1 == d && t1 == t) {
			bfindc=c;
			bfindd=d;
			bfindt=t;
			bfindz=z.a;
			bfinda=b+1;
			return b+1;
		}
		i++;
	}

	return 0;
}

/* finds an object and deletes the heap from this object on 
	including the object itself */
address_t bfree(mem_t t, mem_t c, mem_t d) {
	address_t b = memsize;
	mem_t t1;
	mem_t c1, d1;
	address_t i=0;

	if (DEBUG) { outsc("*** bfree called for "); outch(c); outch(d); outsc(" on heap with token "); outnumber(t); outcr(); }

	while (i < nvars) { 

/*
		c1=mem[b--];
		d1=mem[b--];
		t1=mem[b--];
*/

		c1=memread2(b--);
		d1=memread2(b--);
		t1=memread2(b--);

		if (t == t1 && c == c1 && d == d1) {

/* set the number of variables to the new value */
			nvars=i;
			if (DEBUG) { outsc("*** bfree setting nvars to "); outnumber(nvars); outcr(); }

/* clean up - this is somehow optional, one could drop this */

			if (DEBUG) { outsc("*** bfree clearing "); outnumber(himem); outspc(); outnumber(b+3); outcr(); }
			for (i=himem; i<=b+3; i++) memwrite2(i, 0);

/* now set the memory to the right address */			
			himem=b+3;

/* forget the chache !! */
			bfindc=0;
			bfindd=0;
			bfindt=0;
			bfindz=0;
			bfinda=0;

			return himem;
		}

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
		i++;
	}
	return 0;
}

/* the length of an object */
address_t blength (mem_t t, mem_t c, mem_t d) {
	if (bfind(t, c, d)) return z.a; else return 0;
}
#endif

/* ununsed so far, simple variables are created on the fly */
void createvar(mem_t c, mem_t d){
	return;
}

/* get and create a variable */
number_t getvar(mem_t c, mem_t d){
	address_t a;

	if (DEBUG) { outsc("* getvar "); outch(c); outch(d); outspc(); outcr(); }

/* the static variable array */
	if (c >= 65 && c <= 91 && d == 0) return vars[c-65];

/* the special variables */
	if ( c == '@' )
		switch (d) {
			case 'A':
				return availch();
			case 'S': 
				return ert;
			case 'I':
				return id;
			case 'O':
				return od;
			case 'C':
				if (availch()) return inch(); else return 0;
			case 'E':
				return elength()/numsize;
			case 0:
				return (himem-top)/numsize;
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

/* set and create a variable */
void setvar(mem_t c, mem_t d, number_t v){
	address_t a;

	if (DEBUG) { outsc("* setvar "); outch(c); outch(d); outspc(); outnumber(v); outcr(); }

/* the static variable array */
	if (c >= 65 && c <= 91 && d == 0) {
		vars[c-65]=v;
		return;
	}

/* the special variables */
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
	if (!(a=bfind(VARIABLE, c, d))) a=bmalloc(VARIABLE, c, d, 0);
	if (er != 0) return;

/* set the valus */
	z.i=v;
	setnumber(a, numsize);
#else 	
	error(EVARIABLE);
#endif
}

/* clr all variables */
void clrvars() {
	address_t i;
	for (i=0; i<VARSIZE; i++) vars[i]=0;
#ifdef HASAPPLE1
	nvars=0;
/*	for (i=himem; i<memsize; i++) mem[i]=0; */
	for (i=himem; i<memsize; i++) memwrite2(i, 0);
	himem=memsize;
	bfindc=bfindd=bfindt=0;
	bfinda=bfindz=0;
#endif
}

/* the BASIC memory access function */
void getnumber(address_t m, mem_t n){
	mem_t i;

	z.i=0;

	switch (n) {
		case 1:
/*			z.i=mem[m];  */
			z.i=memread2(m);
			break;
		case 2:
/*
			z.b.l=mem[m++];
			z.b.h=mem[m];
*/
			z.b.l=memread2(m++);
			z.b.h=memread2(m);
			break;
		default:
/*			for (i=0; i<n; i++) z.c[i]=mem[m++]; */
			for (i=0; i<n; i++) z.c[i]=memread2(m++);
	}
}

/* test code only for the SPI RAM code, this function goes through the 
  ro buffer to avoit rw buffer page faults */
void pgetnumber(address_t m, mem_t n){
  mem_t i;

  z.i=0;

  switch (n) {
    case 1:
      z.i=memread(m);
      break;
    case 2:
      z.b.l=memread(m++);
      z.b.h=memread(m);
      break;
    default:
      for (i=0; i<n; i++) z.c[i]=memread(m++);
  }
}

/* the eeprom memory access */
void egetnumber(address_t m, mem_t n){
	mem_t i;

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
void setnumber(address_t m, mem_t n){
	mem_t i;

	switch (n) {
		case 1:
		/*	mem[m]=z.i; */
			memwrite2(m, z.i);
			break;
		case 2: 
/*
			mem[m++]=z.b.l;
			mem[m]=z.b.h;
*/
			memwrite2(m++, z.b.l);
			memwrite2(m++, z.b.h);
			break;
		default:

 		/*	for (i=0; i<n; i++) mem[m++]=z.c[i]; */
 			for (i=0; i<n; i++) memwrite2(m++, z.c[i]);
	}
}

/* set a number at a eepromlocation */
void esetnumber(address_t m, mem_t n){
	mem_t i; 

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
address_t createarray(mem_t c, mem_t d, address_t i, address_t j) {
	address_t a, zat, at;

#ifdef HASAPPLE1
	if (DEBUG) { outsc("* create array "); outch(c); outch(d); outspc(); outnumber(i); outcr(); }
#ifndef HASMULTIDIM
	if (bfind(ARRAYVAR, c, d)) error(EVARIABLE); else return bmalloc(ARRAYVAR, c, d, i);
#else
	if (bfind(ARRAYVAR, c, d)) error(EVARIABLE); else {
		a=bmalloc(ARRAYVAR, c, d, i*j);
		zat=z.a; /* preserve z.a because it is needed on autocreate later */
		z.a=j;
		at=a+i*j*numsize; 
/*
		mem[at++]=z.b.l; // test code, assuming 16 bit address_t here, should be ported to setnumber 
		mem[at]=z.b.h;
*/
		memwrite2(at++, z.b.l);
		memwrite2(at, z.b.h);

		z.a=zat;
		return a;
	}
#endif
#endif
	return 0;
}

/* finds if an array is twodimensional and what the second dimension structure is 
	test code, should be rewritten to properly use getnumber*/
address_t getarrayseconddim(address_t a, address_t za) {
#ifdef HASMULTIDIM
	address_t zat1, zat2;
	zat1=z.a;
/*
	z.b.l=mem[a+za-2]; // test code, assuming 16 bit address_t here, should be ported to setnumber 
	z.b.h=mem[a+za-1];
*/
	z.b.l=memread2(a+za-2); // test code, assuming 16 bit address_t here, should be ported to setnumber 
	z.b.h=memread2(a+za-1);
	zat2=z.a;
	z.a=zat1;
	return zat2;
#else
	return 1;
#endif
}

/* generic array access function */
void array(mem_t m, mem_t c, mem_t d, address_t i, address_t j, number_t* v) {
	address_t a;
	address_t h;
	char e = 0;
#ifdef HASMULTIDIM
	address_t dim=1, zat;
#endif

	if (DEBUG) { outsc("* get/set array "); outch(c); outspc(); outnumber(i); outcr(); }

/* special arrays EEPROM, Display and Wang, dimension j is ignored here and not handled */
	if (c == '@') {
		switch(d) {
			case 'E': 
				h=elength()/numsize;
				a=elength()-numsize*i;
				e=1;
				break;
			case 'D': 
#if defined(DISPLAYDRIVER) && defined(DISPLAYCANSCROLL)
				if (dsp_rows == 0 || dsp_columns == 0) { return; }
				if (i<1 || i>dsp_columns*dsp_columns ) { return; }
				i--;
				a=i%dsp_columns;
				h=i/dsp_columns;
/* this should be encapsulated later into the display object */
				if (m == 's') {
					if (*v == 0) e=' '; else e=*v;
					dspprintchar(e, a, h);
					if (*v == 32) dspbuffer[h][a]=0; else dspbuffer[h][a]=*v;
				} else {
					*v=(number_t)dspbuffer[h][a];          			
				}
#endif
				return;
#if !defined(ARDUINO) || defined(ARDUINORTC)
			case 'T':
				if (m == 'g') *v=rtcget(i); 
				else if (m == 's') rtcset(i, *v);
				return;
#endif
#if defined(ARDUINO) && defined(ARDUINOSENSORS)
			case 'S':
				if (m == 'g') *v=sensorread(i, 0); 
				return;
#endif
			case 0: 
			default:
				h=(himem-top)/numsize;
				a=himem-numsize*(i)+1;
				break;
		}
	} else {
#ifdef HASAPPLE1
/* dynamically allocated arrays autocreated if needed */ 
		if ( !(a=bfind(ARRAYVAR, c, d)) ) a=createarray(c, d, ARRAYSIZEDEF, 1);
		if (er != 0) return;
#ifndef HASMULTIDIM
		h=z.a/numsize;
#else 
		h=(z.a - addrsize)/numsize;
#endif

		if (DEBUG) { outsc("** in array base address"); outnumber(a); outcr(); }

/* redudant code to getarrayseconddim */
#ifdef HASMULTIDIM
		dim=getarrayseconddim(a, z.a);
		a=a+((i-arraylimit)*dim+(j-arraylimit))*numsize;
#else 
		a=a+(i-arraylimit)*numsize;
#endif
#else
		error(EVARIABLE); 
		return;
#endif
	}

/* is the index in range */
#ifdef HASMULTIDIM
  if (DEBUG) { 
  	outsc("** in array "); 
  	outnumber(i); outspc(); 
  	outnumber(j); outspc(); 
  	outnumber(dim); outspc(); 
  	outnumber(h); outspc();
  	outnumber(a); outcr();
  }
	if ( (j < arraylimit) || (j >= dim + arraylimit) || (i < arraylimit) || (i >= h/dim + arraylimit)) { error(EORANGE); return; }
#else
	if (DEBUG) { outsc("** in array "); outnumber(i);  outspc(); outnumber(a); outcr(); }
	if ( (i < arraylimit) || (i >= h + arraylimit) ) { error(EORANGE); return; }
#endif

/* set or get the array */
	if (m == 'g') {
		if (! e) { getnumber(a, numsize); } else { egetnumber(a, numsize); }
		*v=z.i;
	} else if ( m == 's') {
		z.i=*v;
		if (! e) { setnumber(a, numsize); } else { esetnumber(a, numsize); }
	}
}

/* create a string on the heap, i is the length of the string, j the dimension of the array */
address_t createstring(char c, char d, address_t i, address_t j) {
#ifdef HASAPPLE1
	address_t a, zt;
	
	if (DEBUG) { outsc("Create string "); outch(c); outch(d); outspc(); outnumber(nvars); outcr(); }

#ifndef HASSTRINGARRAYS
/* if no string arrays are in the code, we reserve the number of bytes i and space for the index */
	if (bfind(STRINGVAR, c, d)) error(EVARIABLE); else a=bmalloc(STRINGVAR, c, d, i+strindexsize);
	if (er != 0) return 0;
	return a;
#else
/* string arrays need the number of array elements which address_ hence addresize bytes and then 
		the space for j strings */
	if (bfind(STRINGVAR, c, d)) error(EVARIABLE); else a=bmalloc(STRINGVAR, c, d, addrsize+j*(i+strindexsize));
	if (er != 0) return 0;

/* preserve z.a, this is a side effect of bmalloc and bfind */
	zt=z.a;

/* set the dimension of the array */
	z.a=j;
	setnumber(a+j*(i+strindexsize), addrsize);
	z.a=zt;

	return a;
#endif
	if (er != 0) return 0;
	return a;
#else 
	return 0;	
#endif
}

/* this is an experimental helper for @X$, generates a string  
	this is a method to insert a user defined string, e.g. from an I/O device 
	makemyxstring() can be called multiple times in the code for the same string 
	operation in BASIC, it cannot be used as a trigger for an I/O operation*/

void makemyxstring() {
	int i;
	const char text[] = "hello world";
	for(i=0; i<SBUFSIZE-1 && text[i]!=0 ; i++) sbuffer[i+1]=text[i];
	sbuffer[0]=i;
}

/* get a string at position b, the -1+stringdexsize is needed because a string index starts with 1 
 * 	in addition to the memory pointer, return the address in memory.
 *	We use a pointer to memory here instead of going through the mem interface with an integer variable
 *	This makes string code lean to compile but is awkward for systems with serial memory
 * 
 * The code may look like doing multiple heap scans for just one string as stringdim and lenstring are
 * 	called here which causes bfind to called multiple time. This is harmless as bfind caches the last 
 * 	heap address.
 * 
 * Getstring returns a pointer to the first string element in question. 
 * 
 * There are two side effects of getstring
 *	- ax has the address in memory of the string, if the string is on heap. 0 otherwise.
 *	- z.a has the number of bytes in the string payload area inculding the string length counter
 */

char* getstring(char c, char d, address_t b, address_t j) {	
	address_t k, zt, dim, maxlen;

	ax=0;
	if (DEBUG) { outsc("* get string var "); outch(c); outch(d); outspc(); outnumber(b); outcr(); }

/* direct access to the input buffer - deprectated but still there */
	if ( c == '@' && d == 0) {
		return ibuffer+b;
	}  

#ifdef HASAPPLE1
/* special strings */
    
/* the time string */
#if !defined(ARDUINO) || defined(ARDUINORTC)
	if ( c == '@' && d == 'T') {
		rtcmkstr();
		return rtcstring+1+b;
	}
#endif

/* a user definable special string in sbuffer, makemyxtring is a 
	user definable function  */
	if ( c == '@' && d == 'X' ) {
		makemyxstring();
		return sbuffer+b;
	}

    
/* the arguments string on POSIX systems */
#ifndef ARDUINO
    if ( c == '@' && d == 'A' ) {
        k=0;
        if (bargc > 1) while(k < SBUFSIZE && bargv[2][k] !=0) { sbuffer[k]=bargv[2][k]; k++; } 
        return sbuffer+b;
    }
#endif

	if ( c == '@') { error(EVARIABLE); return 0;}

/* dynamically allocated strings, create on the fly */
	if (!(ax=bfind(STRINGVAR, c, d))) ax=createstring(c, d, STRSIZEDEF, 1);

	if (DEBUG) {
		outsc("** heap address "); outnumber(ax); outcr(); 
		outsc("** byte length "); outnumber(z.a); outcr(); 
	}

	if (er != 0) return 0;

#ifndef HASSTRINGARRAYS

	if ((b < 1) || (b > z.a-strindexsize )) {
		error(EORANGE); return 0;
	}

	ax=ax+strindexsize+(b-1);
	
#else 

/* the dimension of the string array */

	zt=z.a;
	getnumber(ax+z.a-addrsize, addrsize);
	dim=z.a;

	if ((j < arraylimit) || (j >= dim + arraylimit )) {
		error(EORANGE); return 0;
	}

 	if (DEBUG) { outsc("** string dimension "); outnumber(dim); outcr(); }

/* the max length of a string */
	maxlen=(zt-addrsize)/dim-strindexsize;

	if ((b < 1) || (b > maxlen )) {
		error(EORANGE); return 0;
	}

	if (DEBUG) { outsc("** maximum string length "); outnumber(maxlen); outcr(); }
	
/* the base address of a string */
	ax=ax+(j-arraylimit)*(maxlen + strindexsize);

	if (DEBUG) { outsc("** string base address "); outnumber(ax); outcr(); }

/* get the actual length */
/*	getnumber(ax, strindexsize); */

/* the address */
	ax=ax+b-1+strindexsize;

/* leave the string data record length in z.a */
	z.a=maxlen+strindexsize;
#endif

	if (DEBUG) { outsc("** payload address address "); outnumber(ax); outcr(); }

/* return value is 0 if we have no direct memory access, the caller needs to handle the string 
	through the mem address, we return no pointer but provide a copy of the string in the first 
	string buffer*/
#ifdef USEMEMINTERFACE
	for (k=0; k<z.a && k<SPIRAMSBSIZE; k++) spistrbuf1[k]=memread2(ax+k);
	return 0;
#else
	return (char *)&mem[ax];
#endif

#else 
	return 0;
#endif
}
 
#ifdef HASAPPLE1
/* in case of a string array, this function finds the number 
	of array elements */
address_t strarraydim(char c, char d) {
address_t a, b;
#ifndef HASSTRINGARRAYS
	return 1;
#else 
	if ((a=bfind(STRINGVAR, c, d))) {
		b=z.a; /* the byte length of the objects payload */
		getnumber(a+b-addrsize, addrsize); /* the dimension of the array is the first number */
		return z.a;
	} else 
		return 1;
#endif
}

/* dimension of a string as in DIM a$(100), only needed on assign! */ 
address_t stringdim(char c, char d) {

/* input buffer, payload size is buffer -1 as the first byte is length */	
	if (c == '@') return BUFSIZE-1;

/* length of the payload from the parameters */
#ifndef HASSTRINGARRAYS
	else return blength(STRINGVAR, c, d)-strindexsize;
#else 
	if (bfind(STRINGVAR, c, d)) {
		return (z.a-addrsize)/strarraydim(c, d)-strindexsize;
	} else 
		return 0;
#endif

}

/* the length of a string as in LEN(A$) */
address_t lenstring(char c, char d, address_t j){
	char* b;
	address_t a;

/* the input buffer, length is first byte */
	if (c == '@' && d == 0) return ibuffer[0];

/* the time */
#if !defined(ARDUINO) || defined(ARDUINORTC) 
	if (c == '@' && d == 'T') {
		rtcmkstr();
		return rtcstring[1];
	}
#endif

/* a user definable special string in sbuffer 
	makemyxstring has to be called here because 
	in the code sometimes getstring is called first 
	and sometimes lenstring */
	if ( c == '@' && d == 'X' ) {
		makemyxstring();
		return sbuffer[0];
	}
    
/* the arguments string on POSIX systems */
#ifndef ARDUINO
    if ( c == '@' && d == 'A' ) {
        a=0;
        if (bargc > 1) while(a < SBUFSIZE && bargv[2][a] !=0) a++;
        return a;
    }
#endif
    
/* locate the string */
	a=bfind(STRINGVAR, c, d);
	if (er != 0) return 0;

/* string does not yet exist, return length 0 */
	if (a == 0) return 0;

/* get the string length from memory */
#ifndef HASSTRINGARRAYS
	getnumber(a, strindexsize);
	return z.a;
#else 
	a=a+(stringdim(c, d)+strindexsize)*(j-arraylimit);
	getnumber(a, strindexsize);
	return z.a;
#endif
}

/* set the length of a string */
void setstringlength(char c, char d, address_t l, address_t j) {
	address_t a, zt; 
	if (DEBUG) {
		outsc("** setstringlength "); 
		outch(c); outch(d); 
		outspc(); outnumber(l); outspc(); outnumber(j);
		outcr();
	} 

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

/* error handling at this point wrong and potentially not needed */
/*
	if (l < z.a) {
		z.a=l;
		setnumber(a, strindexsize);
	} else
		error(EORANGE);
}
*/

/* multiple calls of bfind here is harmless as bfind caches  */ 
	a=a+(stringdim(c, d)+strindexsize)*(j-arraylimit);

	if (DEBUG) { outsc("**  setstringlength writing to "); outnumber(a); outsc(" value "); outnumber(l); outcr(); }

	z.a=l;
	setnumber(a, strindexsize);
}

#endif

/*
 *  Layer 0 - keyword handling - PROGMEM logic goes here
 *		getkeyword(), getmessage(), and getokenvalue() are 
 *		the only access to the keyword array in the code.  
 *
 *	Same for messages and errors
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
	if (i >= sizeof(message) || i < 0) return 0;
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
 *	Layer 0 - error handling
 *
 * The general error handler. The static variable er
 * contains the error state. 
 * 
 * debugtoken() writes a token for debug
 * bdebug() is the general debug message function
 *
 * Strategy: the error() function writes the message and then 
 * clears the stack. All calling functions must check er and 
 * return after funtion calls with no further messages etc.
 * reseterror() sets the error state to normal and end the 
 * run loop.
*/ 
void error(mem_t e){
	er=e;
/* set input and output device back to default */
	iodefaults();
/* find the line number */
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
	/* outsc("** at "); outnumber(here); */
	outcr();
	clearst();
	clrforstack();
	clrgosubstack();
}

void reseterror() {
	er=0;
	here=0;
	st=SINT;
}

void debugtoken(){
	outsc("* "); /* outnumber(here); outsc(" * "); */

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

void bdebug(const char *c){
	outch('*');
	outspc();
	outsc(c); 
	debugtoken();
	outcr();
}

/*
 *	Arithmetic and runtime operations are mostly done
 *	on a stack of number_t.
 *
 *	push(), pop(), clearst() handle the stack
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

/*
 * clear the cursor for the READ/DATA mechanism
 */
void clrdata() {
#ifdef HASDARTMOUTH
	data=0;
#endif
}

/* 
 *	Stack handling for gosub and for
 */
void pushforstack(){
	index_t i, j;

	if (DEBUG) { outsc("** forsp and here in pushforstack "); outnumber(forsp); outspc(); outnumber(here); outcr(); }
	
/* before pushing into the for stack we check is an
	 old for exists - this is on reentering a for loop */
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

/* 
 *	Input and output functions.
 * 
 * ioinit(): called at setup to initialize what ever io is needed
 * outch(): prints one ascii character 
 * inch(): gets one character (and waits for it)
 * checkch(): checks for one character (non blocking)
 * ins(): reads an entire line (uses inch except for pioserial)
 *
 */
void ioinit() {

/* a standalone system runs from keyboard and display */
#ifdef STANDALONE
	idd = IKEYBOARD;
	odd = ODSP;
#endif

/* this is only for RASPBERRY - wiring has to be started explicitly */
	wiringbegin();

/* all serial protocolls, ttl channels, SPI and Wire */
	serialbegin();
#ifdef ARDUINOPRT
	prtbegin();
#endif
#ifdef ARDUINOSPI
	spibegin();
#endif
#ifdef ARDUINOWIRE
	wirebegin();
#endif

/* filesystems and networks */
  fsbegin(1);
#ifdef ARDUINOMQTT
	netbegin();  
	mqttbegin();
#endif

/* the displays */
	kbdbegin();
#ifdef DISPLAYDRIVER
	dspbegin();
#endif
#ifdef ARDUINOVGA
	vgabegin();  /* mind this - the fablib code is special here */
#endif
#ifdef ARDUINOSENSORS
	sensorbegin();
#endif

/* the eeprom dummy */
	ebegin();

/* activate the iodefaults */
	iodefaults();
}

void iodefaults() {
	od=odd;
	id=idd;
	form=0;
}

/* 
 *	Layer 0 - The generic IO code 
 *
 * inch() reads one character from the stream, mostly blocking
 * checkch() reads one character from the stream, unblocking, a peek(), 
 *	inmplemented inconsistently
 * availch() checks availablibity in the stream
 * inb() a block read function for serial interfacing, developed for 
 * 	AT message receiving 
 */ 

/* the generic inch code reading one character from a stream */
char inch() {
	switch(id) {
		case ISERIAL:
			return serialread();		
#ifdef FILESYSTEMDRIVER
		case IFILE:
			return fileread();
#endif
#ifdef ARDUINOWIRE
		case IWIRE:
			ins(sbuffer, 1);
			if (sbuffer[0]>0) return sbuffer[1]; 
			else return 0;
#endif
#ifdef ARDUINORF24
/* radio is not character oriented, this is only added to make GET work
		or single byte payloads, radio, like file is treated nonblocking here */
		case IRADIO:
			radioins(sbuffer, SBUFSIZE-1);
			if (sbuffer[0]>0) return sbuffer[1]; 
			else return 0;
#endif
#ifdef ARDUINOMQTT
    case IMQTT:
    	return mqttinch();
#endif
#ifdef ARDUINOPRT
		case ISERIAL1:
			return prtread();
#endif				
#if defined(HASKEYBOARD) || defined(HASKEYPAD)				
		case IKEYBOARD:
			return kbdread();
#endif
	}
	return 0;
}

/* checking on a character in the stream */
char checkch(){
	switch (id) {
		case ISERIAL:
			return serialcheckch();
#ifdef FILESYSTEMDRIVER
		case IFILE:
			return fileavailable();
#endif
#ifdef ARDUINORF24
		case IRADIO:
			return radio.available();
#endif
#ifdef ARDUINOMQTT
			case IMQTT:
				if (mqtt_messagelength>0) return mqtt_buffer[0];
#endif   
#ifdef ARDUINOWIRE
			case IWIRE:
				return 0;
#endif
#ifdef ARDUINOPRT
			case ISERIAL1:
				return prtcheckch(); 
#endif
		case IKEYBOARD:
#if defined(HASKEYBOARD)	|| defined(HASKEYPAD)
			return kbdcheckch();
#endif
			break;
	}
	return 0;
}

/* character availability */
short availch(){
	switch (id) {
    case ISERIAL:
      return serialavailable(); 
#ifdef FILESYSTEMDRIVER
   	case IFILE:
    	return fileavailable();
#endif
#ifdef ARDUINORF24
    case IRADIO:
    	return radioavailable();
#endif    		
#ifdef ARDUINOMQTT
    case IMQTT:
    	return mqtt_messagelength;
#endif    		
#ifdef ARDUINOWIRE
    case IWIRE:
    	return wireavailable();
#endif
#ifdef ARDUINOPRT
    case ISERIAL1:
      return prtavailable();
#endif
    case IKEYBOARD:
#if defined(HASKEYBOARD) || defined(HASKEYPAD)  
      return kbdavailable();
#endif
	    break;
	}
	return 0;
}

#ifdef ARDUINOPRT
/* 
 *	the block mode reader for esp and sensor modules 
 * 	on a serial interface, it tries to read as many 
 * 	characters as possible into a buffer
 *	blockmode = 1 reads once availch() bytes
 *	blockmode > 1 implements a timeout mechanism and tries 
 *		to read until blockmode milliseconds have expired
 *		this is needed for esps and other sensors without
 *		flow control and volatile timing to receive more 
 *		then 64 bytes 
 */
void inb(char *b, short nb) {
	long m;
	index_t i = 0;
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
#endif

/* 
 *	ins() is the generic reader into a string, by default 
 *	it works in line mode and ends reading after newline
 *
 *	the first element of the buffer is the lower byte of the length
 *
 * 	this is corrected later in xinput, z.a has to be set as 
 *	a side effect
 *
 *	for streams providing entire strings as an input the 
 *	respective string method is called
 *
 *	all other streams are read using consins() for character by character
 *	input until a terminal character is reached
 */
void ins(char *b, address_t nb) {
  	switch(id) {
#ifdef ARDUINOWIRE
  		case IWIRE:
			wireins(b, nb);
			break;
#endif
#ifdef ARDUINOMQTT
		case IMQTT:
			mqttins(b, nb);	
			break;	
#endif
#ifdef ARDUINORF24
  		case IRADIO:
  			radioins(b, nb);
  			break;
#endif
  		default:
#ifdef ARDUINOPRT
/* blockmode only implemented for ISERIAL1 right now */
  			if (blockmode > 0 && id == ISERIAL1 ) inb(b, nb); else 
#endif
  			consins(b, nb);
  	}  
}

/*
 * outch() outputs one character to a stream
 * block oriented i/o like in radio not implemented here
 */
void outch(char c) {
	switch(od) {
		case OSERIAL:
			serialwrite(c);
			break;
#ifdef FILESYSTEMDRIVER
		case OFILE:
			filewrite(c);
			break;
#endif
#ifdef ARDUINOPRT
		case OPRT:
			prtwrite(c);
			break;
#endif
#ifdef ARDUINOVGA
		case ODSP: 
			vgawrite(c);
			break;
#else
#ifdef DISPLAYDRIVER
		case ODSP: 
			dspwrite(c);
			break;
#endif
#endif
#ifdef ARDUINOMQTT
		case OMQTT:
			mqttouts(&c, 1); /* buffering for the PRINT command */
			break;
#endif
		default:
			break;
	}
	byield(); /* yield after every character for ESP8266 */
}

/* send a newline */
void outcr() {
#ifdef ARDUINOPRT
	if (sendcr) outch('\r');
#endif
	outch('\n');
} 

/* send a space */
void outspc() {
	outch(' ');
}

/*
 *	outs() outputs a string of length x at index ir - basic style
 *	default is a character by character operation, block 
 *	oriented write needs special functions
 */
void outs(char *ir, address_t l){
	address_t i;

	switch (od) {
#ifdef ARDUINORF24
		case ORADIO:
			radioouts(ir, l);
			break;
#endif
#ifdef ARDUINOWIRE
		case OWIRE:
			wireouts(ir, l);
			break;
#endif
#ifdef ARDUINOMQTT
		case OMQTT:
			mqttouts(ir, l);
			break;
#endif
		default:
			for(i=0; i<l; i++) outch(ir[i]);
	}
	byield(); /* triggers yield on ESP8266 */
}

/* output a zero terminated string at ir - c style */
void outsc(const char *c){
	while (*c != 0) outch(*c++);
}

/* output a zero terminated string in a formated box padding spaces 
		needed for catalog output */
void outscf(const char *c, short f){
	short i = 0;
  
	while (*c != 0) { outch(*c++); i++; }
	if (f > i) {
		f=f-i;
		while (f--) outspc();
	}
}

/* 
 *	reading a number from a char buffer 
 *	maximum number of digits is adjusted to 16
 *	ugly here, testcode when introducting 
 *	number_t was only 16 bit before
 */
address_t parsenumber(char *c, number_t *r) {
	address_t nd = 0;

	*r=0;
	while (*c >= '0' && *c <= '9' && *c != 0) {
		*r=*r*10+*c++-'0';
		nd++;
		if (nd == SBUFSIZE) break;
	}
	return nd;
}


#ifdef HASFLOAT
/* a poor man's atof implementation with character count */
address_t parsenumber2(char *c, number_t *r) {
	address_t nd = 0;
	index_t i;
	number_t fraction = 0;
	number_t exponent = 0;
	char nexp = 0;

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
		if (*c == '-') {c++; nd++; nexp=1;};
		i=parsenumber(c, &exponent);
		nd+=i;
		while ((--exponent)>=0) if (nexp) *r=*r/10; else *r=*r*10;		
	}

	return nd;
}
#endif

/*
 *	convert a number to a string
 *	the use of long v here is a hack related to HASFLOAT 
 *	unfinished here and needs to be improved
 */
address_t writenumber(char *c, number_t vi){
	address_t nd = 0;	
	long v;
	index_t i,j;
	mem_t s = 1;
	char c1;

/* not really needed any more */
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
/*
 * this is for floats, going back to library functions
 * for a starter.
 */
address_t writenumber2(char *c, number_t vi) {
	index_t i;
	number_t f;
	index_t exponent = 0; 
	mem_t eflag=0;

/* pseudo integers are displayed as integer
		zero trapped here */
	f=floor(vi);
	if (f == vi && fabs(vi) < maxnum) {
		return writenumber(c, vi);
	}

/* floats are displayed using the libraries */
#ifndef ARDUINO
	return sprintf(c, "%g", vi);
#else
	f=vi;
	while (fabs(f)<1.0)   { f=f*10; exponent--; }
 	while (fabs(f)>=10.0-0.00001) { f=f/10; exponent++; }

/* small numbers */
	if (exponent > -2 && exponent < 7) { 
		dtostrf(vi, 0, 5, c);
	} else {
		dtostrf(f, 0, 5, c);
		eflag=1;
	}
	
/* remove trailing zeros */
	for (i=0; (i < SBUFSIZE && c[i] !=0 ); i++);
	i--;
	while (c[i] == '0' && i>1) {i--;}
	i++;

/* add the exponent */
	if (eflag) {
		c[i++]='E';
		i+=writenumber(c+i, exponent);
	}

	c[i]=0;
	return i;

#endif
}
#endif

/*
 * innumber() uses sbuffer as a char buffer to get a number input 
 */
char innumber(number_t *r) {
	index_t i = 1;
	index_t s = 1;

again:
	*r=0;
	sbuffer[1]=0;
	ins(sbuffer, SBUFSIZE);
	while (i < SBUFSIZE) {
		if (sbuffer[i] == ' ' || sbuffer[i] == '\t') i++;
		if (sbuffer[i] == BREAKCHAR) return BREAKCHAR;
		if (sbuffer[i] == 0) { ert=1; return 1; }
		if (sbuffer[i] == '-') {
			s=-1;
			i++;
		}
#ifndef HASFLOAT
		if (sbuffer[i] >= '0' && sbuffer[i] <= '9') {
			(void) parsenumber(&sbuffer[i], r);
#else
		if ((sbuffer[i] >= '0' && sbuffer[i] <= '9') || sbuffer[i] == '.') {
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

/* prints a number */
void outnumber(number_t n){
	address_t nd;

#ifndef HASFLOAT
	nd=writenumber(sbuffer, n);
#else
	nd=writenumber2(sbuffer, n);
#endif 
	outs(sbuffer, nd);

/* number formats in Palo Alto style */
	while (nd < form) {outspc(); nd++; };
}

/* 
 *	Layer 1 functions - providing data into the global variable and 
 *	changing the interpreter state
 */

/*
 *	Lexical analyser - tokenizes the input line.
 *
 *	nexttoken() increments the input buffer index bi and delivers values in the global 
 *		variable token, with arguments in the accumulator x and the index register ir
 *		xc is used in the routine. 
 *
 *	xc, ir and x change values in nexttoken and deliver the result to the calling
 *	function.
 *
 *	bi and ibuffer should not be changed or used for any other function in 
 *	interactive node as they contain the state of nexttoken(). In run mode 
 *	bi and ibuffer are not used as the program is fully tokenized in mem.
 */ 

/* skip whitespaces */
void whitespaces(){
	while (*bi == ' ' || *bi == '\t') bi++;
}

/* the token stream */
void nexttoken() {
  
/* RUN mode vs. INT mode, in RUN mode we read from mem via gettoken() */
	if (st == SRUN || st == SERUN) {
		gettoken();
		if (debuglevel>1) debugtoken();
		return;
	}

/* after change in buffer logic the first byte is reserved for the length */
	if (bi == ibuffer) bi++;

/* remove whitespaces outside strings */
	whitespaces();

/* end of line token */
	if (*bi == '\0') { 
		token=EOL; 
		if (DEBUG) debugtoken();
		return; 
	}

/* unsigned numbers, value returned in x */
#ifndef HASFLOAT
	if (*bi <='9' && *bi >= '0'){		
		bi+=parsenumber(bi, &x);
#else
	if ((*bi <='9' && *bi >= '0') || *bi == '.') {	
		bi+=parsenumber2(bi, &x);
#endif
		token=NUMBER;
		if (DEBUG) debugtoken();
		return;
	}

/* strings between " " or " EOL, value returned in ir */
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

/* single character operators are their own tokens */
	if (*bi == '+' || *bi == '-' || *bi == '*' || *bi == '/' || *bi == '%'  ||
		*bi == '\\' || *bi == ':' || *bi == ',' || *bi == '(' || *bi == ')' ) { 
			token=*bi; 
			bi++; 
			if (DEBUG) debugtoken();
			return; 
	}  

/*
 *	relations
 *	single character relations are their own token
 *	>=, =<, =<, =>, <> are tokenized
 */
	if (*bi == '=') {
		bi++;
		whitespaces();
		if (*bi == '>') {
			token=GREATEREQUAL;
			bi++;
		} else if (*bi == '<') {
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
 *	keyworks and variables
 *
 *	isolate a word, bi points to the beginning, x is the length of the word
 *	ir points to the end of the word after isolating.
 *	@ is a letter here to make the special @ arrays possible 
 */
	x=0;
	ir=bi;
	while (-1) {
/* toupper code */
		if (*ir >= 'a' && *ir <= 'z') {
			*ir-=32;
			ir++;
			x++;
		} else if (*ir >= '@' && *ir <= 'Z') { 
			ir++;
			x++;
		} else {
			break;
		}
	}

/* 
 *	ir is reused here to implement string compares
 *	scanning the keyword array. 
 *	Once a keyword is detected the input buffer is advanced 
 *	by its length, and the token value is returned. 
 *
 *	keywords are an array of null terminated strings.
 */
	yc=0;
	while (gettokenvalue(yc) != 0) {
		ir=getkeyword(yc);
		xc=0;
		while (*(ir+xc) != 0) {
			if (*(ir+xc) != *(bi+xc)) {
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
 *	a variable has length 1 in the first version
 *	its literal value is returned in xc
 *	in addition to this, a number or $ is accepted as 
 *	second character like in Apple 1 BASIC
 */
/*
	if ( x == 1 || (x == 2 && *bi == '@') ) {
		token=VARIABLE;
		xc=*bi;
		yc=0;
		bi++;
		if (*bi >= '0' && *bi <= '9') { 
			yc=*bi;
			bi++;
		} 
		if (xc == '@' && x == 2) {
			yc=*bi;
			bi++;
		}
*/
/* general two letter variables test code */
	if ( x == 1 || x == 2 ) {
		token=VARIABLE;
		xc=*bi;
		yc=0;
		bi++;
		if ((*bi >= '0' && *bi <= '9') || (*bi >= 'A' && *bi <= 'Z')) { 
			yc=*bi;
			bi++;
		} 
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


/* other single characters are parsed and stored */
	token=*bi;
	bi++;
	if (DEBUG) debugtoken();
	return;
}

/* 
 * Layer 1 - program editor 
 *
 *	Editing the program, the structure of a line is 
 *	LINENUMBER linenumber(2 or more bytes) token(n bytes)
 *
 * store* stores something to memory 
 * get* retrieves information
 *
 *	No matter how long number_t is in the C implementation
 *	we pack into bytes, this is clumsy but portable
 *	the store and get commands all operate on here 
 *	and advance it
 *
 *	storetoken() operates on the variable top. 
 *	We always append at the end and then sort. 
 *
 *	gettoken() operate on the variable here 
 *	which will also be the key variable in run mode.
 *
 *	tokens are stored including their payload.
 *
 *	This group of functions changes global states and
 *	cannot be called at program runtime with saving
 *	the relevant global variable to the stack.
 */

/* 
 *  check if we still have memory left, on RAM only systems we make
 *  sure that we don't hit himem. On systems with EEPROM program storage we make
 *  sure that we stay within the EEPROM range.
 */
char nomemory(number_t b){
#ifndef EEPROMMEMINTERFACE
	if (top >= himem-b) return 1; else return 0;
#else
  if (top >= elength()-eheadersize-b) return 1; else return 0;
#endif
}

/* store a token - check free memory before changing anything */
void storetoken() {
	index_t i=x;
	switch (token) {
		case LINENUMBER:
			if ( nomemory(addrsize+1) ) break;
/*			mem[top++]=token;	*/
			memwrite2(top++, token);
			z.a=x;
			setnumber(top, addrsize);
			top+=addrsize;
			return;	
		case NUMBER:
			if ( nomemory(numsize+1) ) break;
/*			mem[top++]=token;	*/
			memwrite2(top++, token);
			z.i=x;
			setnumber(top, numsize);
			top+=numsize;
			return;
		case ARRAYVAR:
		case VARIABLE:
		case STRINGVAR:
			if ( nomemory(3) ) break;
/*
			mem[top++]=token;
			mem[top++]=xc;
			mem[top++]=yc;
*/
			memwrite2(top++, token);
			memwrite2(top++, xc);
			memwrite2(top++, yc);
			return;
		case STRING:
			if ( nomemory(x+2) ) break;
/*
			mem[top++]=token;
			mem[top++]=i;
			while (i > 0) {
				mem[top++] = *ir++;
				i--;
			}
*/
			memwrite2(top++, token);
			memwrite2(top++, i);
			while (i > 0) {
				memwrite2(top++, *ir++);
				i--;
			}	
			return;
		default:
			if ( nomemory(1) ) break;
/*
			mem[top++]=token;
*/
			memwrite2(top++, token);
			return;
	}
	error(EOUTOFMEMORY);
} 


/* 
 * wrappers around mem access in genereal
 *
 * memread is used only in the token stream, it reads from a stream
 * read only. If run is done from eeprom then bytes are taken from this 
 * stream, this would also be the place to implement direct run from 
 * another device like a file system or embedded programs on flash
 *
 * memread2 and memwrite2 always go to ram. 
 *
 * currently only the SPIRAM interface is implemented. This is 
 * handled in hardware-arduino.h.
 *
 */
#ifndef USEMEMINTERFACE
mem_t memread(address_t a) {
	if (st != SERUN) {
		return mem[a];
	} else {
		return eread(a+eheadersize);
	}
}

mem_t memread2(address_t a) { return mem[a]; }

void memwrite2(address_t a, mem_t c) { mem[a]=c; }
#else 
#ifdef SPIRAMINTERFACE
mem_t memread(address_t a) {
	if (st != SERUN) {
		/* return spiramrawread(a); */
    return spiram_robufferread(a);
	} else {
		return eread(a+eheadersize);
	}
}

mem_t memread2(address_t a) {
	/* return spiramrawread(a); */
  return spiram_rwbufferread(a);
}

void memwrite2(address_t a, mem_t c) {
	/* spiramrawwrite(a, c); */
  spiram_rwbufferwrite(a, c);
}
#else
#ifdef EEPROMMEMINTERFACE
mem_t memread(address_t a) {
  if (a < elength()-eheadersize) return eread(a+eheadersize); else return mem[a-(elength()-eheadersize)];
}

mem_t memread2(address_t a) { return memread(a); }

void memwrite2(address_t a, mem_t c) { 
   if (a < elength()-eheadersize) eupdate(a+eheadersize, c); else mem[a-(elength()-eheadersize)]=c;
}
#endif
#endif
#endif


/* get a token from memory */
void gettoken() {
	int i;

/* if we have reached the end of the program, EOL is always returned
		we don't rely on mem having a trailing EOL */
	if (here >= top) {
		token=EOL;
		return;
	}

	token=memread(here++);
	switch (token) {
		case LINENUMBER:
#ifdef USEMEMINTERFACE
			if (st != SERUN) pgetnumber(here, addrsize); else egetnumber(here+eheadersize, addrsize);
#else
			if (st != SERUN) getnumber(here, addrsize); else egetnumber(here+eheadersize, addrsize);
#endif
			x=z.a;
			here+=addrsize;
			break;
		case NUMBER:	
#ifdef USEMEMINTERFACE
			if (st !=SERUN) pgetnumber(here, numsize); else egetnumber(here+eheadersize, numsize);
#else
			if (st !=SERUN) getnumber(here, numsize); else egetnumber(here+eheadersize, numsize);
#endif
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
			x=(unsigned char)memread(here++);	
/* if we run interactive or from mem, pass back the mem location 
 * of the string constant
 * if we run from EEPROM or SPI ram and cannot simply pass a pointer 
 * we (ab)use the input buffer which is not needed here, this limits
 * the string constant length to the length of the input buffer
 */
#if defined(USEMEMINTERFACE)
			for(i=0; i<x; i++) spistrbuf1[i]=memread(here+i);
			ir=spistrbuf1;
#else
			if (st == SERUN) { 					
				for(i=0; i<x; i++) ibuffer[i]=memread(here+i);	
				ir=ibuffer;
			} else {
				ir=(char*)&mem[here]; 
			}
#endif
			here+=x;	
		}
}

/* goto the first line of a program */
void firstline() {
	if (top == 0) {
		x=0;
		return;
	}
	here=0;
	gettoken();
}

/* goto the next line, search forward */
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

/* 
 * the experimental line cache mechanism, useful for large codes 
 * not fully tested, super primitive so far
 * addlinecache does not test if the line already exist because it 
 * assumes that findline calls it only if a new line is to be stored
 */
#if defined(LINECACHESIZE) && LINECACHESIZE>0
const unsigned char linecachedepth = LINECACHESIZE;
typedef struct {address_t l; address_t h;} linecacheentry;
linecacheentry linecache[LINECACHESIZE];
unsigned char linecachehere = 0;

void clrlinecache() {
	unsigned char i;
	for(i=0; i<linecachedepth; i++) linecache[i].l=linecache[i].h=0;
	linecachehere=0;
}

void addlinecache(address_t l, address_t h) {
	linecache[linecachehere].l=l; 
	linecache[linecachehere].h=h;
	linecachehere=(linecachehere+1)%linecachedepth;
}

address_t findinlinecache(address_t l){
	unsigned char i;
	for(i=0; i<linecachedepth && linecache[i].l != 0; i++) {
		if (linecache[i].l == l) return linecache[i].h;
	}
	return 0;
}
#else
void clrlinecache() {}
void addlinecache(address_t l, address_t h) {}
address_t findinlinecache(address_t l){ return 0; }
#endif


/* find a line, look in cache then search from the beginning */
void findline(address_t l) {
	address_t a;
/* we know it already, here to advance */
	if ((a=findinlinecache(l))) { 
		here=a; 
		token=LINENUMBER;
		x=l; 
		return;
	}

/* we need to search */
	here=0;
	while (here < top) {
		gettoken();
		if (token == LINENUMBER && x == l ) {
/* now that we know we cache */
			addlinecache(l, here);
			return;
		}
	}
	error(ELINE);
}

/* finds the line of a location */
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
		if (here >= h) break;
		gettoken();
	}
	here=here2;
	if (token == LINENUMBER)
		return l1;
	else 
		return l;
}

/*
 *	Move a block of storage beginng at b ending at e
 *	to destination d. No error handling here!!
 */
void moveblock(address_t b, address_t l, address_t d){
	address_t i;

/* removed outsc("** Moving block: "); outnumber(b); outspc(); outnumber(l); outspc(); outnumber(d); outcr(); */
	if (d+l > himem) {
		error(EOUTOFMEMORY);
		return;
	}
	
	if (l<1) 
		return;

	if (b < d)
		for (i=l; i>0; i--)
      /* mem[d+i-1]=mem[b+i-1]; */
			memwrite2(d+i-1, memread2(b+i-1));
	else 
		for (i=0; i<l; i++) 
	/*		mem[d+i]=mem[b+i]; */
			memwrite2(d+i, memread2(b+i));

/* removed outsc("** Done moving /n"); */
}

/* zero a block of memory */
void zeroblock(address_t b, address_t l){
	address_t i;

	if (b+l > himem) {
		error(EOUTOFMEMORY);
		return;
	}
	if (l<1) return;

	/* for (i=0; i<l+1; i++) mem[b+i]=0; */
	for (i=0; i<l+1; i++) memwrite2(b+i, 0);
}

/*
 *	Line editor: 
 *
 *	stage 1: no matter what the line number is - store at the top
 *   	remember the location in here.
 *	stage 2: see if it is only an empty line - try to delete this line
 *	stage 3: calculate lengthes and free memory and make room at the 
 *		appropriate place
 *	stage 4: copy to the right place 
 *
 *	Very fragile code. 
 *
 *	zeroblock statements commented out after EOL code was fixed
 */
#ifdef DEBUG
/* diagnosis function */
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
	const index_t lnlength=addrsize+1;
	index_t linelength;
	number_t newline; 
	address_t here2, here3; 
	address_t t1, t2;

/* zero is an illegal line number */
	if (x == 0) {
		error(ELINE);
		return;
	}

/* the data pointers becomes invalid once the code has been changed */
	clrdata();

/* line cache is invalid on line storage */
	clrlinecache();

/*
 *	stage 1: append the line at the end of the memory,
 *	remember the line number on the stack and the old top in here
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

	x=t1;									/* recall the line number */
	linelength=top-here;	/* calculate the number of stored bytes */

/* 
 *	stage 2: check if only a linenumber stored - then delete this line
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
 *	stage 3, a nontrivial line with linenumber x is to be stored
 *	try to find it first by walking through all lines 
 */
	else {	
		y=x;
		here2=here;
		here=lnlength;
		nextline();
/* there is no nextline after the first line, we are done */		
		if (x == 0) return;
/* go back to the beginning */
		here=0; 
		here2=0;
		while (here < top) {
			here3=here2;
			here2=here;
			nextline();
			if (x > y) break;
		}

/* 
 *	at this point y contains the number of the line to be inserted
 *	x contains the number of the first line with a higher line number
 *	or 0 if the line is to be inserted at the end
 *	here points to the following line and here2 points to the previous line
 */
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
		if (x == y) {		/* the line already exists and has to be replaced */
			here2=t2;  		/* this is the line we are dealing with */
			here=t1;   		/* this is the next line */
			y=here-here2;	/* the length of the line as it is  */
			if (linelength == y) {     /* no change in line length */
				moveblock(top-linelength, linelength, here2);
				top=top-linelength;
			} else if (linelength > y) { /* the new line is longer than the old one */
				moveblock(here, top-here, here+linelength-y);
				here=here+linelength-y;
				top=top+linelength-y;
				moveblock(top-linelength, linelength, here2);
				top=top-linelength;
			} else {					/* the new line is short than the old one */
				moveblock(top-linelength, linelength, here2);
				top=top-linelength;
				moveblock(here, top-here, here2+linelength);
				top=top-y+linelength;
			}
		} else {         /* the line has to be inserted in between */
			here=t1;
			moveblock(here, top-here, here+linelength);
			moveblock(top, linelength, here);
		}
	}
}

/* 
 * Layer 1 - the code in this section calculates an expression
 * with a recursive descent algorithm 
 *
 * all function use the stack to pass values back. We use the 
 * Backus-Naur form of basic from here https://rosettacode.org/wiki/BNF_Grammar
 * implementing a C style logical expression model
 */

/* the terminal symbol - ELSE is one too */
char termsymbol() {
	return ( token == LINENUMBER ||  token == ':' || token == EOL || token == TELSE);
  /* return ( token == LINENUMBER ||  token == ':' || token == EOL); */
}

/* a little helpers - one token expect */ 
char expect(mem_t t, mem_t e) {
	nexttoken();
	if (token != t) {error(e); return 0; } else return 1;
}

/* a little helpers - expression expect */
char expectexpr() {
	nexttoken();
	expression();
	if (er != 0) return 0; else return 1;
}

/* parses a list of expression, this may be recursive! */
void parsearguments() {
	short argsl;

/* begin counting */
	argsl=0; 

/* having 0 args at the end of a command is legal */
	if (!termsymbol()) {

/* list of expressions separated by commas */
		do {
			expression();
			if (er != 0) break;
			argsl++;
			if (token != ',') break;
			nexttoken();
		} while (1);
	}

/* because of the recursion ... */
	args=argsl;
}


/* expect exactly n arguments */
void parsenarguments(char n) {
	parsearguments();
	if (args != n) error(EARGS);
}

/* counts and parses the number of arguments given in brakets */
void parsesubscripts() {
	args=0;
	if (token != '(') return; /* zero arguments is legal here */
	nexttoken();
#ifdef HASMULTIDIM
	if (token == ')') {
		args=-1;
		return; /* () is interpreted as no argument at all -> needed for string arrays */
	}
#endif
	parsearguments();
	if (er != 0) return; 
	if (token != ')') {error(EARGS); return; } /* we return with ) as a last token on success */
}


/* parse a function argument ae is the number of 
	expected expressions in the argument list */
void parsefunction(void (*f)(), short ae){
	nexttoken();
	parsesubscripts();
	if (er != 0) return;
	if (args == ae) f(); else error(EARGS);
}

/* helper function in the recursive decent parser */
void parseoperator(void (*f)()) {
	nexttoken();
	f();
	if (er !=0 ) return;
	y=pop();
	x=pop();
}

#ifdef HASAPPLE1
/* substring evaluation, mind the rewinding here - a bit of a hack 
 * 	this is one of the few places in the code where the token stream 
 *  cannot be interpreted forward i.e. without looking back. Reason is 
 * 	an inconsistency the the way expression() is implemented
 */
void parsesubstring() {
	char xc1, yc1; 
	address_t h1; /* remember the here */
	char* bi1;		/* or the postion in the input buffer */
	mem_t a1; 		/* some info on args remembered as well for multidim */
	address_t j; 	/* the index of a string array */

/* remember the string name */
	xc1=xc;
	yc1=yc;

/* Remember the token before the first parsesubscript */
  if (st == SINT) bi1=bi; else h1=here; 

	nexttoken();
	parsesubscripts();
	if (er != 0) return; 

#ifndef HASSTRINGARRAYS

/* the stack has - the first index, the second index */
	switch(args) {
		case 2: 
			break;
		case 1:
			push(lenstring(xc1, yc1, arraylimit));
			break;
		case 0: 
/* rewind the token if there were no braces to be in sync	*/
			if (st == SINT) bi=bi1; else here=h1; 
/* no rewind in the () construct */			
		case -1:
			args=0;		
			push(1);
			push(lenstring(xc1, yc1, arraylimit));	
			break;
	}

#else 
/* for a string array the situation is more complicated as we 
 		need to parse the argument completely including the possible subscript */

/* how many args has the first parsesubsscript scanned, if none, we are done
	rewind and set the parametes*/

/* the stack has - the first string index, the second string index, the array index */	
	if ( args == 0 ) {

		if (st == SINT) bi=bi1; else here=h1; 

		j=arraylimit; 
		push(1);
		push(lenstring(xc1, yc1, j));	
		push(j);

/* if more then zero or an empty (), we need a second parsesubscript */
	} else {

		if (st == SINT) bi1=bi; 
		else h1=here;

		a1=args;

		nexttoken();
		parsesubscripts();

		switch (args) {
			case 1:
				j=pop();
				break;
			case 0:
				j=arraylimit;
				if (st == SINT) bi=bi1; else here=h1;
				break;
			default:
				error(EARGS);
				return;
		}	

/* which part of the string */
		switch(a1) {
			case 2: 
				break;
			case 1:
				push(lenstring(xc1, yc1, j));
				break;
			case 0: 	
			case -1:	
				push(1);
				push(lenstring(xc1, yc1, j));	
				break;
		}

/* and which index element */
		push(j);

	}
#endif

}
#endif

/* 
 * ABS absolute value 
 */
void xabs(){
	if ((x=pop())<0) { x=-x; }
	push(x);
}

/*
 * SGN evaluates the sign
 */
void xsgn(){
	x=pop();
	if (x>0) x=1;
	if (x<0) x=-1;
	push(x);
}

/*
 * PEEK on an arduino, negative values of peek address 
 * the EEPROM range -1 .. -1024 on an UNO
 */
void xpeek(){
	address_t amax;

	x=pop();

/* this is a hack again, 16 bit numbers can't peek big addresses */
	if ((long) memsize > (long) maxnum) amax=(address_t) maxnum; else amax=memsize;

	if (x >= 0 && x<amax) 
		/* push(mem[(unsigned int) x]); */
		push(memread2((address_t) x));
	else if (x < 0 && -x <= elength())
		push(eread(-x-1));
	else {
		error(EORANGE);
		return;
	}
}

/*
 * MAP Arduino map function
 */ 
void xmap() {
	long v, in_min, in_max, out_min, out_max;
	out_max=pop();
	out_min=pop();
	in_max=pop();
	in_min=pop();
	v=pop();
	push((v - in_min) * (out_max - out_min) / (in_max - in_min) + out_min);
}

/*
 * RND very basic random number generator with constant seed.
 */
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
/*
 * SQR - a very simple approximate square root formula 
 *	for integers, for floats we use the library
 */
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
/*
 * POW(X, N) evaluates powers, replaces ^ 
 */ 
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

/* 
 * stringvalue() evaluates a string value, 0 if there is no string
 * ir2 has the string location, the stack has the length
 * ax the memory byte address, x is set to the lower index which is a nasty
 * side effect
 */
char stringvalue() {
	mem_t xcl, ycl;
	address_t k;
	// int ix, iy;

	if (DEBUG) outsc("** entering stringvalue \n");

	if (token == STRING) {
		ir2=ir;
		push(x);
#ifdef USEMEMINTERFACE
    for(k=0; k<x && x<SPIRAMSBSIZE; k++ ) spistrbuf1[k]=ir[k];
    ir2=spistrbuf1;
#endif
#ifdef HASAPPLE1
	} else if (token == STRINGVAR) {
		xcl=xc;
		ycl=yc;
		parsesubstring();
		if (er != 0) return 0;
#ifdef HASSTRINGARRAYS
		k=pop();
#else 
		k=arraylimit;
#endif
#ifdef HASFLOAT
		y=floor(pop());
		x=floor(pop());
#else
		y=pop();
		x=pop();
#endif
		ir2=getstring(xcl, ycl, x, k);
/* if the memory interface is active spistrbuf1 has the string */
#ifdef USEMEMINTERFACE
		if (ir2 == 0) ir2=spistrbuf1;
#endif
		if (y-x+1 > 0) push(y-x+1); else push(0);
	  if (DEBUG) { outsc("** in stringvalue, length "); outnumber(y-x+1); outsc(" from "); outnumber(x); outspc(); outnumber(y); outcr(); }
		xc=xcl;
		yc=ycl;
	} else if (token == TSTR) {	
		nexttoken();
		if ( token != '(') { error(EARGS); return 0; }
		nexttoken();
		expression();
		if (er != 0) return 0;
#ifdef HASFLOAT
		push(writenumber2(sbuffer, pop()));
#else
		push(writenumber(sbuffer, pop()));
#endif
		ir2=sbuffer;
		x=1;
		if (er != 0) return 0;
		if (token != ')') {error(EARGS);return 0;	}
#endif
	} else {
		return 0;
	}
	return 1;
}


/*
 * (numerical) evaluation of a string expression, used for 
 * comparison and for string rightvalues as numbers
 * the token rewind here is needed as streval is called in 
 * factor - no factor function should nexttoken
 */
void streval(){
	char *irl;
	address_t xl, x;
	mem_t t;
	address_t h1;
	char* b1;
	index_t k;

	if (!stringvalue()) {
		error(EUNKNOWN);
		return;
	} 
	if (er != 0) return;
	irl=ir2;
	xl=pop();

/* with the mem interface -> copy, irl now point to buffer2 */ 
#ifdef USEMEMINTERFACE
	for(k=0; k<SPIRAMSBSIZE && k<xl; k++) spistrbuf2[k]=irl[k];
	irl=spistrbuf2;
#endif

/* get ready for rewind. */
	if (st != SINT)
		h1=here; 
	else 
		b1=bi;

	t=token;
	nexttoken();

	if (token != '=' && token != NOTEQUAL) {
/* rewind one token if not comparison	 */
		if (st != SINT)
			here=h1; 
		else 
			bi=b1;
		token=t;
		if (xl == 0) push(0); else push(irl[0]); // a zero string length evaluate to 0
		return; 
	}

/* questionable !! */
	t=token;
	nexttoken(); 
	//debugtoken();

	if (!stringvalue()){
		error(EUNKNOWN);
		return;
	} 
	x=pop();
	if (er != 0) return;

	if (x != xl) goto neq;
	for (x=0; x < xl; x++) if (irl[x] != ir2[x]) goto neq;

	if (t == '=') push(1); else push(0);
	return;
neq:
	if (t == '=') push(0); else push(1);
	return;
}

#ifdef HASFLOAT
/* 
 * floating point arithmetic 
 * SIN, COS, TAN, ATAN, LOG, EXP, INT
 * INT is always there
 */
void xsin() { push(sin(pop())); }
void xcos() { push(cos(pop())); }
void xtan() { push(tan(pop())); }
void xatan() { push(atan(pop())); }
void xlog() { push(log(pop())); }
void xexp() { push(exp(pop())); }
void xint() { push(floor(pop())); }
#else 
void xint() {}
#endif

/*
 * Recursive expression parser functions 
 *
 * factor(); term(); addexpression(); compexpression();
 * notexpression(); andexpression(); expression() 
 *
 * doing
 *
 * functions, numbers; *, /, %; +, -; =, <>, =>, <=;
 * NOT; AND; OR   
 *
 */

/*
 *	factor() - contrary to all other function
 *	nothing here should end with a new token - this is handled 
 *	in factors calling function
 * 
 * evaluates constants, variables and all functions
 */
void factor(){
	if (DEBUG) bdebug("factor\n");
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
			parsesubscripts();
			if (er != 0 ) return;
#ifndef HASMULTIDIM
			if (args != 1) { error(EARGS); return; }	
			x=pop();
			xc=pop();
			yc=pop();
			array('g', xc, yc, x, arraylimit, &y);	
#else
			switch(args) {
				case 1:
					x=pop();
					y=arraylimit;
					break;
				case 2:
					y=pop();
					x=pop();
					break;
				default:
					error(EARGS); 
					return;
			}
			xc=pop();
			yc=pop();
			array('g', xc, yc, x, y, &y);
#endif
			push(y); 
			break;
		case '(':
			nexttoken();
			expression();
			if (er != 0 ) return;
			if (token != ')') { error(EARGS); return; } 
			break;
/* Palo Alto BASIC functions */
		case TABS: 
			parsefunction(xabs, 1);
			break;
		case TRND: 
			parsefunction(rnd, 1);
			break;
		case TSIZE:
			push(himem-top);
			break;
/* Apple 1 BASIC functions */
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
			if (!stringvalue()) {
#ifdef HASDARKARTS
				expression();
				if (er != 0) return;
				z.a=pop();
				push(blength(TBUFFER, z.a%256, z.a/256));
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
		case TOPEN:
			parsefunction(xfopen, 1);
			break;
		case TSENSOR:
			parsefunction(xfsensor, 2);
			break;
		case TVAL:
			nexttoken();
			if (token != '(') { error(EARGS); return; }
			nexttoken();
			if (!stringvalue()) { error(EUNKNOWN); return; }
			if (er != 0) return;
/* not super clean - handling of terminal symbol dirty
		stringtobuffer needed !! */
			while(*ir2==' ' || *ir2=='\t') ir2++;
			if(*ir2=='-') { y=-1; ir2++;} else y=1;
#ifdef HASFLOAT
			if (parsenumber2(ir2, &x) == 0) ert=1;	
#else 
			if (parsenumber(ir2, &x) == 0) ert=1;
#endif			
			(void) pop();
			push(x*y);
			nexttoken();
			if (token != ')') {
				error(EARGS);
				return;	
			}
			break;
		case TINSTR:
			nexttoken();
			if (token != '(') { error(EARGS); return; }
			nexttoken();
			expression();
			if (er != 0) return;
			if (token != ',') { error(EARGS); return; }
			nexttoken();
			if (!stringvalue()) { error(EUNKNOWN); return; }
			y=pop();
			xc=pop();
			for (z.a=1; z.a<=y; z.a++) {if ( ir2[z.a-1] == xc ) break; }
			if (z.a > y ) z.a=0; 
			push(z.a);
			nexttoken();
			if (token != ')') {
				error(EARGS);
				return;	
			}
			break;
#endif
		case THIMEM:
			push(himem);
			break;
/* Apple 1 string compare code */
		case STRING:
		case STRINGVAR:
			streval();
			if (er != 0 ) return;
			break;
#endif
/*  Stefan's tinybasic additions */
#ifdef HASSTEFANSEXT
		case TSQR: 
			parsefunction(sqr, 1);
			break;
		case TMAP: 
			parsefunction(xmap, 5);
			break;
		case TUSR:
			parsefunction(xusr, 2);
			break;
		case TPOW:
			parsefunction(xpow, 2);
			break;
#endif
/* Arduino I/O */
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
#if defined(ARDUINO) && defined(A0)
			push(A0);
#else 
			push(0);
#endif			
			break;
		case TLED:
#ifdef LED_BUILTIN
			push(LED_BUILTIN);
#else
			push(0);
#endif
			break;
#endif
/* mathematical functions in case we have float */
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
/* int is always present to make programs compatible */
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
/* this is a version of FIND operating in the BUFFER space 
			parsefunction(xfind, 1); */
/* FIND even more apocryphal - operating in the variable space */
			xfind2();
			break;
#endif
#ifdef HASIOT
		case TNETSTAT:
			x=0;
			if (netconnected()) x=1;
			if (mqttstate() == 0) x+=2;
			push(x);
			break;
#endif

/* unknown function */
		default:
			error(EUNKNOWN);
			return;
	}
}

/*
 *	term() evaluates multiplication, division and mod
 */
void term(){
	if (DEBUG) bdebug("term\n"); 
	factor();
	if (er != 0) return;

nextfactor:
	nexttoken();
	if (DEBUG) bdebug("in term\n");
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
	if (DEBUG) bdebug("leaving term\n");
}

/* add and subtract */
void addexpression(){
	if (DEBUG) bdebug("addexp\n");
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

/* comparisions */
void compexpression() {
	if (DEBUG) bdebug("compexp\n"); 
	addexpression();
	if (er != 0) return;
	switch (token){
		case '=':
			parseoperator(compexpression);
			if (er != 0) return;
			push(x == y ? -1 : 0);
			break;
		case NOTEQUAL:
			parseoperator(compexpression);
			if (er != 0) return;
			push(x != y ? -1 : 0);
			break;
		case '>':
			parseoperator(compexpression);
			if (er != 0) return;
			push(x > y ? -1 : 0);
			break;
		case '<':
			parseoperator(compexpression);
			if (er != 0) return;
			push(x < y ? -1 : 0);
			break;
		case LESSEREQUAL:
			parseoperator(compexpression);
			if (er != 0) return;
			push(x <= y ? -1 : 0);
			break;
		case GREATEREQUAL:
			parseoperator(compexpression);
			if (er != 0) return;
			push(x >= y ? -1 : 0);
			break;
	}
}

#ifdef HASAPPLE1
/* boolean NOT */
void notexpression() {
	if (DEBUG) bdebug("notexp\n");
	if (token == TNOT) {
		nexttoken();
		expression();
		if (er != 0) return;
		push(~(int)pop());
	} else 
		compexpression();
}

/* boolean AND and at the same time bitwise */
void andexpression() {
	if (DEBUG) bdebug("andexp\n");
	notexpression();
	if (er != 0) return;
	if (token == TAND) {
		parseoperator(expression);
		if (er != 0) return;
		/* push(x && y); */
		push((int)x & (int)y);
	} 
}

/* expression function and boolean OR at the same time bitwise !*/
void expression(){
	if (DEBUG) bdebug("exp\n"); 
	andexpression();
	if (er != 0) return;
	if (token == TOR) {
		parseoperator(expression);
		if (er != 0) return;
		/* push(x || y); */
		push((int)x | (int)y);
	}  
}
#else 

/* expression function simplified */
void expression(){
	if (DEBUG) bdebug("exp\n"); 
	compexpression();
	if (er != 0) return;
	if (token == TOR) {
		parseoperator(expression);
		if (er != 0) return;
		/* push(x || y); */
		push((int)x | (int)y);
	}  
}
#endif


/* 
 * Layer 2 - The commands and their helpers
 *    
 * Palo Alto BASIC languge set - PRINT, LET, INPUT, GOTO, GOSUB, RETURN,
 *    	IF, FOR, TO, NEXT, STEP, BREAK, STOP, END, LIST, NEW, RUN, REM
 *    	BREAK is not Palo ALto but fits here, eEND is identical to STOP.
 */

/*
 *	PRINT command 
 */
void xprint(){
	char semicolon = 0;
	char oldod;
	char modifier = 0;

	form=0;
	oldod=od;

	nexttoken();

processsymbol:

	if (termsymbol()) {
		if (! semicolon) outcr();
		/* nexttoken(); */
		od=oldod;
		form=0;
		return;
	}
	semicolon=0;

	if (stringvalue()) {
		if (er != 0) return;
 		outs(ir2, pop());
 		nexttoken();
		goto separators;
	}

/* modifiers of the print statement */
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

	if (token != ',' && token != ';') {
		expression();
		if (er != 0) return;
		outnumber(pop());
	}

separators:
	if (token == ',')  {
		if (!modifier) outspc(); 
		nexttoken();	
	}
	if (token == ';') {
		semicolon=1;
		nexttoken();
	}
	modifier=0;

	goto processsymbol;
}

/* 
 *	LET assigment code for various lefthand and righthand side. 
 *
 *
 *	lefthandside is a helper function for reuse in other 
 *	commands. It determines the address the value is to be 
 *	assigned to and whether the assignment target is a 
 *	"pure" i.e. subscriptless string expression
 *
 *	assignnumber assigns a number to a given lefthandside
 *
 * in lefthandside the type of the object is determined and 
 * possible subscripts are parsed
 * 
 * Variables have no subscripts. The arguments are unchanged.
 * Arrays may have two subscript which will go to i, j
 * Strings may have a string subscript, going to i and an array subscript 
 * going to j
 * Strings without a subscript i.e. pure strings, set the ps flag
 * 
 */
void lefthandside(address_t* i, address_t* i2, address_t* j, mem_t* ps) {
	switch (token) {
		case VARIABLE:
			nexttoken();
			break;
		case ARRAYVAR:
			nexttoken();
			parsesubscripts();
			nexttoken();
			if (er != 0) return;
			switch(args) {
				case 1:
					*i=pop();
					*j=arraylimit;
					break;
				case 2:
					*j=pop();
					*i=pop();
					break;
				default:
					error(EARGS);
					return;
			}
			break;
#ifdef HASAPPLE1
		case STRINGVAR:
#ifndef HASSTRINGARRAYS
			*j=arraylimit;
			nexttoken();
			parsesubscripts();
			if (er != 0) return;
			switch(args) {
				case 0:
					*i=1;
					*ps=1;
					break;
				case 1:
					*ps=0;
					nexttoken();
					*i=pop();
					break;
				case 2:
					*ps=0;
					nexttoken();
					*i2=pop();
					*i=pop();
					break; 
				default:
					error(EARGS);
					return;
			}
#else 
			*j=arraylimit;
			nexttoken();
			parsesubscripts();
			if (er != 0) return;
			switch(args) {
				case -1:
					nexttoken();
				case 0:
					*i=1;
					*ps=1;
					break;
				case 1:
					*ps=0;
					nexttoken();
					*i=pop();
					break;
				case 2:
					*ps=0;
					nexttoken();
					*i2=pop();
					*i=pop();
					break;
				default:
					error(EARGS);
					return;
			}
/* we deal with a string array */ 
			if (token == '(') {
				parsesubscripts();
				if (er != 0) return;
				switch(args) {
					case 1:
						*j=pop();
						nexttoken();
						break;
					default:
						error(EARGS);
						return;
				}
			}
#endif
			break;
#endif
		default:
			error(EUNKNOWN);
			return;
	}
}

void assignnumber(signed char t, char xcl, char ycl, address_t i, address_t j, char ps) {
	switch (t) {
		case VARIABLE:
			x=pop();
			setvar(xcl, ycl, x);
			break;
		case ARRAYVAR: 
			x=pop();	
			array('s', xcl, ycl, i, j, &x);
			break;
#ifdef HASAPPLE1
		case STRINGVAR:
			ir=getstring(xcl, ycl, i, j);
			if (er != 0) return;
#ifndef USEMEMINTERFACE
			ir[0]=pop();
#else 
			if (ir == 0) memwrite2(ax, pop());
#endif
			if (ps)
				setstringlength(xcl, ycl, 1, j);
			else 
				if (lenstring(xcl, ycl, j) < i && i <= stringdim(xcl, ycl)) setstringlength(xcl, ycl, i, j);
			break;
#endif
	}
}


/*
 *	LET - the core assigment function, this is different from other BASICs
 */
void assignment() {
	mem_t t;  /* remember the left hand side token until the end of the statement, type of the lhs */
	mem_t ps=1;  /* also remember if the left hand side is a pure string of something with an index */
	mem_t xcl, ycl; /* to preserve the left hand side variable names */
	address_t i=1; /* and the beginning of the destination string */
	address_t i2=0; /* and the end of the destination string */  
	address_t j=arraylimit; /* the second dimension of the array */
	address_t lensource, lendest, newlength, strdim, copybytes;
	mem_t s;
	index_t k;

/* this code evaluates the left hand side */
	ycl=yc;
	xcl=xc;
	t=token;

	lefthandside(&i, &i2, &j, &ps);
	if (er != 0) return;

	if (DEBUG) {
		outsc("** in assignment lefthandside with ");
		outnumber(i); outspc();
		outnumber(j); outspc();
		outnumber(ps); outcr();
	}

/* the assignment part */
	if (token != '=') {
		error(EUNKNOWN);
		return;
	}
	nexttoken();

/* here comes the code for the right hand side */
	switch (t) {
/* the lefthandside is a scalar, evaluate the righthandside as a number */
		case VARIABLE:
		case ARRAYVAR: 
			expression();
			if (er != 0) return;
			assignnumber(t, xcl, ycl, i, j, ps);
			break;
#ifdef HASAPPLE1
/* the lefthandside is a string, try evaluate the righthandside as a stringvalue */
		case STRINGVAR: 
			s=stringvalue();
			if (er != 0) return;

/* and then as an expression */
			if (!s) {
				expression();
				if (er != 0) return;
				assignnumber(t, xcl, ycl, i, j, ps);
				break;
			}

/* this is the string righthandside code - how long is the rightandside */
			lensource=pop();

/* the destination address of the lefthandside, on an SPI RAM system, we 
	 save the source in the second string buffer and let ir2 point to it
	 this is needed to avoid the overwrite from the second getstring
	 then reuse much of the old code */
#ifdef USEMEMINTERFACE
			for(k=0; k<SPIRAMSBSIZE; k++) spistrbuf2[k]=ir2[k];
			ir2=spistrbuf2;
#endif			
			ir=getstring(xcl, ycl, i, j);
			if (er != 0) return;

/* the length of the original string */
			lendest=lenstring(xcl, ycl, j);

/* the dimension i.e. the maximum length of the string */
			strdim=stringdim(xcl, ycl);

			if (DEBUG) {
				outsc("* assigment stringcode "); outch(xcl); outch(ycl); outcr();
				outsc("** assignment source string length "); outnumber(lensource); outcr();
				outsc("** assignment dest string length "); outnumber(lendest); outcr();
				outsc("** assignment old string length "); outnumber(lenstring(xcl, ycl, 1)); outcr();
				outsc("** assignment dest string dimension "); outnumber(stringdim(xcl, ycl)); outcr();
			};

/* does the source string fit into the destination if we have no destination second index*/
			if ((i2 == 0) && ((i+lensource-1) > strdim)) { error(EORANGE); return; };

/* if we have a second index, is it in range */
			if((i2 !=0) && i2>strdim) { error(EORANGE); return; };

/* caculate the number of bytes we truely want to copy */
			if (i2 > 0) copybytes=((i2-i+1) > lensource) ? lensource : (i2-i+1); else copybytes=lensource;

/* this code is needed to make sure we can copy one string to the same string 
	without overwriting stuff, we go either left to right or backwards */
#ifndef USEMEMINTERFACE
			if (x > i) 
				for (k=0; k<copybytes; k++) ir[k]=ir2[k];
			else
				for (k=copybytes-1; k>=0; k--) ir[k]=ir2[k]; 

#else
/* on an SPIRAM system we need to go through the mem interface 
	for write, if ir is zero i.e. is not a valid memory location */
			if (ir != 0) {
				if (x > i) 
					for (k=0; k<copybytes; k++) ir[k]=ir2[k];
				else
					for (k=copybytes-1; k>=0; k--) ir[k]=ir2[k]; 
			} else {
				for (k=0; k<copybytes; k++) memwrite2(ax+k, ir2[k]);
			}

#endif

/* 
 * classical Apple 1 behaviour is string truncation in substring logic, with
 * two index destination string we follow another route. We extend the string 
 * for the number of copied bytes
 */
			if (i2 == 0) {
				newlength = i+lensource-1;	
			} else {
				if (i+copybytes > lendest) newlength=i+copybytes-1; else newlength=lendest;
			} 
		
			setstringlength(xcl, ycl, newlength, j);
			nexttoken();
			break;
#endif
	}

/* nexttoken(); - the expression code does this already - bug as here for a long time */
}

/*
 *	INPUT ["string",] variable [,["string",] variable]* 
 */
void showprompt() {
	outsc("? ");
}

void xinput(){
	mem_t oldid = id;
	mem_t prompt = 1;
	address_t l;
	number_t xv;
	mem_t xcl, ycl;
	address_t k;

	nexttoken();

/* modifiers of the input statement (stream) */
	if (token == '&') {
		if(!expectexpr()) return;
		oldid=id;
		id=pop();
		if (id != ISERIAL || id !=IKEYBOARD) prompt=0;
		if (token != ',') {
			error(EUNKNOWN);
			return;
		} else 
			nexttoken();
	}
/* unlink print, form can appear only once in input after the
		stream, it controls character count in wire */
#ifdef ARDUINOWIRE
	if (token == '#') {
		if(!expectexpr()) return;
		form=pop();
		if (token != ',') {
			error(EUNKNOWN);
			return;
		} else 
			nexttoken();
	}
#endif

nextstring:
	if (token == STRING && id != IFILE) {
		prompt=0;   
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
		if (prompt) showprompt();
		if (innumber(&xv) == BREAKCHAR) {
			st=SINT;
			token=EOL;
			goto resetinput;
		} else {
			setvar(xc, yc, xv);
		}
	} 

/* we don't use the lefthandside call here because the input loop token handling
	is different */
#ifndef HASMULTIDIM
	if (token == ARRAYVAR) {
		xcl=xc; 
		ycl=yc;
		nexttoken();
		parsesubscripts();
		if (er != 0 ) return;
		if (args != 1) {
			error(EARGS);
			return;
		}

		if (prompt) showprompt();
		if (innumber(&xv) == BREAKCHAR) {
			st=SINT;
			token=EOL;
			goto resetinput;
		} else {
			array('s', xcl, ycl, pop(), arraylimit, &xv);
		}
	}
#else
	if (token == ARRAYVAR) {
		xcl=xc; 
		ycl=yc;
		nexttoken();
		parsesubscripts();
		if (er != 0 ) return;
		switch(args) {
			case 1:
				x=pop();
				y=arraylimit; 
				break;
			case 2:
				y=pop();
				x=pop();
				break;
			default:
				error(EARGS);
				return;
		}

		if (prompt) showprompt();
		if (innumber(&xv) == BREAKCHAR) {
			st=SINT;
			token=EOL;
			goto resetinput;
		} else {
			array('s', xcl, ycl, x, y, &xv);
		}
	}
#endif

#ifdef HASAPPLE1
/* strings are not passed through the input buffer but inputed directly 
    in the string memory location, ir after getstring points to the first data byte */
  if (token == STRINGVAR) {
    ir=getstring(xc, yc, 1, arraylimit); 
    if (prompt) showprompt();
    l=stringdim(xc, yc);
/* the form parameter in WIRE can be used to set the expected number of bytes */
    if (id == IWIRE && form != 0 && form < l) l=form; 
#ifndef USEMEMINTERFACE   
    ins(ir-1, l);
/* this is the length information correction for large strings, ins
    stored the string length in z.a as a side effect */
    if (xc != '@' && strindexsize == 2) { 
      *(ir-2)=z.b.l;
      *(ir-1)=z.b.h;
    } 
#else
    if (ir == 0) {
      ins(spistrbuf1, l);
      for(int k=0; k<SPIRAMSBSIZE && k<l; k++) memwrite2(ax+k, spistrbuf1[k+1]);
      memwrite2(ax-2, z.b.l);
      memwrite2(ax-1, z.b.h);
    } else {
      ins(ir-1, l);
/* this is the length information correction for large strings, ins
    stored the string length in z.a as a side effect */
      if (xc != '@' && strindexsize == 2) { 
        *(ir-2)=z.b.l;
        *(ir-1)=z.b.h;
      } 
    }
#endif
  }
#endif

	nexttoken();
	if (token == ',' || token == ';') {
		nexttoken();
		goto nextstring;
	}

	if (!termsymbol()) {
		error(EUNKNOWN);
	}

resetinput:
	id=oldid;
	form=0;
}

/*
 *	GOTO, GOSUB, RETURN and their helpers
 *
 *	GOTO and GOSUB function for a simple one statement goto
 */
void xgoto() {
	mem_t t=token;

	if (!expectexpr()) return;
	if (t == TGOSUB) pushgosubstack();
	if (er != 0) return;

	x=pop();

	if (DEBUG) { outsc("** goto/gosub evaluated line number "); outnumber(x); outcr(); }
	findline((address_t) x);
	if (er != 0) return;
	if (DEBUG) { outsc("** goto/gosub branches to "); outnumber(here); outcr(); }

/* goto in interactive mode switched to RUN mode
		no clearing of variables and stacks */
	if (st == SINT) st=SRUN;

	nexttoken();
}

/*
 *	RETURN retrieves here from the gosub stack
 */
void xreturn(){ 
	popgosubstack();
	if (er != 0) return;
	nexttoken();
}


/* 
 *	IF statement together with THEN 
 * 		ELSE not implemented properly
 */
void xif() {
	
	if (!expectexpr()) return;
	x=pop();
	if (DEBUG) { outnumber(x); outcr(); } 

/* on condition false skip the entire line and all : until a potential ELSE */
	if (!x)  {
		while(token != LINENUMBER && token != EOL && token != TELSE) nexttoken();

/* if we have ELSE at this point we want to execute this part of the line as the condition 
		was false, isolated ELSE is GOTO */
#ifdef HASSTEFANSEXT
/* look if ELSE is at the next line */
		if (token == LINENUMBER) nexttoken();
/* now process ELSE */
		if (token == TELSE) {
			nexttoken();
			if (token == NUMBER) {
				findline((address_t) x);
				return;	
			} 
		} 	
#endif		
	}	

/* a THEN is interpreted as simple one statement goto	if it is followed by a line number*/
#ifdef HASAPPLE1
	if (token == TTHEN) {
		nexttoken();
		if (token == NUMBER) {
			findline((address_t) x);
		}
	} 
#endif
}

/* if else is encountered in the statement line, the rest of the code is skipped 
 		as else code execution is triggered in the xif function */
#ifdef HASSTEFANSEXT
void xelse() {
	while(token != LINENUMBER && token != EOL) nexttoken();
}
#endif

/* 
 *	FOR, NEXT and the apocryphal BREAK
 *
 * find the NEXT token or the end of the program
 */ 
void findnextcmd(){
	while (1) {			
		if (token == TNEXT) {
	    if (fnc == 0) return;
	    else fnc--;
		}
	  if (token == TFOR) fnc++;
/* no NEXT found - different for interactive and program mode */
	  if (st == SRUN || st == SERUN) {
	  	if (here >= top) {
	    	error(TFOR);
	    	return;
	   	}
	  } else {
	  	if (bi-ibuffer > BUFSIZE) {
	  		error(TFOR);
	    	return;
	  	}
	  }
		nexttoken(); 
	}
}

/*
 *	FOR variable [= expression [to expression]] [STEP expression]
 *	for stores the variable, the increment and the boudary on the 
 *	for stack. Changing steps and boundaries during the execution 
 *	of a loop has no effect.
 *
 *	This is different from many other BASICS as FOR can be used 
 *	as an open loop with no boundary
 */
void xfor(){
	mem_t xcl, ycl;
	number_t b=1;
	number_t e=maxnum;
	number_t s=1;
	
/* there has to be a variable */
	if (!expect(VARIABLE, EUNKNOWN)) return;
	xcl=xc;
	ycl=yc;

/* this is not standard BASIC all combinations of 
		FOR TO STEP are allowed */
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
		error(EUNKNOWN);
		return;
	}

	if (st == SINT)
		here=bi-ibuffer;

/*  here we know everything to set up the loop */
	setvar(xcl, ycl, b);
	if (DEBUG) { 
		outsc("** for loop with parameters var begin end step : ");
		outch(xcl); outch(ycl); outspc(); outnumber(b); outspc(); outnumber(e); outspc(); outnumber(s); outcr(); 
	}
	xc=xcl;
	yc=ycl;
	x=e;
	y=s;
	pushforstack();
	if (er != 0) return;

/*
 *	this tests the condition and stops if it is fulfilled already from start 
 *	there is an apocryphal feature here: STEP 0 is legal triggers an infinite loop
 */
	if ( (y > 0 && getvar(xc, yc)>x) || (y < 0 && getvar(xc, yc)<x ) ) { 
		dropforstack();
		findnextcmd();
		nexttoken();
		if (token == VARIABLE) nexttoken(); /* more evil - this should really check */
	}
}

/*
 *	BREAK - an apocryphal feature here is the BREAK command ending a loop
 */
void xbreak(){
	dropforstack();
	if (er != 0) return;
	findnextcmd();
	nexttoken();	
	if (token == VARIABLE) nexttoken(); /* more evil - this should really check */
}

/*
 * CONT as a loop control statement, as apocryphal as BREAK, simply 
 * advance to next and the continue to process
 */
void xcont() {
	findnextcmd();
}

/* 
 *	NEXT variable statement 
 */
void xnext(){
	mem_t xcl=0;
	mem_t ycl;
	address_t h;
	number_t t;

	nexttoken();

/* one variable is accepted as an argument, no list */
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

/* remember the current position */
	h=here;
	popforstack();
	if (er != 0) return;
/* a variable argument in next clears the for stack 
		down as BASIC programs can and do jump out to an outer next */
	if (xcl) {
		while (xcl != xc || ycl != yc ) {
			popforstack();
			if (er != 0) return;
		} 
	}

/* y=0 an infinite loop with step 0 */
	t=getvar(xc, yc)+y;
	setvar(xc, yc, t);

/* do we need another iteration, STEP 0 always triggers an infinite loop */
	if ( (y==0) || (y > 0 && t <= x) || (y < 0 && t >= x) ) {
/* push the loop with the new values back to the for stack */
		pushforstack();
		if (st == SINT) bi=ibuffer+here;
	} else {
/* last iteration completed we stay here after the next */
		here=h;
	}
	nexttoken();
	if (DEBUG) {outsc("** after next found token "); debugtoken(); }
}

/* 
 *	TOKEN output - this is also used in save, list does a minimal formatting with a simple heuristic
 */
void outputtoken() {
	address_t i;

	if (spaceafterkeyword) {
		if (token != '(' && token != LINENUMBER && token !=':' ) outspc();
		spaceafterkeyword=0;
	}

	switch (token) {
		case NUMBER:
			outnumber(x);
			break;
		case LINENUMBER: 
			outnumber(x);
			outspc();
			break;
		case ARRAYVAR:
		case STRINGVAR:
		case VARIABLE:
			outch(xc); 
			if (yc != 0) outch(yc);
			if (token == STRINGVAR) outch('$');
			break;
		case STRING:
			outch('"'); 
			outs(ir, x); 
			outch('"');
			break;;
		default:
			if (token < -3 && token > -122) {
				if ((token == TTHEN || 
						token == TELSE ||
						token == TTO || 
						token == TSTEP || 
						token == TGOTO || 
						token == TGOSUB ||
						token == TOR ||
						token == TAND ) && lastouttoken != LINENUMBER) outspc(); 
				for(i=0; gettokenvalue(i)!=0 && gettokenvalue(i)!=token; i++);
				outsc(getkeyword(i)); 
				if (token != GREATEREQUAL && token != NOTEQUAL && token != LESSEREQUAL) spaceafterkeyword=1;
				break;
			}	
			if (token >= 32) {
				outch(token);
				if (token == ':') outspc();
				break;
			} 
			outch(token); outspc(); outnumber(token);
	}

	lastouttoken=token;
}

/*
 * LIST programs to an output device
 */
void xlist(){
	number_t b, e;
	mem_t oflag = 0;

	lastouttoken=0;
	spaceafterkeyword=0;

	nexttoken();
 	parsearguments();
	if (er != 0) return;

	switch (args) {
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
		if (token == LINENUMBER && x >= b) oflag=1;
		if (token == LINENUMBER && x >  e) oflag=0;
		if (oflag) outputtoken();
		gettoken();
		if (token == LINENUMBER && oflag) {
			outcr();
/* wait after every line on small displays
   removed if ( dspactive() && (dsp_rows < 10) ){ if ( inch() == 27 ) break;} */
			if (dspactive()) 
				if ( dspwaitonscroll() == 27 ) break;
		}
	}
	if (here == top && oflag) outputtoken();
    if (e == 32767 || b != e) outcr(); /* supress newlines in "list 50" - a little hack */

	nexttoken();
 }

/*
 *	RUN and CONTINUE are the same function
 */
void xrun(){
	if (token == TCONT) {
		st=SRUN;
		nexttoken();
	} else {
		nexttoken();
		parsearguments();
		if (er != 0 ) return;
		if (args > 1) { error(EARGS); return; }
		if (args == 0) {
			here=0;
		}
		else {
			findline(pop());
		}
		if (er != 0) return;
		if (st == SINT) st=SRUN;
		clrvars();
		clrgosubstack();
		clrforstack();
		clrdata();
		clrlinecache();
		ert=0;
		nexttoken();
	}

/* once statement is called it stays into a loop until the token stream 
		is exhausted. Then we return to interactive mode. */
	statement();
	st=SINT;
/* flush the EEPROM when changing to interactive mode */
	eflush();

/* if called from command line with file arg - exit after run */
#ifndef ARDUINO
    if (bnointafterrun) restartsystem();
#endif
}

/*
 * NEW the general cleanup function - new deletes everything
 */
void resetbasicstate() {
 
 /* all stacks are purged */
  clearst();
  clrgosubstack();
  clrforstack();
  clrdata();
  clrvars();
  clrlinecache();
  
/* error status reset */
  reseterror();

/* let here point to the beginning of the program */
  here=0;

/* interactive mode */
  st=SINT;
  
}
 
void xnew(){ 
/* reset the state of the interpreter */
  resetbasicstate();

/* program memory back to zero and variable heap cleared */
  himem=memsize;
	zeroblock(0, memsize);
	top=0;

/* on EEPROM systems also clear the stored state and top */
#ifdef EEPROMMEMINTERFACE
  eupdate(0, 0);
  z.a=top;
  esetnumber(1, addrsize);
#endif
}

/* 
 *	REM - skip everything 
 */
void xrem() {
	while (token != LINENUMBER && token != EOL && here <= top) nexttoken(); 
}

/* 
 *	The Apple 1 BASIC additions
 * CLR, DIM, POKE, TAB
 */

/* 
 *	CLR - clearing variable space
 */
void xclr() {

#ifdef HASDARKARTS
	mem_t xcl, ycl, t;

	nexttoken();

	if (termsymbol()) {
		clrvars();
		clrgosubstack();
		clrforstack();
		clrdata();
		clrlinecache();
		ert=0;
	} else {

		xcl=xc;
		ycl=yc;
		t=token;

		switch (t) {
			case VARIABLE:
				if (xcl == '@' || ycl == 0) { error(EVARIABLE); return; }
				break;
			case ARRAYVAR: 
				nexttoken();
				if (token != '(') { error(EVARIABLE); return; }
				nexttoken();
				if (token != ')') { error(EVARIABLE); return; }
				break;
			case STRINGVAR:
				if (xcl == '@') { error(EVARIABLE); return; }
				break;
			default:
				expression();
				if (er != 0) return;
				ax=pop();
				xcl=ax%256;
				ycl=ax/256;
				t=TBUFFER;
		}

		ax=bfree(t, xcl, ycl);
		if (ax == 0) { 
			if (t != TBUFFER) { error(EVARIABLE); return; }
			else ert=1;
		}
	}

#else
	clrvars();
	clrgosubstack();
	clrforstack();
	clrdata();
	clrlinecache();
	ert=0;
#endif
	nexttoken();
}

#ifdef HASAPPLE1
/* 
 *	DIM - the dimensioning of arrays and strings from Apple 1 BASIC
 */
void xdim(){
	mem_t xcl, ycl; 
	mem_t t;

	nexttoken();

nextvariable:
	if (token == ARRAYVAR || token == STRINGVAR ){
		
		t=token;
		xcl=xc;
		ycl=yc;

		nexttoken();
		parsesubscripts();
		if (er != 0) return;

#ifndef HASMULTIDIM
		if (args != 1) {error(EARGS); return; }

		x=pop();
		if (x<=0) {error(EORANGE); return; }
		if (t == STRINGVAR) {
			if ( (x>255) && (strindexsize==1) ) {error(EORANGE); return; }
			(void) createstring(xcl, ycl, x, 1);
		} else {
			(void) createarray(xcl, ycl, x, 1);
		}
#else 
		if (args != 1 && args != 2) {error(EARGS); return; }

		x=pop();
		if (args == 2) {y=x; x=pop(); } else { y=1; }

		if (x <= 0 || y<=0) {error(EORANGE); return; }
		if (t == STRINGVAR) {
			if ( (x>255) && (strindexsize==1) ) {error(EORANGE); return; }
/* running from an SPI RAM means that we need to go through buffers in real memory which 
	limits string sizes, this should ne be here but encapsulated, todo after multidim string 
	implementation is complete */
#ifdef SPIRAMSBSIZE
			if (x>SPIRAMSBSIZE-1) {error(EORANGE); return; }
#endif			
			(void) createstring(xcl, ycl, x, y);
		} else {
			(void) createarray(xcl, ycl, x, y);
		}
#endif	
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
 *	POKE - low level poke to the basic memory, works only up to 32767
 */
void xpoke(){
	address_t amax;

/* like in peek
	this is a hack again, 16 bit numbers can't peek big addresses */
	if ( (long) memsize > (long) maxnum) amax=(address_t) maxnum; else amax=memsize;

	nexttoken();
	parsenarguments(2);
	if (er != 0) return;

	y=pop();
	x=pop();

	if (x >= 0 && x<amax) 
		/* mem[(unsigned int) x]=y; */
		memwrite2((address_t) x, y);
	else if (x < 0 && x >= -elength())
		eupdate(-x-1, y);
	else {
		error(EORANGE);
	}
}

/*
 *	TAB - spaces command of Apple 1 BASIC 
 * 		charcount mechanism for relative tab on
 */
void xtab(){
	nexttoken();
	parsenarguments(1);
	if (er != 0) return;

	x=pop();
#ifdef HASMSTAB
	if (reltab && od == OSERIAL) {
		if (charcount >= x ) x=0; else x=x-charcount-1;
	} 
#endif	
	while (x-- > 0) outspc();	
}
#endif

/* 
 *	Stefan's additions to Palo Alto BASIC
 * DUMP, SAVE, LOAD, GET, PUT, SET
 *
 */

/*
 *	DUMP - memory dump program
 */
void xdump() {
	address_t a;
	
	nexttoken();
	parsearguments();
	if (er != 0) return;

	switch (args) {
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

/*
 * helper of DUMP, wrote the memory out
 */
void dumpmem(address_t r, address_t b) {
	address_t j, i;	
	address_t k;

	k=b;
	i=r;
	while (i>0) {
		outnumber(k); outspc();
		for (j=0; j<8; j++) {
			outnumber(memread(k++)); outspc();
			if (k > memsize) break;
		}
		outcr();
		i--;
		if (k > memsize) break;
	}
#if defined(ARDUINOEEPROM) || defined(ARDUINOI2CEEPROM) || ( !defined(ARDUINO) && EEPROMSIZE>0)
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
 *	creates a C string from a BASIC string
 *	after reading a BASIC string ir2 contains a pointer
 *	to the data and x the string length
 */
void stringtobuffer(char *buffer) {
	index_t i;
	i=x;
	if (i >= SBUFSIZE) i=SBUFSIZE-1;
	buffer[i--]=0;
	while (i >= 0) { buffer[i]=ir2[i]; i--; }
}

/* get a file argument */
void getfilename(char *buffer, char d) {
	index_t s;
	char *sbuffer;

	s=stringvalue();
	if (er != 0) return;
	if (DEBUG) {outsc("** in getfilename2 stringvalue delivered "); outnumber(s); outcr(); }

	if (s) {
		x=pop();
		if (DEBUG) {outsc("** in getfilename2 copying string of length "); outnumber(x); outcr(); }
		stringtobuffer(buffer);
		if (DEBUG) {outsc("** in getfilename2 stringvalue string "); outsc(buffer); outcr(); }
		nexttoken();
	} else if (termsymbol()) {
		if (d) {
			sbuffer=getmessage(MFILE);
			s=0;
			while ((sbuffer[s] != 0) && (s < SBUFSIZE-1)) { buffer[s]=sbuffer[s]; s++; }
			buffer[s]=0;
			x=s;
		} else {
			buffer[0]=0;
			x=0;
		}
		nexttoken();
	} else {
/* error(EUNKNOWN); */
		expression();
		if (er != 0) return;
		buffer[0]=pop();
		buffer[1]=0;
	}
}

#if defined(FILESYSTEMDRIVER)
/*
 *	SAVE a file either to disk or to EEPROM
 */
void xsave() {
	char filename[SBUFSIZE];
	address_t here2;
	mem_t t;

	nexttoken();
	getfilename(filename, 1);
	if (er != 0) return;
	t=token;

	if (filename[0] == '!') {
		esave();
	} else {		
		if (DEBUG) { outsc("** Opening the file "); outsc(filename); outcr(); };
	 	
		if (!ofileopen(filename, "w")) {
			error(EFILE);
			//nexttoken();
			return;
		} 

/* save the output mode and then save */
		push(od);
		od=OFILE;
		
/* the core save - xlist() not used any more */
    ert=0; /* reset error to trap file problems */
		here2=here;
		here=0;
		gettoken();
		while (here < top) {
			outputtoken();
      if (ert) break;
			gettoken();
			if (token == LINENUMBER) outcr();
		}
		if (here == top) outputtoken();
   		outcr(); 
   	
/* back to where we were */
   		here=here2;

/* restore the output mode */
		od=pop();

/* clean up */
		ofileclose();

/* did an accident happen */
    if (ert) { printmessage(EGENERAL); outcr();  ert=0; }
	}

/* and continue remembering, where we were */
	token=t;
	//nexttoken();
}

/*
 * LOAD a file 
 */
void xload(const char* f) {
	char filename[SBUFSIZE];
	address_t ch;
	address_t here2;
	mem_t chain = 0;

	if (f == 0) {
		nexttoken();
		getfilename(filename, 1);
		if (er != 0) return;
	} else {
		for(ch=0; ch<SBUFSIZE && f[ch]!=0; ch++) filename[ch]=f[ch];
		//nexttoken(); //
	}

	if (filename[0] == '!') {
		eload();
		// nexttoken();
	} else {

/*	if load is called during runtime it chains
 *	load the program as new but perserve the variables
 *	gosub and for stacks are cleared 
 */
		if (st == SRUN) { 
			chain=1; 
			st=SINT; 
			top=0;
			clrgosubstack();
			clrforstack();
			clrdata();
		}

		if (!f)
			if (!ifileopen(filename)) {
				error(EFILE);
				//nexttoken();
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
/* after a successful load we save top to the EEPROM header */
#ifdef EEPROMMEMINTERFACE
    z.a=top;
    esetnumber(1, addrsize);
#endif

/* go back to run mode and start from the first line */
		if (chain) {
			st=SRUN;
			here=0;
			nexttoken();
		}
		// nexttoken();
	}
}
#else 
/*
 *	SAVE a file to EEPROM - minimal version for small Arduinos
 */
void xsave() {
	esave(); 
	nexttoken();
}
/*
 *	LOAD a file from EEPROM - minimal version for small Arduinos
 */
void xload(const char* f) {
	eload();
	nexttoken();
}
#endif

/*
 *	GET just one character from input 
 */
void xget(){
	mem_t t;		/* remember the left hand side token until the end of the statement, type of the lhs */
	mem_t ps=1;	/* also remember if the left hand side is a pure string of something with an index */
	mem_t xcl, ycl;	/* to preserve the left hand side variable names	*/
	address_t i=1;	/* and the beginning of the destination string  	*/
 	address_t i2=1; /* and the end of the destination string    */
	address_t j=1;	/* the second dimension of the array if needed		*/
	mem_t oid=id;

	nexttoken();

/* modifiers of the get statement */
	if (token == '&') {

		if (!expectexpr()) return;
		id=pop();		
		if (token != ',') {
			error(EUNKNOWN);
			return;
		}
		nexttoken();
	}

/* this code evaluates the left hand side - remember type and name */
	ycl=yc;
	xcl=xc;
	t=token;

/* find the indices */
	lefthandside(&i, &i2, &j, &ps);
	if (er != 0) return;

/* get the data */
	if (availch()) push(inch()); else push(0);

/* store the data element as a number */
	assignnumber(t, xcl, ycl, i, j, ps);

	//nexttoken();
	id=oid;
}

/*
 *	PUT writes one character to an output stream
 */
void xput(){
	mem_t ood=od;
	index_t i;

	nexttoken();

/* modifiers of the put statement */
	if (token == '&') {

		if(!expectexpr()) return;
		od=pop();
		if (token != ',') {
			error(EUNKNOWN);
			return;
		}
		nexttoken();
	}

	parsearguments();
	if (er != 0) return;

	for (i=args-1; i>=0; i--) sbuffer[i]=pop();
	outs(sbuffer, args);

	od=ood;
}

/* 
 *	SET - the command itself is also apocryphal it is a low level
 *	control command setting certain properties
 *	syntax, currently it is only SET expression, expression
 */
void xset(){
	address_t fn;
	address_t args;

	nexttoken();
	parsenarguments(2);
	if (er != 0) return;

	args=pop();
	fn=pop();
	switch (fn) {	
/* runtime debug level */
		case 0:
			debuglevel=args;
			break;	
/* autorun/run flag of the EEPROM 255 for clear, 0 for prog, 1 for autorun */
		case 1: 
			eupdate(0, args);
			break;
/* change the output device */			
		case 2: 
			switch (args) {
				case 0:
					od=OSERIAL;
					break;
				case 1:
					od=ODSP;
					break;
			}		
			break;
/* change the default output device */			
		case 3: 
			switch (args) {
				case 0:
					od=(odd=OSERIAL);
					break;
				case 1:
					od=(odd=ODSP);
					break;
			}		
			break;
 /* change the input device */			
		case 4:
			switch (args) {
				case 0:
					id=ISERIAL;
					break;
				case 1:
					id=IKEYBOARD;
					break;
			}		
			break;
 /* change the default input device */					
		case 5:
			switch (args) {
				case 0:
					idd=(id=ISERIAL);
					break;
				case 1:
					idd=(id=IKEYBOARD);
					break;
			}		
			break;	
#ifdef ARDUINOPRT
/* set the cr behaviour */
		case 6: 
			sendcr=(char)args;
			break;
/* set the blockmode behaviour */
		case 7: 
			blockmode=args;
			break;
/* set the second serial ports baudrate */
		case 8:
			prtset(args);
			break;
#endif
/* set the power amplifier level of the radio module */
#ifdef ARDUINORF24
		case 9: 
			radioset(args);
      break;
#endif		
/* display update control for paged displays */
    case 10:
      dspsetupdatemode(args);
      break;
/* change the serial device to a true TAB */
#ifdef HASMSTAB
    case 11:
      reltab=args;
      break;
#endif
/* change the lower array limit */
#ifdef HASARRAYLIMIT
    case 12:
      arraylimit=args;
      break;
#endif

	}
}

/*
 *	NETSTAT - network status command, rudimentary
 */
void xnetstat(){
#if defined(ARDUINOMQTT)

	nexttoken();
 	parsearguments();
	if (er != 0) return;
	
	switch (args) {
		case 0: 
			if (netconnected()) outsc("Network connected \n"); else outsc("Network not connected \n");
			outsc("MQTT state "); outnumber(mqttstate()); outcr();
 			outsc("MQTT out topic "); outsc(mqtt_otopic); outcr();
			outsc("MQTT inp topic "); outsc(mqtt_itopic); outcr();
			outsc("MQTT name "); outsc(mqttname); outcr();
			break;
		case 1: 
			ax=pop();
			switch (ax) {
				case 0: 
					netstop();
					break;
				case 1:
					netbegin();
					break;
				case 2:
					if (!mqttreconnect()) ert=1;
					break;
				default:
					error(EARGS);
					return;
			}
			break;
		default:
			error(EARGS);
			return;
	}
#endif	
	nexttoken();		
}

/*
 * The arduino io functions.
 *
 *	DWRITE - digital write 
 */
void xdwrite(){
	nexttoken();
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	dwrite(y, x);	
}

/*
 * AWRITE - analog write 
 */
void xawrite(){
	nexttoken();
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	awrite(y, x);
}

/*
 * PINM - pin mode
 */
void xpinm(){
	nexttoken();
	parsenarguments(2);
	if (er != 0) return; 
	x=pop();
	y=pop();
	pinm(y, x);	
}

/*
 * DELAY in milliseconds
 *
 * if background tasks are needed by the hardware.
 * delay is broken down in 1 ms intervalls in 
 * order to call byield every YIELDINTERVAL ms. This is 
 * typically needed for network functions.
 * Otherwise just plain delay.
 */
void xdelay(){
	unsigned long i;

	nexttoken();
	parsenarguments(1);
	if (er != 0) return;

#ifdef ARDUINOBGTASK
	x=pop();
	if (x>0) {
		for(i=0; i<x && i<maxnum; i++) {
			delay(1);
			if (i%YIELDINTERVAL == 0) byield();
		}
	}
#else 
	delay(pop());
#endif
}

/* tone if the platform has it -> BASIC command PLAY */
#ifdef HASTONE
void xtone(){
	nexttoken();
	parsearguments();
	if (er != 0) return;
	if (args>4 || args<2) {
		error(EARGS);
		return;
	}
	btone(args);	
}
#endif

#ifdef HASGRAPH
/*
 *	COLOR setting, accepting one or 3 arguments
 */
void xcolor() {
	short r, g, b;
	nexttoken();
	parsearguments();
	if (er != 0) return;
	switch(args) {
		case 1: 
			vgacolor(pop());
			break;
		case 3:
			b=pop();
			g=pop();
			r=pop();
			rgbcolor(r, g, b);
			break;
		default:
			error(EARGS);
			break;
	}
}

/*
 * PLOT a pixel on the screen
 */
void xplot() {
	short x0, y0;
	nexttoken();
	parsenarguments(2);
	if (er != 0) return; 
	y0=pop();
	x0=pop();
	plot(x0, y0);
}

/*
 * LINE draws a line 
 */
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

#ifdef HASDARKARTS
/*
 * MALLOC allocates a chunk of memory 
 */
void xmalloc() {
	address_t s;
	s=pop();
	if (s<1) {error(EORANGE); return; };
	z.a=pop();
	push(bmalloc(TBUFFER, z.a%256, z.a/256, s));
}

/*
 * FIND an object on the heap
 * xfind is the buffer space very simple function
 * xfind2 can find things in the variable name space
 */
void xfind() {
	z.a=pop();
	push(bfind(TBUFFER, z.a%256, z.a/256));
}

void xfind2() {
	address_t a;

	nexttoken();
	if (token != '(') {
		error(EUNKNOWN);
		return;
	}
	nexttoken();
	a=bfind(token, xc, yc);
	switch (token) {
		case VARIABLE:
		case STRINGVAR:
			nexttoken();
			break;
		case ARRAYVAR:
			nexttoken();
			if (token != '(') {
				error(EUNKNOWN);
				return;
			}
			nexttoken();
			if (token != ')') {
				error(EUNKNOWN);
				return;
			}		
			nexttoken();
			break;
		default:
			expression();
			if (er != 0) return;
			z.a=pop();
			a=bfind(TBUFFER, z.a%256, z.a/256);
	}
	if (token != ')') {
		error(EUNKNOWN);
		return;
	}
	push(a);
}

/*
 *	EVAL can modify a program, there are serious side effects
 *	which are not caught (and cannot be). All FOR loops and RETURN 
 *	vectors break if EVAL inserts in their range
 */
void xeval(){
	short i, l;
	address_t mline, line;


/* get the line number to store */
	if (!expectexpr()) return;
	line=pop();

	if (token != ',') {
		error(EUNKNOWN);
		return;
	}

/* the line to be stored */
	nexttoken();
	if (! stringvalue()) {
		error(EARGS); return; 
	}

/* here we have the string to evaluate in ir2 and copy it to the ibuffer
		only one line allowed, BUFSIZE is the limit */
	l=pop();
	if (l>BUFSIZE-1) {error(EORANGE); return; }
	for (i=0; i<l; i++) ibuffer[i+1]=ir2[i];
	ibuffer[l+1]=0;
	if (DEBUG) {outsc("** Preparing to store line "); outnumber(line); outspc(); outsc(ibuffer+1); outcr(); }

/* we find the line we are currently at */
	if (st != SINT) {
		mline=myline(here);
		if (DEBUG) {outsc("** myline is "); outnumber(mline); outcr(); }
	}

/* go to interactive mode and try to store the line */
	x=line;							// the linennumber
	push(st); st=SINT;	// go to (fake) interactive mode
	bi=ibuffer;					// go to the beginning of the line
	storeline();				// try to store it
	st=pop();						// go back to run mode

/* find my line - side effects not checked here */
	if (st != SINT) {
		findline(mline);
		nextline();
	}
}
#endif


#ifdef HASIOT

/* 
 * AVAIL of a stream - are there characters in the stream
 */
void xavail() {
	mem_t oid=id;
	id=pop();
	push(availch());
	id=oid;
}

/* 
 * IoT functions - sensor reader, experimentral
 */
void xfsensor() {
	short s, a;
	a=pop();
	s=pop();
	push(sensorread(s, a));
}

/*
 * Going to sleep for battery saving - implemented for ESP8266 and ESP32 
 * in hardware-*.h
 */

void xsleep() {
	nexttoken();
	parsenarguments(1);
	if (er != 0) return; 
	activatesleep(pop());
}

/*
 * Low level assignment function to be done 
 * converts all kind of stuff
 */

void xassign() {
	nexttoken();
}


#endif


/*
 *	BASIC DOS - disk access programs, to control mass storage from BASIC
 */

// string match helper in catalog 
char streq(const char *s, char *m){
	short i=0;
	while (m[i]!=0 && s[i]!=0 && i < SBUFSIZE){
		if (s[i] != m[i]) return 0;
		i++;
	}	
	return 1;
}

/*
 *	CATALOG - basic directory function
 */
void xcatalog() {
#if defined(FILESYSTEMDRIVER) 
	char filename[SBUFSIZE];
	const char *name;

	nexttoken();
	getfilename(filename, 0);
	if (er != 0) return; 

	rootopen();
	while (rootnextfile()) {
		if (rootisfile()) {
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
#else
	nexttoken();
#endif
}

/*
 *	DELETE a file
 */
void xdelete() {
#if defined(FILESYSTEMDRIVER)
	char filename[SBUFSIZE];

	nexttoken();
	getfilename(filename, 0);
	if (er != 0) return; 

	removefile(filename);
#else 
	nexttoken();
#endif
}

/*
 *	OPEN a file or I/O stream - very raw mix of different functions
 */
void xopen() {
#if defined(FILESYSTEMDRIVER) || defined(ARDUINORF24) || defined(ARDUINOMQTT) || defined(ARDUINOWIRE) 
	char stream = IFILE; // default is file operation
	char filename[SBUFSIZE];
	int mode;

/* which stream do we open? default is FILE */
	nexttoken();
	if (token == '&') {
		if (!expectexpr()) return;
		stream=pop();
		if (token != ',') {error(EUNKNOWN); return; }
		nexttoken();
	}
	
/* the filename and its length */
	getfilename(filename, 0);
	if (er != 0) return; 

/* and the arguments */
	args=0;
	//nexttoken();
	if (token == ',') { 
		nexttoken(); 
		parsearguments();
	}

/* getting an argument, no argument is read, i.e. mode 0 */
	if (args == 0 ) { 
		mode=0; 
	} else if (args == 1) {
		mode=pop();
	} else {
		error(EARGS);
		return;
	}
	switch(stream) {
#ifdef ARDUINOPRT
		case ISERIAL1:
			prtclose();
			if (mode == 0) mode=9600;
			if (prtopen(filename, mode)) ert=0; else ert=1;
			break;
#endif
#ifdef FILESYSTEMDRIVER
		case IFILE:
			switch (mode) {
				case 1:
					ofileclose();
					if (ofileopen(filename, "w")) ert=0; else ert=1;
					break;
				case 2:
					ofileclose();
					if (ofileopen(filename, "a")) ert=0; else ert=1;
					break;
				default:
				case 0:
					ifileclose();
					if (ifileopen(filename)) ert=0; else ert=1;		
					break;
			}
			break;
#endif
#ifdef ARDUINORF24
		case IRADIO:
			if (mode == 0) {
				iradioopen(filename);
			} else if (mode == 1) {
				oradioopen(filename);
			}
			break;
#endif
#ifdef ARDUINOWIRE
		case IWIRE:
			wireopen(filename[0], mode);
			break;
#endif
#ifdef ARDUINOMQTT
		case IMQTT:
			if (mode == 0) {
				mqttsubscribe(filename);
			} else if (mode == 1) {
				mqttsettopic(filename); 
			}
			break;
#endif
		default:
			error(EORANGE);
			return;

	}
#endif
	nexttoken();
}

/*
 *	OPEN as a function, currently only implemented for MQTT
 */
void xfopen() {
	short chan = pop();
	if (chan == 9) push(mqttstate()); else push(0);
}

/*
 *	CLOSE a file or stream 
 */
void xclose() {
#if defined(FILESYSTEMDRIVER) || defined(ARDUINORF24) || defined(ARDUINOMQTT) || defined(ARDUINOWIRE)
	char stream = IFILE;
	char mode;

	nexttoken();
	if (token == '&') {
		if (!expectexpr()) return;
		stream=pop();
		if (token != ',' && ! termsymbol()) {error(EUNKNOWN); return; }
		nexttoken();
	}

	parsearguments();
	if (args == 0) { 
		mode=0; 
	} else if (args == 1) {
		mode=pop();
	} else {
		error(EARGS);
		return;
	}

	switch(stream) {
		case IFILE:
			if (mode == 1 || mode == 2) ofileclose(); else if (mode == 0) ifileclose();
			break;
	}
#endif
	nexttoken();
}

/*
 * FDISK - format internal disk storages of RP2040, ESP and the like
 */
void xfdisk() {
#if defined(FILESYSTEMDRIVER)
	nexttoken();
	parsearguments();
	if (er != 0) return;
	if (args > 1) error(EORANGE);
	if (args == 0) push(0);
	outsc("Format disk (y/N)?");
	consins(sbuffer, SBUFSIZE);
	if (sbuffer[1] == 'y') formatdisk(pop());
#endif
	nexttoken();
}

#ifdef HASSTEFANSEXT
/*	
 *	USR low level function access of the interpreter
 *	for each group of functions there is a call vector
 *	and and argument.
 *
 * USR arguments from 0 to 31 are reserved for the 
 * interpreter status and the input output stream 
 * mechanisms. All other values are free and can be used
 * for individual functions - see case 32 for an example.
 */
void xusr() {
	address_t fn;
	int arg;

	arg=pop();
	fn=pop();
	switch(fn) {
/* USR(0,y) delivers all the internal constants and variables or the interpreter */		
		case 0: 
			switch(arg) {
				case 0: push(bsystype); break;
				case 1: /* language set identifier, odd because USR is part of STEFANSEXT*/
					z.a=0;
#ifdef HASAPPLE1
					z.a|=1;	
#endif
#ifdef HASARDUINOIO
					z.a|=2;
#endif
#ifdef HASFILEIO
					z.a|=4;
#endif 
#ifdef HASDARTMOUTH
					z.a|=8;
#endif
#ifdef HASGRAPH
					z.a|=16;
#endif
#ifdef HASDARKARTS
					z.a|=32;
#endif
#ifdef HASIOT
					z.a|=64;
#endif
					push(z.a);
					break;
				case 2: push(0); /* reserved for system speed identifier */			 
#ifdef HASFLOAT
				case 3:	push(1); break;
#else 
				case 3: push(0); break;
#endif
				case 4: push(numsize); break;
				case 5: push(maxnum); break;
				case 6: push(addrsize); break;
				case 7: push(maxaddr); break;
				case 8: push(strindexsize); break;
				case 9: push(memsize+1); break;
				case 10: push(elength()); break;
				case 11: push(GOSUBDEPTH); break;
				case 12: push(FORDEPTH); break;
				case 13: push(STACKSIZE); break;
				case 14: push(BUFSIZE); break;
				case 15: push(SBUFSIZE); break;
				case 16: push(ARRAYSIZEDEF); break;
				case 17: push(STRSIZEDEF); break;
/* - 24 reserved */
				case 24: push(top); break;
				case 25: push(here); break;
				case 26: push(himem); break;
				case 27: push(nvars); break;
				case 28: push(freeRam()); break;
				case 29: push(gosubsp); break;
				case 30: push(forsp); break;
				case 31: push(fnc); break;
				case 32: push(sp); break;
#ifdef HASDARTMOUTH
				case 33: push(data); break;
#else
				case 33: push(0); break;
#endif
/* - 48 reserved */
				case 48: push(id); break;
				case 49: push(idd); break;
				case 50: push(od); break;
				case 51: push(odd); break;
				default: push(0);
			}
			break;	
/* access to properties of stream 1 - serial	*/
		case 1:
			push(serialstat(arg));
			break;
/* access to properties of stream 2 - display and keyboard */			
		case 2: 
#if defined(DISPLAYDRIVER)
			push(dspstat(arg));
#elif defined(ARDUINOVGA)
			push(vgastat(arg));
#else 
			push(0);
#endif
			break;
/* access to properties of stream 4 - printer */
#ifdef ARDUINOPRT		
		case 4: 
			push(prtstat(arg));	
			break;			
#endif	
/* access to properties of stream 7 - wire */
#ifdef ARDUINOWIRE		
		case 7: 
			push(wirestat(arg));	
			break;			
#endif
/* access to properties of stream 8 - radio */
#ifdef ARDUINORF24	
		case 8: 
			push(radiostat(arg));	
			break;			
#endif
/* access to properties of stream 9 - mqtt */
#ifdef ARDUINOMQTT	
		case 9: 
			push(mqttstat(arg));	
			break;			
#endif
/* access to properties of stream 16 - file */
#ifdef FILESYSTEMDRIVER	
		case 16: 
			push(fsstat(arg));	
			break;			
#endif
/* user function 32 and beyond can be used freely */
		case 32:
/* put your code here, always push a result to the stack 
 * example for a function calulating 2*x
 *		push(arg*2);
 */
			push(0);
			break;
/* all USR values not assigned return 0 */
		default:
			push(0);
	}
}

#endif

/*
 * CALL currently only to exit the interpreter
 */
void xcall() {
	int r;

	if (!expectexpr()) return;
	r=pop();
	switch(r) {
		case 0:
/* flush the EEPROM dummy and the output file and then exit */
			eflush();  
			ofileclose();
			restartsystem();
			break;
/* call values to 31 reserved, add your own programs here */
		case 32:
/* your custom code here, always call nexttoken() ! */
			nexttoken();
			return;
		default:
			error(EORANGE);
			return;			
	}
}

/* the dartmouth stuff */
#ifdef HASDARTMOUTH
/*
 * DATA is simply skipped when encountered as a command
 */
void xdata() {
	while (!termsymbol()) nexttoken();
}

/* 
 * for READ find the next data record, helper of READ
 */
void nextdatarecord() {
	address_t h;
	mem_t s=1;

/* save the location of the interpreter */
	h=here;

/* data at zero means we need to init it, by searching the first data record */
	if (data == 0) {
		here=0;
		while (here<top && token!=TDATA) gettoken();
		data=here;
	} 

processdata:
/*
 * data at top means we have exhausted all data, 
 * nothing more to be done here, however we simulate
 * a number value of 0 here and don't throw an error
 * this is not Dartmouth style, more consistent with
 * iterables
 */
	if (data == top) { 
		token=NUMBER;
		x=0;
		ert=1;
		here=h;
		return;
	}
	
/* we process the data record */
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

/*
 *	READ - find data records and insert them to variables
 *	this code resembles get - generic stream read code needed later
 */
void xread(){
	mem_t t, t0;	/* remember the left hand side token until the end of the statement, type of the lhs */
	mem_t ps=1;	/* also remember if the left hand side is a pure string of something with an index 	*/
	mem_t xcl, ycl; /* to preserve the left hand side variable names	*/
	address_t i=1;  /* and the beginning of the destination string */
    address_t i2=0;  /* and the end of the destination string */
	address_t j=arraylimit;	/* the second dimension of the array if needed */
	mem_t datat;	/* the type of the data element */
	address_t lendest, lensource, newlength;
	int k;
	
	nexttoken();

/* this code evaluates the left hand side - remember type and name */
	ycl=yc;
	xcl=xc;
	t=token;

	if (DEBUG) {outsc("assigning to variable "); outch(xcl); outch(ycl); outsc(" type "); outnumber(t); outcr();}

/* find the indices and draw the next token of read */
	lefthandside(&i, &i2, &j, &ps);
	if (er != 0) return;

/* if the token after lhs is not a termsymbol, something is wrong */
	if (! termsymbol()) {error(EUNKNOWN); return; }
	t0=token;

	nextdatarecord();
	if (er != 0) return;

/* assign the value to the lhs - redundant code to assignment */
	switch (token) {
		case NUMBER:
/* a number is stored on the stack */
			push(x);
			assignnumber(t, xcl, ycl, i, j, ps);
			break;
		case STRING:	
/* a string is stored in ir2 */
			ir2=ir;
			lensource=x;

/* if we use the memory interface we have to save the source */ 
#ifdef USEMEMINTERFACE
			for(k=0; k<SPIRAMSBSIZE; k++) spistrbuf2[k]=ir2[k];
			ir2=spistrbuf2;
#endif	

/* the destination address of the lefthandside, on the fly create included */
			ir=getstring(xcl, ycl, i, j);
			if (er != 0) return;

/* the length of the lefthandside string */
			lendest=lenstring(xcl, ycl, j);

			if (DEBUG) {
				outsc("* read stringcode "); outch(xcl); outch(ycl); outcr();
				outsc("** read source string length "); outnumber(lensource); outcr();
				outsc("** read dest string length "); outnumber(lendest); outcr();
				outsc("** read dest string dimension "); outnumber(stringdim(xcl, ycl)); outcr();
			};

/* does the source string fit into the destination */
			if ((i+lensource-1) > stringdim(xcl, ycl)) { error(EORANGE); return; }

/* this code is needed to make sure we can copy one string to the same string 
	without overwriting stuff, we go either left to right or backwards */
#ifndef USEMEMINTERFACE
			if (x > i) 
				for (k=0; k<lensource; k++) { ir[k]=ir2[k];}
			else
				for (k=lensource-1; k>=0; k--) ir[k]=ir2[k]; 
#else 
/* on an SPIRAM system we need to go through the mem interface 
	for write */
			if (ir != 0) {
				if (x > i) 
					for (k=0; k<lensource; k++) ir[k]=ir2[k];
				else
					for (k=lensource-1; k>=0; k--) ir[k]=ir2[k]; 
			} else 
				for (k=0; k<lensource; k++) memwrite2(ax+k, ir2[k]);
#endif

/* classical Apple 1 behaviour is string truncation in substring logic */
			newlength = i+lensource-1;	
		
			setstringlength(xcl, ycl, newlength, j);
			break;
		default:
			error(EUNKNOWN);
			return;
	}

/* no nexttoken here as we have already a termsymbol */
	if (DEBUG) {
		outsc("** leaving xread with "); outnumber(token); outcr();
		outsc("** at here "); outnumber(here); outcr();
		outsc("** and data pointer "); outnumber(data); outcr();
	}
	token=t0;
}

/*
 *	RESTORE sets the data pointer to zero right now 
 */
void xrestore(){
	data=0;
	nexttoken();
}


/*
 *	DEF a function, functions are tokenized as FN Arrayvar
 */
void xdef(){
	mem_t xcl1, ycl1, xcl2, ycl2;
	address_t a;

/*  do we define a function */
	if (!expect(TFN, EUNKNOWN)) return;

/* the name of the function, it is tokenized as an array */
	if (!expect(ARRAYVAR, EUNKNOWN)) return;
	xcl1=xc;
	ycl1=yc;

/* the argument variable */ 
	if (!expect('(', EUNKNOWN)) return;
	if (!expect(VARIABLE, EUNKNOWN)) return;
	xcl2=xc;
	ycl2=yc;
	if (!expect(')', EUNKNOWN)) return;

/* the assignment */
	if (!expect('=', EUNKNOWN)) return;

/* ready to store the function */
	if (DEBUG) {
		outsc("** DEF FN with function "); 
		outch(xcl1); outch(ycl1); 
		outsc(" and argument ");
		outch(xcl2); outch(ycl2);
		outsc(" at here "); 
		outnumber(here);
		outcr();
	}

/* find the function */
	if ((a=bfind(TFN, xcl1, ycl1))==0) a=bmalloc(TFN, xcl1, ycl1, 0);
	if (DEBUG) {outsc("** found function structure at "); outnumber(a); outcr(); }

/* store the payload - the here address - and the name of the variable */
	z.a=here;
	setnumber(a, addrsize);
/*
	mem[a+addrsize]=xcl2;
	mem[a+addrsize+1]=ycl2;
*/
	memwrite2(a+addrsize, xcl2);
	memwrite2(a+addrsize+1, ycl2);

/* skip whatever comes next */
	while (!termsymbol()) nexttoken();
}

/*
 * FN function evaluation
 */
void xfn() {
	char fxc, fyc;
	char vxc, vyc;
	address_t a;
	address_t h1, h2;
	number_t xt;

/* the name of the function */
	if (!expect(ARRAYVAR, EUNKNOWN)) return;
	fxc=xc;
	fyc=yc;

/* and the argument */
	nexttoken();
	if (token != '(') {error(EUNKNOWN); return; }

	nexttoken();
	expression();
	if (er != 0) return;

	if (token != ')') {error(EUNKNOWN); return; }

/* find the function structure and retrieve the payload */
	if ((a=bfind(TFN, fxc, fyc))==0) {error(EUNKNOWN); return; }
	getnumber(a, addrsize);
	h1=z.a;
/*	
	vxc=mem[a+addrsize];
	vyc=mem[a+addrsize+1];
*/
	vxc=memread2(a+addrsize);
	vyc=memread2(a+addrsize+1);

/* remember the original value of the variable and set it */
	xt=getvar(vxc, vyc);
	if (DEBUG) {outsc("** saving the original running var "); outch(vxc); outch(vyc); outspc(); outnumber(xt); outcr();}

	setvar(vxc, vyc, pop());

/* store here and then evaluate the function */
	h2=here;
	here=h1;
	if (DEBUG) {outsc("** evaluating expressing at "); outnumber(here); outcr(); }

	if (!expectexpr()) return;

/* restore everything */
	here=h2;
	setvar(vxc, vyc, xt);

/* no nexttoken as this is called in factor !! */
}

/*
 *	ON is a bit like IF  
 */
void xon(){
	number_t cr;
	int ci;
	mem_t t;
	address_t tmp, line = 0;
	
	if(!expectexpr()) return;

/* the result of the condition, can be any number
		even large */
	cr=pop();
	if (DEBUG) { outsc("** in on condition found "); outnumber(cr); outcr(); } 

/* is there a goto or gosub */
	if (token != TGOSUB && token != TGOTO)  {
		error(EUNKNOWN);
		return;
	}

/* remember if we do gosub or goto */
	t=token;

/* how many arguments have we got here */
	nexttoken();
	parsearguments();
	if (er != 0) return;
	if (args == 0) { error(EARGS); return; }
	
/* do we have more arguments then the condition? */
	if (cr > args && cr <= 0) ci=0; else ci=(int)cr;

/* now find the line to jump to and clean the stack */
	while (args) {
		tmp=pop();
		if (args == ci) line=tmp;
		args--;
	}
	
	if (DEBUG) { outsc("** in on found line as target "); outnumber(line); outcr(); }
/* no line found to jump to */
	if (line == 0) {
		nexttoken();
		return;
	}

/* prepare for the jump	*/
	if (t == TGOSUB) pushgosubstack();
	if (er != 0) return;

	findline(line);
	if (er != 0) return;

/* goto in interactive mode switched to RUN mode
		no clearing of variables and stacks */
	if (st == SINT) st=SRUN;

	nexttoken();
}
#endif

/* 
 *	statement processes an entire basic statement until the end 
 *	of the line. 
 *
 *	The statement loop is a bit odd and requires some explanation.
 *	A statement function called in the central switch here must either
 *	call nexttoken as its last action to feed the loop with a new token 
 *	and then break or it must return which means that the rest of the 
 *	line is ignored. A function that doesn't call nexttoken and just 
 *	breaks causes an infinite loop.
 *
 *	statement is called once in interactive mode and terminates 
 *	at end of line. 
 */
void statement(){
	if (DEBUG) bdebug("statement \n"); 
	while (token != EOL) {
#ifdef HASSTEFANSEXT
		if (debuglevel == 1) debugtoken();
		if (debuglevel) outcr();
#endif
		switch(token){
			case LINENUMBER:
				nexttoken();
				break;
/* Palo Alto BASIC language set + BREAK */
			case TPRINT:
				xprint();
				break;
			case TLET:
				nexttoken();
				if ((token != ARRAYVAR) && (token != STRINGVAR) && (token != VARIABLE)) {
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
			case TEND:		/* return here because new input is needed */
				*ibuffer=0;	/* clear ibuffer - this is a hack */
				st=SINT;	/* switch to interactive mode */
				eflush(); 	/* if there is an EEPROM dummy, flush it here (protects flash storage!) */
				ofileclose();
				return;
			case TLIST:		
				xlist();
				break;
			case TNEW: 		/* return here because new input is needed */
				xnew();
				return;
			case TCONT:		/* cont behaves differently in interactive and in run mode */
				if (st==SRUN || st==SERUN) {
					xcont();
					break;
				}		/* no break here, because interactively CONT=RUN minus CLR */
			case TRUN:
				xrun();
				return;	
			case TREM:
				xrem();
				break;
/* Apple 1 language set */
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
/* Stefan's tinybasic additions */
			case TSAVE:
				xsave();
				break;
			case TLOAD: 
				xload(0);
				if (st == SINT) return; // interactive load doesn't like break as the ibuffer is messed up; */
				else break;
#ifdef HASSTEFANSEXT
			case TDUMP:
				xdump();
				break;	
			case TGET:
				xget();
				break;
			case TPUT:
				xput();
				break;			
			case TSET:
				xset();
				break;
			case TNETSTAT:
				xnetstat();
				break;
			case TCLS:
				outch(12);
				nexttoken();
				break;
/* low level functions as part of Stefan's extension */
			case TCALL:
				xcall();
				break;	
#endif
/* Arduino IO */
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
/* BASIC DOS function */
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
			case TFDISK:
				xfdisk();
				break;
#endif
/* graphics */
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
#ifdef HASSTEFANSEXT
			case TELSE:
				xelse();
				break;
#endif
#ifdef HASDARKARTS
			case TEVAL:
				xeval();
				break;
#endif
#ifdef HASIOT
			case TASSIGN:
				xassign();
				break;
			case TSLEEP:
				xsleep();
				break;	
#endif
/* and all the rest */
/*			case UNKNOWN:
				error(EUNKNOWN);
				return; */
			case ':':
				nexttoken();
				break;
			default:
/*  very tolerant - tokens are just skipped, this is anarchy */
				/* if (DEBUG) { outsc("** hoppla - unexpected token, skipped "); debugtoken(); }
				nexttoken(); */
/*  strict syntax checking */
				error(EUNKNOWN);
				return;
				//debugtoken();
				//nexttoken();
		}
/* after each statement we check on a break character 
		on an Arduino entering "#" at runtime stops the program */
		if (checkch() == BREAKCHAR) {
			st=SINT; 
			xc=inch(); 
			return;
		}; 

/* yield after each statement which is a 30-100 microsecond cycle 
		ALL backgriund tasks are handled in byield */
		byield();

/* interrupt handling - not yet implemented, only stubs*/
#ifdef HASINTERRUPTS
    if (interruptvector) {
      handleinterrupt();
      interruptvector=0;
    }
#endif

/* when an error is encountred the statement loop is ended */
		if (er) return;
	}
}

/*
 *	the setup routine - Arduino style
 */
void setup() {

/* start measureing time */
	timeinit();

/* init all io functions */
	ioinit();

/* timer stuff - experimental */
/* rtcsqw(); */

/* get the BASIC memory, either as memory array with
	ballocmem() or as an SPI serical memory */
#if defined(SPIRAMINTERFACE) && MEMSIZE == 0
	himem=memsize=spirambegin();
#else 
#if defined(EEPROMMEMINTERFACE)
/* 
 *  for an EEPROMMEM system, the memory consists of the 
 *  EEPROM from 0 to elength()-eheadersize and then the RAM.
 */
  himem=memsize=ballocmem()+(elength()-eheadersize);
#else
	himem=memsize=ballocmem();
#endif
#endif

#ifndef EEPROMMEMINTERFACE
/* be ready for a new program if we run on RAM*/
 	xnew();	
#else
/* if we run on an EEPROM system, more work is needed */
  if (eread(0) == 0 || eread(0) == 1) { /* have we stored a program and don't do new, god help us*/
    egetnumber(1, addrsize);
    top=z.a;    
    resetbasicstate(); /* the little brother of new, reset the state but let the program memory be */
    for (address_t a=elength(); a<memsize; a++) memwrite2(a, 0); /* clear the heap i.e. the basic RAM*/
  } else {
    eupdate(0, 0); /* now we have stored a program of length 0 */
    z.a=0;
    esetnumber(1, addrsize);
    xnew();
  }
#endif

/* check if there is something to autorun and prepare 
		the interpreter to got into autorun once loop is reached */
 	if (!autorun()) {
		printmessage(MGREET); outspc();
		printmessage(EOUTOFMEMORY); outspc(); 
		outnumber(memsize+1); outspc();
		outnumber(elength()); outcr();
 	}
}

/* 
 *	the loop routine for interactive input 
 */
void loop() {

/*
 *	autorun state was found in setup, autorun now but only once
 *	autorun BASIC programs return to interactive after completion
 *	autorun code always must loop in itself
 */
	if (st == SERUN) {
		xrun();
 /* on an EEPROM system we don't set top to 0 here */
 #ifndef EEPROMMEMINTERFACE
		top=0;
 #endif
		st=SINT;
	} else if (st == SRUN) {
		here=0;
		xrun();
		st=SINT;
	}

/* always return to default io channels once interactive mode is reached */
	iodefaults();

/* the prompt and the input request */
	printmessage(MPROMPT);
	ins(ibuffer, BUFSIZE-2);
        
/* tokenize first token from the input buffer */
	bi=ibuffer;
	nexttoken();

/* a number triggers the line storage, anything else is executed */
	if (token == NUMBER) {
		storeline();	
/* on an EEPROM system we store top after each succesful line insert */
#ifdef EEPROMMEMINTERFACE
    z.a=top;
    esetnumber(1, addrsize);
#endif
	} else {
 		/* st=SINT; */
		statement();   
		st=SINT;
	}

/* here, at last, all errors need to be catched and back to interactive input*/
	if (er) reseterror();
}

/* if we are not on an Arduino */
#ifndef ARDUINO
int main(int argc, char* argv[]){

/* save the arguments if there are any */
    bargc=argc;
    bargv=argv;
  
/* do what an Arduino would do, this loops for every interactive input */
	setup();
	while (1)
		loop();
}
#endif
