#ifndef SERVER3D_H
#define SERVER3D_H
#include "printInterface.h"
#include "DNSPrinter.h"
#include "AsyncTCP.h"
#include <ESPAsyncWebServer.h>


const String httpuser = "admin";
const String httppassword = "admin";
const int webserverporthttp = 80;

#define FIRMWARE_VERSION "v0.0.1"

#define PAGE_INDEX_HTML         "/html/index.html"           //ruta del archivo html pagina menu SD.
#define PAGE_INDEX_CSS          "/styles/styles.css"         //ruta del archivo css pagina menu SD.
#define PAGE_INDEX_JS           "/scripts/scripts.js"        //ruta del archivo JavaScript pagina menu SD.
#define JS_HIGHCHARTS_CHART     "/scripts/highcharts.js"     //ruta del archivo JavaScript gráfico pagina menu de la impresora.
#define PAGE_LOGOUT_HTML        "/html/logout.html"          //ruta del archivo html pagina de loguot.
#define PAGE_REBOOT_HTML        "/html/reboot.html"          //ruta del archivo html pagina de reboot.


void configureWebServer(AsyncWebServer *server);
String listFiles(bool ishtml = false);
bool checkUserWebAuth(AsyncWebServerRequest * request);
void notFound(AsyncWebServerRequest *request);
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final);

#endif
