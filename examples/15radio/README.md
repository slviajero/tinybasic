# Example programs for Arduino rf24 in BASIC 

## RF24 library

## Programs 

- RF24Reader: rf24rdr.bas - read a rf24 message from a named pipe
- RF24Writer: rf24wrt.bas - write a rf24 message from a named pipe

## BASIC language features

rf24rdr.bas: opening a rf24 pipe for read with OPEN &8, scan with AVAIL(8), read the message with INPUT &8 

rd24wrt.bas: opening a rf24 pipe for write with OPEN &8, send a message with PRINT &8 

