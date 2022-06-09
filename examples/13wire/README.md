# Example programs for Arduino wire in BASIC 

## Wire library

Wire in BASIC can be accessed through the I/O stream 7. The standard file I/O command work with this stream. 

Wire data can be sent and received using the standard BASIC command like PRINT, INPUT, PUT, and GET. Standard sensors and wire devices with a simple protocol can be read out and programmed directly. Because of the simplicity of BASIC Wire programs are really short and elegant in this language.

The BASIC interface to the Arduino wire library does not try to fix the pecularities of this code. Please see the end of this text for some information.

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
- AnalogReader: analogr.bas - read an analog input as slave and transfer the data as ASCII 
- AnalogMaster: analogm.bas - master code for AnalogReader
- AnalogReader2: analogr2.bas - read an analog input as slave and transfer the data as binary
- AnalogMaster2: analogm2.bas - master code for AnalogReader2

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

analogr.bas: printing ASCII data to I2C

analogm.bas: reading ASCII data from I2C

analogr2.bas: transfering integer binary data to I2C

analogm2.bas: reading binary integer data from I2C

## Pecularities of the Wire library

The Arduino Wire master functions cannot handle variable message sizes. If the master requests 32 bytes and the slave transfers only 10, 22 bytes are padded with -1 characters i.e. EOF. The mechanisms of Wire.available() and the return values of Wire.requestFrom() are not working as one would assume. This feature also shows in BASIC. The reason for this is that Arduino uses the stream class to build its Wire interface. The wire protocol and the underlying TWI library, however, are not really a stream. They are more a datagram type transfer mechanism with a more complex state engine. Still, BASIC can do a lot of fun things with I2C.

Opening an Wire connection as a master for slave with address A is done with 

OPEN &7, A

Data is requested by 

INPUT &7, A$

This is translated to a Wire data request of the master on the length of the string but with a maximum of 32 bytes. A successful read with leave the state variable @S with value 0 and yield a string with 32 bytes (or smaller if the string is smaller). 

The command

INPUT &7, #8, A$

requests 8 bytes from the slave. The length of the return string will be 8.

The string return length is independent of the number of bytes the slave send. BASIC does not try to correct the behaviour.

ASCII number data from the slave can be captures with a plain 

INPUT &7, X

@S returns the status of the number conversions and the read fairly reliably. 

Salve code is activated by opening 

OPEN &7, A, 1

Two callback functions handle data transfer from two buffers. Once a slave has opened wire the function AVAIL(7) returns a non zero value if the master has send data. The data has to be captured with one read operation

INPUT &7, A$

into a string and can be processed then. Partial read is not possible. GET &7, A will return the first byte of the payload and discard the rest. Wire is not a stream but buffer / block oriented.

PRINT &7, A$ 

will write an entire string to a buffer. Maximum length is 32 bytes. If the master requests data, the entire buffer transfered and the buffer count cleared. USR(7, 1) returns the number of bytes the buffer has chached. If USR(7, 1) is larger 0 then the master has not yet read. If it changes to 0 the read has occured. 

USR(7, 0) indicates if the Wire code is compiled into BASIC. A program can check if it runs on a Wire capable machine by doing something like this

IF NOT USR(7, 0) THEN PRINT "No wire": END
