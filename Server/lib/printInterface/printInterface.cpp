#include "printInterface.h"

//Pines, Serial, Conexión con SD --> inicializar.
void configPrintInterface(void){
  pinMode(BTN_PRINT_POWER,INPUT);
  pinMode(PIN_POWER_ON,OUTPUT); 
  pinMode(ENABLE_SPI_ESP32,OUTPUT);
  pinMode(ENABLE_SPI_PRINTER,OUTPUT);
  Serial.begin(BAUDIOSPEED1); 
  Serial2.begin(BAUDIOSPEED2, SERIAL_8N1, RXD2, TXD2);
  closeSD();
}

//ON-OFF impresora, al pulsar botón cambia el estado de: ON --> OFF o OFF --> ON.
bool printerPower(bool estadoPrevio){
  if(digitalRead(BTN_PRINT_POWER)){
    estadoPrevio = !estadoPrevio;
    digitalWrite(PIN_POWER_ON, estadoPrevio);
  }
  return estadoPrevio;
}

//Consulta temperaturas de la impresora y lo retorna en formato JSON.
String Temperaturas(void)
{
  String jsonTemperatura = "";
  
  if(digitalRead(PIN_POWER_ON)) Serial2.println("M105"); //Si la impresora está encendida, envía comando para consultar temperaturas - Ref: https://marlinfw.org/docs/gcode/M105.html
  else return jsonTemperatura;
  
  if (Serial2.available() > 0)
  {
    String buffer = Serial2.readString();//buffer almacena lo retornado por la impresora.
    Serial.print("BUFFER: ");
    Serial.println(buffer);
    byte posBuffer[5];

    //Formato dentro de variable "buffer"-->  ok T:100.39 /101.00 B:24.84 /0.00 @:38 B@:0
    posBuffer[0] = buffer.indexOf(':') + 1;                   //posición dentro del String luego de "T:"
    posBuffer[1] = buffer.indexOf('/', posBuffer[0]);         //posición dentro del String antes del 1er "/"
    posBuffer[2] = buffer.indexOf(':', posBuffer[1]) + 1;     //posición dentro del String luego de "B:"
    posBuffer[3] = buffer.indexOf('/', posBuffer[2]);         //posición dentro del String antes del 2do "/"
    posBuffer[4] = buffer.indexOf(':', posBuffer[3]);         //posición dentro del String antes del 3er ":"
    
    //con base en las posiciones obtenidas--> consigo como sub strings las temperaturas se arma el JSON:
    jsonTemperatura = "{\"Ex0Temp\":";
    jsonTemperatura += buffer.substring(posBuffer[0], posBuffer[1]);         // temp leida en hotEnd.
    jsonTemperatura += ",\"Ex0TempSet\":";
    jsonTemperatura += buffer.substring(posBuffer[1] + 1, posBuffer[2] - 2); // temp configurada del hotEnd.
    jsonTemperatura += ",\"BedTemp\":";
    jsonTemperatura += buffer.substring(posBuffer[2], posBuffer[3]);         // temp leida en cama.
    jsonTemperatura += ",\"BedTempSet\":";
    jsonTemperatura += buffer.substring(posBuffer[3] + 1, posBuffer[4] - 2); // temp configurada de la cama.
    jsonTemperatura += "}";
  }

  return jsonTemperatura;
}



//Recibe la ruta completa del archivo y retorna únicamente el nombre del mismo.
String correct_PATH(String Path)
{ 
  Serial.println("inicio correct PATH");
  // convertir String en Char Array:  https://circuits4you.com/2018/03/08/arduino-convert-string-to-character-array/#:~:text=Example%20Code%20for%20Converting%20String,toCharArray(char_array%2C%20str_len)%3B
  int str_len = Path.length() + 1;             // Length (with one extra character for the null terminator)
  char cadena[str_len];                        // Prepare the character array (the Path)
  Path.toCharArray(cadena, Path.length() + 1); // Copy it over
  // Fin de conversión.
  
  // Subdividir cadena de texto con base en delimitadores: https://parzibyte.me/blog/2018/11/13/separar-cadena-delimitadores-c-strtok/
  char *token2 = strtok(cadena, "/");  //separa con base en "/" que contiene el directorio...
  if (token2 != NULL)
  {
    String corectPATH = "";
    while (token2 != NULL)
    {
      // Sólo en la primera pasamos la cadena; en las siguientes pasamos NULL
      //printf("token2: %s\n", token2);
      corectPATH = String(token2);
      token2 = strtok(NULL, "/");
    }
    Serial.println("fin 1 correct PATH");
    return corectPATH;
  }
  else
    Serial.println("fin 2 correct PATH");
    return "ERROR PATH NULL";
}



