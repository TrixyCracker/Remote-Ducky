#include "./libraries/Keyboard_IT/Keyboard_IT.h"
#include <Wire.h>

volatile bool new_script_ready = false;

String i2c_buffer = "";
String script_buffer = "";

int default_delay = 0;

void setup()
{
  // put your setup code here, to run once:

#ifdef DEBUG
  Serial.begin(115200);
#endif

  Wire.begin(0x66); // join as slave with 0x66 address
  Wire.onReceive(read_buffer32);

  Keyboard.begin();
  delay(1000);
}

void loop()
{
  // put your main code here, to run repeatedly:

  if (new_script_ready)
  {
    new_script_ready = false;

    script_buffer = i2c_buffer;
    i2c_buffer = "";
    
    start_script();
  }

  delay(500);
}

void read_buffer32()
{ 
  static char c;

  for (int i = 0; i < 32; ++i)
  {
    c = (char)Wire.read();
    
    if (c == '\0')
    {
      new_script_ready = true;
      break;
    }
    i2c_buffer += c;
  }

}

void start_script()
{
  static String line_buffer;
  line_buffer = "";

  static bool replay;
  replay = false;

  static int min_index;
  static int max_index;

#ifdef DEBUG
  Serial.print(F("BUFFER : "));
  Serial.println(script_buffer.c_str());
#endif

  max_index = script_buffer.indexOf("@@");

  if (script_buffer.substring(0, max_index) == "REPLAY")
  {
    script_buffer = script_buffer.substring(max_index + 2);
    replay = true;
  }

  do
  {
    min_index = 0;
    max_index = 0;

    while (true)
    {
      max_index = script_buffer.indexOf("@@", min_index);

      if (max_index == -1) // last line
      {
        line_buffer = script_buffer.substring(min_index);
        parser(line_buffer);
        
        break;
      }
      else
      {
        line_buffer = script_buffer.substring(min_index, max_index);
        parser(line_buffer);
      }

      min_index = max_index + 2; // +2 for let out '@@'
    }

  } while (replay && !new_script_ready);
  
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
    else
      return;
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
