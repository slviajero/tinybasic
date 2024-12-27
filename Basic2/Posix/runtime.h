/*
 *
 * $Id: runtime.h,v 1.1 2024/02/25 04:43:16 stefan Exp stefan $
 *
 * Stefan's basic interpreter 
 *
 * Prototypes for the runtime environment of the BASIC interpreter.
 *
 * Needs to be included by runtime.c or runtime.cpp and basic.c
 * 
 * Parametrized by hardware.h.
 *
 * Author: Stefan Lenz, sl001@serverfabrik.de
 * 
 */

#if !defined(__RUNTIMEH__)
#define __RUNTIMEH__ 

/* we debug the runtime library */
#define RTDEBUG 1

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
 *  Input and output channels.
    *  The channels are used to identify the I/O devices in the
    * runtime environment. 
    * 
    * NULL is the memory channel outputting to a buffer.
    * SERIAL is the standard serial channel and the default device. 
    * DSP is the display channel.
    * PRT is the second serial channel used for printing and communication
    *   with external devices.
    * WIRE is the I2C channel.
    * RADIO is the RF24 channel.
    * MQTT is the MQTT channel.
    * FILE is the file system channel.
 */
#define ONULL 0
#define OSERIAL 1
#define ODSP 2
#define OPRT 4
#define OWIRE 7
#define ORADIO 8
#define OMQTT  9
#define OFILE 16

#define INULL 0
#define ISERIAL 1
#define IKEYBOARD 2
#define ISERIAL1 4
#define IWIRE 7
#define IRADIO 8
#define IMQTT  9
#define IFILE 16

/* 
 *  The main IO interface. This is how BASIC uses I/O functions
 *
 * ioinit(): called at setup to initialize what ever io is needed
 * outch(): prints one ascii character 
 * inch(): gets one character (and waits for it)
 * checkch(): checks for one character (non blocking)
 * ins(): reads an entire line (uses inch except for pioserial)
 *
 */
void ioinit();
void iodefaults();
int cheof(int);
char inch();
char checkch();
uint16_t availch();
uint16_t inb(char*, int16_t);
uint16_t ins(char*, uint16_t);
void outch(char);
void outs(char*, uint16_t);

/* 
 *  Global variables of the runtime env, visible to BASIC
 */
extern int8_t id; /* active input stream */
extern int8_t od; /* active output stream */
extern int8_t idd; /* default input stream in interactive mode */
extern int8_t odd; /* default output stream in interactive mode */
extern int8_t ioer; /* the io error */

/* io control flags */
extern uint8_t kbdrepeat;
extern uint8_t blockmode;
extern uint8_t sendcr;

/* breaks */
extern char breakcondition;

/* counts the outputed characters on streams 0-3, used to emulate a real tab */
extern uint8_t charcount[3]; /* devices 1-4 support tabing */

/* the memory buffer comes from BASIC in this version */
extern char ibuffer[BUFSIZE]; /* the input buffer */

/* only needed in POSIX worlds */
extern uint8_t breaksignal; 
extern uint8_t vt52active;

/* the string buffer the interpreter needs, here to be known by BASIC */
extern char spistrbuf1[SPIRAMSBSIZE], spistrbuf2[SPIRAMSBSIZE];

/* the mqtt variable the interpreter needs */
#define MQTTLENGTH 32
extern char mqtt_otopic[MQTTLENGTH];
extern char mqtt_itopic[MQTTLENGTH];
extern char mqttname[];

/* 
 * accessing the fastticker information
 */
extern uint16_t avgfasttick;

/* 
 * A byte in the runtime memory containing the system type
 */
extern uint8_t bsystype;

/*
 * These functions are always empty on Arduino, they are only used in 
 * the POSIX branch of the code.
 * 
 * BASIC calls these functions once to start the timing, wiring, and signal handling.
 */

void timeinit();	/* handling time - part of the Arduino core - only needed on POSIX OSes */
void wiringbegin();	/* starting wiring is only needed on raspberry */
void signalon();	/* POSIX signals - not needed on Ardunino */

