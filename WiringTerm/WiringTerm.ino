#include "display.h"

void setup(){
  Serial.begin(9600);  
  dspbegin();
}

void loop() {
  char ch;
  
  if (Serial.available()) {
    ch=Serial.read();
    dspwrite(ch);
    Serial.write(ch);
  }
  delay(0); /* this is yield on some platforms */
}
