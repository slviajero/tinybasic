/******************************************************************************************************************************************
  FlashStorage_SAMD.hpp
  For SAMD21/SAMD51 using Flash emulated-EEPROM

  The FlashStorage_SAMD library aims to provide a convenient way to store and retrieve user's data using the non-volatile flash memory
  of SAMD21/SAMD51. It now supports writing and reading the whole object, not just byte-and-byte.

  Based on and modified from Cristian Maglie's FlashStorage (https://github.com/cmaglie/FlashStorage)

  Built by Khoi Hoang https://github.com/khoih-prog/FlashStorage_SAMD
  Licensed under LGPLv3 license
  
  Orginally written by Cristian Maglie
  
  Copyright (c) 2015 Arduino LLC.  All right reserved.
  Copyright (c) 2020 Khoi Hoang.
  
  This program is free software: you can redistribute it and/or modify it under the terms of the GNU General Public License 
  as published bythe Free Software Foundation, either version 3 of the License, or (at your option) any later version.
  This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public License along with this library. 
  If not, see (https://www.gnu.org/licenses/)
  
  Version: 1.3.2

  Version Modified By   Date        Comments
  ------- -----------  ----------   -----------
  1.0.0   K Hoang      28/03/2020  Initial coding to add support to SAMD51 besides SAMD21
  1.1.0   K Hoang      26/01/2021  Add supports to put() and get() for writing and reading the whole object. Fix bug.
  1.2.0   K Hoang      18/08/2021  Optimize code. Add debug option
  1.2.1   K Hoang      10/10/2021  Update `platform.ini` and `library.json`
  1.3.0   K Hoang      25/01/2022  Fix `multiple-definitions` linker error. Add support to many more boards.
  1.3.1   K Hoang      25/01/2022  Reduce number of library files
  1.3.2   K Hoang      26/01/2022  Make compatible with old libraries and codes
 ******************************************************************************************************************************************/

// The .hpp contains only definitions, and can be included as many times as necessary, without `Multiple Definitions` Linker Error
// The .h contains implementations, and can be included only in main(), .ino with setup() to avoid `Multiple Definitions` Linker Error

#pragma once

#ifndef FlashStorage_SAMD_hpp
#define FlashStorage_SAMD_hpp

#if !( defined(ARDUINO_SAMD_ZERO) || defined(ARDUINO_SAMD_MKR1000) || defined(ARDUINO_SAMD_MKRWIFI1010) \
      || defined(ARDUINO_SAMD_NANO_33_IOT) || defined(ARDUINO_SAMD_MKRFox1200) || defined(ARDUINO_SAMD_MKRWAN1300) || defined(ARDUINO_SAMD_MKRWAN1310) \
      || defined(ARDUINO_SAMD_MKRGSM1400) || defined(ARDUINO_SAMD_MKRNB1500) || defined(ARDUINO_SAMD_MKRVIDOR4000) \
      || defined(ARDUINO_SAMD_CIRCUITPLAYGROUND_EXPRESS) || defined(__SAMD51__) || defined(__SAMD51J20A__) \
      || defined(__SAMD51J19A__) || defined(__SAMD51G19A__) || defined(__SAMD51P19A__)  \
      || defined(__SAMD21E15A__) || defined(__SAMD21E16A__) || defined(__SAMD21E17A__) || defined(__SAMD21E18A__) \
      || defined(__SAMD21G15A__) || defined(__SAMD21G16A__) || defined(__SAMD21G17A__) || defined(__SAMD21G18A__) \
      || defined(__SAMD21J15A__) || defined(__SAMD21J16A__) || defined(__SAMD21J17A__) || defined(__SAMD21J18A__) )
  #error This code is designed to run on SAMD21/SAMD51 platform! Please check your Tools->Board setting.
#endif

#ifndef FLASH_STORAGE_SAMD_VERSION
  #define FLASH_STORAGE_SAMD_VERSION            "FlashStorage_SAMD v1.3.2"

  #define FLASH_STORAGE_SAMD_VERSION_MAJOR      1
  #define FLASH_STORAGE_SAMD_VERSION_MINOR      3
  #define FLASH_STORAGE_SAMD_VERSION_PATCH      2