/*
 * Memory allocation functions.
 * 
 * BASIC calls freememorysize() to detemine how much memory can be allocated 
 *  savely on the heap.
 * BASIC calls restartsystem() for a complete reboot. 
 * freeRam() is the actual free heap. Used in BASIC only in USR.
 * 
 * Arduino data from https://docs.arduino.cc/learn/programming/memory-guide
 */

long freememorysize(); /* determine how much actually to allocate */
void restartsystem(); /* cold start of the MCU */
long freeRam();	/* try to find the free heap after alocating all globals */

/* This is unfinished, don't use, sleep and RTC interrupt code. */ 
void rtcsqw();
void aftersleepinterrupt(void);
void activatesleep(long);

/* 
 * Start the SPI bus. Called once on start. 
 * 
 * Some libraries also try to start the SPI which may lead to on override of 
 * the PIN settings if the library code is not clean - currenty no conflict known.
 * for the libraries used here.
 */
void spibegin();

/* 
 *  Timeing functions and background tasks. 
 *  
 *  byield() is called after every statement and in all 
 *  waiting loops for I/O. BASIC gives back the unneeded 
 *  CPU cycles by calling byield().
 *  
 *  byield() it allows three levels of background tasks. 
 *
 *  BASICBGTASK controls if time for background tasks is 
 *  needed, usually set by hardware features
 *
 *  YIELDINTERVAL by default is 32, generating a 32 ms call
 *    to the network loop function.
 *
 *  LONGYIELDINTERVAL by default is 1000, generating a one second
 *    call to maintenance functions.
 *    
 *  fastticker() is called in every byield for fast I/O control.
 *    It is currently only used for the tone emulation.
 *    
 *  byield calls back bloop() in the BASIC interpreter for user 
 *    defined background tasks.
 *
 * the time intervall in ms needed for 
 *  ESP8266 yields, network client loops 
 *  and other timing related functions 
 */
#define LONGYIELDINTERVAL 1000
#define YIELDINTERVAL 32

void byield(); /* the yield function called in empty loops */
void bdelay(uint32_t); /* a delay function using byield() */
void fastticker(); /* a function for very frequent background tasks */
void yieldfunction(); /* everything that needs to be done often - 32 ms */
void longyieldfunction(); /* everything that needs to be done not so often - 1 second */
void yieldschedule(); /* scheduler call for some platforms */

/* 
 *  EEPROM handling, these function enable the @E array and 
 *  loading and saving to EEPROM with the "!" mechanism
 * 
 * The EEPROM code can address EEPROMS up to 64 kB. It returns 
 * signed byte values which corresponds to the definition of 
 * mem_t in BASIC. This is needed because running from EEPROM
 * requires negative token values to be recongized. 
 * 
 */ 
void ebegin(); 
void eflush();
uint16_t elength();
void eupdate(uint16_t, int8_t);
int8_t eread(uint16_t);

/* 
 *  The wrappers of the arduino io functions.
 *  
 *  The normalize the differences of some of the Arduino cores 
 *  and raspberyy PI wiring implementations.
 *  
 *  pulseout generates microsecond pulses. 
 *  
 *  awrite requires ESP32 2.0.2 core, else disable awrite().
 */ 
uint16_t aread(uint8_t);
uint8_t dread(uint8_t);
void awrite(uint8_t, uint16_t);
void dwrite(uint8_t, uint8_t);
void pinm(uint8_t, uint8_t);
uint32_t pulsein(uint8_t, uint8_t, uint32_t);
void pulseout(uint16_t, uint8_t, uint16_t, uint16_t, uint16_t, uint16_t);
void playtone(uint8_t, uint16_t, uint16_t, uint8_t);
void tonetoggle(); /* internal function of the tone emulation, called by byield */

void breakpinbegin();
uint8_t getbreakpin();

/*
 * DISPLAY driver code section, the hardware models define a set of 
 * of functions and definitions needed for the display driver. These are 
 *
 * dsp_rows, dsp_columns: size of the display
 * dspbegin(), dspprintchar(c, col, row), dspclear(), dspupdate()
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
 * 
 * For non RGB ready displays, rgbcolor translates to the native color
 * when BASIC requests an rgb color, in this case the nearest 4 bit 
 * color of the display is also stored for use in the text DISPLAY 
 * driver code
 */

 /* generate a 4 bit vga color from a given rgb color */
