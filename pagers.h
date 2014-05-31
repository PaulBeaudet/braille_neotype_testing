//Pagers //Haptic feedback hardware

byte pagers[]=
{//arangement for the spark neomouse 
  A0,A4,A1,A5,A7,A6,// NOTE ---set the desired button pins here--- NOTE
};//these really need to be assigned in corrispondence with the input pin arrangement
//starting from least significant bit on

#define NUMPAGERS sizeof(pagers)

//------------SETUP --------------------------

void pagersUp()
{ //setup the pager motor pins as outputs
  for (byte set=0;set<NUMPAGERS;set++)
  { 
    pinMode(pagers[set], OUTPUT);
  }
}
//-------------- addressing---------------
#define DEFAULTPWM 100

void patternVibrate(byte pins)//
{
  for (byte i=0; i<NUMPAGERS; i++) // !! only the first 6 bits from the least significant are necisarry !!
  {//!! convert to read from least significant bit!!
    if (pins & (1 << i)) // show desired bit (pin)
    { // imagine incoming byte as an array of bits, one for each pager
      analogWrite(pagers[i], DEFAULTPWM);
    }
    else
    {
      analogWrite(pagers[i], 0);
    }
  }
}

void patternVibrate(byte pins, byte pwm)//
{
  for (byte i=0; i<NUMPAGERS; i++) // !! only the first 6 bits from the least significant are necisarry !!
  {//!! convert to read from least significant bit!!
    if (pins & (1 << i)) // show desired bit (pin)
    { // imagine incoming byte as an array of 8 bits, one for each pager
      analogWrite(pagers[i], pwm);
    }
    else
    {
      analogWrite(pagers[i], 0);
    }
  }
}






