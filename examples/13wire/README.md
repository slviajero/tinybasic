# Example programs for Arduino wire in BASIC 

## Wire library

Wire in BASIC can be accessed through the I/O stream 7. The standard file I/O command work with this stream. 

The BASIC interface to the Arduino wire library does not try to fix the pecularities of this code. Specifically the counting of characters a slave returns to the master does not work consistently. This is hidden in the original Arduino wire examples. 

## Programs 

- Scan: scan.bas - scan the I2C bus for devices
- Ident: ident.bas - identify some well known device types
- EEPROM: eeprom.bas - read and write serial EEPROMS directly
- Clock: clock.bas - read out some data a DS3231 real time clock
- MasterWriter: masterw.bas - standard Arduino example, write a number and a string to a slave
- SlaveReceiver: slaver.bas - standard Arduino example, write the number for MasterWriter
- MasterReader: masterr.bas - standard Arduino example, read a small string from a slave
- SlaveSender: slaves.bas - standard Arduino example, write the small string for MasterReader 
- RegisterEngine: register.bas - a generic slave program, handling max 32 byte registers with read and write function plus optional payload code

## BASIC language features

scan.bas: opening a wire device with OPEN &7, error handling with @S

ident.bas: Device types in &7, otherwise like scan.bas

eeprom.bas: use strings and PUT to send and receive wire messages, addressing

clock.bas: use of AND for bitwise operations, I2C addressing 

masterw.bas: building strings for transmission, error handling with @S

slaver.bas: check the number of received character with AVAIL(7)

masterr.bas: using the #N modifier in INPUT to limit the number of received bytes

slaves.bas: opening a channel for slave function, using USR(7, 1) to check if a master has read

register.bas: bidirectional slave communication with a master

## Pecularities of the Wire library