#define FLASH_STORAGE_SAMD_VERSION_INT        1003002

#endif

#include <Arduino.h>

#if defined(SAMD_INDUSTRUINO_D21G)
  #define BOARD_NAME      "SAMD21 INDUSTRUINO_D21G"
#elif defined(ARDUINO_SAML_INDUSTRUINO_420MAKER)
  #define BOARD_NAME      "SAML21 INDUSTRUINO_420MAKER"
#endif

#if !defined(Serial)
  #define Serial          SERIAL_PORT_MONITOR
#endif

/////////////////////////////////////////////////////

#ifndef FLASH_DEBUG
  #define FLASH_DEBUG               0
#endif

#if !defined(FLASH_DEBUG_OUTPUT)
  #define FLASH_DEBUG_OUTPUT    Serial
#endif

const char FLASH_MARK[]  = "[FLASH] ";
const char FLASH_SP[]    = " ";

#define FLASH_PRINT          FLASH_DEBUG_OUTPUT.print
#define FLASH_PRINTLN        FLASH_DEBUG_OUTPUT.println
#define FLASH_FLUSH          FLASH_DEBUG_OUTPUT.flush

#define FLASH_PRINT_MARK     FLASH_PRINT(FLASH_MARK)
#define FLASH_PRINT_SP       FLASH_PRINT(FLASH_SP)

/////////////////////////////////////////////////////

#define FLASH_LOGERROR(x)         if(FLASH_DEBUG>0) { FLASH_PRINT("[FLASH] "); FLASH_PRINTLN(x); }
#define FLASH_LOGERROR0(x)        if(FLASH_DEBUG>0) { FLASH_PRINT(x); }
#define FLASH_HEXLOGERROR0(x)     if(FLASH_DEBUG>0) { FLASH_PRINTLN(x, HEX); }
#define FLASH_LOGERROR1(x,y)      if(FLASH_DEBUG>0) { FLASH_PRINT("[FLASH] "); FLASH_PRINT(x); FLASH_PRINT_SP; FLASH_PRINTLN(y); }
#define FLASH_LOGERROR2(x,y,z)    if(FLASH_DEBUG>0) { FLASH_PRINT("[FLASH] "); FLASH_PRINT(x); FLASH_PRINT_SP; FLASH_PRINT(y); FLASH_PRINT_SP; FLASH_PRINTLN(z); }
#define FLASH_LOGERROR3(x,y,z,w)  if(FLASH_DEBUG>0) { FLASH_PRINT("[FLASH] "); FLASH_PRINT(x); FLASH_PRINT_SP; FLASH_PRINT(y); FLASH_PRINT_SP; FLASH_PRINT(z); FLASH_PRINT_SP; FLASH_PRINTLN(w); }

/////////////////////////////////////////////////////

#define FLASH_LOGDEBUG(x)         if(FLASH_DEBUG>1) { FLASH_PRINT("[FLASH] "); FLASH_PRINTLN(x); }
#define FLASH_LOGDEBUG0(x)        if(FLASH_DEBUG>1) { FLASH_PRINT(x); }
#define FLASH_HEXLOGDEBUG0(x)     if(FLASH_DEBUG>1) { FLASH_PRINTLN(x, HEX); }
#define FLASH_LOGDEBUG1(x,y)      if(FLASH_DEBUG>1) { FLASH_PRINT("[FLASH] "); FLASH_PRINT(x); FLASH_PRINT_SP; FLASH_PRINTLN(y); }
#define FLASH_LOGDEBUG2(x,y,z)    if(FLASH_DEBUG>1) { FLASH_PRINT("[FLASH] "); FLASH_PRINT(x); FLASH_PRINT_SP; FLASH_PRINT(y); FLASH_PRINT_SP; FLASH_PRINTLN(z); }
#define FLASH_LOGDEBUG3(x,y,z,w)  if(FLASH_DEBUG>1) { FLASH_PRINT("[FLASH] "); FLASH_PRINT(x); FLASH_PRINT_SP; FLASH_PRINT(y); FLASH_PRINT_SP; FLASH_PRINT(z); FLASH_PRINT_SP; FLASH_PRINTLN(w); }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Concatenate after macro expansion
#define PPCAT_NX(A, B) A ## B
#define PPCAT(A, B) PPCAT_NX(A, B)

