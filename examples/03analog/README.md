# Arduino examples 03.Analog ported to basic

## Programs 

- AnalogIn: analogin.bas - convert an analog value to a pulse
- AnalogInOutSerial: analogios.bas - write an analog sensor value to the serial output
- AnalogWriteMega: analogwm.bas - multiple analog writes 
- Calibration: calib.bas - calibrate a sensor input for minium and maximum
- Fading: fading.bas - fade in and out a led.
- Smoothing: smooth.bas - average of 10 sample to smooth out data

## BASIC language features 

The examples in the folder need #BASICINTEGER as a language stetting. This is a full featured integer BASIC.

AZERO is a BASIC constant for the lowest analog port A0. 

Arduino functions AREAD to read analog inputs. 

AWRITE and DWRITE to write digital outputs. PINM to set pins. DELAY in ms.

MAP as the equivalent to the Arduino map() function.

FOR NEXT loops and the STEP command.

MILLIS BASIC function with a divisor to measure longer time intervals.

DIM to dimension arrays, array access.

## Original files

See https://docs.arduino.cc/built-in-examples/ for further information
