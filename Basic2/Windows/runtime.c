/*
 *
 * $Id: runtime.c,v 1.1 2024/02/25 04:43:16 stefan Exp stefan $
 *
 * Stefan's basic interpreter 
 * 
 * This is the Posix runtime environment for BASIC. It maps the functions 
 * needed for the various subsystems to the MCU specific implementations. 
 *
 * Author: Stefan Lenz, sl001@serverfabrik.de
 *
 * 
 * Configure the hardware settings in hardware.h.
 *
 */

#include "hardware.h"
#include "runtime.h"

/* a small character buffer to receive strings */
#define CBUFSIZE 4 
char* cbuffer[CBUFSIZE];

/* if the BASIC interpreter provides a loop function it will superseed this one */
#ifndef MSDOS
void __attribute__((weak)) bloop() {}
#else
void bloop() {}
#endif


/* 
 *  Global variables of the runtime env.
 */

int8_t id; // active input stream 
int8_t od; // active output stream 
int8_t idd = ISERIAL; // default input stream in interactive mode 
int8_t odd = OSERIAL; // default output stream in interactive mode 
int8_t ioer = 0; // the io error variable, always or-ed with ert in BASIC

/* counts the outputed characters on streams 0-3, used to emulate a real tab */
#ifdef HASMSTAB
uint8_t charcount[3]; /* devices 1-4 support tabing */
#endif

/* the pointer to the buffer used for the &0 device */
char* nullbuffer = ibuffer;
uint16_t nullbufsize = BUFSIZE; 

/* the system type */
#if defined(MSDOS)
uint8_t bsystype = SYSTYPE_MSDOS;
#elif defined(RASPPI)
uint8_t bsystype = SYSTYPE_PASPPI;
#elif defined(MINGW)
uint8_t bsystype = SYSTYPE_MINGW;
#elif defined(POSIX)
uint8_t bsystype = SYSTYPE_POSIX;
#else
uint8_t bsystype = SYSTYPE_UNKNOWN;
#endif

/* libraries from OSes */

/* Wiring Code, which library to use */
#ifdef POSIXWIRING
#include <wiringPi.h>
#endif

#ifdef POSIXPIGPIO
#include <pigpiod_if2.h>
#undef POSIXWIRING
int pigpio_pi = 0;
#endif

/* 
 * Default serial baudrate and serial flags for the 
 * two supported serial interfaces. Serial is always active and 
 * connected to channel &1 with 9600 baud. 
 * 
 * channel 4 (POSIXPRT) can be either in character or block 
 * mode. Blockmode is set as default here. This means that all 
 * available characters are always loaded to a string -> inb()
 */
/* 
 * Arduino default serial baudrate and serial flags for the 
 * two supported serial interfaces. Set to 0 on POSIX OSes 
 */
const uint16_t serial_baudrate = 0;
const uint16_t serial1_baudrate = 0;
uint8_t sendcr = 0;
uint8_t blockmode = 0;

/* 
 *  Input and output functions.
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

/* run standalone on second serial, set the right parameters */
#ifdef STANDALONESECONDSERIAL
  idd = ISERIAL1;
  odd = OPRT;
  blockmode = 0;
  sendcr = 0;
#endif

/* signal handling - by default SIGINT which is ^C is always caught and 
  leads to program stop. Side effect: the interpreter cannot be stopped 
  with ^C, it has to be left with CALL 0, works on Linux, Mac and MINGW
  but not on DOSBOX MSDOS as DOSBOS does not handle CTRL BREAK correctly 
  DOS can be interrupted with the CONIO mechanism using BREAKCHAR. 
*/ 
  signalon();

/* this is only for RASPBERRY - wiring has to be started explicitly */
  wiringbegin();

/* all serial protocolls, ttl channels, SPI and Wire */
  serialbegin();
  
#ifdef POSIXPRT
  prtbegin();
#endif
#ifdef ARDUINOSPI
  spibegin();
#endif
#ifdef HASWIRE
  wirebegin();
#endif

/* filesystems and networks */
  fsbegin();
#ifdef POSIXMQTT
  netbegin();  
  mqttbegin();
#endif

/* the keyboards */
#if defined(HASKEYBOARD) || defined(HASKEYPAD)
  kbdbegin();
#endif
/* the displays */
#if defined(DISPLAYDRIVER) || defined(GRAPHDISPLAYDRIVER)
  dspbegin();
#endif
#if defined(ARDUINOVGA) || defined(POSIXFRAMEBUFFER)
  vgabegin();  /* mind this - the fablib code and framebuffer is special here */
#endif
/* sensor startup */
#ifdef ARDUINOSENSORS
  sensorbegin();
#endif
/* clocks and time */
#if defined(HASCLOCK)
  rtcbegin();
#endif

/* the eeprom dummy */
  ebegin();

/* activate the iodefaults */
  iodefaults();
}

void iodefaults() {
  od=odd;
  id=idd;
}

/* 
 *  Layer 0 - The generic IO code 
 *
 * inch() reads one character from the stream, mostly blocking
 * checkch() reads one character from the stream, unblocking, a peek(), 
 *  inmplemented inconsistently
 * availch() checks availablibity in the stream
 * inb() a block read function for serial interfacing, developed for 
 *  AT message receiving 
 */ 

/* this is odd ;-) */
int cheof(int c) { if ((c == -1) || (c == 255)) return 1; else return 0; }

/* the generic inch code reading one character from a stream */
char inch() {
  switch(id) {
  case ONULL:
    return bufferread();
  case ISERIAL:
    return serialread();   
#ifdef POSIXPRT
  case ISERIAL1:
    return prtread();
#endif
#if defined(HASKEYBOARD) || defined(HASKEYPAD) || defined(HASVT52)        
  case IKEYBOARD:
#if defined(HASVT52)
    if (vt52avail()) return vt52read(); /* if the display has a message, read it */
#endif
#if defined(HASKEYBOARD) || defined(HASKEYPAD)  
    return kbdread();
#endif
#endif
#if defined(HASWIRE) && defined(HASFILEIO)
  case IWIRE:
    return wireread();
#endif
#ifdef HASRF24
  case IRADIO:
    return radioread();
#endif
#ifdef POSIXMQTT
   case IMQTT:
    return mqttread();
#endif
#ifdef FILESYSTEMDRIVER
  case IFILE:
    return fileread();
#endif      
  }
  return 0;
}

/* 
 * checking on a character in the stream, this is 
 * normally only used for interrupting a program,
 * for many streams this is just mapped to avail
 */
