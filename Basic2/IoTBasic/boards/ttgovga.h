/* 
 * VGA system with SD card, based on the TTGO VGA 1.4 
 * ESP32 
 * standalone by default, with MQTT
 */
#define ARDUINOEEPROM
#define ARDUINOVGA
#define ARDUINOSD
#undef  ARDUINOMQTT /* currently broken */
#define SDPIN   13
#define STANDALONE
/* this is a large screen with 48 kB memory, good fonts, should work for most situations */
#define MEMSIZE 48000
#define TTGOVGARESOLUTION VGA_640x400_70Hz
/* a smaller screen, slightly odd fonts but graph and 60 kB, good screen 
#define MEMSIZE 60000
#define TTGOVGARESOLUTION VGA_512x384_60Hz 
*/
/* Other options that work:
 * #define MEMSIZE 48000
 * #define TTGOVGARESOLUTION VGA_640x480_73Hz
 * 
 * #define TTGOVGARESOLUTION VGA_640x384_60Hz 
 * 
 * #define TTGOVGARESOLUTION VGA_640x200_70HzRetro 
 */
