# Arduino examples 01.Basic ported to basic

## Programs 

- BlinkWithoutDelay: blinkwod.bas - blink using the millis function
- Button: button.bas - light the internal LED from a digital button input
- DigitalInputPullup: digitip.bas - use the internal pullup resistor 
- StateChangeDetection: statecd.bas - state change of a button

## BASIC language features 

The examples in the folder need #BASICINTEGER as a language stetting. This is a full featured integer BASIC.

MILLIS() has an argument in BASIC. It is the divisor of the time in milliseconds. MILLIS(1000) in the example counts seconds. 

IF THEN ELSE is used in button.bas. ELSE is not part of the Palo Alto or Apple1 BASIC original language.

PINM in BASIC can have the argument 0 for INPUT, 1 for OUTPUT and 2 for INPUT_PULLUP


