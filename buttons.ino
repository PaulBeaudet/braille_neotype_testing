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

word courseOfEvents()
{ // detection of complex gesture derivitive input
  static byte lastInput = 0; 
  static boolean timeFrame = false;

  byte input = debounceEvent();

  if(input || timeFrame)
  {//if input is coming in or the time frame is open for a gesture
    if(timeFrame)
    {
      // gesturning logic !!!!!!!!!!!!!!!!!!!!!!
      timeFrame=false;
      return word(lastInput, input);
    }
    else
    {
      timeCheck(GESTURETIMER, GESTUREWINDOW);
      lastInput=input;
      timeFrame=true;
    }
  } 
}

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
      timeStart=true;
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