#if defined(__SAMD51__)

  #define Flash(name, size) \
  __attribute__((__aligned__(8192))) \
  static const uint8_t PPCAT(_data,name)[(size+8191)/8192*8192] = { }; \
  FlashClass name(PPCAT(_data,name), size);
  
  #define staticFlash(name, size) \
  __attribute__((__aligned__(8192))) \
  static const uint8_t PPCAT(_data,name)[(size+8191)/8192*8192] = { }; \
  static FlashClass name(PPCAT(_data,name), size);

  #define FlashStorage(name, T) \
  __attribute__((__aligned__(8192))) \
  static const uint8_t PPCAT(_data,name)[(sizeof(T)+8191)/8192*8192] = { }; \
  FlashStorageClass<T> name(PPCAT(_data,name));
  
  #define staticFlashStorage(name, T) \
  __attribute__((__aligned__(8192))) \
  static const uint8_t PPCAT(_data,name)[(sizeof(T)+8191)/8192*8192] = { }; \
  static FlashStorageClass<T> name(PPCAT(_data,name));
  
#else

  #define Flash(name, size) \
  __attribute__((__aligned__(256))) \
  static const uint8_t PPCAT(_data,name)[(size+255)/256*256] = { }; \
  FlashClass name(PPCAT(_data,name), size);
  
  #define staticFlash(name, size) \
  __attribute__((__aligned__(256))) \
  static const uint8_t PPCAT(_data,name)[(size+255)/256*256] = { }; \
  static FlashClass name(PPCAT(_data,name), size);

  #define FlashStorage(name, T) \
  __attribute__((__aligned__(256))) \
  static const uint8_t PPCAT(_data,name)[(sizeof(T)+255)/256*256] = { }; \
  FlashStorageClass<T> name(PPCAT(_data,name));
  
  #define staticFlashStorage(name, T) \
  __attribute__((__aligned__(256))) \
  static const uint8_t PPCAT(_data,name)[(sizeof(T)+255)/256*256] = { }; \
  static FlashStorageClass<T> name(PPCAT(_data,name));

#endif

//////////////////////////////////////////////////////////////////////////////////////////////////////////

class FlashClass 
{
  public:
    FlashClass(const void *flash_addr = NULL, uint32_t size = 0);

    void write(const void *data) { write(flash_address, data);        }
    void erase()                 { erase(flash_address, flash_size);  }
    void read(void *data)        { read(flash_address, data);         }

    void write(const volatile void *flash_ptr, const void *data);
    void erase(const volatile void *flash_ptr, uint32_t size);
    void read(const volatile void *flash_ptr, void *data);

  private:
    void erase(const volatile void *flash_ptr);

    const uint32_t PAGE_SIZE, PAGES, MAX_FLASH, ROW_SIZE;
    const volatile void *flash_address;
    const uint32_t flash_size;
};

/////////////////////////////////////////////////////

template<class T>
class FlashStorageClass 
{
  public:
    FlashStorageClass(const void *flash_addr) : flash(flash_addr, sizeof(T)) { };

    // Write data into flash memory.
    // Compiler is able to optimize parameter copy.
    inline void write(T &data)  { flash.erase(); flash.write(&data); }

    // Overloaded version of read.
    // Compiler is able to optimize copy-on-return.
    inline void read(T &data)  { flash.read(&data); }

  private:
    FlashClass flash;
};

//////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifndef EEPROM_EMULATION_SIZE
  #define EEPROM_EMULATION_SIZE 1024
#endif

const uint32_t SAMD_EEPROM_EMULATION_SIGNATURE = 0xFEEDDEED;

typedef struct 
{
  byte data[EEPROM_EMULATION_SIZE];
  bool valid;
  uint32_t signature;
} EEPROM_EMULATION;

/////////////////////////////////////////////////////


#if defined(__SAMD51__)
  staticFlashStorage(eeprom_storage, EEPROM_EMULATION); 
#else
  staticFlashStorage(eeprom_storage, EEPROM_EMULATION);
#endif


class EEPROMClass 
{
  public:
  