char checkch(){
  switch (id) {
  case ONULL:
    return buffercheckch();
  case ISERIAL:
    return serialcheckch();
#ifdef FILESYSTEMDRIVER
  case IFILE:
    return fileavailable();
#endif
#ifdef HASRF24
  case IRADIO:
    return radioavailable();
#endif
#ifdef POSIXMQTT
  case IMQTT:
    return mqttcheckch();
#endif   
#if (defined(HASWIRE) && defined(HASFILEIO))
  case IWIRE:
    return 0;
#endif
#ifdef POSIXPRT
  case ISERIAL1:
    return prtcheckch(); 
#endif
  case IKEYBOARD:
#if defined(HASKEYBOARD)  || defined(HASKEYPAD)
    return kbdcheckch(); /* here no display read as this is only for break and scroll control */
#endif
    break;
  }
  return 0;
}

/* character availability */
uint16_t availch(){
  switch (id) {
  case ONULL:
    return bufferavailable();
  case ISERIAL:
    return serialavailable(); 
#ifdef FILESYSTEMDRIVER
  case IFILE:
    return fileavailable();
#endif
#ifdef HASRF24
  case IRADIO:
    return radioavailable();
#endif        
#ifdef POSIXMQTT
  case IMQTT:
    return mqttavailable();
#endif        
#if (defined(HASWIRE) && defined(HASFILEIO))
  case IWIRE:
    return wireavailable();
#endif
#ifdef POSIXPRT
  case ISERIAL1:
    return prtavailable();
#endif
  case IKEYBOARD:
#if defined(HASKEYBOARD) || defined(HASKEYPAD) || defined(HASVT52)
#if defined(HASVT52)
    if (vt52avail()) return vt52avail(); /* if the display has a message, read it */
#endif
#if defined(HASKEYBOARD) || defined(HASKEYPAD) 
    return kbdavailable();
#endif
#endif
    break;
  }
  return 0;
}

/* 
 *  the block mode reader for esp and sensor modules 
 *  on a serial interface, it tries to read as many 
 *  characters as possible into a buffer
 *  blockmode = 1 reads once availch() bytes
 *  blockmode > 1 implements a timeout mechanism and tries 
 *    to read until blockmode milliseconds have expired
 *    this is needed for esps and other sensors without
 *    flow control and volatile timing to receive more 
 *    then 64 bytes 
 */
 
uint16_t inb(char *b, int16_t nb) {
  long m;
  uint16_t z;
  int16_t i = 0; // check this 

  if (blockmode == 1) {
    i=availch();
    if (i>nb-1) i=nb-1;
    b[0]=(unsigned char)i;
    z=i;
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
    z=i;
    b[i+1]=0;
  } else {
    b[0]=0;
    z=0;
    b[1]=0; 
  } 
  return z;
}

/*
 * reading from the console with inch, local echo is handled by the terminal
 */
uint16_t consins(char *b, uint16_t nb) {
  char c;
  uint16_t z;

  z=1;
  while(z < nb) {
    c=inch();
    if (c == '\r') c=inch();
    if (c == '\n' || cheof(c)) { /* terminal character is either newline or EOF */
      break;
    } else {
      b[z++]=c;
    } 
  }
  b[z]=0x00;
  z--;
  b[0]=(unsigned char)z;
  return z;
}


/* 
 *  ins() is the generic reader into a string, by default 
 *  it works in line mode and ends reading after newline
 *
 *  the first element of the buffer is the lower byte of the length
 *
 *  this is corrected later in xinput, z.a has to be set as 
 *  a side effect
 *
 *  for streams providing entire strings as an input the 
 *  respective string method is called
 *
 *  all other streams are read using consins() for character by character
 *  input until a terminal character is reached
 */
uint16_t ins(char *b, uint16_t nb) {
  switch(id) {
  case ONULL:
    return bufferins(b, nb);
  case ISERIAL:
    return serialins(b, nb);
    break;
#if defined(HASKEYBOARD) || defined(HASKEYPAD)
  case IKEYBOARD:
    return kbdins(b, nb);
#endif
#ifdef POSIXPRT
  case ISERIAL1:
    return prtins(b, nb);
 #endif   
#if defined(HASWIRE) && defined(HASFILEIO)
  case IWIRE:
    return wireins(b, nb);
#endif
#ifdef HASRF24
  case IRADIO:
    return radioins(b, nb);
#endif
#ifdef POSIXMQTT
  case IMQTT:
    return mqttins(b, nb);  
#endif
#ifdef FILESYSTEMDRIVER
  case IFILE:
    return consins(b, nb);
#endif
  default:
    b[0]=0; b[1]=0;
    return 0;
  }  
}

/*
 * outch() outputs one character to a stream
 * block oriented i/o like in radio not implemented here
 */
void outch(char c) {

/* do we have a MS style tab command, then count characters on stream 1-4 but not in fileio */
/* this does not work for control characters - needs to go to vt52 later */

#ifdef HASMSTAB
  if (od > 0 && od <= OPRT) {
    if (c > 31) charcount[od-1]+=1;
    if (c == 10) charcount[od-1]=0;
  }
#endif

  switch(od) {
  case ONULL:
    bufferwrite(c);
    break;
  case OSERIAL:
    serialwrite(c);
    break;
#ifdef POSIXPRT
  case OPRT:
    prtwrite(c);
    break;
#endif    
#ifdef FILESYSTEMDRIVER
  case OFILE:
    filewrite(c);
    break;
#endif
#if defined(ARDUINOVGA)
  case ODSP: 
    vgawrite(c);
    break;
#elif defined(DISPLAYDRIVER) || defined(GRAPHDISPLAYDRIVER)
  case ODSP: 
    dspwrite(c);
    break;
#endif
#ifdef POSIXMQTT
  case OMQTT:
    mqttwrite(c); /* buffering for the PRINT command */
    break;
#endif
  default:
    break;
  }
  byield(); /* yield after every character for ESP8266 */
}

/*
 *  outs() outputs a string of length x at index ir - basic style
 *  default is a character by character operation, block 
 *  oriented write needs special functions
 */
void outs(char *ir, uint16_t l){
  uint16_t i;

  switch (od) {
#ifdef HASRF24
  case ORADIO:
    radioouts(ir, l);
    break;
#endif
#if (defined(HASWIRE) && defined(HASFILEIO))
  case OWIRE:
    wireouts(ir, l);
    break;
#endif
#ifdef POSIXMQTT
  case OMQTT:
    mqttouts(ir, l);
     break;
#endif
#ifdef GRAPHDISPLAYDRIVER
  case ODSP:
    dspouts(ir, l);
    break;
#endif
  default:
    for(i=0; i<l; i++) outch(ir[i]);
  }
  byield(); /* triggers yield after each character output */
}


/*  handling time, remember when we started, needed in millis() */
struct timeb start_time;
void timeinit() { ftime(&start_time); }

