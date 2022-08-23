#include "DNSPrinter.h"

//basado en: https://www.youtube.com/watch?v=46BLISvOsrM

//recibe como parámetro el nombre a utilizar e inicializa el DNS.
void iniciarDNS(const char* domainDNS){
      if (!MDNS.begin(domainDNS)) {
    Serial.println("Error configurando mDNS!");
  }
  else{
  Serial.println("mDNS configurado");
  MDNS.addService("http", "tcp", 80);
  }
}

