# Arduino examples 04.Communications ported to basic

## Programs 

- Dimmer: dimmer.bas - read a character and set an analog output
- Graph: graph.bas - read an analog input and output the result to serial, can be used as a graphics program with a processing counterpart
- Midi: midi.bas - play a notes on a midi input. See the Arduino original for the wiring. This program does not fully work as the serial baudrate control is missing. BASIC needs to be recompiled with the standard midi baudrate of 31250 on Serial for it to work.
- MultiSerial: multiser.bas - read from one serial input and write to the other and vice versa.
- PhysicalPixel: physpix.bas - read a byte and switch on the internal LED with it.
- SerialCallResponse: sercs.bas - communication examples with a serial character as start
- SerialCallResponseASCII: sercs.bas - communication examples with a serial character as start and ASCII output
- VirtualColorMixer: virtcm.bas - read 3 analog inputs and output them.

## BASIC language features 

The examples in the folder need #BASICINTEGER as a language stetting. This is a full featured integer BASIC.

Character input with AVAIL and GET. 

Using AWRITE, DWRITE and PINM for analog output.

DELAY for millisecond delay.

Using PUT to output a series of characters.

Using the second serial port &4 in BASIC. For this #define ARDUINOPRT has to be set at compile time.

Using the PRINT command for list output.

The Arduino MAP command for map() in C++.

## Original files

See https://docs.arduino.cc/built-in-examples/ for further information and the processing counterparts