/* starting wiring for raspberry */
void wiringbegin() {
#ifdef POSIXWIRING
  wiringPiSetup();
#endif
#ifdef POSIXPIGPIO
  pigpio_pi=pigpio_start("localhost","8888");
  printf("** GPIO started with result %d\n", pigpio_pi);
  printf("** pigpio version %d.\n", get_pigpio_version(pigpio_pi));
  printf("** Hardware revision %d.\n", get_hardware_revision(pigpio_pi));
#endif
}

/* signal handling */
#ifdef POSIXSIGNALS
#include <signal.h>
uint8_t breaksignal = 0;

/* simple signal handler */
void signalhandler(int sig){
  breaksignal=1;
  signal(BREAKSIGNAL, signalhandler);
}

/* activate signal handling */
void signalon() {
  signal(BREAKSIGNAL, signalhandler);
}

/* deactivate signal handling unused and not yet done*/
void signaloff() {}

#endif

/*
 * helper functions OS, heuristic on how much memory is available in BASIC
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
void restartsystem() { exit(0);}
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
void dspsetupdatemode(uint8_t c) {}
void dspwrite(char c){}
void dspbegin() {}
uint8_t dspstat(uint8_t c) {return 0; }
char dspwaitonscroll() { return 0; }
uint8_t dspactive() {return 0; }
void dspsetscrollmode(uint8_t c, uint8_t l) {}
void dspsetcursor(uint8_t c) {}

#ifndef POSIXFRAMEBUFFER
/* these are the graphics commands */
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) {}
void vgacolor(uint8_t c) {}
void plot(int x, int y) {}
void line(int x0, int y0, int x1, int y1)   {}
void rect(int x0, int y0, int x1, int y1)   {}
void frect(int x0, int y0, int x1, int y1)  {}
void circle(int x0, int y0, int r) {}
void fcircle(int x0, int y0, int r) {}

/* stubs for the vga code part analogous to ESP32 */
void vgabegin(){}
void vgawrite(char c){}
#else
/* 
 * This is the first draft of the linux framebuffer code 
 * currently very raw, works only if the framebuffer is 24 bit 
 * very few checks, all kind of stuff can go wrong here.
 * 
 * Main ideas and some part of the code came from this
 * article https://www.mikrocontroller.net/topic/379335
 * by Andy W.
 * 
 * Bresenham's algorithm came from the Wikipedia article 
 * and this very comprehensive discussion
 * http://members.chello.at/~easyfilter/bresenham.html
 * by Alois Zingl from the Vienna Technikum. I also recommend
 * his thesis: http://members.chello.at/%7Eeasyfilter/Bresenham.pdf
 * 
 */
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <linux/fb.h>
#include <sys/mman.h>
#include <string.h>

/* 'global' variables to store screen info */
char *framemem = 0;
int framedesc = 0;

/* info from the frame buffer itself */
struct fb_var_screeninfo vinfo;
struct fb_fix_screeninfo finfo;
struct fb_var_screeninfo orig_vinfo;

/* the color variable of the frame buffer */
long framecolor = 0xffffff;
int  framevgacolor = 0x0f;
long framescreensize = 0;
int framecolordepth = 0;

