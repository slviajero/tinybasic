/* 
 * Stefan's XMC EEPROM Library
 *
 * See the licence file on for copyright/left.
 * (GNU GENERAL PUBLIC LICENSE, Version 3, 29 June 2007)
 *
 * Author: Stefan Lenz, sl001@serverfabrik.de
 *  
 *  A poor man's EEPROM emulation in the XMC flash.
 *  We buffer entire pages in RAM and only program the flash is we hve a page fault.
 *  
 *  Flash geometry: 16 bytes per block, 16 blocks per page. Organized in 4096 byte sectors. 
 *  We use one sector i.e. 16 pages as a flat EEPROM data. 
 *  
 *  The whole code is meant primarily to be used with the IoT BASIC interpreter and implements
 *  only a reduced API and a reduced buffering and flash protection algorithm:
 *  
 *  Buffering strategy: 
 *    - One pagebuffer for an entire page. Takes all the writes to the page and keeps it
 *    until a pagefault occurs, i.e. writes go beyond the page, then flush and reload
 *    monitor if the page has changed. Always read the page and then write into the pagebuffer.
 *    - One blockbuffer for read. If the read is outside the page loaded for write, read the
 *    blockbuffer and hand back bytes from it. If the read is inside the page loaded for 
 *    write, satisfy read request from there. 
 *    
 *  The library does not do additional reduction of erase cycles like the more 
 *  sophisticated solutions to. It works well if 
 *    1. you mostly write entire chunks of memory like the BASIC interpreter does on program save
 *    2. when you do frequent read/write you mostly stay in one page like the BASIC interpreter does
 *       on EEPROM variable access
 *  
 */

#include "xmc_flash.h"

#ifndef XMCEEPROMLIB_H
#define XMCEEPROMLIB_H


// we keep this static and outside the class, needed for the memory managenent of one specifiy application
// change this if you find it annoying -> into the class.
static uint8_t xmceeprom_pagebuffer[256];
static uint8_t xmceeprom_blockbuffer[16];

class XMCEEPROM {
public:
    // no constructor needed
    XMCEEPROM(void) {};
    ~XMCEEPROM(void) {};

    // begin is optional, as all the constants are already set for 64 kB flash and a 4kB EEPROM
    void begin(uint16_t eepromsize=4096, uint32_t flashsize=65536) {
      xmcdatasize=(eepromsize & 0xff00); // data size is reduced to full pages 
      xmcflashsize=flashsize; // anything goes here
      xmcdatasector = xmcflashbase + xmcflashsize - xmcdatasize;
    };

    // read - done through the blockbuffer
    uint8_t read(int address) {
      // address in rage ?
      if (address >=0 && address < xmcdatasize) {
        // do we have a page in memory for write and try to read from it 
        if (page != -1 && address/256 == page) return pagebuffer[address%256];
        // if not, read the block we are dealing with into the blockbuffer, if not already there
        if (block == -1 || block != address/16) readblock(address/16);
        // if that worked, satisfy read from the block buffer
        if (error == 0) return blockbuffer[address%16]; else return 255;
      } else {
        error = -1;
        return 255;
      }
    };

    // write a byte, this is really update() of course */
    void write(int address, uint8_t val) {
      // address in rage ?
      if (address >=0 && address < xmcdatasize) {
        // do we have a page fault and valid data to flush
        if (page != -1 && page != address/256) { writepage(); readpage(address/256); }
        // get the requested page if we need to
        if (page == -1) readpage(address/256);
        if (error != 0) return;
        // we have the requested page in the pagebuffer, set the value and mark the page changed
        if (pagebuffer[address%256] != val) {
          // here, one would additionally check, if the change can be done without erase
          // this would mean to check if only bytes that were high go to low. In this case one could 
          // write straight through here and keep the page_changed to 0
          pagebuffer[address%256]=val;
          page_changed=1;
          // if the block buffer overlaps with the page buffer, keep it up to date as well */
          if (block != -1 && address/16 == block) blockbuffer[address%16]=val;
        }
      } else {
        error = -1;
      }
    };

    // this is trivial but helps with compatibility
    void update(int address, uint8_t val) { write(address, val); }
    
    // length is defined in the constants
    uint16_t length() { return xmcdatasize; };
    // commit like in the ESP world - flush the write page 
    void commit() { writepage(); };
    void end() { commit(); };
    int status() { return error; };
 private: 
    // the real internal data type of flash would be words but we do byte
    // the pagebuffer has the page we are writing 
    uint8_t* pagebuffer = xmceeprom_pagebuffer;
    // the page we have in the buffer, -1 means no valid data
    int page=-1;
    // the page changed status 
    int page_changed = 0;
    // the blockbuffer has the block we are reading  
    uint8_t* blockbuffer = xmceeprom_blockbuffer;
    // the block we have loaded for read
    int block=-1;
    // the error status
    int error = 0;
    
    // the geometry of the flash
    const unsigned long xmcflashbase = 0x10001000;    // where does the flash start
    unsigned long xmcflashsize = 0x00010000;    // a 64kB flash
    // static unsigned long xmcflashsize = 0x00008000;  // a 32kB flash by default
    unsigned int  xmcdatasize  = 0x00001000;    // a 4kB data sector - this is the EEPROM size
    unsigned long xmcdatasector = xmcflashbase + xmcflashsize - xmcdatasize; // we use the last sector  

    // read a page into the pagebuffer and set the page variable
    void readpage(uint8_t p) {
        if (p >= 0 && p < 16) {
          XMC_FLASH_ReadBlocks( (uint32_t*) (xmcdatasector+p*256), (uint32_t*) pagebuffer, 16);
          if ((error = XMC_FLASH_GetStatus()) == 0 ) {
            page=p; 
            page_changed=0;
          } else 
            page=-1;    
        } else {
          error = -1;
        }
    }
    
    // write out the current page in programming mode 
    void writepage() {
      if (page != -1 && page_changed) {
        XMC_FLASH_ProgramPage( (uint32_t*) (xmcdatasector+page*256), (uint32_t*) pagebuffer);
        error = XMC_FLASH_GetStatus();
        page_changed=0;
      }    
    }
    
    // read a block to the blockbuffer
    void readblock(int b) {
      if (b>=0 && b<256) {
        XMC_FLASH_ReadBlocks( (uint32_t*) (xmcdatasector+b*16), (uint32_t*) blockbuffer, 1);
        if ((error = XMC_FLASH_GetStatus()) == 0 ) block=b; else block=-1;
      } else {
        error = -1;
      }
    }
};


XMCEEPROM EEPROM;

#endif