uint8_t rgbtovga(uint8_t, uint8_t, uint8_t);

/* 
 * prototypes for screen handling
 */
void dspbegin(); 
void dspprintchar(char, uint8_t, uint8_t);
void dspclear();
void dspupdate();
void dspsetcursor(uint8_t);
void dspsavepen();
void dsprestorepen();
void dspsetfgcolor(uint8_t);
void dspsetbgcolor(uint8_t);
void dspsetreverse(uint8_t);
uint8_t dspident();
void rgbcolor(uint8_t, uint8_t, uint8_t);
void vgacolor(uint8_t);
void plot(int, int);
void line(int, int, int, int);
void rect(int, int, int, int);
void frect(int, int, int, int);
void circle(int, int, int);
void fcircle(int, int, int);

/*
 * this is a generic display code 
 * it combines the functions of LCD and TFT drivers
 * if this code is active 
 *
 * dspprintchar(char c, uint8_t col, uint8_t row)
 * dspclear()
 * dspbegin()
 * dspupdate()
 * dspsetcursor(uint8_t c) 
 * dspsetfgcolor(uint8_t c)  
 * void dspsetbgcolor(uint8_t c)  
 * void dspsetreverse(uint8_t c)  
 * uint8_t dspident()  
 *
 * have to be defined before in a hardware dependent section.
 * Only dspprintchar and dspclear are needed, all other can be stubs
 *
 * VGA systems don't use the display driver for text based output.
 *
 * The display driver exists as a buffered version that can scroll
 * or an unbuffered version that cannot scroll. Interfaces to hardware
 * scrolling are not yet implemented.
 * 
 * A VT52 state engine is implemented and works for buffered and 
 * unbuffered displays. Only buffered displays have the full VT52
 * feature set including most of the GEMDOS extensions described here:
 * https://en.wikipedia.org/wiki/VT52
 * 
 * dspupdatemode controls the page update behaviour 
 *    0: character mode, display each character separately
 *    1: line mode, update the display after each line
 *    2: page mode, update the display after an ETX
 * ignored if the display has no update function
 *
 */

void dspsetcursorx(uint8_t);
void dspsetcursory(uint8_t);
uint8_t dspgetcursorx();
uint8_t dspgetcursory();
void dspbell(); /* to whom the bell tolls - implement this to you own liking */


/* 
 * text color code for the scrolling display 
 * 
 * non scrolling displays simply use the pen color of the display
 * stored in dspfgcolor to paint the information on the screen
 * 
 * for scrolling displays we store the color information of every
 * character in the display buffer to enable scrolling, to limit the 
 * storage requirements, this code translates the color to a 4 bit VGA
 * color. This means that if BASIC uses 24 bit colors, the color may
 * change at scroll
 *
 * for color displays the buffer is a 16 bit object 
 * lower 8 bits plus the sign are the character, 
 * higher 7 the color and font.
 * for monochrome just the character is stored
 */

#ifdef DISPLAYHASCOLOR
typedef short dspbuffer_t;
#else
typedef char dspbuffer_t;
#endif

dspbuffer_t dspget(uint16_t);
dspbuffer_t dspgetrc(uint8_t, uint8_t);
dspbuffer_t dspgetc(uint8_t);

void dspsetxy(dspbuffer_t, uint8_t, uint8_t); /* this functions prints a character and updates the display buffer */
void dspset(uint16_t, dspbuffer_t);
void dspsetscrollmode(uint8_t, uint8_t); /* 0 normal scroll, 1 enable waitonscroll function */

void dspbufferclear(); /* clear the buffer */
void dspscroll(uint8_t, uint8_t); /* do the scroll */
void dspreversescroll(uint8_t); /* do the reverse scroll only one line implemented */

/* 
 * This is the minimalistic VT52 state engine. It is an interface to 
 * process single byte control sequences of the form <ESC> char 
 */

char vt52read(); /* the reader from the buffer, for messages going back from the display */
uint8_t vt52avail(); /* the avail from the buffer */
void vt52push(char); /* putting something into the buffer */
uint8_t vt52number(char); /* something little, generating numbers */
void vt52graphcommand(uint8_t); /* execute one graphics command */

