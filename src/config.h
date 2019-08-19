
/*********VS1053*********/
// VS1053      OLED_KIT
//  SCK          18
//  MISO         19
//  MOSI(SI)     23
//  XDCS         33
//  XCS          32
//  DREQ         35
//  XRST         RST
//  5V           5V
//  GND          GND
//  CS           NC
/************************/
////////////////////////////
#define VS1053_SCK      18
#define VS1053_MOSI     23
#define VS1053_MISO     19
#define VS1053_XDCS     33
#define VS1053_XCS      32
#define VS1053_DREQ     35
////////////////////////////
#define ENCODER_S1      37
#define ENCODER_S2      36
#define ENCODER_KEY     38
#define ENCODER_STEPS   4
////////////////////////////
#define VOLUME_MAX      100
#define VOLUME_MIN      1
#define VOLUME_DEFAULT  80
////////////////////////////
#define BUFF_SIZE 128
////////////////////////////
// spinlock
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

// Гучність 
volatile uint8_t gv_nVolume = 0;
// WiFi settings example, substitute your own
char* ssid = "VT";
const char* password = "vtvtvtvt";
// Налаштування потоку     
//  http://comet.shoutca.st:8563/1
//  http://online2.radioroks.ua/RadioROKS_HardnHeavy
const char *host = "online2.radioroks.ua";
const char *path = "/RadioROKS_HardnHeavy";
int httpPort = 80;
// Розмір буферу
uint8_t g_nStreamBuff[BUFF_SIZE];
