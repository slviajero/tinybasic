/* 
 * VGA system with SD card, based on the TTGO VGA 1.4 
 * ESP32 
 * standalone by default, with MQTT
 */
#define ARDUINOEEPROM
#define ARDUINOVGA
#define ARDUINOSD
#undef ARDUINOMQTT /* not enough memory, no loonger supported */
#define SDPIN   13
#define STANDALONE
/* this is a large screen with 48 kB memory, good fonts, should work for most situations, only this option is supported right now */
#define MEMSIZE 48000
#define TTGOVGARESOLUTION VGA_640x400_70Hz
