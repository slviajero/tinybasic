# Example programs for Arduino systems

## Arduino systems

These demo programs show Arduino hardware features in BASIC, mainly for the standalone systems in https://github.com/slviajero/tinybasic/wiki. 

## Programs 

### Just a plain Arduino board

This runs on all board with a buildin LED. An Arduino UNO would be fine. Compile the interpreter with #define BASICINTEGER for the full language set with integer.

- HelloWorld: hello.bas - the inevitable hello world program
- Blink: blink.bas - blinking with the buildin LED
- Minmax: minmax.bas - reading data, calculating minimum and maximum
- EEPROM: eeprom.bas - storing data in the EEPROM
- Pulse: pulse.bas - measure distances and write the result to serial
- Flame: flame.bas - flame sensor and (active) buzzer for a fire alert

### The LCD shield

- Shield: shield.bas - keypad and display functions of the LCD shield
- Dispbuf: dispbuf.bas - move and object around on the display
- StopWatch: stopw.bas - a very simple stopwatch
- LightSensor: lcdlight.bas - show the light intensity on the display
- LCDPulse: lcdpuls.bas - measure distances and show the result on the LCD
- PIRCount: pircount.bas - measure and display the number of events of a PIR
- 23Matches: 23matchl.bas - play the game of 23 matches on the LCD16x2

## The full TFT system

- Plot: plot.bas - use text graphics on a TFT screen
- Graphics: graph.bas - draw coloured circles to the sreen randomly

## BASIC language features 

hello.bas: the PRINT statement

blink.bas: Arduinio I/O basics PINM, DELAY, LED and DWRITE

minmax.bas: Reading an analog input with AREAD, the AZERO constant

eeprom.bas - the @E() array to store data

pulse.bas - PULSEIN and MAP function in BASIC

flame.bas - the CONT statement in FOR loops, AZERO, AREAD and DWRITE

shield.bas - cursor vars @X, @Y, I/O stream &2, GET, PUT

dispbuf.bas - the @D() display buffer array, ASCII values of the keys

stopw.bas - MILLIS() millisecond timer, cursor vars @X, @Y

lcdlight.bas - analog sensor read AREAD(), change the outpur device with @O

lcdpulse.bas - PULSEIN, MAP, display output and keypad input

pircount.bas - DREAD, the EEPROM @E() array, display control with @X, @Y

23match.bas - keypad function (1=left, 2=right, 4=up, 3=down, return=select)

plot.bas - use of the cursor functions @X, @X

graph.bas - using graphics functions FCIRCLE, COLOR and the cursor control vars @X, @Y



