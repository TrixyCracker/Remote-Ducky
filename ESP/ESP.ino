#include <ESP8266WiFi.h>
#include <ESPAsyncWebServer.h>
#include <Wire.h>

AsyncWebServer server(80);

IPAddress local_ip(192,168,1,128);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

typedef struct
{
  char ssid[32];
  char password[32];
  uint8_t channel;
  uint8_t hidden;
  uint8_t max_connection;
} wifi_info;

wifi_info settings =
{
  .channel=1,
  .hidden=0,
  .max_connection=2
};

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Wire.begin(); // join as master

  SPIFFS.begin();

  setup_settings();
  Serial.println(settings.ssid);
  Serial.println(strlen(settings.ssid));
  Serial.println(settings.password);
  Serial.println(strlen(settings.password));
  Serial.println(settings.hidden);

  //WiFi.softAPConfig(local_ip, gateway, subnet);
  WiFi.softAP("ssid", "password");
  //WiFi.softAP(settings.ssid, settings.password, settings.channel, settings.hidden, settings.max_connection);

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/webserver/index.html", "text/html");
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/webserver/style.css", "text/css");
  });
  server.on("/script.js", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/webserver/script.js", "text/javascript");
  });

  server.on("/delete", HTTP_GET, [](AsyncWebServerRequest *request){

    if (request->hasParam("name"))
    {
      String filepath = "/scripts/" + request->getParam("name")->value();
      
      SPIFFS.remove(filepath.c_str());

      request->send(200, "text/plain", "");
    }
  });

  server.on("/read", HTTP_GET, [](AsyncWebServerRequest *request){

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
    else if (request->hasParam("scriptlist"))
    {
      String script_string = "";
    
      Dir dir = SPIFFS.openDir("/scripts/");
      while(dir.next())
      {
        script_string += dir.fileName().substring(9);
        /*script_string += " (" ;
        script_string += dir.fileSize(); 
        script_string += " bytes)";*/
        script_string += "-";
      }

      script_string[script_string.length() - 1] = '\0';
    
      request->send(200, "text/plain", script_string.c_str());
    }
    
  });

  server.on("/write", HTTP_GET, [] (AsyncWebServerRequest *request) {
    
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

      String path = "/scripts/" + script_name + ".sc";

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

void setup_settings()
{
  File settings_file = SPIFFS.open("/settings.st", "r");

  String file_buffer = "";

  while(settings_file.available())
  {
    file_buffer += (char)settings_file.read();
  }

  settings_file.close();

  int min_index;
  int max_index;
  String str;

  max_index = file_buffer.indexOf('\n');
  str = file_buffer.substring(0, max_index);
  strcpy(settings.ssid, str.c_str());

  min_index = max_index + 1;

  max_index = file_buffer.indexOf('\n', min_index);
  str = file_buffer.substring(min_index, max_index);
  strcpy(settings.password, str.c_str());

  min_index = max_index + 1;

  max_index = file_buffer.indexOf('\n', min_index);
  str = file_buffer.substring(min_index, max_index);
  settings.hidden = str.toInt();
}

void loop() {
  // put your main code here, to run repeatedly:

  
  
}
