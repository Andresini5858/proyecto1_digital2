/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME  "andresini58"
#define IO_KEY       "aio_nKrq8645VoAkagZKMeEoRo4BdpJP"

/******************************* WIFI **************************************/

#define WIFI_SSID "RedMesh"
#define WIFI_PASS "SPIKY2011"

// comment out the following lines if you are using fona or ethernet
#include "AdafruitIO_WiFi.h"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);