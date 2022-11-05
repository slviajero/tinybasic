#include "display.h"

void setup(){
  Serial.begin(9600);  
  dspbegin();
}

void loop() {
  char ch;

 /* read the serial strean and send data to the display */
  if (Serial.available()) {
    ch=Serial.read();
    dspwrite(ch);
    while (dspavail()) Serial.write(dspread());
  }
  delay(0); /* this is yield on some platforms */
}
