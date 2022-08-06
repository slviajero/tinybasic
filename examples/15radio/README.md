# Example programs for Arduino rf24 in BASIC 

## RF24 library

The implementation in BASIC is based on the standard Arduino RF2401 library https://nrf24.github.io/RF24/. 

I/O stream &8 is used for RF24L01 (in short, radio). OPEN opens a pipe for reading or writing. BASIC supports one read and one write pipe. Receiving messages is handled in the background. AVAIL(8) is 1 if a message was received, it does not know about the number of bytes (as it is in the library itself). It indicates if a message is received. @S is set if write is unsuccessful or if OPEN failed due to a missing chip. USR(8,1) is 1 if the chip is connected. Checking USR(8,1) periodically after a successful OPEN will make sure that the hardware is still functioning. This may be relevant for a reader where no check on proper function is done at runtime by the library. 

The pipe address is a string with exactly 5 characters. This sting is used to construct the numerical pipe address. First byte on the string is most significant byte of the pipe address.

## Programs 

- RF24Reader: rf24rdr.bas - read a rf24 message from a named pipe
- RF24Writer: rf24wrt.bas - write a rf24 message from a named pipe
- Advanced Reader: rf24rdra.bas - bidirectional data transfer with error handling (master)
- Advanced Writer: rf24wrta.bas - bidirectional data transfer with error handling (slave)

## BASIC language features

rf24rdr.bas: opening a rf24 pipe for read with OPEN &8, scan with AVAIL(8), read the message with INPUT &8 

rd24wrt.bas: opening a rf24 pipe for write with OPEN &8, send a message with PRINT &8 

rf24rdra.bas: use of @S for error handling of radio, use of two pipes for bidirectional I/O

rf24wrta.bas: use of @S for error handling of radio, use of two pipes for bidirectional I/O


