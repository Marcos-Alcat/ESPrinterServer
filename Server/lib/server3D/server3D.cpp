#include "server3D.h"

// Make size of files human readable
// source: https://github.com/CelliesProjects/minimalUploadAuthESP32
String humanReadableSize(const size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}

// parses and processes webpages
// if the webpage has %SOMETHING% or %SOMETHINGELSE% it will replace those strings with the ones defined
String processor(const String& var) {
  if (var == "FIRMWARE") {
    return FIRMWARE_VERSION;
  }
  if (var == "FREESPIFFS") {
    return humanReadableSize((SD.totalBytes() - SD.usedBytes()));
  }
  if (var == "USEDSPIFFS") {
    return humanReadableSize(SD.usedBytes());
  }
  if (var == "TOTALSPIFFS") {
    return humanReadableSize(SD.totalBytes());
  }
  return "NULL";
}

//Ref: https://github.com/Marcos-Alcat/ESP32_FileServer/tree/master/example-03
// list all of the files, if ishtml=true, return html rather than simple text
String listFiles(bool ishtml) {
  if(!openSD()){closeSD(); return "OPEN SD FAIL";}
  String returnText = "";
  String pesoEnBits = "";
  Serial.println("Listing files stored on SD");
  File root = SD.open("/");
  File foundfile = root.openNextFile();
  if (ishtml)
  { 
    returnText += "<p>Free Storage: <span>" + humanReadableSize((SD.totalBytes() - SD.usedBytes()));
    returnText += "</span> | Used Storage: <span>" + humanReadableSize(SD.usedBytes());
    returnText += "</span> | Total Storage: <span>" + humanReadableSize(SD.totalBytes());
    returnText += "</span></p>";
    returnText += "<table><tr><th align='left'>Name</th><th align='left'>Size</th><th></th><th></th></tr>";
  }
  while (foundfile)
  {
    if (ishtml)
    { 
      pesoEnBits = humanReadableSize(foundfile.size());
      if(pesoEnBits != "0 B"){
      returnText += "<tr align='left'><td> - " + String(foundfile.name()) + "</td><td>" + pesoEnBits + " </td>";
      returnText += "<td><button onclick=\"printDeleteButton(\'/" + String(foundfile.name()) + "\', \'print\')\">Print</button>";
      returnText += "<td><button onclick=\"printDeleteButton(\'/" + String(foundfile.name()) + "\', \'delete\')\">Delete</button></tr>";
      }
    }
    else
    {
      returnText += "File: " + String(foundfile.name()) + " Size: " + humanReadableSize(foundfile.size()) + "\n";
    }
    foundfile = root.openNextFile();
  }
  if (ishtml)
  {
    returnText += "</table>";
  }
  root.close();
  foundfile.close();
  Serial.println("cerrando...");
  closeSD();
  return returnText;
}


//configura el server.
void configureWebServer(AsyncWebServer *server) {
  Serial.println("Mounting SPIFFS ...");
  if (!SPIFFS.begin(true)) {
    // if you have not used SPIFFS before on a ESP32, it will show this error.
    // after a reboot SPIFFS will be configured and will happily work.
    Serial.println("ERROR: Cannot mount SPIFFS, Rebooting");
    ESP.restart();
  }
  
  // configure web server
  Serial.println("Configuring Webserver ...");
  // if url isn't found
  server->onNotFound(notFound);

  // run handleUpload function when any file is uploaded
  server->onFileUpload(handleUpload);

  // visiting this page will cause you to be logged out
  server->on("/logout", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->requestAuthentication();
    request->send(401);
  });

  // presents a "you are now logged out webpage
  server->on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);
    //request->send(SPIFFS, PAGE_LOGOUT_HTML, String(), false, processor);
    request->send(SPIFFS, PAGE_LOGOUT_HTML);
  });

  
//...............................INICIO - CARGA ARCHIVOS INDEX...................................//
  server->on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();

    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send(SPIFFS, PAGE_INDEX_HTML);
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }

  });

    server->on("/scripts.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send(SPIFFS, PAGE_INDEX_JS);
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }

  });

      server->on("/styles.css", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send(SPIFFS, PAGE_INDEX_CSS);
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });
    server->on("/scripts/highcharts.js", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + + " " + request->url();

    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send(SPIFFS, JS_HIGHCHARTS_CHART);
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });

    server->on("/temp", HTTP_GET, [](AsyncWebServerRequest *request)
             { request->send(200, "text/plain", Temperaturas()); });
