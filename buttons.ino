//buttons

byte buttons[]=
{//usb lilypad arrangement
  UNOBUTTONS // !! ---set the desired button pins here--- !!
};
#define NUMBUTTONS sizeof(buttons)

//------------SETUP --------------------------

void buttonUp()// it's cold out there
{ 
  for (byte set=0;set<NUMBUTTONS;set++)
  {//set up the buttons 
    pinMode(buttons[set], INPUT_PULLUP);//this sets pull-up resistor/ie input through 20k to 5v
  }//in this way| input to button/button to ground, is proper and will read as low when pressed
}

//----------------GENERAL -------------------
#define BOUNCETIME 10//ms anytime grater is to know the buton has settled
// wait this long to be sure of a legit press
#define HOLDSTATE 200//ms

byte debounceEvent()
{
  static byte pinState= 0;
  static byte lastState= 0;
  static boolean timeStart = false;

  pinState=buttonsSample();

  if(pinState && pinState == lastState)
  {//given one of the buttons was pressed && given the same sample has occured at least twice
    if(timeStart)
    { // given the clock has been set
      if(timeCheck(BUTTONTIMER))
      { // given time has elapsed
        timeStart = false;
        return pinState; // return a valid debounced event
      }
    }
    else
    {//start the clock
      timeCheck(BUTTONTIMER, BOUNCETIME);
    };
  }
  else
  {
    timeStart=false;
  };
  lastState=pinState;
  return 0; // nothing happend
}

byte buttonsSample()
{
  byte sample=0;
  for (byte i=0; i<NUMBUTTONS; i++) // 
  {
    if(digitalRead(buttons[i]) == LOW)
    {
      bitWrite(sample, i, 1); // set the selected bit high !!
    }
    else
    {
      bitWrite(sample, i, 0); // set the selected bit low !!
    }  
  }
  return sample;
}