/* prepare the framebuffer device */
void vgabegin() {

/* see if we can open the framebuffer device */
  framedesc = open("/dev/fb0", O_RDWR);
  if (!framedesc) {
    printf("** error opening frame buffer \n");
    return;
  } 

/* now get the variable info of the screen */
  if (ioctl(framedesc, FBIOGET_VSCREENINFO, &vinfo)) {
    printf("** error reading screen information \n");
    return;
  }
  printf("** detected screen %dx%d, %dbpp \n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel);

/* BASIC currently does 24 bit color only */
  memcpy(&orig_vinfo, &vinfo, sizeof(struct fb_var_screeninfo)); 
/*
  vinfo.bits_per_pixel = 24; 
  if (ioctl(framedesc, FBIOPUT_VSCREENINFO, &vinfo)) {
    printf("** error setting variable information \n");
    return;
  }
*/

/* how much color have we got */
  framecolordepth = vinfo.bits_per_pixel;

/* get the fixed information of the screen */
  if (ioctl(framedesc, FBIOGET_FSCREENINFO, &finfo)) {
    printf("Error reading fixed information.\n");
    return;
  }

/* now ready to memory map the screen - evil, we assume 24 bit without checking */

  framescreensize = (framecolordepth/8) * vinfo.xres * vinfo.yres;  
  framemem = (char*)mmap(0, framescreensize, PROT_READ | PROT_WRITE, MAP_SHARED, framedesc, 0);
  if ((int)framemem == -1) {
    printf("** error failed to mmap.\n");
    framemem=0;
    return;
  }

/* if all went well we have valid non -1 framemem and can continue */
}

/* this function does not exist in the ESP32 world because we don't care there */
void vgaend() {
  if ((int)framemem) munmap(framemem, framescreensize); 
  if (ioctl(framedesc, FBIOPUT_VSCREENINFO, &orig_vinfo)) {
    printf("** error re-setting variable information \n");
  }
  close(framedesc);
}

/* set the color variable depending on the color depth*/
void rgbcolor(uint8_t r, uint8_t g, uint8_t b) {
  switch (framecolordepth/8) {
  case 4:
    framecolor = (((long)r << 16) & 0x00ff0000) | (((long)g << 8) & 0x0000ff00) | ((long)b & 0x000000ff); /* untested */
    break;
  case 3:
    framecolor = (((long)r << 16) & 0x00ff0000) | (((long)g << 8) & 0x0000ff00) | ((long)b & 0x000000ff);
    break;
  case 2:
    framecolor = ((long) (r & 0xff) >> 3) << 10 | ((long) (g & 0xff) >> 2) << 6 | ((long) (b & 0xff) >> 3); /* untested */
    break;
  case 1:
    framecolor = ((long) (r & 0xff) >> 5) << 5 | ((long) (g & 0xff) >> 5) << 2 | ((long) (b & 0xff) >> 6); /* untested */
    break;
  }
}

/* this is taken from the Arduino TFT code */
void vgacolor(uint8_t c) {
  short base=128;
  framevgacolor=c;
  if (c==8) { rgbcolor(64, 64, 64); return; }
  if (c>8) base=255;
  rgbcolor(base*(c&1), base*((c&2)/2), base*((c&4)/4));  
}

/* plot directly into the framebuffer */
void plot(int x, int y) {
  unsigned long pix_offset;

/* is everything in range, no error here */
  if (x < 0 || y < 0 || x >= vinfo.xres || y >= vinfo.yres) return; 

/* find the memory location */
  pix_offset = (framecolordepth/8) * x + y * finfo.line_length;

  if (pix_offset < 0 || pix_offset+ (framecolordepth/8-1) > framescreensize) return;

/* write to the buffer */
  switch (framecolordepth/8) {
  case 4:
    *((char*)(framemem + pix_offset  )) = (unsigned char)(framecolor         & 0x000000ff);
    *((char*)(framemem + pix_offset+1)) = (unsigned char)((framecolor >>  8) & 0x000000ff);
    *((char*)(framemem + pix_offset+3)) = (unsigned char)((framecolor >> 16) & 0x000000ff);
    break;
  case 3:
    *((char*)(framemem + pix_offset  )) = (unsigned char)(framecolor         & 0x000000ff);
    *((char*)(framemem + pix_offset+1)) = (unsigned char)((framecolor >>  8) & 0x000000ff);
    *((char*)(framemem + pix_offset+2)) = (unsigned char)((framecolor >> 16) & 0x000000ff);
    break;
  case 2:
    *((char*)(framemem + pix_offset  )) = (unsigned char)((framecolor & 0x1f) + (((framecolor >> 5) & 0x03) << 6));
    *((char*)(framemem + pix_offset+1)) = (unsigned char)((framecolor >> 7) & 0xff);
    break;
  case 1:
    *((char*)(framemem + pix_offset  )) = (unsigned char)(framecolor         & 0x000000ff);
    break;
  }

}

/* Bresenham's algorith from Wikipedia */
void line(int x0, int y0, int x1, int y1) {
  int dx, dy, sx, sy;
  int error, e2;
  
  dx=abs(x0-x1);
  sx=x0 < x1 ? 1 : -1;
  dy=-abs(y1-y0);
  sy=y0 < y1 ? 1 : -1;
  error=dx+dy;

  while(1) {
    plot(x0, y0);
    if (x0 == x1 && y0 == y1) break;
    e2=2*error;
    if (e2 > dy) {
      if (x0 == x1) break;
      error=error+dy;
      x0=x0+sx;
    }
    if (e2 <= dx) {
      if (y0 == y1) break;
      error=error+dx;
      y0=y0+sy;
    }
  }
}

/* rects could also be drawn with hline and vline */
void rect(int x0, int y0, int x1, int y1) {
  line(x0, y0, x1, y0);
  line(x1, y0, x1, y1);
  line(x1, y1, x0, y1);
  line(x0, y1, x0, y0); 
}

/* filled rect, also just using line right now */
void frect(int x0, int y0, int x1, int y1) {
  int dx, sx;
  int x;
  sx=x0 < x1 ? 1 : -1;
  for(x=x0; x != x1; x=x+sx) line(x, y0, x, y1);
}

/* Bresenham for circles, based on Alois Zingl's work */
void circle(int x0, int y0, int r) {
  int x, y, err;
  x=-r;
  y=0; 
  err=2-2*r;
  do {
    plot(x0-x, y0+y);
    plot(x0-y, y0-x);
    plot(x0+x, y0-y);
    plot(x0+y, y0+x);
    r=err;
    if (r <= y) err+=++y*2+1;
    if (r > x || err > y) err+=++x*2+1;
  } while (x < 0);
}

/* for filled circles draw lines instead of points */
void fcircle(int x0, int y0, int r) {
  int x, y, err;
  x=-r;
  y=0; 
  err=2-2*r;
  do {
    line(x0-x, y0+y, x0+x, y0+y);
    line(x0+x, y0-y, x0-x, y0-y);
    r=err;
    if (r <= y) err+=++y*2+1;
    if (r > x || err > y) err+=++x*2+1;
  } while (x < 0);
}

/* not needed really, now, later yes ;-) */
void vgawrite(char c) {}
#endif

/* 
 * Keyboard code stubs
 * keyboards can implement 
 *  kbdbegin()
 * they need to provide
 *  kbdavailable(), kbdread(), kbdcheckch()
 * the later is for interrupting running BASIC code
 */
void kbdbegin() {}
uint8_t kbdstat(uint8_t c) {return 0; }
uint8_t kbdavailable(){ return 0;}
char kbdread() { return 0;}
char kbdcheckch() { return 0;}

/* vt52 code stubs - unused here - needed for basic.c */
uint8_t vt52avail() {return 0;}
char vt52read() { return 0; }

/* Display driver would be here, together with vt52 */

/* 
 * Real Time clock code 
 */

void rtcbegin() {}

uint16_t rtcget(uint8_t i) {
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
      return ltime->tm_wday;
    case 4:
      return ltime->tm_mday;
    case 5:
      return ltime->tm_mon+1;
    case 6:
      return ltime->tm_year-100;
    default:
      return 0;
  }
}

void rtcset(uint8_t i, uint16_t v) {}

/* 
 * Wifi and MQTT code 
 */
#ifndef POSIXMQTT
void netbegin() {}
uint8_t netconnected() { return 0; }
void mqttbegin() {}
uint8_t mqttstat(uint8_t c) {return 0; }
uint8_t mqttstate() {return 0;}
void mqttsubscribe(const char *t) {}
void mqttsettopic(const char *t) {}
void mqttouts(const char *m, uint16_t l) {}
uint16_t mqttins(char *b, uint16_t nb) { return 0; };
char mqttread() {return 0;};
#else 
/* we use mosquitto */
#include <mosquitto.h>
/* we assume to be on the network */
void netbegin() {}
uint8_t netconnected() { return 1; }
/* the mqtt code */
void mqttbegin() {}
uint8_t mqttstat(uint8_t c) {return 0; }
uint8_t mqttstate() {return 0;}
void mqttsubscribe(const char *t) {}
void mqttsettopic(const char *t) {}
void mqttouts(const char *m, uint16_t l) {}
uint16_t mqttins(char *b, uint16_t nb) { return 0; };
char mqttread() {return 0;};
#endif

/* 
 *  EEPROM handling, these function enable the @E array and 
 *  loading and saving to EEPROM with the "!" mechanism
 *  a filesystem based dummy
 */ 
int8_t eeprom[EEPROMSIZE];
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

uint16_t elength() { return EEPROMSIZE; }
void eupdate(uint16_t a, int8_t c) { if (a>=0 && a<EEPROMSIZE) eeprom[a]=c; }
int8_t eread(uint16_t a) { if (a>=0 && a<EEPROMSIZE) return eeprom[a]; else return -1;  }


/* 
 *	the wrappers of the arduino io functions
 */ 

#if !defined(POSIXWIRING) && !defined(POSIXPIGPIO)
uint16_t aread(uint8_t p) { return 0; }
uint8_t dread(uint8_t p) { return 0; }
void awrite(uint8_t p, uint16_t v){}
void dwrite(uint8_t p, uint8_t v){}
void pinm(uint8_t p, uint8_t m){}

/* wrapper around pulsein */
uint32_t pulsein(uint8_t pin, uint8_t val, uint32_t t) { return 0; }

/* write a pulse in microsecond range */
void pulseout(uint16_t unit, uint8_t pin, uint16_t duration, uint16_t val, uint16_t repetition, uint16_t interval) {}
#endif

