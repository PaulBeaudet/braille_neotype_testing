//buttons
#define NEEDEDTIMERS 3 //List the amount of timers that will be needed here
//multiply this number by 8 bytes to understand ram footprint

uint32_t timers[2][NEEDEDTIMERS] = {};// create global timers to modify 

void timeCheck(byte whichTimer, uint32_t durration)
{//used for setting the durration of the timer
  timers[1][whichTimer]=durration; //set durration
  timers[0][whichTimer]=millis();  // note the time set
}

bool timeCheck(byte whichTimer)
{//used for checking the timer
  if(millis() - timers[0][whichTimer] > timers[1][whichTimer])
  {// if the durration has elapsed
    return true;
  }
  else
  {
    return false;
  }
}

//timers
#define PRESS 0 // Timing for a capacitive press to "count"
#define HOLD 1  // Timing for a hold to "count"
#define BUTTONTIMER 2 // timer for buttons in the case button input instead of capcitive
//Assosiated Timings
#define BOUNCETIME 50 //ms
#define HOLDTIME 500 //ms

#define SPARKBUTTONS D2,D3,D4,D5,D6,D7,A2,A3,
#define UNOBUTTONS 8,9,10,11,12,13,

byte buttons[]=
{//usb lilypad arrangement
  SPARKBUTTONS // !! ---set the desired button pins here--- !!
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

byte debouncedInput()
{
  static byte pinState= 0;
  static byte lastState= 0;
  static bool timeStart = false;

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
// ------------ filters -----------------

byte inputIntention(byte letter)
{//clasifies human intention, returns intended letter
  static byte lastLetter= 0;
  static bool printFlag = 0;
  static bool upperFlag = 0;

  if (letter)
  {// given we are dealing with a value other then zero
    if (letter==lastLetter)
    {
      if(printFlag)
      {// if the go ahead to print has been flagged then holds can detected
        if (timeCheck(HOLD))
        {//if the hold timer elapses then flag for and upper case
          upperFlag= true;
        }
      }
      else if (timeCheck(PRESS))
      {
        printFlag = true;
        timeCheck(HOLD, HOLDTIME);
      }
    }
    else
    {
      timeCheck(PRESS, BOUNCETIME);
    }
  }
  else if(printFlag)
  {// if the chord as been let go and there was a lagit key last time
    printFlag= false;
    if ( lastLetter == 128)
    {
      upperFlag= false;
      return lastLetter;
    }
    if(upperFlag && lastLetter > 32)
    {// upperflag capitilizes letters given hold state
      lastLetter= lastLetter - 32;
      upperFlag= false;
      return lastLetter;// ex. "a" = 97: 97 - 32 = 65: 65 = "A"
    }
    else
    {
      return lastLetter;
    }
  }
  lastLetter = letter;
  return 0;// return the false case in the event of no intentions 
}





