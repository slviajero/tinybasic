/*
 *Arduino Nano Every board with PS2 keyboard and sensor
 */
#undef ARDUINOPICOSERIAL 
#define ARDUINOPS2
#define DISPLAYCANSCROLL
#define ARDUINOLCDI2C
#define ARDUINOEEPROM
#define ARDUINOPRT
#define ARDUINOEFS
#define ARDUINORTC
#define ARDUINOWIRE
#define EFSEEPROMADDR 0x050 /* use clock EEPROM 0x057, set to 0x050 for external EEPROM */
#define STANDALONE
