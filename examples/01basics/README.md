# Arduino examples 01.Basic ported to basic

## Programs 

- AnalogReadSerial: analogrs.bas - read an analog port and outputs the result
- BareMinimum: baremin.bas - an empty template for a sketch
- Blink: blink.bas - blink the internal led
- DigitalReadSerial: digitrs.bas - read a digital input and print it
- Fade: fade.bas - fade a LED connected to a PWN pin
- ReadAnalogeVoltage: readav1.bas and readav2.bas are the floating point and integer version of a analog read of a voltage

## BASIC language features 

The examples in the folder need #BASICINTEGER as a language stetting. This is a full featured integer BASIC.

AZERO is a BASIC constant for the lowest analog port A0. 

LED is a BASIC constat for the buildin LED. It is LED_BUILTIN in the Arduino IDE.

FOR I : NEXT is a infinite loop. This is not a standard BASIC feature. The variable I is iterated from positive to negative values once the maximum integer is reached.

## Original files

See https://docs.arduino.cc/built-in-examples/ for further information
