/* 
 * VGA system with SD card, based on the TTGO VGA 1.4 
 * ESP32 
 * standalone by default, with MQTT
 */
#define ARDUINOEEPROM
#define ARDUINOVGA
#define ARDUINOSD
#define SDPIN   13
#define STANDALONE
/*
 * This is a large screen with 48 kB memory, good fonts. 
 * with these settings a circle is round in my old 4:3 TFT.
 * should work for most situations. Ideally the canvas 
 * matches the TFT screen resolution.
 * 
 * Set COLUMNS and ROW for a smaller terminal.
 * 
 * The font is set automatically. Can be changed here for 
 * smaller terminals. 
 */
#define TTGOVGACONTROLLER VGA16Controller
#define TTGOVGARESOLUTION VGA_512x384_60Hz
#define TTGOVGAFONT FONT_8x16
#define TTGOVGACOLUMNS -1
#define TTGOVGAROWS -1
#define MEMSIZE 48000
#undef ARDUINOMQTT
// Runnings configs:
/* VGA4 and network - flickery screen 640*480 */
/* 
#define TTGOVGACONTROLLER VGA4Controller
#define TTGOVGACOLUMNS -1
#define TTGOVGAROWS -1
#define MEMSIZE 48000
#define ARDUINOMQTT
*/
/* VGA4 and network - flickery screen 640*480 */
/*
#define TTGOVGACONTROLLER VGA4Controller
#define TTGOVGARESOLUTION VGA_512x384_60Hz
#define TTGOVGACOLUMNS -1
#define TTGOVGAROWS -1
#define MEMSIZE 64000
#define ARDUINOMQTT
*/
/* VGA16, no network, stable sreen 640*480 */
/*
#define TTGOVGACONTROLLER VGA16Controller
#define TTGOVGACOLUMNS -1
#define TTGOVGAROWS -1
#define MEMSIZE 32000
#undef ARDUINOMQTT
*/
/* full VGA controller at 320*200 an 32 kB */
/*
#define TTGOVGACONTROLLER VGAController
#define TTGOVGARESOLUTION VGA_320x200_75HzRetro 
#define TTGOVGAFONT FONT_8x8
#define TTGOVGACOLUMNS -1
#define TTGOVGAROWS -1
#define MEMSIZE 32000
#undef ARDUINOMQTT
*/
/* A 4:3 screen with full VGA */
/*
#define TTGOVGACONTROLLER VGAController
#define TTGOVGARESOLUTION VGA_400x300_60Hz
#define TTGOVGAFONT FONT_8x8
#define TTGOVGACOLUMNS -1
#define TTGOVGAROWS -1
#define MEMSIZE 32000
#undef ARDUINOMQTT
*/

