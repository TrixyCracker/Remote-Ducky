#include "libraries/Keyboard_IT/Keyboard_IT.h"
#include <Wire.h>

bool is_string_received = false;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);

  Wire.begin(0x66); // join as slave with 0x66 address
  Wire.onReceive(read_buffer32);

  Keyboard.begin();
  delay(1000);
}

int default_delay;

String bufferr = "";
String partial_buffer = "";
int first_newline;
void loop() {
  // put your main code here, to run repeatedly:

  if (is_string_received)
  {
    Serial.println(bufferr.c_str());

    first_newline = bufferr.indexOf("\\n");
    while(true) {

      partial_buffer = bufferr.substring(0, first_newline);
      parser(partial_buffer);

      bufferr = bufferr.substring(first_newline + 2);

      first_newline = bufferr.indexOf("\\n");

      if (first_newline == -1)
      {
        parser(bufferr);
        break;
      }
      
    }

    is_string_received = false;
    bufferr = "";
  }
 
}

char c;
void read_buffer32()
{

  for (int i = 0; i < 32; ++i)
  {
    c = (char)Wire.read();
    if (c == '\0')
    {
      Serial.println("STRING OK");
      is_string_received = true;
      break;
    }
    bufferr += c;
  }
    
}


String parser_buffer;
void parser(String str)
{
  static int first_space;
  first_space = str.indexOf(' ');
  
  if (first_space == -1) 
  {

    if (str == "ENTER")
    {
      Keyboard.press(KEY_RETURN);
      Keyboard.release(KEY_RETURN);
    }
    else if (str == "DEFAULT_DELAY" || str == "DEFAULTDELAY")
    {
      delay(default_delay);
    }
    else return;
    
  }
  
  if (str.substring(0, first_space) == "STRING")
  {
    for (int i = first_space + 1; i < str.length(); ++i)
      Keyboard.write((char)str[i]);
  }
  else if (str.substring(0, first_space) == "DELAY")
  {
    static int delay_time;
    delay_time = str.substring(first_space + 1).toInt();  
    delay(delay_time);
  }
  else if (str.substring(0, first_space) == "DEFAULT_DELAY" || str.substring(0, first_space) == "DEFAULTDELAY")
  {
    default_delay = str.substring(first_space + 1).toInt();
  }
  else if (str.substring(0, first_space) == "GUI")
  {
    parser_buffer = str.substring(first_space + 1);

    if (parser_buffer.length() == 1)
    {
      Keyboard.press(KEY_LEFT_GUI);
      Keyboard.press((char)str[4]);
      Keyboard.releaseAll();
    }
  }
  else if (str.substring(0, first_space) == "ALT")
  {
    parser_buffer = str.substring(first_space + 1);

    if (parser_buffer.length() == 1)
    {
      Keyboard.press(KEY_LEFT_ALT);
      Keyboard.press((char)str[4]);
      Keyboard.releaseAll();
    }
  }

}
