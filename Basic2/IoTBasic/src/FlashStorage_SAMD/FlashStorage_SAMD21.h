/******************************************************************************************************************************************
  FlashStorage_SAMD21.h
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

#ifndef FlashStorage_SAMD21_h
#define FlashStorage_SAMD21_h

#ifndef BOARD_NAME
  #define BOARD_NAME    "Unknown SAMD21 board"
#endif

static const uint32_t pageSizes[] = { 8, 16, 32, 64, 128, 256, 512, 1024 };

/////////////////////////////////////////////////////

FlashClass::FlashClass(const void *flash_addr, uint32_t size) :
  PAGE_SIZE(pageSizes[NVMCTRL->PARAM.bit.PSZ]),
  PAGES(NVMCTRL->PARAM.bit.NVMP),
  MAX_FLASH(PAGE_SIZE * PAGES),
  ROW_SIZE(PAGE_SIZE * 4),
  flash_address((volatile void *)flash_addr),
  flash_size(size)
{
}

/////////////////////////////////////////////////////

static inline uint32_t read_unaligned_uint32(const void *data)
{
  union 
  {
    uint32_t u32;
    uint8_t u8[4];
  } res;
  
  const uint8_t *d = (const uint8_t *)data;
  
  res.u8[0] = d[0];
  res.u8[1] = d[1];
  res.u8[2] = d[2];
  res.u8[3] = d[3];
  return res.u32;
}

/////////////////////////////////////////////////////

void FlashClass::write(const volatile void *flash_ptr, const void *data)
{
  // Calculate data boundaries
  uint32_t size = (flash_size + 3) / 4;
  
  volatile uint32_t *dst_addr = (volatile uint32_t *)flash_ptr;
  const uint8_t *src_addr = (uint8_t *)data;

  // Disable automatic page write
  NVMCTRL->CTRLB.bit.MANW = 1;

  // Do writes in pages
  while (size) 
  {
    // Execute "PBC" Page Buffer Clear
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_PBC;
    while (NVMCTRL->INTFLAG.bit.READY == 0) { }

    // Fill page buffer
    uint32_t i;
    
    for (i=0; i<(PAGE_SIZE/4) && size; i++) 
    {
      *dst_addr = read_unaligned_uint32(src_addr);
      src_addr += 4;
      dst_addr++;
      size--;
    }

    // Execute "WP" Write Page
    NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_WP;
    while (NVMCTRL->INTFLAG.bit.READY == 0) { }
  }
}

/////////////////////////////////////////////////////

void FlashClass::read(const volatile void *flash_ptr, void *data)
{
  FLASH_LOGERROR3(F("MAX_FLASH (KB) = "), MAX_FLASH / 1024, F(", ROW_SIZE ="), ROW_SIZE);
  FLASH_LOGERROR1(F("FlashStorage size = "), flash_size);
  FLASH_LOGERROR0(F("FlashStorage Start Address: 0x")); FLASH_HEXLOGERROR0((uint32_t ) flash_address);
  
  FLASH_LOGDEBUG0(F("Read: flash_ptr = 0x")); FLASH_HEXLOGDEBUG0((uint32_t ) flash_ptr);
  FLASH_LOGDEBUG0(F("data = 0x")); FLASH_HEXLOGDEBUG0(* (uint32_t *) data);
  
  memcpy(data, (const void *)flash_ptr, flash_size);
}

/////////////////////////////////////////////////////

void FlashClass::erase(const volatile void *flash_ptr, uint32_t size)
{
  const uint8_t *ptr = (const uint8_t *)flash_ptr;
  
  while (size > ROW_SIZE) 
  {
    erase(ptr);
    ptr += ROW_SIZE;
    size -= ROW_SIZE;
  }
  
  erase(ptr);
}

/////////////////////////////////////////////////////

void FlashClass::erase(const volatile void *flash_ptr)
{
  NVMCTRL->ADDR.reg = ((uint32_t)flash_ptr) / 2;
  NVMCTRL->CTRLA.reg = NVMCTRL_CTRLA_CMDEX_KEY | NVMCTRL_CTRLA_CMD_ER;
  while (!NVMCTRL->INTFLAG.bit.READY) { }
}

#endif      //#ifndef FlashStorage_SAMD21_h

