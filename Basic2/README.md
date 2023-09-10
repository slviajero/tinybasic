# Stefan's IoT BASIC

## Version 2.0 

This is the first alpha version 2.0. The BASIC interpreter code and the runtime environment are seperated now with a clean interface described in runtime.h. Hardware and language configuration files are also seperated. Only hardware.h and language.h have to be edited now to configure wwhich system BASIC is running on and which language features are used. Types in the runtime environment have been cleaned up. Runtime does not need the types and definitions of the interpreter. 

This version is work in progress. The goal is to encapsulate all the heuristics in the runtime environment and make them as much as possible a library which can also be used for other language projects. 

Currently I test mostly on Arduino AVR, Mac, 8266, and the new Aruino Minima boards. Other platforms can be buggy. 

To compile the POSIX version, goto the Posix folder:

gcc basic.c runtime.c -lm

To compile the Arduino version, open IoTBasic/IoTBasic.ino and compile in the Arduino IDE.

Edit hardware.h and language.h to set devices and language features just like in Basic 1.x.



