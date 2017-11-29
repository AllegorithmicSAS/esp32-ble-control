#pragma once

#include <WiFi.h>

const char* ssid     = "Led Driver";
const char* password = "allego";

WiFiServer server(80);

void transportWifiInit(ILedDriver * driver) {
    WiFi.begin(ssid, password);
    
    // WiFiServer server(80);    
}