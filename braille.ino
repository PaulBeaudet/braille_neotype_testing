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

//Timers
#define PRESS 0 // Timing for a capacitive press to "count"
#define HOLD 1  // Timing for a hold to "count"
#define OPENLOOP 2// Timing to enter alternitive modes with a key; as such key would enter and exit mode in same touch w/out
#define HAPTIC 3 // timer for haptic messages
#define BUTTONTIMER 4 // timer for buttons in the case button input instead of capcitive
#define GESTURETIMER 5 // set time for a gesture to occure
//Assosiated Timings
#define BOUNCETIME 50 //ms
#define HOLDTIME 500 //ms
#define GESTUREWINDOW 1000//ms
word hapticTiming = 800; //ms, controls haptic display durration, Future; will be user adjustable 
//Pinout desired pin usage for components on various boards
#define UNOPWMPINS 3,5,6,9,10,11,
#define MIRCOPAGER 11,12,9,10,6,5,
#define SPARKBUTTONS D2,D3,D4,D5,D6,D8,
#define UNOBUTTONS 8,9,10,11,12,13,

// ---------------Main loops and functions--------------------
void setup()
{
  pagersUp();//set the pager pins as outputs
  blueUp();//set-up the bluefruit interface
  capSetup();//set up the cap1188 breakout for capcitive touch
  debugSetup();//bring up serial, set pin 13 for the built in led
}

void loop()
{
  char inputState = capState(); // sample the capcitive input
  outputCondition(inputState); // default output condition loop
  learningGame(inputState); // game that helps learn braille input and output
}

void outputCondition(byte input)
{
  input=ifBraille(input);//filter the input to valid braille or 0 
  if(input) // given the input is in the braille char map
  {
    patternVibrate(input);//provide haptic feedback for keystroke  
  }
  else
  {
    patternVibrate(0); // if not stop previous stroke
  };
  input=inputIntention(input);//  further filter input to "human intents" 
  if(input)// given one time intent has been discerned 
  {
    bluePrint(input);// send a keystroke to the bluefruit!
  }
}

//----------------------------game-------------------

void learningGame(byte input)
{ // in this game the micro "touches" the user with a message and the user copies it
  if ( input == 128 )
  {
    byte gameInput = capState();// var definition
    timeCheck(OPENLOOP, HOLD); // loop exit timer
    while( gameInput != 128)
    {// artificial main loop for game mode
      gameInput = capState();// sample the input
      callAndResponse(ifBraille(gameInput));// display the message to copy and expect its reponse
      if (gameInput==128 && !timeCheck(OPENLOOP))
      {// wait to recive a mode change; prevents expidited loop exit
        gameInput=0;
      }
    }
  }
}

void callAndResponse(char letter)
{
  //static byte userProgress = 0;
  char letterBack = hapticMessage("test ");
  if (letterBack == -128)
  {
    while(capState() != 128)
    {
      hapticMessage();//allow things to wrap up
    }
  }
  else if (letterBack)
  {
    bluePrint(letterBack);
  }  
}

// ------------ filters -----------------
byte ifBraille(uint8_t combination)
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





