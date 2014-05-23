//The following are general purpose functions for the Adafruit Bluefruit EZ-Key HID
#include <SoftwareSerial.h>

// Code for Adafruit Bluefruit EZ-Key serial reports
// http://www.cambiaresearch.com/articles/15/javascript-char-codes-key-codes
SoftwareSerial BT = SoftwareSerial(8, 9);//first is rx, second is tx
//connect assigned tx on arduino to rx on the bluefruit

#define BACK byte(0x08)
#define KEY_RETURN byte(0x0A)
#define KEY_LEFT_ARROW byte(0x0B)
#define KEY_RIGHT_ARROW byte(0x07)
// and others
#define LEFTCONTROL byte(0xE0)
#define LEFTSHIFT byte(0xE1)
#define LEFTALT byte(0xE2)
#define LEFTGUI byte(0xE3)
#define RIGHTCONTROL byte(0xE4)
#define RIGHTSHIFT byte(0xE5)
#define RIGHTALT byte(0xE6)
#define RIGHTGUI byte(0xE7)
#define INSERT byte(0x01)
#define HOME byte(0x02)
#define PAGEUP byte(0x03)
#define DELETE byte(0x04)
#define END byte(0x05)
#define PAGEDOWN byte(0x06)
#define TAB byte(0x09)
#define DOWNARROW byte(0x0C)
#define UPARROW byte(0x0E)
#define ESC byte(0x0B)
#define CAPSLOCK byte(0x1C)

//-----------------------------

/* ------INDEX of Functions
 SETUP
 outputUp(); // bang! bring up serial conection with bluefruit
 
 GENERAL
 bluePrint('char'); // prints passed argument
 
 TESTING
 printabletest();// send printable characters
 nonprinting(); // actuate non-printable keystrokes
 */

// --------------SETUP --------------------------
void blueUp()
{
  BT.begin(9600);
}

//-----------------------GENERAL ------------------
void bluePrint(char key)
{// base blueprint that only takes a single key and prints it
  if(key==8)
  {// if this was set up right maybe only a direct pass would be nessisary
    BT.write(BACK);
  }
  else
  {
    BT.write(key);
    //Serial.print(key);//debugging 
  }
}

void bluePrint(char key, int presses)
{// blueprint that takes number of presses argument
  for(int i=0;i<presses;i++)
  {
    bluePrint(key);
  }
}

void bluePrint(char input[])
{// blueprint that takes a char array or line in "quotes"
  int i = 0;
  while (input[i])
  {
    BT.write(input[i]);
    i++;
  }
}

void bluePrintln(char input[])// take a char array or line in "quotes"
{
  bluePrint(input);
  BT.write(KEY_RETURN);
}

//HID only / serial creative (new lining reproduction)
void enter()
{
  BT.write(KEY_RETURN);
}

void movement(int increment, boolean bearing)
{
  char going;
  if(bearing)//if true aka 1
  {
    going=KEY_LEFT_ARROW;//LEFT=1 defined golobally
  }
  else//if false aka 0
  {
    going=KEY_RIGHT_ARROW;//RIGHT=0 defined golobally
  }
  for (int i=0;i<increment;i++)
  {//go that direction as many times as asked
    BT.write(going);
  }
}

//------ braille particular -----------

void blueBraille(byte pads)
{
  for(byte i=0; i<ENCODEAMT; i++)
  {
    if(pads == pgm_read_byte(&byteToBraille[1][i]))
    {
      BT.write(pgm_read_byte(&byteToBraille[0][i])); 
    }
  }
}

//--------------------------------------------------- testing
void blueTesting()
{
  static char count = '1';

  bluePrintln("This is a test of bluefruit!");
  count++;

  if (count > '9')
  {
    while(1){
      ;
    }
  }
}