#if defined(POSIXWIRING)
uint16_t aread(uint8_t p) { return analogRead(p); }
uint8_t dread(uint8_t p) { return digitalRead(p); }
void awrite(uint8_t p, uint16_t v){ analogWrite(p, v); }
void dwrite(uint8_t p, uint8_t v){ if (v) digitalWrite(p, HIGH); else digitalWrite(p, LOW); }

/* we normalize the pinMode as ESP32, ESP8266, and other boards behave rather
 *  differently. Following Arduino conventions we map 0 always to INPUT  
 *  and 1 always to OUTPUT, all the other numbers are passed through to the HAL 
 *  layer of the platform.
 *  Example: OUTPUT on ESP32 is 3 and 1 is assigned to INPUT.
 *  XMC also needs special treatment here.
 *  uint8_t is here for a reason.
 */
void pinm(uint8_t p, uint8_t m){
  if (m == 0) m=INPUT; 
  else if (m == 1) m=OUTPUT;
  pinMode(p,m);
}

/* wrapper around pulsein */
uint32_t pulsein(uint8_t pin, uint8_t val, uint32_t t) {}

/* write a pulse in microsecond range */
void pulseout(uint16_t unit, uint8_t pin, uint16_t duration, uint16_t val, uint16_t repetition, uint16_t interval) { return 0;}
#endif


#if defined(POSIXPIGPIO)
uint16_t aread(uint8_t p) { return 0; }
uint8_t dread(uint8_t p) { return gpio_read(pigpio_pi, p); }
void awrite(uint8_t p, uint16_t v) { set_PWM_dutycycle(pigpio_pi, p, v); }
void dwrite(uint8_t p, uint8_t v){ gpio_write(pigpio_pi, p, v); }
void pinm(uint8_t p, uint8_t m){ set_mode(pigpio_pi, p, m); }

/* wrapper around pulsein */
uint32_t pulsein(uint8_t pin, uint8_t val, uint32_t t) {}

/* write a pulse in microsecond range */
void pulseout(uint16_t unit, uint8_t pin, uint16_t duration, uint16_t val, uint16_t repetition, uint16_t interval) { return 0; }

#endif

#if defined(BREAKPIN) && defined(INPUT_PULLUP)
void breakpinbegin() { pinm(BREAKPIN, INPUT_PULLUP); }
uint8_t getbreakpin() { return dread(BREAKPIN); } 
#else 
/* there is no pins hence no breakpin */
void breakpinbegin() {}
uint8_t getbreakpin() { return 1; } /* we return 1 because the breakpin is defined INPUT_PULLUP */
#endif

/* we need to do millis by hand except for RASPPI with wiring */
#if !defined(POSIXWIRING)
unsigned long millis() { 
  struct timeb thetime;
  ftime(&thetime);
  return (thetime.time-start_time.time)*1000+(thetime.millitm-start_time.millitm);
}

/* this is just a stub, only needed in fasttickerprofile */
unsigned long micros() { return 0; }
#endif

void playtone(uint8_t pin, uint16_t frequency, uint16_t duration, uint8_t volume) {}

/* 
 *	The byield function is called after every statement
 *	it allows two levels of background tasks. 
 *
 *	BASICBGTASK controls if time for background tasks is 
 * 	needed, usually set by hardware features
 *
 * 	YIELDINTERVAL by default is 32, generating a 32 ms call
 *		to the network loop function. 
 *
 * 	LONGYIELDINTERVAL by default is 1000, generating a one second
 *		call to maintenance functions. 
 */

uint32_t lastyield=0;
uint32_t lastlongyield=0;

void byield() { 

/* the fast ticker for all fast timing functions */
  fastticker();

/* the loop function for non BASIC stuff */
  bloop();

#if defined(BASICBGTASK)
/* yield all 32 milliseconds */
  if (millis()-lastyield > YIELDINTERVAL-1) {
    yieldfunction();
    lastyield=millis();
  }

/* yield every second */
  if (millis()-lastlongyield > LONGYIELDINTERVAL-1) {
    longyieldfunction();
    lastlongyield=millis();
  }
 #endif
 
 /* call the background task scheduler on some platforms implemented in hardware-* */
  yieldschedule();
}

/* delay must be implemented to use byield() while waiting */
void bdelay(uint32_t t) { 
  unsigned long i;
  if (t>0) {
    i=millis();
    while (millis() < i+t) byield();
  } 
}

/* fastticker is the hook for all timing functions */
void fastticker() {
/* fastticker profiling test code */
#ifdef FASTTICKERPROFILE
  fasttickerprofile();
#endif
/* toggle the tone pin */
#ifdef ARDUINOTONEEMULATION
  tonetoggle();
#endif
}

/* everything that needs to be done often - 32 ms */
void yieldfunction() {}

/* everything that needs to be done not so often - 1 second */
void longyieldfunction() {
#ifdef BASICBGTASK
/* polling for the BREAKCHAR */
#ifdef POSIXNONBLOCKING
  if (checkch() == BREAKCHAR) breakcondition=1;
#endif
#endif
}

void yieldschedule() {}

/* 
 *  The file system driver - all methods needed to support BASIC fs access
 *  MSDOS to be done 
 *
 * file system code is a wrapper around the POSIX API
 */
void fsbegin() {}
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
uint8_t fsstat(uint8_t c) { return 1; }

/*
 *  File I/O function on an Arduino
 * 
 * filewrite(), fileread(), fileavailable() as byte access
 * open and close is handled separately by (i/o)file(open/close)
 * only one file can be open for write and read at the same time
 */
void filewrite(char c) { 
  if (ofile) 
    fputc(c, ofile); 
  else 
    ioer=1;
}

char fileread(){
  char c;
  if (ifile) c=fgetc(ifile); else { ioer=1; return 0; }
  if (cheof(c)) ioer=-1;
  return c;
}

uint8_t ifileopen(const char* filename){
  ifile=fopen(filename, "r");
  return ifile!=0;
}

void ifileclose(){
  if (ifile) fclose(ifile);
  ifile=0;  
}

uint8_t ofileopen(const char* filename, const char* m){
  ofile=fopen(filename, m);
  return ofile!=0;
}

void ofileclose(){ 
  if (ofile) fclose(ofile); 
  ofile=0;
}

int fileavailable(){ return !feof(ifile); }

/*
 * directory handling for the catalog function
 * these methods are needed for a walkthtrough of 
 * one directory
 *
 * rootopen()
 * while rootnextfile()
 *  if rootisfile() print rootfilename() rootfilesize()
 *  rootfileclose()
 * rootclose()
 */
#ifdef MSDOS
#include <dos.h>
#include <dir.h>
struct ffblk *bffblk; 
#endif

void rootopen() {
#ifndef MSDOS
  root=opendir ("./");
#else 
  (void) findfirst("*.*", bffblk, 0);
#endif
}

