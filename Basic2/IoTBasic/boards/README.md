This is a folder for predefined boards with components. 

Purpose of this folder is to store information of entire systems. It helps to define them here is a file and then include the file in hardware.h. This way, PIN settings and hardware settings don't have to be done by hand.

   dummy.h: 
       do nothing.
   avrlcd.h:
       a AVR system with a LCD shield
   wemosshield.h: 
       a ESP8266 UNO lookalike with a modified datalogger shield
       great hardware for small BASIC based IoT projects.
   megashield.h: 
       an Arduino Mega with Ethernet Shield optional keyboard 
       and i2c display
   megatft.h: 
   duetft.h:
       TFT 7inch screen systems, standalone
   nanoboard.h:
       Arduino Nano Every board with PS2 keyboard and sensor 
       kit.
   megaboard.h:
       A board for the MEGA with 64 kB RAM, SD Card, and real time
       clock.
   unoboard.h:
       A board for an UNO with 64kB memory and EEPROM disk
       fits into an UNO flash only with integer
   esp01board.h:
       ESP01 based board as a sensor / MQTT interface
  rp2040board.h:
       A ILI9488 hardware design based on an Arduino connect RP2040.
  rp2040board2.h:
       like the one above but based on the Pi Pico core
  esp32board.h:
       same like above with an ESP32 core
  mkboard.h:
       a digital signage and low energy board
  tdeck.h: 
       the LILYGO T-Deck - not yet finished
  ttgovga.h:
       the TTGO VGA 1.4 board with VGA output
  esp32cam.h
       the camera - many hw specific definitions coming

