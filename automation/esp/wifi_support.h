#ifndef wifi_support_h
#define wifi_support_h


#include <WiFi.h>
#include <ArduinoOTA.h>


/**************** Function Prototypes *******************/
void setupWifi(const char* ssid, const char* pwd);
void setupOTA(int port,char* host_name);

#endif