uint8_t rootnextfile() {
#ifndef MSDOS
  file = readdir(root);
  return (file != 0);
#else 
  return (findnext(bffblk) == 0);
#endif
}

uint8_t rootisfile() {
#if !defined(MSDOS) && !defined(MINGW)
  return (file->d_type == DT_REG);
#else
  return 1;
#endif
}

const char* rootfilename() { 
#ifndef MSDOS
  return (file->d_name);
#else
  return (bffblk->ff_name);
#endif  
}

uint32_t rootfilesize() { 
#ifndef MSDOS
  return 0;
#else
  return (bffblk->ff_fsize);
#endif
}

void rootfileclose() {}
void rootclose(){
#ifndef MSDOS
  (void) closedir(root);
#endif  
}

/*
 * remove method for files
 */
void removefile(const char *filename) {
  remove(filename);
}

/*
 * formatting for fdisk of the internal filesystems
 */
void formatdisk(uint8_t i) {
  puts("Format not implemented on this platform.");
}

/* 
 *  The buffer code, a simple buffer to store output and 
 *  input data. It can be used as a device in BASIC using the 
 *  modifier &0. 
 */

/* use the input buffer variable from BASIC here, it is extern to runtime */
void bufferbegin() {}

/* write to the buffer, works only until 127 
  uses vt52 style commands to handle the buffer content*/
void bufferwrite(char c) {
  if (!nullbuffer) return;
  switch (c) {
  case 12: /* clear screen */
    nullbuffer[nullbuffer[0]+1]=0;
    nullbuffer[0]=0;
    break;
  case 10: 
  case 13: /* cr and lf ignored */
    break;
  case 8: /* backspace */
    if (nullbuffer[0]>0) nullbuffer[0]--;
    break;
  default:
    if (nullbuffer[0] < nullbufsize-1 && nullbuffer[0] < 127) {
      nullbuffer[++nullbuffer[0]]=c;
      nullbuffer[nullbuffer[0]+1]=0; /* null terminate */
    }    
    break;
  }
}

/* read not needed right now */
char bufferread() { return 0; }
uint16_t bufferavailable() { return 0; }
char buffercheckch() { return 0; }
void bufferflush() { }
uint16_t bufferins(char *b, uint16_t nb) { return 0; }

/*
 * Primary serial code, if NONBLOCKING is set, 
 * platform dependent I/O is used. This means that 
 * UNIXes use fcntl() to implement a serialcheckch
 * and MSDOS as well als WIndows use kbhit(). 
 * This serves only to interrupt programs with 
 * BREAKCHAR at the moment. 
 */
#ifdef POSIXNONBLOCKING
#if !defined(MSDOS) && !defined(MINGW)
#include <fcntl.h>

/* we need to poll the serial port in non blocking mode 
    this slows it down so that we don't block an entire core 
    read speed here is one character per millisecond which
    is 8000 baud, no one can type that fast but tedious when
    from stdin */
/*
void freecpu() {
  struct timespec intervall;
  struct timespec rtmp;
  intervall.tv_sec=0;
  intervall.tv_nsec=1000000;
  nanosleep(&intervall, &rtmp);
}
*/

/* for non blocking I/O try to modify the stdin file descriptor */
void serialbegin() {
/* we keep I/O mostly blocking here */
/*
 fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
*/
}

/* get and unget the character in a non blocking way */
char serialcheckch(){ 
  fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
  int ch=getchar();
  ungetc(ch, stdin);
  fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_NONBLOCK);
  return ch;
}

/* check EOF, don't use feof()) here */
uint16_t serialavailable() { 
 if (cheof(serialcheckch())) return 0; else return 1;
}

/* two versions of serialread */
char serialread() { 
  char ch;
/* blocking to let the OS handle the wait - this means: no call to byield() in interaction */
  ch=getchar();
  return ch;
/* this is the code that waits - calls byield() often just like on the Arduino */
/*
  while (cheof(serialcheckch())) { byield(); freecpu(); }
  return getchar(); 
*/
}

/* flushes the serial code in non blocking mode */
void serialflush() {
  fcntl(0, F_SETFL, fcntl(0, F_GETFL) | O_NONBLOCK);
  while (!cheof(getchar()));
  fcntl(0, F_SETFL, fcntl(0, F_GETFL) & ~O_NONBLOCK);
}
#else
/* the non blocking MSDOS and MINGW code */
#include <conio.h>
/* we go way back in time here and do it like DOS did it */ 
void serialbegin(){}

/* we go through the terminal on read */
char serialread() { 
  return getchar();
}

/* check if a key is hit, get it and return it */
char serialcheckch(){ 
  if (kbhit()) return getch();
}

/* simple version */
uint16_t serialavailable() {
  return 1;
}

/* simple version */
void serialflush() { }

#endif
#else 
/* the blocking code only uses puchar and getchar */
void serialbegin(){}
char serialread() { return getchar(); }
char serialcheckch(){ return 1; }
uint16_t serialavailable() { return 1; }
void serialflush() {}
#endif

uint8_t serialstat(uint8_t c) {
  if (c == 0) return 1;
  if (c == 1) return serial_baudrate/1000;
  return 0;
}

/* send the CSI sequence to start with ANSI */
void sendcsi() {
  putchar(27); putchar('['); /* CSI */
}

/* the vt52 state engine */
#ifdef POSIXVT52TOANSI
#include <stdlib.h>
uint8_t dspesc = 0;
uint8_t vt52s = 0;
int cursory = 0;
uint8_t vt52active = 1;

/* something little */
uint8_t vt52number(char c) {
  uint8_t b=c;
  if (b>31) return b-32; else return 0;
}

/* set the cursor */
void dspsetcursory(uint8_t i) {
  cursory=i;
}

/* remember the position */
void dspsetcursorx(uint8_t i) {
  sendcsi();
  printf("%d;%dH", abs(cursory)+1, i+1);
}

/* set colors, vga here */
void dspsetfgcolor(uint8_t co) {
  sendcsi();
  if (co < 8) {
    putchar('3');
  } else {
    putchar('9');
    co=co-8;
  }
  putchar('0'+co);
  putchar('m');
}

void dspsetbgcolor(uint8_t co) {
  sendcsi();
  if (co < 8) {
    putchar('4');
  } else {
    putchar('1'); putchar('0');
    co=co-8;
  }
  putchar('0'+co);
  putchar('m');
}

