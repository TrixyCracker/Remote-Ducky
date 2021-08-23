#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

const char* PARAM_INPUT_1 = "script";

AsyncWebServer server(80);

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Wire.begin(); // join as master

  SPIFFS.begin();

  WiFi.softAP("ssid", "password");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/webserver/index.html", "text/html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/webserver/style.css", "text/css");
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/webserver/script.js", "text/javascript");
  });

  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String str;
    
    if (request->hasParam(PARAM_INPUT_1)) {

      str = request->getParam(PARAM_INPUT_1)->value();

      //if (str.substring(str.length() - 5) != "\\nEND")
      {
        //str += "\\nEND";
      }

      request->send(200, "text/plain", "");

      Serial.print("SCRIPT : ");
      Serial.println(str.c_str());

      Serial.println("BEGIN TRANSMISSION");

      while(true) {
        if (32 >= str.length())
        {
          if (str.length() == 32)
            i2c_send_string(str.c_str(), str.length());
          else
            i2c_send_string(str.c_str(), str.length() + 1);
          break;
        }
        else
        {
          i2c_send_string(str.substring(0, 32).c_str(), 32);
          str = str.substring(32);
        }
      }

      Serial.println("FINISHED TRANSMISSION");
      
    }
  });

  server.begin();

}

void i2c_send_string(const char * str, int sizee)
{
  Wire.beginTransmission(0x66);
  for (int i = 0; i < sizee; ++i)
  {
    Wire.write((char)str[i]);
  }
  Wire.endTransmission();
}

void loop() {
  // put your main code here, to run repeatedly:

  
  
}
