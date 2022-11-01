/*
 *
 * $Id: hardware-posix.h,v 1.4 2022/08/15 18:08:56 stefan Exp stefan $
 *
 *	Stefan's basic interpreter 
 *
 *	Playing around with frugal programming. See the licence file on 
 *	https://github.com/slviajero/tinybasic for copyright/left.
 *	(GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 *	Author: Stefan Lenz, sl001@serverfabrik.de
 *
 *	Hardware definition file coming with basic.c aka TinybasicArduino.ino
 *
 *	Link to some of the POSIX OS features to mimic a microcontroller platform
 *	See hardware-arduino for more details on the interface.
 *
 *	Supported: filesystem, real time clock, (serial) I/O on the text console
 *	Not supported: radio, wire, SPI, MQTT
 *
 *	Partially supported: Wiring library on Raspberry PI
 *
 */

/* simulates SPI RAM, only test code, keep undefed if you don't want to do something special */
#undef SPIRAMSIMULATOR

/* use a serial port as printer interface */
#define ARDUINOPRT


#if ! defined(ARDUINO) && ! defined(__HARDWAREH__)
#define __HARDWAREH__ 

/* 
 * the system type and system capabilities
 */

#ifndef MSDOS
const char bsystype = SYSTYPE_POSIX;
#else
const char bsystype = SYSTYPE_MSDOS;
#endif

/* 
 * Arduino default serial baudrate and serial flags for the 
 * two supported serial interfaces. Set to 0 on POSIX OSes 
 */
const int serial_baudrate = 0;
const int serial1_baudrate = 0;
char sendcr = 0;
short blockmode = 0;

/*  handling time, remember when we started, needed in millis() */
struct timeb start_time;
void timeinit() { ftime(&start_time); }

/* starting wiring for raspberry */
void wiringbegin() {
#ifdef RASPPI
	wiringPiSetup();
#endif
}

/*
 * helper functions OS, heuristic on how much memory is 
 * available in BASIC
 */
long freememorysize() {
#ifdef MSDOS
	return 48000;
#else
	return 65536;
#endif  
}

long freeRam() {
	return freememorysize();
}


/* 
 * the sleep and restart functions
 */
void restartsystem() {exit(0);}
void activatesleep(long t) {}


/* 
 * start the SPI bus 
 */
void spibegin() {}


/*
 * DISPLAY driver code section, the hardware models define a set of 
 * of functions and definitions needed for the display driver. These are 
 *
 * dsp_rows, dsp_columns: size of the display
 * dspbegin(), dspprintchar(c, col, row), dspclear()
 * 
 * All displays which have this functions can be used with the 
 * generic display driver below.
 * 
 * Graphics displays need to implement the functions 
 * 
 * rgbcolor(), vgacolor()
 * plot(), line(), rect(), frect(), circle(), fcircle() 
 * 
 * Color is currently either 24 bit or 4 bit 16 color vga.
 */
const int dsp_rows=0;
const int dsp_columns=0;
void dspsetupdatemode(char c) {}
void dspwrite(char c){};
void dspbegin() {};
int dspstat(char c) {return 0; }
char dspwaitonscroll() { return 0; }
char dspactive() {return FALSE; }
void dspsetscrollmode(char c, short l) {}
void dspsetcursor(short c, short r) {}
void rgbcolor(int r, int g, int b) {}
void vgacolor(short c) {}
void plot(int x, int y) {}
void line(int x0, int y0, int x1, int y1)   {}
void rect(int x0, int y0, int x1, int y1)   {}
void frect(int x0, int y0, int x1, int y1)  {}
void circle(int x0, int y0, int r) {}
void fcircle(int x0, int y0, int r) {}
void vgabegin(){}
void vgawrite(char c){}

/* 
 * Keyboard code stubs
 * keyboards can implement 
 * 	kbdbegin()
 * they need to provide
 * 	kbdavailable(), kbdread(), kbdcheckch()
 * the later is for interrupting running BASIC code
 */
void kbdbegin() {}
int kbdstat(char c) {return 0; }
char kbdavailable(){ return 0;}
char kbdread() { return 0;}
char kbdcheckch() { return 0;}

/* Display driver would be here, together with vt52 */


/* 
 * Real Time clock code 
 */
char rtcstring[18] = { 0 };

