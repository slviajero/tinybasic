/* a board based on the Arduino MKR 1010 Wifi 
 *  made for low energy games 
 */
#define ILI_CS 7
#define ILI_DC 4
#define ILI_RST 6
#define ILI_LED A3
#undef  ARDUINOPICOSERIAL
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#define ARDUINOEFS
#define ARDUINOMQTT
#define ARDUINOWIRE
/* EEPROM emulation is possible, takes 2k of memory */
#undef ARDUINOEEPROM
/* careful with the setting, lockout possible easily */
#undef ARDUINOUSBKBD
#undef STANDALONE
