i/* an ESP01 board, using the internal flash 
 *  with the ESP01-8266 only pins 0 and 2 are usable freely
 *  on ESP01-ESP32C3 this is 9 and 2 while 2 is an analog pin
 *  9 cannot be pulled on low by any peripheral on boot because this 
 *  brings the board to flash mode
 */
#undef ARDUINOEEPROM
#define ESPSPIFFS
#define ARDUINOMQTT
#define ARDUINOWIRE
#if defined(ARDUINOWIRE) && defined(ARDUINO_ARCH_ESP8266)
#define SDA_PIN 0
#define SCL_PIN 2
#endif
/* see:  https://github.com/espressif/arduino-esp32/issues/6376 
 *  nothing should block the port, e.g. DHT or anything
 */
#if defined(ARDUINOWIRE) && defined(ARDUINO_ARCH_ESP32)
#define SDA_PIN 9
#define SCL_PIN 2
#endif
/*
 *
 * Currently only 8=SDA and 9=SCL works / tested with AHT10
 */