/* identical to arduino code -> isolate */
char* rtcmkstr() {
	int cc = 1;
	short t;
	char ch;
	t=rtcget(2);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]=':';
	t=rtcread(1);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]=':';
	t=rtcread(0);
	rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='-';
	t=rtcread(3);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='/';
	t=rtcread(4);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc++]='/';
	t=rtcread(5);
	if (t/10 > 0) rtcstring[cc++]=t/10+'0';
	rtcstring[cc++]=t%10+'0';
	rtcstring[cc]=0;
	rtcstring[0]=cc-1;

	return rtcstring;
}

short rtcread(char i) {
	struct timeb thetime;
	struct tm *ltime;
	ftime(&thetime);
	ltime=localtime(&thetime.time);
	switch (i) {
		case 0: 
			return ltime->tm_sec;
		case 1:
			return ltime->tm_min;
		case 2:
			return ltime->tm_hour;
		case 3:
			return ltime->tm_mday;
		case 4:
			return ltime->tm_mon+1;
		case 5:
			return ltime->tm_year-100;
		case 6:
			return ltime->tm_wday;
		case 7:
			return 0;
		default:
			return 0;
	}
}
short rtcget(char i) {return rtcread(i);}
void rtcset(char i, short v) {}


/* 
 * Wifi and MQTT code 
 */
void netbegin() {}
char netconnected() { return 0; }
void mqttbegin() {}
int mqttstat(char c) {return 0; }
int  mqttstate() {return -1;}
void mqttsubscribe(char *t) {}
void mqttsettopic(char *t) {}
void mqttouts(char *m, short l) {}
void mqttins(char *b, short nb) { z.a=0; };
char mqttinch() {return 0;};

/* 
 *	EEPROM handling, these function enable the @E array and 
 *	loading and saving to EEPROM with the "!" mechanism
 *	a filesystem based dummy
 */ 
signed char eeprom[EEPROMSIZE];
void ebegin(){ 
	int i;
	FILE* efile;
	for (i=0; i<EEPROMSIZE; i++) eeprom[i]=-1;
	efile=fopen("eeprom.dat", "r");
	if (efile) fread(eeprom, EEPROMSIZE, 1, efile);
}

void eflush(){
	FILE* efile;
	efile=fopen("eeprom.dat", "w");
	if (efile) fwrite(eeprom, EEPROMSIZE, 1, efile);
	fclose(efile);
}

address_t elength() { return EEPROMSIZE; }
void eupdate(address_t a, short c) { if (a>=0 && a<EEPROMSIZE) eeprom[a]=c; }
short eread(address_t a) { if (a>=0 && a<EEPROMSIZE) return eeprom[a]; else return -1;  }

/* 
 *	the wrappers of the arduino io functions, to avoid 
 */	
#ifndef RASPPI
void aread(){ return; }
void dread(){ return; }
void awrite(number_t p, number_t v){}
void dwrite(number_t p, number_t v){}
void pinm(number_t p, number_t m){}
#else
void aread(){ push(analogRead(pop())); }
void dread(){ push(digitalRead(pop())); }
void awrite(number_t p, number_t v){
	if (v >= 0 && v<256) analogWrite(p, v);
	else error(EORANGE);
}
void dwrite(number_t p, number_t v){
	if (v == 0) digitalWrite(p, LOW);
	else if (v == 1) digitalWrite(p, HIGH);
	else error(EORANGE);
}
void pinm(number_t p, number_t m){
	if (m>=0 && m<=1) pinMode(p, m);
	else error(EORANGE); 
}
#endif

/* handling time - raspberry take delay function from wiring */
#if ! defined(MSDOS) && ! defined(MINGW) && ! defined(RASPPI)
void delay(number_t t) {usleep(t*1000);}
#endif
// ms style stuff
#if defined(MINGW)
void delay(number_t t) {Sleep(t);}
#endif

void bmillis() {
	struct timeb thetime;
	time_t dt;
	number_t m;
	ftime(&thetime);
	dt=(thetime.time-start_time.time)*1000+(thetime.millitm-start_time.millitm);
	m=(number_t) ( dt/(time_t)pop() % (time_t)maxnum);
	push(m);
}
// we need to to millis by hand except for RASPPI with wiring
#if ! defined(RASPPI)
long millis() { push(1); bmillis(); return pop(); }
#endif
void bpulsein() { pop(); pop(); pop(); push(0); }
void btone(short a) { pop(); pop(); if (a == 3) pop(); }