//...............................FIN - CARGA ARCHIVOS INDEX.....................................//



  server->on("/reboot", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();

    if (checkUserWebAuth(request)) {
      request->send(SPIFFS, PAGE_REBOOT_HTML);
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      ESP.restart();
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });


  server->on("/listfiles", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      request->send(200, "text/plain", listFiles(true));
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });
  
  //------------------Recibe el comando para eliminar o imprimir un archivo--------------------------//
  server->on("/file", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);

      if (request->hasParam("name") && request->hasParam("action")) {
        const char *fileName = request->getParam("name")->value().c_str();
        const char *fileAction = request->getParam("action")->value().c_str();

        logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url() + "?name=" + String(fileName) + "&action=" + String(fileAction);
        openSD();
        if (!SD.exists(fileName)) {
          Serial.println(logmessage + " ERROR: file does not exist");
          request->send(400, "text/plain", "ERROR: file does not exist");
        } else {
          Serial.println(logmessage + " file exists");
          if (strcmp(fileAction, "print") == 0) {
              closeSD();
              request->send(200, "text/plain",print_SD_File(fileName));
          } else if (strcmp(fileAction, "delete") == 0) {
            logmessage += " deleted";
            SD.remove(fileName);
            request->send(200, "text/plain", "Deleted File: " + String(fileName));
          } else {
            logmessage += " ERROR: invalid action param supplied";
            request->send(400, "text/plain", "ERROR: invalid action param supplied");
          }
          Serial.println(logmessage);
        }
        closeSD();
      } else {
        request->send(400, "text/plain", "ERROR: name and action params required");
      }
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });
  //------------------fin de funcion----------------------------------------------------//


 //------------------Recibe los valores de los sliders que controlan temperatura y velocidad--------------------------//
  server->on("/slider", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);

      if (request->hasParam("name") && request->hasParam("value")) {
        const char *sliderName = request->getParam("name")->value().c_str();
        String sliderValue = request->getParam("value")->value().c_str();
        if (strcmp(sliderName, "slider-ui color1") == 0) {
            sliderValue = "M140 S" + sliderValue;                            //Set Bed temperature: https://marlinfw.org/docs/gcode/M140.html
            Serial2.println(sliderValue);
          } 
        else if (strcmp(sliderName, "slider-ui color2") == 0) {
            sliderValue = "M104 S" + sliderValue;                            //Set Hotend Temperature: https://marlinfw.org/docs/gcode/M104.html
            Serial2.println(sliderValue);
          } 
          request->send(400, "text/plain", "OK");
      }
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });
  //--------------------------------------fin de funcion----------------------------------------------------//


//------------------Recibe los valores para controlar ejes/extrusor, Homing, apaga y enciende impresora ----//
  server->on("/controlMove", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);
      if (request->hasParam("move") && request->hasParam("value")) {
        const char *moveName = request->getParam("move")->value().c_str();
        String moveValue = request->getParam("value")->value().c_str();
        request->send(400, "text/plain", sendPrinterControl(moveName, moveValue));
      }
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });
  //--------------------------------------fin de funcion----------------------------------------------------//


//------------------Recibe los valores de los sliders que controlan temperatura y velocidad--------------------------//
  server->on("/sendGCODE", HTTP_GET, [](AsyncWebServerRequest * request) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    if (checkUserWebAuth(request)) {
      logmessage += " Auth: Success";
      Serial.println(logmessage);

      if (request->hasParam("value")) {
        String value = request->getParam("value")->value().c_str();
        request->send(400, "text/plain", sendGCODE(value));
      }
    } else {
      logmessage += " Auth: Failed";
      Serial.println(logmessage);
      return request->requestAuthentication();
    }
  });
  //--------------------------------------fin de funcion----------------------------------------------------//

  server->begin();// start web server
  Serial.println("Starting Webserver ...");
}

void notFound(AsyncWebServerRequest *request) {
  String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
  Serial.println(logmessage);
  request->send(404, "text/plain", "Not found");
}

// used by server.on functions to discern whether a user has the correct httpapitoken OR is authenticated by username and password
bool checkUserWebAuth(AsyncWebServerRequest * request) {
  bool isAuthenticated = false;

  if (request->authenticate(httpuser.c_str(), httppassword.c_str())) {
    Serial.println("is authenticated via username and password");
    isAuthenticated = true;
  }
  return isAuthenticated;
}

// handles uploads to the filserver
void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  // make sure authenticated before allowing upload
  if (checkUserWebAuth(request)) {
    String logmessage = "Client:" + request->client()->remoteIP().toString() + " " + request->url();
    Serial.println(logmessage);

    if (!index) {
      logmessage = "Upload Start: " + String(filename);
      // open the file on first call and store the file handle in the request object
      openSD();
      request->_tempFile = SD.open("/" + filename, "w");
      Serial.println(logmessage);
    }

    if (len) {
      // stream the incoming chunk to the opened file
      request->_tempFile.write(data, len);
      logmessage = "Writing file: " + String(filename) + " index=" + String(index) + " len=" + String(len);
      Serial.println(logmessage);
    }

    if (final) {
      logmessage = "Upload Complete: " + String(filename) + ",size: " + String(index + len);
      // close the file handle as the upload is now done
      request->_tempFile.close();
      Serial.println(logmessage);
      closeSD();
      request->redirect("/");
    }
  } else {
    Serial.println("Auth: Failed");
    return request->requestAuthentication();
  }
}
