#ifndef PRINTINTERFACE_H
#define PRINTINTERFACE_H
#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <FS.h>
#include <SPIFFS.h>

#define RXD2 16               //pin Rx de conexión para Serial2, comunicación serial con la impresora 3D.
#define TXD2 17               //pin Tx de conexión para Serial2, comunicación serial con la impresora 3D.
#define BAUDIOSPEED1 115200    //velocidad comunicación Serial1.
#define BAUDIOSPEED2 250000    //velocidad comunicación Serial2, comunicación serial con la impresora 3D.
#define BTN_PRINT_POWER 34    //botón de encendido en GPIO34.
#define PIN_POWER_ON 4        //pin que enciende o apaga la fuente de la impresora, GPIO4.
#define ENABLE_SPI_ESP32 14   //pin de habilitación para BUS SPI con SD y ESP32.
#define ENABLE_SPI_PRINTER 2  //pin de habilitación para BUS SPI con SD e impresora 3D.

String Temperaturas(void);
String print_SD_File(String);
void configPrintInterface(void);
bool printerPower(bool);
boolean openSD(void);
void closeSD(void);
String sendPrinterControl(const char *, String);
String sendGCODE(String);
#endif