/* 
 *	the byield function is called after every statement
 *	it allows two levels of background tasks. 
 */
void byield() {}


/* 
 *	The file system driver - all methods needed to support BASIC fs access
 *	MSDOS to be done 
 *
 * file system code is a wrapper around the POSIX API
 */
void fsbegin(char v) {}
#define FILESYSTEMDRIVER
FILE* ifile;
FILE* ofile;
#ifndef MSDOS
DIR* root;
struct dirent* file; 
#else
void* root;
void* file;
#endif 

/* POSIX OSes always have filesystems */
int fsstat(char c) {return 1; }

/*
 *	File I/O function on an Arduino
 * 
 * filewrite(), fileread(), fileavailable() as byte access
 * open and close is handled separately by (i/o)file(open/close)
 * only one file can be open for write and read at the same time
 */
void filewrite(char c) { if (ofile) fputc(c, ofile); else ert=1;}

char fileread(){
	char c;
	if (ifile) c=fgetc(ifile); else { ert=1; return 0; }
	if (c == -1 ) ert=-1;
	return c;
}

char ifileopen(const char* filename){
	ifile=fopen(filename, "r");
	return (int) ifile;
}

void ifileclose(){
	if (ifile) fclose(ifile);
	ifile=NULL;	
}

char ofileopen(char* filename, const char* m){
	ofile=fopen(filename, m);
	return (int) ofile; 
}

void ofileclose(){ if (ofile) fclose(ofile); }

int fileavailable(){ return !feof(ifile); }

/*
 * directory handling for the catalog function
 * these methods are needed for a walkthtrough of 
 * one directory
 *
 * rootopen()
 * while rootnextfile()
 * 	if rootisfile() print rootfilename() rootfilesize()
 *	rootfileclose()
 * rootclose()
 */
void rootopen() {
#ifndef MSDOS
	root=opendir ("./");
#endif
}

int rootnextfile() {
#ifndef MSDOS
  file = readdir(root);
  return (file != 0);
#else 
  return FALSE;
#endif
}

int rootisfile() {
#ifndef MSDOS
  return (file->d_type == DT_REG);
#else
  return FALSE;
#endif
}

const char* rootfilename() { 
#ifndef MSDOS
  return (file->d_name);
#else
  return 0;
#endif  
}

int rootfilesize() { return 0; }
void rootfileclose() {}
void rootclose(){
#ifndef MSDOS
  (void) closedir(root);
#endif  
}

/*
 * remove method for files
 */
void removefile(char *filename) {
	remove(filename);
}

/*
 * formatting for fdisk of the internal filesystems
 */
void formatdisk(short i) {
	outsc("Format not implemented on this platform\n");
}

/*
 *	Primary serial code uses putchar / getchar
 */
void serialbegin(){}
int serialstat(char c) {
	if (c == 0) return 1;
  if (c == 1) return serial_baudrate;
  return 0;
}
void serialwrite(char c) { 
#ifdef HASMSTAB
	if (c > 31) charcount+=1;
	if (c == 10) charcount=0;
#endif
	putchar(c); 
}
char serialread() { return getchar(); }
short serialcheckch(){ return TRUE; }
short serialavailable() {return TRUE; }

/*
 * reading from the console with inch 
 * this mixes interpreter levels as inch is used here 
 * this code needs to go to the main interpreter section after 
 * thorough rewrite
 */
void consins(char *b, short nb) {
	char c;
	
	z.a=1;
	while(z.a < nb) {
		c=inch();
		if (c == '\r') c=inch();
		if (c == '\n' || c == -1 ) { /* terminal character is either newline or EOF */
			break;
		} else {
			b[z.a++]=c;
		} 
	}
	b[z.a]=0x00;
	z.a--;
	b[0]=(unsigned char)z.a;
}

/* 
 * handling the second serial interface - only done on Mac so far 
 * 
 * Tried to learn from https://www.pololu.com/docs/0J73/15.5
 *
 */
#ifdef ARDUINOPRT
#include <fcntl.h>
#include <termios.h>

int prtfile;

/* hardware related settings */
const char prtport[] = "/dev/cu.wchusbserial1420";

