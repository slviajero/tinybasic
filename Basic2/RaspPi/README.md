The Raspberry PI fileset of BASIC.

Largely identical to Posix. 

Settings here are active framebuffer and PIGPIO support. 

Compile with 

gcc basic.c runtime.c -lm -lpipgpiod_lf2

and start pigpiod before using BASIC. 

If Wiring is activated in hardware.h compile with

gcc basic.c runtime.c -lm -lwiringPi
