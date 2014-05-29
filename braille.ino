//braille.ino--- This sketch uses is meant for the cap1188 breakout and a corrisponding darlington array
// of pager motors. All in order to feel and transmit brialle. Transmission via bluefruit
#include "buttons.h"
#include "logicBraille.h"

// ---------------Main loops and functions--------------------
void setup()
{
  pagersUp();//set the pager pins as outputs
  blueUp();//set-up the bluefruit interface
  buttonUp();//set up the buttons
}

void loop()
{
  if (outputCondition(debouncedInput()) == 128) // default output condition loop
  {
    modeKey(); // creates an alternitive loop base on a "function" or "mode" key
  }
}

byte outputCondition(byte input)
{
  input=ifBraille(input);//filter the input to valid braille or 0  
  hapticResponce(input); // turn the pagers on or off
  input=inputIntention(input);//  further filter input to "human intents"
  return blueResponce(input); // after trying to possibly print a char return what char was printed
}


void modeKey()
{ // in this game the micro "touches" the user with a message and the user copies 
  while(1)
  {// artificial main loop for game mode
    byte input = outputCondition(debouncedInput());// sample the input for loop condition and responce
    if(input == 128)
    {
      return;
    }
    callAndResponse(input);// display the message to copy and expect its reponse   
  }
}


