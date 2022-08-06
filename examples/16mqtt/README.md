# Example programs for Arduino MQTT in BASIC 

## MQTT library

The BASIC interpretes used the pubsub library as base MQTT library plus the network libraries of the respective platform.

MQTT uses the I/O stream 8. It implements sending and receiving messages with INPUT, PRINT and opening of topics with OPEN. NETSTAT displays the network status and can be used as a function to check the status in logical expressions.

Currently only unencrypted and unauthenticated MQTT is supported. The MQTT server and the network credential are compiled into the BASIC code and cannot be changed by a BASIC program.

## Programs 

- Analog: analog.bas - send the result of an analog input
- Digital: digital.bas - read a message and switch on the led, ESP style inverted led logic
- MqttModem: mqttmod.bas - a ESP12 MQTT modem program, received data on serial and writes to MQTT

## BASIC language features

analog.bas: open a topic with OPEN, PRINT &9 and NETSTAT

digital.bas: read an MQTT message with INPUT 

mqttmod.bas: bidirectional communication, use of AVAIL





