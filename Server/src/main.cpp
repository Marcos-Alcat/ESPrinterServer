#include "server3D.h"
#include "OTA.h"

bool estadoOnOff = 1; //variable de estado para botón ON-OFF físico.

void setup() {
  configPrintInterface();
  conectionWiFi(); 
  iniciarOTA();
  configureWebServer(new AsyncWebServer(webserverporthttp));
  iniciarDNS("esprinterserver");
}


void loop() {
estadoOnOff = printerPower(estadoOnOff);
loopOTA(); 
}
