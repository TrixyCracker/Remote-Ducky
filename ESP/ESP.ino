#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

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

  server.on("/scriptlist", HTTP_GET, [](AsyncWebServerRequest *request){
    String script_string = "";
    
    Dir dir = SPIFFS.openDir("/scripts/");
    while(dir.next())
    {
      script_string += dir.fileName().substring(9);
      script_string += "-";
    }

    script_string[script_string.length() - 1] = '\0';
    
    request->send(200, "text/plain", script_string.c_str());
  });

  server.on("/scriptread", HTTP_GET, [](AsyncWebServerRequest *request){

    if (request->hasParam("name"))
    {
      String script_name = request->getParam("name")->value();

      String path = "/scripts/" + script_name;

      String script = "";

      File script_file = SPIFFS.open(path.c_str(), "r");

      while(script_file.available()) 
        script += (char)script_file.read();

      script_file.close();

      request->send(200, "text/plain", script.c_str());
    }
    
  });

  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
    if (request->hasParam("run")) {

      String script = request->getParam("run")->value();

      request->send(200, "text/plain", "");

      Serial.print("SCRIPT : ");
      Serial.println(script.c_str());

      Serial.println("BEGIN TRANSMISSION");

      while(true) {
        if (32 >= script.length())
        {
          if (script.length() == 32)
            i2c_send_string(script.c_str(), script.length());
          else
            i2c_send_string(script.c_str(), script.length() + 1);
          break;
        }
        else
        {
          i2c_send_string(script.substring(0, 32).c_str(), 32);
          script = script.substring(32);
        }
      }

      Serial.println("FINISHED TRANSMISSION");
      
    }
    else if (request->hasParam("name") && request->hasParam("script"))
    {

      String script_name = request->getParam("name")->value();
      String script_code = request->getParam("script")->value();

      String path = "/scripts/" + script_name;

      request->send(200, "text/plain", "");

      File script_file = SPIFFS.open(path.c_str(), "w");
      script_file.print(script_code.c_str());
      script_file.close();
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
