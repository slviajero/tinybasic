/******************************************************************************************************************************************
  FlashAsEEPROM_SAMD.h
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

#ifndef FlashAsEEPROM_SAMD_h
#define FlashAsEEPROM_SAMD_h

#include <FlashStorage_SAMD.hpp>
#include <FlashStorage_SAMD_Impl.h>

#endif    //#ifndef FlashAsEEPROM_SAMD_h
