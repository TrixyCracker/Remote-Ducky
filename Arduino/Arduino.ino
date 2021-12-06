#include "Keyboard_IT.h"
#include <Wire.h>

#define DEBUG

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

#ifdef DEBUG
    Serial.print("SCRIPT: ");
    Serial.println(script_buffer.c_str());
#endif

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
  static String line_buffer = "";
  
  static int min_index = 0;
  static int max_index = 0;

  while (true)
  {
    max_index = script_buffer.indexOf("@@", min_index);

    if (max_index == -1)
    {
      line_buffer = script_buffer.substring(min_index);
    }
    else
    {
      line_buffer = script_buffer.substring(min_index, max_index);
    }

#ifdef DEBUG
    Serial.print("Linea: ");
    Serial.println(line_buffer.c_str());
#endif

    parse_key_modifiers_string(line_buffer);
    parse_command_string(line_buffer);

    if (max_index == -1) break;

    min_index = max_index + 2;
  }

}

void parse_command_string(String script_line)
{
  static int first_space;
  first_space = script_line.indexOf(' ');

  String command = script_line.substring(0, first_space);

  if (first_space > 0) // Check command with args
  {

    if (command == "STRING")
    {
      for (int i = first_space + 1; i < script_line.length(); ++i)
      {
        Keyboard.write((char)script_line.c_str()[i]);
      }
    }
    else if (command == "DELAY")
    {
      static int delay_time;
      delay_time = script_line.substring(first_space + 1).toInt();
      delay(delay_time);
    }
    else if (command == "DEFAULT_DELAY" || command == "DEFAULTDELAY")
    {
      default_delay = script_line.substring(first_space + 1).toInt();
    }

  }
  else // Check command without args
  {
    if (script_line == "DEFAULT_DELAY" || script_line == "DEFAULTDELAY")
    {
      delay(default_delay);
    }
  }

}

void parse_key_modifiers_string(String script_line)
{
  bool first_cycle_flag = false;

  int min_index = 0;
  int max_index = 0;

  String command;

  while (true)
  {
    max_index = script_line.indexOf(' ', min_index);

    if (max_index == -1)
    {
      command = script_line.substring(min_index);
    }
    else
    {
      command = script_line.substring(min_index, max_index);
    }

    if (command == "CTRL")
    {
      Keyboard.press(KEY_LEFT_CTRL);
    }
    else if (command == "SHIFT")
    {
      Keyboard.press(KEY_LEFT_SHIFT);
    }
    else if (command == "GUI")
    {
      Keyboard.press(KEY_LEFT_GUI);
    }
    else if (command == "ALT")
    {
      Keyboard.press(KEY_LEFT_ALT);
    }
    else if (command == "UPARROW")
    {
      Keyboard.press(KEY_UP_ARROW);
    }
    else if (command == "DOWNARROW")
    {
      Keyboard.press(KEY_DOWN_ARROW);
    }
    else if (command == "LEFTARROW")
    {
      Keyboard.press(KEY_LEFT_ARROW);
    }
    else if (command == "RIGHTARROW")
    {
      Keyboard.press(KEY_RIGHT_ARROW);
    }
    else if (command == "BACKSPACE")
    {
      Keyboard.press(KEY_BACKSPACE);
    }
    else if (command == "TAB")
    {
      Keyboard.press(KEY_TAB);
    }
    else if (command == "ENTER" || command == "RETURN")
    {
      Keyboard.press(KEY_RETURN);
    }
    else if (command == "ESC")
    {
      Keyboard.press(KEY_ESC);
    }
    else if (command == "INSERT")
    {
      Keyboard.press(KEY_INSERT);
    }
    else if (command == "DELETE" || command == "CANC")
    {
      Keyboard.press(KEY_DELETE);
    }
    else if (command == "PAGEUP")
    {
      Keyboard.press(KEY_PAGE_UP);
    }
    else if (command == "PAGEDOWN")
    {
      Keyboard.press(KEY_PAGE_DOWN);
    }
    else if (command == "HOME")
    {
      Keyboard.press(KEY_HOME);
    }
    else if (command == "END")
    {
      Keyboard.press(KEY_END);
    }
    else if (command == "F1")
    {
      Keyboard.press(KEY_F1);
    }
    else if (command == "F2")
    {
      Keyboard.press(KEY_F2);
    }
    else if (command == "F3")
    {
      Keyboard.press(KEY_F3);
    }
    else if (command == "F4")
    {
      Keyboard.press(KEY_F4);
    }
    else if (command == "F5")
    {
      Keyboard.press(KEY_F5);
    }
    else if (command == "F6")
    {
      Keyboard.press(KEY_F6);
    }
    else if (command == "F7")
    {
      Keyboard.press(KEY_F7);
    }
    else if (command == "F8")
    {
      Keyboard.press(KEY_F8);
    }
    else if (command == "F9")
    {
      Keyboard.press(KEY_F9);
    }
    else if (command == "F10")
    {
      Keyboard.press(KEY_F10);
    }
    else if (command == "F11")
    {
      Keyboard.press(KEY_F11);
    }
    else if (command == "F12")
    {
      Keyboard.press(KEY_F12);
    }
    else if (first_cycle_flag && command.length() == 1)
    {
      Keyboard.press(script_line[min_index]);
    }

    if (max_index == -1)
    {
      Keyboard.releaseAll();
      delay(10);
      break;
    }

    first_cycle_flag = true;

    min_index = max_index + 1;

    delay(10);
  }
}