/* the buffer to read one character */
char prtbuf = 0;

void prtbegin() {

/* try to open the device file */
	prtfile=open(prtport, O_RDWR | O_NOCTTY);
	if (prtfile == -1) {
		perror(prtport);
		return;
	} 

/* configure the device */
	struct termios opt;
	(void) tcgetattr(prtfile, &opt);


/* raw terminal settings
  opt.c_iflag &= ~(INLCR | IGNCR | ICRNL | IXON | IXOFF);
  opt.c_oflag &= ~(ONLCR | OCRNL);
  opt.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
*/

/* timeout settings on read 100ms, read every character */
  opt.c_cc[VTIME] = 1;
  opt.c_cc[VMIN] = 0;

/* set the baudrate */
  cfsetospeed(&opt, B9600);
  cfsetispeed(&opt, cfgetospeed(&opt));

/* set the termin attributes */
  tcsetattr(prtfile, TCSANOW, &opt);
}

int prtstat(char c) {return 1; }
void prtset(int s) {}

/* write the characters byte by byte */
void prtwrite(char c) {
	int i=write(prtfile, &c, 1);
	if (i != 1) ert=1;
}

/* read just one byte, map no bytes to EOF = -1 */
char prtread() {
	char c;

/* something in the buffer? return it! */
	if (prtbuf) {
		c=prtbuf;
		prtbuf=0;
	} else {
/* try to read */
		int i=read(prtfile, &c, 1);
		if (i < 0) {
			ert=1;
			return 0;
		} 
		if (i == 0) return -1;
	}
	return c;
}

/* not yet implemented */
short prtcheckch(){ 
	if (!prtbuf) { /* try to read */
		int i=read(prtfile, &prtbuf, 1);
		if (i <= 0) prtbuf=0;
	}
	return prtbuf; 
}

short prtavailable(){ 
	return prtcheckch()!=0; 
}

#else
void prtbegin() {}
int prtstat(char c) {return 0; }
void prtset(int s) {}
void prtwrite(char c) {}
char prtread() {return 0;}
short prtcheckch(){ return 0; }
short prtavailable(){ return 0; }
#endif


/* 
 * The wire code 
 */ 
void wirebegin() {}
int wirestat(char c) {return 0; }
void wireopen(char s, char m) {}
void wireins(char *b, uint8_t l) { b[0]=0; z.a=0; }
void wireouts(char *b, uint8_t l) {}
short wireavailable() { return 1; }

/* 
 *	Read from the radio interface, radio is always block 
 *	oriented. 
 */
int radiostat(char c) {return 0; }
void radioset(int s) {}
void radioins(char *b, short nb) { b[0]=0; b[1]=0; z.a=0; }
void radioouts(char *b, short l) {}
void iradioopen(char *filename) {}
void oradioopen(char *filename) {}
short radioavailable() { return 0; }

/* Arduino sensors */
void sensorbegin() {}
number_t sensorread(short s, short v) {return 0;};

/*
 * Experimental code to simulate 64kb SPI SRAM modules
 * 
 * currently used to test the string code of the mem 
 * interface
 *
 */

#ifdef SPIRAMSIMULATOR
#define USEMEMINTERFACE

static mem_t* spiram;

/* the RAM begin method sets the RAM to byte mode */
address_t spirambegin() {
  spiram=(mem_t*)malloc(65536);
  if (maxnum>32767) return 65534; else return 32766;  
}

/* the simple unbuffered byte write, with a cast to signed char */
void spiramrawwrite(address_t a, mem_t c) {spiram[a]=c;}

/* the simple unbuffered byte read, with a cast to signed char */
mem_t spiramrawread(address_t a) {return spiram[a];}

/* the buffers calls, also only simulated here */

void spiram_rwbufferwrite(address_t a, mem_t c) {spiram[a]=c;}

mem_t spiram_rwbufferread(address_t a) {return spiram[a];}

mem_t spiram_robufferread(address_t a) {return spiram[a];}



/* to handle strings in SPIRAM situations two more buffers are needed 
 * they store intermediate results of string operations. The buffersize 
 * limits the maximum string length indepents of how big strings are set
 */
#define SPIRAMSBSIZE 128
char spistrbuf1[SPIRAMSBSIZE];
char spistrbuf2[SPIRAMSBSIZE];
#endif


#endif

