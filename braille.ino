//braille.ino--- This sketch uses is meant for the cap1188 breakout and a corrisponding darlington array
// of pager motors. All in order to feel and transmit brialle. Transmission via bluefruit
#include<avr/pgmspace.h>//explicitly stated read only memory

// brialle convertion array
#define ENCODEAMT 27 // size is defined to structure iteration amount
prog_char const byteToBraille [2][ENCODEAMT] // write convertion data to persistent memory to save ram
{
  { // input in characters
    ' ','t','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','u','v','w','x','y','z',  }
  ,
  { //corrisponding braille binary output in decimal form, read from least significant bit
     32, 30, 1 , 5 , 3 , 11, 9 , 7 , 15, 13, 6 , 14, 17, 21, 19, 27, 25, 23, 31, 29, 22, 49, 53, 46, 51, 59, 57 ,  } 
};//each bit in the corrisponding bytes represents a "bump" state

void setup()
{
  pagersUp();//set the pager pins as outputs
  blueUp();//set-up the bluefruit interface
  capSetup();//set up the cap1188 breakout for capcitive touch
  debugSetup();//bring up serial, set pin 13 for the built in led
}

void loop()
{
  byte input = capState(); // sample the input
  inputIntention(ifBraille(input)); // if the input is valid braille filter it to "human intents" and print over bluetooth
  hapticFeeback(input); //provide a haptic response
}

// ---------------Main functions--------------------

void hapticFeedback(byte input)
{
  if(input)
  {//only actuate when given data
    patternVibrate(input);//provides haptic feedback for keystroke
  }
  else
  {
    patternVibrate(0);
  };
}

#define BOUNCETIME 50 //ms
#define HOLDTIME 500 //ms
#define PRESS 0 // maybe the numbers of timer functions can be enumerated in the future
#define HOLD 1  // in order to avoid conflicts

void inputIntention(char letter)
{//clasifies human intention, varifying keystroke and sending over bluetooth
  static char lastLetter= 0;
  static boolean printFlag = 0;
  static boolean upperFlag = 0;

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
    btShiftPrint(lastLetter, upperFlag);
    upperFlag= false;
    printFlag= false;
  }

  lastLetter = letter;
}






