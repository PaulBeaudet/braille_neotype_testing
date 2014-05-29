//Pagers //Haptic feedback hardware

uint32_t ptimer[2][1] = {};// create global timers to modify 

void ptimeCheck(byte whichTimer, uint32_t durration)
{//used for setting the durration of the timer
  ptimer[1][whichTimer]=durration; //set durration
  ptimer[0][whichTimer]=millis();  // note the time set
}

boolean ptimeCheck(byte whichTimer)
{//used for checking the timer
  if(millis() - ptimer[0][whichTimer] > ptimer[1][whichTimer])
  {// if the durration has elapsed
    return true;
  }
  else
  {
    return false;
  }
}

#define HAPTIC 0 // timer for haptic messages
int hapticTiming = 800; //ms, controls haptic display durration, Future; will be user adjustable 

//Pinout desired pin usage for components on various boards
#define UNOPWMPINS 3,5,6,9,10,11,
#define MIRCOPAGER 11,12,9,10,6,5,
#define SPARKPWM A0,A4,A1,A5,A7,A6,
byte pagers[]=
{//arangement for the spark neomouse 
  SPARKPWM// NOTE ---set the desired button pins here--- NOTE
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
    { // imagine incoming byte as an array of 8 bits, one for each pager
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






