/* an ESP32 board with an ILI9488 display, 
   some SD problems here with some hardware */
#define ILI_CS 12
#define ILI_DC 27
#define ILI_RST 14
#define ILI_LED 26
#undef ARDUINOPICOSERIAL
#define ESPSPIFFS
#define DISPLAYCANSCROLL
#define ARDUINOILI9488
#define ARDUINOEEPROM
#define ARDUINOMQTT
#define ARDUINOWIRE