//Manda a imprimir el archivo seleccionado.
String print_SD_File(String Path)
{
  String corregido = "";
  corregido = correct_PATH(Path);  //Envió directorio completo y me retorna solo el nombre del archivo.
  Serial.print("corregido: ");
  Serial.println(corregido);

  String buffer = "";

  Serial2.println("M20 L"); //GCODE para lista de archivos en SD - Ref: https://marlinfw.org/docs/gcode/M020.html
  buffer = Serial2.readString();
  /*
  Formato de texto leído que se almacena en variable "buffer":
   Begin file list
   GCODES/PALETS~1.GCO 312413 /palets.gcode
   GCODES/4SOPOR~1.GCO 25388448 /4 soportes.gcode
   GCODES/6ESLAB~1.GCO 1313623 /6 eslabones.gcode
   End file list
   ok
  */
  Serial.println();
  Serial.print("Bufer: ");
  Serial.println(buffer);

  // convertir String en Char Array:  https://circuits4you.com/2018/03/08/arduino-convert-string-to-character-array/#:~:text=Example%20Code%20for%20Converting%20String,toCharArray(char_array%2C%20str_len)%3B
  int str_len = buffer.length() + 1;                   // Length (with one extra character for the null terminator)
  char char_array[str_len];                            // Prepare the character array (the buffer)
  buffer.toCharArray(char_array, buffer.length() + 1); // Copy it over
  // Fin de conversión.

  // Subdividir cadena de texto con base en delimitadores: https://parzibyte.me/blog/2018/11/13/separar-cadena-delimitadores-c-strtok/
  char delimitador[] = "\n";
  char *token = strtok(char_array, delimitador);

  if (token != NULL)
  {
    while (token != NULL)
    {
      token = strtok(NULL, delimitador);
      String aStringObject = token; // convierto ArrayCahr(token) en String(aStringObject).
      Serial.print("string:");
      Serial.println(aStringObject);
      if (aStringObject.indexOf(corregido) != -1)
      { // indexOf retorna -1 si no encuentra el sub string.
        // Serial.print("string:");
        // Serial.println(aStringObject);
        Serial.println();
        Serial.print("cod printer path:");
        Serial.println(aStringObject.substring(0, aStringObject.indexOf(' ')));
        Serial2.println("M23 " + aStringObject.substring(0, aStringObject.indexOf(' '))); // Select SD file: https://marlinfw.org/docs/gcode/M023.html
        String confirmFileSelected = Serial2.readString();
        // Serial.println(confirmFileSelected);
        if (confirmFileSelected.indexOf("File selected"))
        {
          Serial2.println("M24");                          // Ref: https://marlinfw.org/docs/gcode/M024.html
          return "Starting print, file: " + corregido;     // pudo realizar todo el proceso y se mandó a imprimir.
        }
      }
    }
    return "Fail print, file: " + corregido;                // significa que recorrió todo con el while, pero no encontró nada.
  }
  else
  {
    return "Fail print, file: " + corregido;
  }
  // Fin sub división.
}

//Conecta BUS SPI entre SD e Impresora. En caso de iniciar conexión con SD retorna "TRUE" coca contrario "FALSE".
boolean openSD(void) {
  if(digitalRead(ENABLE_SPI_PRINTER) && !digitalRead(ENABLE_SPI_ESP32)) return true;
  digitalWrite(ENABLE_SPI_PRINTER, HIGH);  //PIN 2 conectado a 2OE
  digitalWrite(ENABLE_SPI_ESP32, LOW);    //PIN 14 conectado a 1OE
  delay(1); 
  return SD.begin();
}

//Desconecta BUS SPI entre SD e Impresora.
void closeSD(void) {
  Serial.println("M21");
  digitalWrite(ENABLE_SPI_ESP32, HIGH);  //PIN 14 conectado a 1OE
  digitalWrite(ENABLE_SPI_PRINTER, LOW); //PIN 2 conectado a 2OE
  Serial2.println("M21"); //GCODE para inicar conexión de SD con impresora --> https://marlinfw.org/docs/gcode/M021.html
}

//Mueve ejes y extrusor, Homing, apaga y enciende impresora: 
String sendPrinterControl(const char *receptor, String value){
    if (strcmp(receptor, "X") == 0) {
        Serial2.println("G91");             //https://marlinfw.org/docs/gcode/G091.html
        value = "G0 X" + value; 
        Serial2.println(value);
        Serial2.println("G90");             //https://marlinfw.org/docs/gcode/G090.html                        
    } 
    else if (strcmp(receptor, "Y") == 0) {
        Serial2.println("G91");
        value = "G0 Y" + value; 
        Serial2.println(value);
        Serial2.println("G90");                        
    }
    else if (strcmp(receptor, "Z") == 0) {
        Serial2.println("G91");
        value = "G0 Z" + value; 
        Serial2.println(value);
        Serial2.println("G90");                              
    } 
    else if (strcmp(receptor, "HOMEALL") == 0) {
            value = "G28";                         //https://marlinfw.org/docs/gcode/G028.html 
            Serial2.println(value);                     
    } 
    else if (strcmp(receptor, "HOMEX") == 0) {
            value = "G28 X";
            Serial2.println(value);                               
    }
    else if (strcmp(receptor, "HOMEY") == 0) {
            value = "G28 Y";
            Serial2.println(value);                              
    } 
    else if (strcmp(receptor, "HOMEZ") == 0) {
            value = "G28 Z";
            Serial2.println(value);                            
    }
    else if (strcmp(receptor, "BTNON") == 0) {
            digitalWrite(PIN_POWER_ON, HIGH);         //enciende impresora.                  
    }
    else if (strcmp(receptor, "BTNOFF") == 0) {
            digitalWrite(PIN_POWER_ON, LOW);          //apaga impresora.                        
    }
    return "OK"; 
}


//Envía comandos GCODE introducidos de forma manual. 
String sendGCODE(String value){
  String buffer = "";
  Serial2.println(value);
  if (Serial2.available()>0){
      buffer = Serial2.readString();
  }
  return buffer;
}