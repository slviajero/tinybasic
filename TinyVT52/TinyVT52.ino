#define HASVT52
#define MEMSIZE 128

#include "basic.h"
#include "hardware-arduino.h"

/* the inch routine - generates on incoming character stream - taken from BASIC */
char inch() {
  switch(id) {
    case ISERIAL:
      return serialread();    
#ifdef ARDUINOPRT
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
  }
  return 0;
}

/* checking on a character in the stream */
char checkch(){
  switch (id) {
    case ISERIAL:
      return serialcheckch();
#ifdef ARDUINOPRT
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
short availch(){
  switch (id) {
    case ISERIAL:
      return serialavailable(); 
#ifdef ARDUINOPRT
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
 * outch() outputs one character to a stream
 * block oriented i/o like in radio not implemented here
 */
void outch(char c) {
  switch(od) {
    case OSERIAL:
      serialwrite(c);
      break;
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
    default:
      break;
  }
  byield(); /* yield after every character for ESP8266 */
}


void setup() {
  serialbegin();
  dspbegin();  
}

  
void loop() {
  char ch;

/* first test, just use the serial port as in */
  
/* read from the serial port and write to the display */ 
    id=OSERIAL;
    od=ODSP;

/* send all characters we have to the display */
  while (availch()) {
    ch=inch();
    outch(ch);
  }

/* see if the display has something to say, this is relevant for id but also for keypads */
  id=IKEYBOARD;
  od=OSERIAL;

/* send all characters from the display to the serial stream */
  while (availch()) {
    ch=inch();
    outch(ch);
  } 
}