  EEPROMClass() : _initialized(false), _dirty(false), _commitASAP(true)  
  {
    // Empty
  }

/////////////////////////////////////////////////////
  /**
   * Read an eeprom cell
   * @param index
   * @return value
   */
  //uint8_t read(int address);

  uint8_t read(int address)
  {
    if (!_initialized) 
      init();
      
    return _eeprom.data[address];
  }

/////////////////////////////////////////////////////

  /**
   * Update a eeprom cell
   * @param index
   * @param value
   */

  void update(int address, uint8_t value)
  {
    if (!_initialized) 
      init();
      
    if (_eeprom.data[address] != value) 
    {
      _dirty = true;
      _eeprom.data[address] = value;
    }
  }

/////////////////////////////////////////////////////

  /**
   * Write value to an eeprom cell
   * @param index
   * @param value
   */
  void write(int address, uint8_t value)
  {
    update(address, value);
  }

/////////////////////////////////////////////////////
  

  /**
   * Read from eeprom cells to an object
  * @param index
  * @param value
  */
  //Functionality to 'get' data to objects to from EEPROM.
  template< typename T > T& get( int idx, T &t )
  {       
    // Copy the data from the flash to the buffer if not yet
    if (!_initialized) 
      init();
      
    uint16_t offset = idx;
    uint8_t* _pointer = (uint8_t *) &t;
    
    for ( uint16_t count = sizeof(T) ; count ; --count, ++offset )
    {  
      *_pointer++ = _eeprom.data[offset];
    }
      
    return t;
  }

/////////////////////////////////////////////////////

  /**
  * Read from eeprom cells to an object
  * @param index
  * @param value
  */
  //Functionality to 'get' data to objects to from EEPROM.
  template< typename T > const T& put( int idx, const T &t )
  {            
    // Copy the data from the flash to the buffer if not yet
    if (!_initialized) 
      init();
    
    uint16_t offset = idx;
       
    const uint8_t* _pointer = (const uint8_t *) &t;
    
    for ( uint16_t count = sizeof(T) ; count ; --count, ++offset )
    {              
      _eeprom.data[offset] = *_pointer++;
    }

    if (_commitASAP)
    {
      _dirty = false;
      _eeprom.valid = true;
      // Save the data from the buffer
      eeprom_storage.write(_eeprom);
    }
    else  
    {
      // Delay saving the data from the buffer to the flash. Just flag and wait for commit() later
      _dirty = true;    
    }
         
    return t;
  }

/////////////////////////////////////////////////////

  /**
   * Check whether the eeprom data is valid
   * @return true, if eeprom data is valid (has been written at least once), false if not
   */
  bool isValid()
  {
    if (!_initialized) 
    init();

    return _eeprom.valid;
  }

/////////////////////////////////////////////////////

  /**
   * Write previously made eeprom changes to the underlying flash storage
   * Use this with care: Each and every commit will harm the flash and reduce it's lifetime (like with every flash memory)
   */
  void commit()
  {
    if (!_initialized) 
    init();

    if (_dirty) 
    {
    _dirty = false;
    _eeprom.valid = true;
    // Save the data from the buffer
    eeprom_storage.write(_eeprom);
    }
  }

/////////////////////////////////////////////////////
 
  uint16_t length() { return EEPROM_EMULATION_SIZE; }

  void setCommitASAP(bool value = true) { _commitASAP = value; }
  bool getCommitASAP() { return _commitASAP; }    
  
/////////////////////////////////////////////////////    

  private:

  void init()
  { 
    // Use reference
    eeprom_storage.read(_eeprom);
    
    if (_eeprom.signature != SAMD_EEPROM_EMULATION_SIGNATURE)
    {
      memset(_eeprom.data, 0xFF, EEPROM_EMULATION_SIZE);
      _eeprom.signature = SAMD_EEPROM_EMULATION_SIGNATURE;
    }

    _eeprom.valid = true;

    _initialized = true;
  }

    bool _initialized;
    EEPROM_EMULATION _eeprom;
    bool _dirty;
    bool _commitASAP;
};

/////////////////////////////////////////////////////

static EEPROMClass EEPROM;

/////////////////////////////////////////////////////

#endif    //#ifndef FlashStorage_SAMD_hpp
