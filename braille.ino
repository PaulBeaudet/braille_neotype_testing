//braille.ino--- This sketch uses is meant for the cap1188 breakout and a corrisponding darlington array
// of pager motors. All in order to feel and transmit brialle. Transmission via bluefruit
#include<avr/pgmspace.h>//explicitly stated read only memory

// brialle convertion array
#define ENCODEAMT 28 // size is defined to structure iteration amount
prog_char const byteToBraille [2][ENCODEAMT] // write convertion data to persistent memory to save ram
{
  { // input in characters
    ' ','t','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','u','v','w','x','y','z', 8,     }
  ,
  { //corrisponding braille binary output in decimal form, read from least significant bit
    32, 30, 1 , 5 , 3 , 11, 9 , 7 , 15, 13, 6 , 14, 17, 21, 19, 27, 25, 23, 31, 29, 22, 49, 53, 46, 51, 59, 57 ,64,     } 
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
  char input = ifBraille(capState()); //if the input is valid braille sample it
  hapticFeedback(input); //provide a haptic response
  input = inputIntention(input); //  filter input to "human intents"
  if (input) // if a human intentended keystroke was detected
  {
    bluePrint(input);// send a keystroke to the bluefruit!
  }
  //learningGame(capState()); // game that helps learn braille input
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

char inputIntention(char letter)
{//clasifies human intention, returns intended letter
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
    printFlag= false;
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

//----------------------------game-------------------

void learningGame(byte input)
{ // in this game the micro "touches" the user with a message and the user copies it
  if ( input == 128 )
  {
    byte gameInput = capState();// var definition
    timeCheck(2, 500); // loop exit timer
    while( gameInput != 128)
    {// artificial main loop for game mode
      gameInput = capState();// sample the input
      char letter = ifBraille(gameInput);
      callAndResponse(letter);// display the message to copy and expect its reponse
      if (gameInput==128 && !timeCheck(2))
      {// wait to recive a mode change; prevents expidited loop exit
        gameInput=0;
      }
    }
  }
}

void callAndResponse(char letter)
{
  static byte userProgress = 0;

  if (hapticMessage("lets play a game ", 800))
  {
    
  }
  
  //letter = inputIntention(letter);
}