/* vt52 state engine, a smaller version of the Arduino code*/
void dspvt52(char* c){
  
/* reading and processing multi byte commands */
  switch (vt52s) {
    case 'Y':
      if (dspesc == 2) { 
        dspsetcursory(vt52number(*c));
        dspesc=1; 
        *c=0;
        return;
      }
      if (dspesc == 1) { 
        dspsetcursorx(vt52number(*c)); 
        *c=0; 
      }
      vt52s=0; 
      break;
    case 'b':
      dspsetfgcolor(vt52number(*c));
      *c=0;
      vt52s=0;
      break;
    case 'c':
      dspsetbgcolor(vt52number(*c));
      *c=0;
      vt52s=0;
      break;
  }
 
/* commands of the terminal in text mode */
  switch (*c) {
    case 'v': /* GEMDOS / TOS extension enable wrap */
      break;
    case 'w': /* GEMDOS / TOS extension disable wrap */
      break;
    case '^': /* Printer extensions - print on */
      break;
    case '_': /* Printer extensions - print off */
      break;
    case 'W': /* Printer extensions - print without display on */
      break;
    case 'X': /* Printer extensions - print without display off */
      break;
    case 'V': /* Printer extensions - print cursor line */
      break;
    case ']': /* Printer extension - print screen */
      break;
    case 'F': /* enter graphics mode */
      break;
    case 'G': /* exit graphics mode */
      break;
    case 'Z': // Ident 
      break;
    case '=': // alternate keypad on 
    case '>': // alternate keypad off 
      break;
    case 'b': // GEMDOS / TOS extension text color 
    case 'c': // GEMDOS / TOS extension background color 
      vt52s=*c;
      dspesc=1;
      *c=0;
      return;
    case 'e': // GEMDOS / TOS extension enable cursor
      break;
    case 'f': // GEMDOS / TOS extension disable cursor 
      break;
    case 'p': // GEMDOS / TOS extension reverse video 
      break;
    case 'q': // GEMDOS / TOS extension normal video 
      break;
    case 'A': // cursor up
      sendcsi();
      putchar('A');
      break;
    case 'B': // cursor down 
      sendcsi();
      putchar('B');
      break;
    case 'C': // cursor right 
      sendcsi();
      putchar('C');
      break; 
    case 'D': // cursor left 
      sendcsi();
      putchar('D');
      break;
    case 'E': // GEMDOS / TOS extension clear screen 
      *c=12; 
      dspesc=0;
      return;
    case 'H': // cursor home 
      *c=2;
      dspesc=0;
      return;  
    case 'Y': // Set cursor position 
      vt52s='Y';
      dspesc=2;
      *c=0;
      return;
    case 'J': // clear to end of screen 
      sendcsi();
      putchar('J');
      break;
    case 'd': // GEMDOS / TOS extension clear to start of screen 
      sendcsi();
      putchar('1'); putchar('J');
      break;
    case 'K': // clear to the end of line
      sendcsi();
      putchar('K');
      break;
    case 'l': // GEMDOS / TOS extension clear line 
      sendcsi();
      putchar('2'); putchar('K');
      break;
    case 'o': // GEMDOS / TOS extension clear to start of line
      sendcsi();
      putchar('1'); putchar('K');
      break;
    case 'k': // GEMDOS / TOS extension restore cursor
      break;
    case 'j': // GEMDOS / TOS extension save cursor
      break;
    case 'I': // reverse line feed
      putchar(27);
      putchar('M');
      break;
    case 'L': // Insert line
      break;
    case 'M': // Delete line - questionable 
      sendcsi();
      putchar('2'); putchar('K');
      break;
  }
  dspesc=0;
  *c=0;
}
#endif


uint16_t serialins(char* b, uint16_t nb) { return consins(b, nb); }

void serialwrite(char c) { 

/* the vt52 state engine */
#ifdef POSIXVT52TOANSI
  if (dspesc) { 
    dspvt52(&c); 
    if (c == 0) return;
  }

/* ESC is caught here and we only listen to VT52 not to ANSI */
  if (c == 27 && vt52active) {
    dspesc=1;
    return;
  }
#endif

/* this is the character translation routine to convert the Arduino 
  style characters 12 for CLS and 2 for HOME to ANSI, makes 
  BASIC programs more compatible */
#ifdef POSIXTERMINAL
  switch (c) {
/* form feed is clear screen - compatibility with Arduino code */
    case 12:
      sendcsi();
      putchar('2'); putchar('J');
/* home sequence in the arduino code */
    case 2: 
      sendcsi();
      putchar('H');
      return;
  }
#endif

/* finally send the plain character */  
  putchar(c);
}

/* 
 * handling the second serial interface - only done on Mac so far 
 * test code
 * 
 * Tried to learn from https://www.pololu.com/docs/0J73/15.5
 *
 */
#ifdef POSIXPRT
#include <fcntl.h>
#if !defined(MSDOS) && !defined(MINGW)
#include <termios.h>
#endif

/* the file name of the printer port */
int prtfile;

/* the buffer to read one character */
char prtbuf = 0;


void prtbegin() {}

char prtopen(char* filename, uint16_t mode) {
#if !defined(MSDOS) && !defined(MINGW)

/* try to open the device file */
  prtfile=open(filename, O_RDWR | O_NOCTTY);
  if (prtfile == -1) {
    perror(filename);
    return 0;
  } 

/* get rid of garbage */
  tcflush(prtfile, TCIOFLUSH);

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
  switch (mode) {
    case 9600: 
      cfsetospeed(&opt, B9600);
      break;
    default:
      cfsetospeed(&opt, B9600);
      break;
  }
  cfsetispeed(&opt, cfgetospeed(&opt));

/* set the termin attributes */
  tcsetattr(prtfile, TCSANOW, &opt);
#endif

  return 1;
}

void prtclose() {
  if (prtfile) close(prtfile);
}

uint8_t prtstat(uint8_t c) {return 1; }

void prtset(uint32_t s) {}

/* write the characters byte by byte */
void prtwrite(char c) {
  int i=write(prtfile, &c, 1);
  if (i != 1) ioer=1;
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
      ioer=1;
      return 0;
    } 
    if (i == 0) return -1;
  }
  return c;
}

/* not yet implemented */
char prtcheckch(){ 
  if (!prtbuf) { /* try to read */
    int i=read(prtfile, &prtbuf, 1);
    if (i <= 0) prtbuf=0;
  }
  return prtbuf; 
}

uint16_t prtavailable(){ 
  return prtcheckch()!=0; 
}

uint16_t prtins(char* b, uint16_t nb) {
    if (blockmode > 0) return inb(b, nb); else return consins(b, nb);
}

#else
void prtbegin() {}
uint8_t prtstat(uint8_t c) {return 0; }
void prtset(uint32_t s) {}
void prtwrite(char c) {}
char prtread() {return 0;}
char prtcheckch(){ return 0; }
uint16_t prtavailable(){ return 0; }
uint16_t prtins(char* b, uint16_t nb) { return 0; }
#endif


/* 
 * The wire code 
 */ 
#if defined(POSIXWIRE) && defined(POSIXPIGPIO)
uint8_t wire_slaveid = 0;

/* open the wire connection in pigpio */
void wirebegin() {
}

