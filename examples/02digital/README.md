# Arduino examples 02.Digital ported to basic

## Programs 

- BlinkWithoutDelay: blinkwod.bas - blink using the millis function
- Button: button.bas - light the internal LED from a digital button input
- Debounce: debounce.bas - debounce a button 
- DigitalInputPullup: digitip.bas - use the internal pullup resistor 
- StateChangeDetection: statecd.bas - state change of a button
- Melody: melody.bas and melodys.bas - play a melody, 1k version with all the tones and small version
- TonePitchFollower: tonepf.bas - follow an analog input with a sound pitch

## BASIC language features 

The examples in the folder need #BASICINTEGER as a language stetting. This is a full featured integer BASIC.

MILLIS() has an argument in BASIC. It is the divisor of the time in milliseconds. MILLIS(1000) in the example counts seconds. 

IF THEN ELSE is used in button.bas. ELSE is not part of the Palo Alto or Apple1 BASIC original language.

PINM in BASIC can have the argument 0 for INPUT, 1 for OUTPUT and 2 for INPUT_PULLUP

PLAY is the Arduino tone() command, PLAY with a zero argument as frequency is noTone()

Use DATA and READ to store data, use DIM to dimension arrays

## Original files

See https://docs.arduino.cc/built-in-examples/ for further information


