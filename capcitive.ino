#include <Wire.h>
#include <SPI.h>
#include <Adafruit_CAP1188.h>


// Use I2C, no reset pin!
Adafruit_CAP1188 cap = Adafruit_CAP1188();

void capSetup()
{
  if (!cap.begin()) 
  {
    while (1){;}
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



