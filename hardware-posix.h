/*

	$Id: hardware-posix.h,v 1.2 2022/04/10 06:25:05 stefan Exp stefan $

	Stefan's basic interpreter 

	Playing around with frugal programming. See the licence file on 
	https://github.com/slviajero/tinybasic for copyright/left.
    (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)

	Author: Stefan Lenz, sl001@serverfabrik.de

	Hardware definition file coming with basic.c aka TinybasicArduino.ino

	Link to some of the POSIX OS features to mimic a microcontroller platform
	See hardware-arduino for more details on the interface.

	Supported: filesystem, real time clock, (serial) I/O on the text console
	Not supported: radio, wire, SPI, MQTT

	Partially supported: Wiring library on Raspberry PI

*/


#if ! defined(ARDUINO) && ! defined(__HARDWAREH__)
#define __HARDWAREH__ 

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

/* 
 * the sleep and restart functions
 */
void restartsystem() {exit(0);}
void activatesleep() {}


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
void dspwrite(char c){};
void dspbegin() {};
char dspwaitonscroll() { return 0; };
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
uint8_t eeprom[EEPROMSIZE];
void ebegin(){ 
	int i;
	FILE* efile;
	for (i=0; i<EEPROMSIZE; i++) eeprom[i]=255;
	efile=fopen("eeprom.dat", "r");
	if (efile) fread(eeprom, EEPROMSIZE, 1, efile);
}

void eflush(){
	FILE* efile;
	efile=fopen("eeprom.dat", "w");
	if (efile) fwrite(eeprom, EEPROMSIZE, 1, efile);
}

address_t elength() { return EEPROMSIZE; }
void eupdate(address_t a, short c) { if (a>=0 && a<EEPROMSIZE) eeprom[a]=c; }
short eread(address_t a) { if (a>=0 && a<EEPROMSIZE) return eeprom[a]; else return 255;  }

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
#if defined(MINGW) || defined(MSDOS)
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
void serialwrite(char c) { putchar(c); }
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

/* handling the second serial interface */
void prtbegin() {}
void prtwrite(char c) {}
char prtread() {return 0;}
short prtcheckch(){ return FALSE; }
short prtavailable(){ return 0; }

/* 
 * The wire code 
 */ 
void wirebegin() {}
void wireopen(char* s) {}
void wireins(char *b, uint8_t l) { b[0]=0; z.a=0; }
void wireouts(char *b, uint8_t l) {}

/* 
 *	Read from the radio interface, radio is always block 
 *	oriented. 
 */
void radioins(char *b, short nb) { b[0]=0; b[1]=0; z.a=0; }
void radioouts(char *b, short l) {}
void iradioopen(char *filename) {}
void oradioopen(char *filename) {}

/* Arduino sensors */
void sensorbegin() {}
number_t sensorread(short s, short v) {return 0;};

#endif