/* 
 * this is a special part of the vt52 code with this, the terminal 
 * can control the digital and analog pins.
 * it is meant for situations where the terminal is controlled by a (powerful)
 * device with no or very few I/O pins. It can use the pins of the Arduino through  
 * the terminal. This works as long as everything stays within the terminals timescale
 * On a 9600 baud interface, the character processing time is 1ms, everything slower 
 * than approximately 10ms can be done through the serial line.
 */
void vt52wiringcommand(uint8_t);

/* the vt52 state engine */
void dspvt52(char*);


/* these functions are used to access the display */
void dspouts(char*, uint16_t);
void dspwrite(char);
uint8_t dspstat(uint8_t);
char dspwaitonscroll();
uint8_t dspactive();
void dspsetupdatemode(uint8_t);
uint8_t dspgetupdatemode(); 
void dspgraphupdate();

/* 
 * code for the VGA system of Fabgl 
 */
void vgabegin(); /* this starts the vga controller and the terminal right now */
int vgastat(uint8_t); /* currently unused */
void vgascale(int*, int*); /* scale the screen size */
void vgawrite(char); 
void vgaend();

/* 
 * Keyboard code for either the Fablib Terminal class or 
 * PS2Keyboard - please note that you need the ESP patched 
 * version here as mentioned above
 * 
 * sets HASKEYBOARD to inform basic about this capability
 * 
 * keyboards can implement 
 * 	kbdbegin()
 * they need to provide
 * 	kbdavailable(), kbdread(), kbdcheckch()
 * the later is for interrupting running BASIC code
 */

void kbdbegin();
uint8_t kbdstat(uint8_t);
uint8_t kbdavailable();
char kbdread();
char kbdcheckch();
uint16_t kbdins(char*, uint16_t);

/* 
 * Arduino Real Time clock. The interface here offers the values as number_t 
 * combining all values. 
 * 
 * The code does not use an RTC library any more all the rtc support is 
 * builtin now. 
 * 
 * A clock must activate the macro #define HASCLOCK to make the clock 
 * available in BASIC.
 * 
 * Four software models are supported
 *  - Built-in clocks of STM32, MKR, and ESP32 are supported by default
 *  - I2C clocks can be activated: DS1307, DS3231, and DS3232 
 *  - A Real Time Clock emulation is possible using millis()
 * 
 * rtcget accesses the internal registers of the clock. 
 * Registers 0-6 are bcd transformed to return 
 * seconds, minutes, hours, day of week, day, month, year
 * 
 * On I2C clocks registers 7-255 are returned as memory cells
 */

/* No begin method needed */
void rtcbegin();
uint16_t rtcget(uint8_t); /* get the time from the registers */
void rtcset(uint8_t, uint16_t);

/* convert the time to a unix time number from https://de.wikipedia.org/wiki/Unixzeit  */
void rtctimetoutime();
void rtcutimetotime();

/*
 * definitions for ESP Wifi and MQTT, super experimental.
 * As networking is only used for MQTT at the moment, 
 * mqtt, Wifi and Ethernet comes all in one. 
 *
 * No encryption/authetication is implemented in MQTT. 
 * Only public, open servers can be used.
 *
 * MQTT topics can only be 32 bytes long.
 * Buffered incoming and outgoing messages can be 128 bytes
 * per default.
 *
 * wifisettings.h is the generic network definition file
 * all network settings are compiled into the code 
 * BASIC cannot change them at runtime.
 */

void mqttsetname(); 
void netbegin();
void netstop();
void netreconnect();
uint8_t netconnected();

/*
 * mqtt event handling in BASIC can be triggered here
 * the prototype uses exactly the Arduino types of the
 * pubsub library.
 * 
 */
void mqttcallback(char*, byte*, unsigned int);

/*
 * mqtt prototypes 
 */
void mqttbegin();
uint8_t mqttstat(uint8_t);
uint8_t mqttreconnect();
uint8_t mqttstate();
void mqttsubscribe(const char*);
void mqttunsubscribe();
void mqttsettopic(const char*);
void mqttouts(const char*, uint16_t);
void mqttwrite(const char);
char mqttread();
uint16_t mqttins(char*, uint16_t);
uint16_t mqttavailable();
char mqttcheckch();

