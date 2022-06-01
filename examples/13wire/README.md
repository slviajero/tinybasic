# Example programs for Arduino wire in BASIC 

## Wire library

Wire in BASIC can be accessed through the I/O stream 7. The standard file I/O command work in this stream. 

## Programs 

- Scan: scan.bas - scan the I2C bus for devices
- Ident: ident.bas - identify some well known device types
- EEPROM: eeprom.bas - read and write serial EEPROMS directly
- Clock: clock.bas - read out a DS3231 real time clock


## BASIC language features

scan.bas: opening a wire device with OPEN &7, error handling with @S

ident.bas: Device types in &7, otherwise like scan.bas

eeprom.bas: use strings and PUT to send and receive wire messages, addressing

clock.bas: use of AND for bitwise operations



