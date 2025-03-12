#if  defined(ESP32) ||  defined(ESP8266)
  #include "wifi_apps_script_ESP.h"
#elif defined(ARDUINO_SAMD_MKRWIFI1010)
  #include "wifi_apps_script_NINA.h"
#else
  #error "Piattaforma non riconosciuta, codice confiurato per esp32, esp8266 o mkr1010"
#endif