/* we return the handle here, inconsistent with the Arduino code */
uint8_t wirestat(uint8_t c) {
  return 1; 
}

void wireopen(char s, uint8_t m) {
  if (m == 0) {
    wire_slaveid=s;
  } else if ( m == 1 ) { 
    outsc("** wire slave mode not implemented"); outcr();
  } else 
    error(EORANGE);
}


/* read a number of bytes, depending on the string length */
uint16_t wireins(char *b, uint8_t l) {
  int handle;
  int16_t z;

  handle=i2c_open(pigpio_pi, POSIXI2CBUS, wire_slaveid, 0);
  if (handle < 0) { 
    printf("** wire handle %d returned \n", handle);
    ioer=1; 
  }

  z=i2c_read_device(pigpio_pi, handle, b+1, l);

  if (z < 0) {
    ioer=-1;
    z=0;
  } 
  b[0]=z;

  i2c_close(pigpio_pi, handle);

  return z;
}


void wireouts(char *b, uint8_t l) {
  int handle;

  handle=i2c_open(pigpio_pi, POSIXI2CBUS, wire_slaveid, 0);
  if (handle < 0) { 
    printf("** wire handle %d returned \n", handle);
    ioer=1; 
  }

  if (i2c_write_device(pigpio_pi, handle, b, l) < 0) ioer=-1;

  i2c_close(pigpio_pi, handle);
}

uint16_t wireavailable() { return 1; }

/* the register access functions */
int16_t wirereadbyte(uint8_t port) { 
  int res, handle;
  handle=i2c_open(pigpio_pi, POSIXI2CBUS, port, 0);
  if (handle < 0) { 
    printf("** wire handle %d returned \n", handle);
    ioer=1; 
    return -1; 
  }
  
  res=i2c_read_byte(pigpio_pi, handle);
  i2c_close(pigpio_pi, handle);
  return res; 
}

/* use the simple wire byte function */ 
void wirewritebyte(uint8_t port, int16_t data) { 
  int res, handle;
  handle=i2c_open(pigpio_pi, POSIXI2CBUS, port, 0);
  if (handle < 0) { ioer=1; return; }
  
  ioer=i2c_write_byte(pigpio_pi, handle, data);

  i2c_close(pigpio_pi, handle);
}


/* this code used the write byte function twice */
/*
void wirewriteword(short port, short data1, short data2) { 
  int res, handle;
  handle=i2c_open(pigpio_pi, POSIXI2CBUS, port, 0);
  if (handle < 0) { ioer=1; return; }
  
  ioer=i2c_write_byte(pigpio_pi, handle, data1);
  ioer+=i2c_write_byte(pigpio_pi, handle, data2);

  i2c_close(pigpio_pi, handle);
}
*/

/* use the raw access function in a buffer */
void wirewriteword(short port, short data1, short data2) { 
  int res, handle;
  mem_t buf[2];

  handle=i2c_open(pigpio_pi, POSIXI2CBUS, port, 0);
  if (handle < 0) { ioer=1; return; }

  buf[0]=data1;
  buf[1]=data2;

  if (i2c_write_device(pigpio_pi, handle, buf, 2) <0 ) ioer=-1;

  i2c_close(pigpio_pi, handle);
}

/* just a helper to make GET work, wire is string oriented */
char wireread() {
    char wbuffer[2];
    if wireins(wbuffer, 1) return wbuffer[1]; else return 0;
}

#else
void wirebegin() {}
uint8_t wirestat(uint8_t c) {return 0; }
void wireopen(char s, uint8_t m) {}
uint16_t wireins(char *b, uint8_t l) { b[0]=0; return 0; }
void wireouts(char *b, uint8_t l) {}
uint16_t wireavailable() { return 1; }
int16_t wirereadbyte(uint8_t port) { return 0; }
void wirewritebyte(uint8_t  port, int16_t data) { return; }
void wirewriteword(uint8_t port, int16_t data1, int16_t data2) { return; }
/* just a helper to make GET work, wire is string oriented */
char wireread() { return 0; }
#endif

/* 
 *  Read from the radio interface, radio is always block 
 *  oriented. 
 */
uint8_t radiostat(uint8_t c) {return 0; }
void radioset(uint8_t s) {}
uint16_t radioins(char *b, uint8_t nb) { b[0]=0; b[1]=0; return 0; }
void radioouts(char *b, uint8_t l) {}
void iradioopen(const char *filename) {}
void oradioopen(const char *filename) {}
uint16_t radioavailable() { return 0; }
char radioread() { return 0; }

/* Arduino sensors */
void sensorbegin() {}
float sensorread(uint8_t s, uint8_t v) {return 0;};


/* 
 *  event handling wrappers, to keep Arduino specifics out of BASIC
 */

uint8_t pintointerrupt(uint8_t pin) { return 0; }
void attachinterrupt(uint8_t inter, void (*f)(), uint8_t mode) {}
void detachinterrupt(uint8_t pin) {}

/*
 * Experimental code to simulate 64kb SPI SRAM modules
 * 
 * currently used to test the string code of the mem 
 * interface
 *
 */

#ifdef SPIRAMSIMULATOR

static int8_t spiram[65536];

/* the RAM begin method sets the RAM to byte mode */
uint16_t spirambegin() {
  return 65534;
}

/* the simple unbuffered byte write, with a cast to signed char */
void spiramrawwrite(uint16_t a, int8_t c) {spiram[a]=c;}

/* the simple unbuffered byte read, with a cast to signed char */
int8_t spiramrawread(uint16_t a) {return spiram[a];}

/* the buffers calls, also only simulated here */

void spiram_rwbufferwrite(uint16_t a, int8_t c) {spiram[a]=c;}

int8_t spiram_rwbufferread(uint16_t a) {return spiram[a];}

int8_t spiram_robufferread(uint16_t a) {return spiram[a];}

/* to handle strings in SPIRAM situations two more buffers are needed 
 * they store intermediate results of string operations. The buffersize 
 * limits the maximum string length indepents of how big strings are set
 */
char spistrbuf1[SPIRAMSBSIZE];
char spistrbuf2[SPIRAMSBSIZE];
#endif

/* 
 * This code measures the fast ticker frequency in microseconds 
 * Activate this only for test purposes. Not really useful on POSIX.
 */

#ifdef FASTTICKERPROFILE
uint32_t lastfasttick = 0;
uint32_t fasttickcalls = 0;
uint16_t avgfasttick = 0;
long devfasttick = 0;

void fasttickerprofile() {
  int delta;
  if (lastfasttick == 0) { lastfasttick=micros(); return; }
  delta=micros()-lastfasttick;
  lastfasttick=micros();
  avgfasttick=(avgfasttick*fasttickcalls+delta)/(fasttickcalls+1);
  fasttickcalls++; 
}
#endif