/* 
 *	The file system driver - all methods needed to support BASIC fs access
 * 	Different filesystems need different prefixes and fs objects, these 
 * 	filesystems use the stream API
 */

char* mkfilename(const char*);
const char* rmrootfsprefix(const char*); /* remove the prefix from the filename */
void fsbegin(); 
uint8_t fsstat(uint8_t);

/*
 *	File I/O function on an Arduino
 * 
 * filewrite(), fileread(), fileavailable() as byte access
 * open and close is handled separately by (i/o)file(open/close)
 * only one file can be open for write and read at the same time
 */
void filewrite(char);
char fileread();
int fileavailable(); /* is int because some of the fs do this */
uint8_t ifileopen(const char*);
void ifileclose();
uint8_t ofileopen(const char*, const char*);
void ofileclose();

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
void rootopen();
uint8_t rootnextfile(); 
uint8_t rootisfile();
const char* rootfilename();
uint32_t rootfilesize();
void rootfileclose(); 
void rootclose();
void removefile(const char*);

/*
 * formatting for fdisk of the internal filesystems
 */
void formatdisk(uint8_t);

/*
 * The buffer I/O device. This is a stream to write to a given bufer
 * from BASIC.
 */

void bufferbegin();
uint8_t bufferstat(uint8_t);
void bufferwrite(char);
char bufferread();
char buffercheckch();
uint16_t bufferavailable();
uint16_t bufferins(char*, uint16_t);
void bufferouts(char*, uint16_t);

/*
 *	Primary serial code uses the Serial object or Picoserial
 *
 *	The picoseria an own interrupt function. This is used to fill 
 *  the input buffer directly on read. Write is standard like in 
 *  the serial code.
 *  
 * As echoing is done in the interrupt routine, the code cannot be 
 * used to receive keystrokes from serial and then display the echo
 * directly to a display. To do this the write command in picogetchar
 * would have to be replaced with a outch() that then redirects to the 
 * display driver. This would be very tricky from the timing point of 
 * view if the display driver code is slow. 
 * 
 * The code for the UART control is mostly taken from PicoSerial
 * https://github.com/gitcnd/PicoSerial, originally written by Chris Drake
 * and published under GPL3.0 just like this code
 * 
 */

void picobegin(uint32_t);
void picowrite(char); /* the write code, sending bytes directly to the UART */
uint16_t picoins(char *, uint16_t); /* the ins code of picoserial, called like this in consins */

/* 
 *  picogetchar: this is the interrupt service routine.  It 
 *  recieves a character and feeds it into a buffer and echos it
 *  back. The logic here is that the ins() code sets the buffer 
 *  to the input buffer. Only then the routine starts writing to the 
 *  buffer. Once a newline is received, the length information is set 
 *  and picoa is also set to 1 indicating an available string, this stops
 *  recevieing bytes until the input is processed by the calling code.
 */
void picogetchar(char);

/* 
 * blocking serial single char read for Serial
 * unblocking for Picoserial because it is not 
 * character oriented -> blocking is handled in 
 * consins instead.
 */
char serialread();
void serialbegin();
uint8_t serialstat(uint8_t); /* state information on the serial port */
void serialwrite(char); /* write to a serial stream */
char serialcheckch(); /* check on a character, needed for breaking */
uint16_t serialavailable(); /* avail method, needed for AVAIL() */ 
void serialflush(); /* flush serial */
uint16_t serialins(char*, uint16_t); /* read a line from serial */

/*
 * reading from the console with inch or the picoserial callback
 * this mixes interpreter levels as inch/outch are used here 
 * this code needs to go to the main interpreter section after 
 * thorough rewrite
 */
uint16_t consins(char *, uint16_t);

void prtbegin(); /* second serial port */
char prtopen(char*, uint16_t); /* the open functions are not needed here */
void prtclose();
uint8_t prtstat(uint8_t);
void prtwrite(char);
char prtread();
char prtcheckch();
uint16_t prtavailable();
void prtset(uint32_t);
uint16_t prtins(char*, uint16_t);

/* 
 * The wire code, direct access to wire communication
 * in master mode wire_slaveid is the I2C address bound 
 * to channel &7 and wire_myid is 0
 * in slave mode wire_myid is the devices slave address
 * and wire_slaveid is 0
 * ARDUINOWIREBUFFER is the maximum length of meesages the 
 * underlying library can process. This is 32 for the Wire
 * library
 */ 

