#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>


// Use I2C, no reset pin!
Adafruit_CAP1188 cap = Adafruit_CAP1188();

void capSetup()
{
  if (!cap.begin()) {
    blinkDebug(1000);
    while (1);
  }
}

//------------------------------
byte capState()
{
  return cap.touched();
}

byte capAct()
{
  static byte lastTouch = 0;
  byte touch = cap.touched();

  if(touch)
  {
    if(touch != lastTouch)
    {
      lastTouch=touch;
      return touch;
    }
  }
  lastTouch=touch;
  return 0;
}

char ifBraille(uint8_t combination)
{//checks for valid usage in the character map
  for(byte i=0; i<ENCODEAMT;i++)
  {
    if(combination == pgm_read_byte(&byteToBraille[1][i]))
    {// for a corrisponding translation
      return pgm_read_byte(&byteToBraille[0][i]);
    }// return the matching letter in the array
  }
  return 0; // no matches try again
}


