/*
 *
 * $Id: hardware.h,v 1.1 2023/08/26 08:23:23 stefan Exp stefan $
 *
 * Stefan's basic interpreter 
 *
 * See the licence file on 
 * https://github.com/slviajero/tinybasic for copyright/left.
 *   (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 * Author: Stefan Lenz, sl001@serverfabrik.de
 * 
 *
 * Runtime environment for the BASIC interpreter, the 
 * hardware configuration file. It contains all hardware 
 * related settings and dependencies.
 *
 * This is hardware.h for POSIX systems and the POSIX runtime
 * environment runtime.c. 
 * 
 * Default setting is POSIX for most UNIXes including Mac.
 * RASPPI activates Raspberry PI related features. 
 * MINGW activates windows related features.
 * MSDOS for MSDOS compiles with tc 2.0 (untested in this version, last tested in 1.4)
 *
 */

#undef MINGW
#undef MSDOS
#undef RASPPI
#define POSIX

/* 
 * Hardware flags of the POSIX systems 
 * POSIXTERMINAL, POSIXVT52TOANSI: ensure compatibility of BASIC programs  
 *  control characters of BASIC are translated to ANSI, bringing the Aruino 
 *  VT52 commands to POSIX
 * POSIXSIGNALS: enables signal handling of ^C interrupting programs
 * POSIXNONBLOCKING: non blocking I/O to handle GET and the BREAKCHAR 
 *  tricky on DOS, not very portable, experimental, use signals instead
 * POSIXFRAMEBUFFER: directly draw to the frame buffer of Raspberry PI
 *  only tested on this platform
 * POSIXWIRE: simple Raspberry PI wire code
 * POSIXMQTT: analogous to ARDUINOMQTT, send and receive MQTT messages (unfinished)
 * POSIXWIRING: use the (deprectated) wiring code for gpio on Raspberry Pi
 * POSIXPIGPIO: use the pigpio library on a Raspberry PI 
 */

#define POSIXTERMINAL
#define POSIXVT52TOANSI
#define POSIXSIGNALS
#undef POSIXNONBLOCKING
#undef POSIXFRAMEBUFFER
#undef POSIXWIRE
#undef POSIXMQTT
#undef POSIXWIRING
#undef POSIXPIGPIO

/* simulates SPI RAM, only test code, keep undefed if you don't want to do something special */
#undef SPIRAMSIMULATOR

/* use a serial port as printer interface - unfinished - similar to Arduino */
#define POSIXPRT

/* use TAB in MS mode */
#define ARDUINOMSTAB

/* used pins and other parameters */

/* set this is you want pin 4 on low interrupting the interpreter */
/* #define BREAKPIN 4 */
#undef BREAKPIN

/* the SIGNAL the interpreters listens to for interrupt */
#define BREAKSIGNAL SIGINT

/* in case of non blocking IO turn on background tasks */
#ifdef POSIXNONBLOCKING
#define BASICBGTASK
#endif

/* frame buffer health check - currently only supported on Raspberry */ 
#ifndef RASPPI
#undef POSIXFRAMEBUFFER
#endif

/* wire parameters for Raspberry*/
#define POSIXI2CBUS 1

/* the size of the EEPROM dummy */
#define EEPROMSIZE 1024

/* they all have this */
#define FILESYSTEMDRIVER

/* and they have this */
#define HASCLOCK

/* some have this */
#if defined(POSIXWIRE) && defined(POSIXPIGPIO)
#define HASWIRE
#endif

/* we can do graphics and tell language.h that we can */
#ifdef POSIXFRAMEBUFFER
#define DISPLAYHASGRAPH
#endif

/* after run behaviour on POSIX systems, 1 to terminate if started
    on the command line with a file argument, 0 to stay active and 
    show a BASIC prompt*/
#define TERMINATEAFTERRUN 1

/* 
 * all the things the POSIX platforms need to source 
 */

/* define all the types Arduino has for compatibility of the interface */

typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef signed short int16_t;
typedef unsigned char byte;
typedef unsigned long long uint64_t;

/*
 * additional prototypes in an non Arduino world 
 */
unsigned long millis();

/* the ususal suspects */
#include <stdio.h>
#include <stdlib.h>

/* floating point maths only needed if HASFLOAT*/
#include <math.h>
#include <float.h>

/* time stuff */
#include <time.h>
#include <sys/types.h>
#include <sys/timeb.h>

/* directories and files */
#ifndef MSDOS
#include <dirent.h>
#include <unistd.h>
#else
#include <dir.h>
#include <dos.h>
#endif

/* windowy things for windows */
#ifdef MINGW
#include <windows.h>
#endif

/* 
 *  IF ARDUINOMSTAB is not set, make this known
 *  underlying terminal cannot handle tabs.
 */
#ifndef ARDUINOMSTAB
#undef TERMHASMSTAB
#endif

/* 
 *  Tell BASIC we have a second serial port
 */
#ifdef POSIXPRT
#define HASSERIAL1
#endif

/* 
 *  Tell BASIC we have a radio adapter
 */
#ifdef ARDUINORF24
#define HASRF24
#endif

/* 
 *  Tell BASIC we have MQTT
 */
#ifdef POSIXMQTT
#define HASMQTT
#endif

/* we definitely don't run on an Arduino */
#undef ARDUINOPROGMEM
#define PROGMEM 
#undef ARDUINO

/* and we use the buffer sizes for real computers */
#define BUFSIZE         256
#define STACKSIZE       256
#define GOSUBDEPTH      64
#define FORDEPTH        64
#define LINECACHESIZE   64

/* the buffer size for simulated serial RAM */
#define SPIRAMSBSIZE 512