void wirebegin();
void wireslavebegin(uint8_t);
uint8_t wirestat(uint8_t); /* wire status - just checks if wire is compiled */
uint16_t wireavailable(); /* available characters - test code ecapsulation prep for slave*/
void wireonreceive(int h); /* eventhandler for received data */
void wireonrequest(); /* event handler for request, deliver the message and forget the buffer */ 

/*
 *	as a master open sets the slave id for the communication
 *	no extra begin while we stay master
 */
void wireopen(char, uint8_t);
char wireread(); /* */
void wirewrite(char c); /* */
uint16_t wireins(char*, uint8_t);  /* input an entire string */
void wireouts(char*, uint8_t); /* send an entire string - truncate radically */
int16_t wirereadbyte(uint8_t);
void wirewritebyte(uint8_t, int16_t);
void wirewriteword(uint8_t, int16_t, int16_t);

/* 
 *	Read from the radio interface, radio is always block 
 *	oriented. This function is called from ins for an entire 
 *	line.
 *
 *	In blockmode the entire message is returned in the 
 *	receiving string while in line mode the length of the 
 *	string is adapted. Blockmode can be used to transfer
 *	binary data.
 */

uint8_t radiostat(uint8_t); 
uint64_t pipeaddr(const char*); /* generate a uint64_t pipe address from the filename string for RF24 */
uint16_t radioins(char*, uint8_t); /* read an entire string */
void radioouts(char *, uint8_t); /* write to radio, no character mode here */
uint16_t radioavailable(); /* radio available */
char radioread();

/* 
 *	we always read from pipe 1 and use pipe 0 for writing, 
 *	the filename is the pipe address, by default the radio 
 *	goes to reading mode after open and is only stopped for 
 *	write
 */
void iradioopen(const char *);
void oradioopen(const char *);
void radioset(uint8_t);

/* 
 *	Arduino Sensor library code 
 *		The sensorread() is a generic function called by 
 *		SENSOR basic function and command. The first argument
 *		is the sensor and the second argument the value.
 *		sensorread(n, 0) checks if the sensorstatus.
 */
void sensorbegin();
float sensorread(uint8_t, uint8_t);

/*
 * prototypes for the interrupt interface
 */
/* some have it and some dont */
#if !(defined(ARDUINO_ARCH_MBED_RP2040) || defined(ARDUINO_ARCH_MBED_NANO) || defined(ARDUINO_ARCH_RENESAS))
typedef int PinStatus;
#endif

uint8_t pintointerrupt(uint8_t);
void attachinterrupt(uint8_t, void (*f)(), uint8_t);
void detachinterrupt(uint8_t);

/*
 * Experimental code to drive SPI SRAM 
 *
 * Currently only the 23LCV512 is implemented, assuming a 
 * 64kB SRAM
 * The code below is taken in part from the SRAMsimple library
 * 
 * two buffers are implemented: 
 * - a ro buffer used by memread, this buffer is mainly reading the token 
 *  stream at runtime. 
 * - a rw buffer used by memread2 and memwrite2, this buffer is mainly accessing
 *  the heap at runtime. In interactive mode this is also the interface to read 
 *  and write program code to memory 
 * 
 */

/* the RAM begin method sets the RAM to sequential mode */
uint16_t spirambegin();
int8_t spiramrawread(uint16_t);
void spiram_bufferread(uint16_t, int8_t*, uint16_t);
void spiram_bufferwrite(uint16_t, int8_t*, uint16_t);
int8_t spiram_robufferread(uint16_t);
void spiram_rwbufferflush(); /* flush the buffer */
int8_t spiram_rwbufferread(uint16_t);
void spiram_rwbufferwrite(uint16_t, int8_t); /* the buffered file write */
void spiramrawwrite(uint16_t, int8_t); /* the simple unbuffered byte write, with a cast to signed char */


/* 
 * This code measures the fast ticker frequency in microseconds 
 * It leaves the data in variable F. Activate this only for test 
 * purposes.
 */
void fasttickerprofile();

// defined RUNTIMEH
#endif
