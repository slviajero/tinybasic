#include "display.h"

void setup(){
  Serial.begin(9600);  
  dspbegin();
}

void loop() {
  char ch=Serial.read();
  if (ch !=0 && ch != -1) {
    dspwrite(ch);
    Serial.write(ch);
  }
}
