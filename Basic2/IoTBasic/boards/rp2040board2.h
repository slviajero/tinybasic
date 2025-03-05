/* an RP2040 Raspberry Pi Pico based board with an ILI9488 display */
#undef ARDUINOPICOSERIAL
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#undef ARDUINOEEPROM
#undef ARDUINOPRT
#undef ARDUINOSD
#define RP2040LITTLEFS
#undef ARDUINOWIRE
#undef ARDUINORTC 
#undef ARDUINOPS2
#undef ARDUINOMQTT
#undef STANDALONE
#define ILI_LED A2
#define ILI_CS  15
#define ILI_RST 14
#define ILI_DC  13
