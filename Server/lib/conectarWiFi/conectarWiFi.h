#ifndef CONECTARWIFI_H   //guarda del header.
#define CONECTARWIFI_H

#include <Arduino.h>
#include <WiFi.h>
const String ssid = "USER";               //SSID de red WiFi.
const String wifipassword = "PSW";    //contraseña de red WiFi.

void conectionWiFi(void);                    

#endif
