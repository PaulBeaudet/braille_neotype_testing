//braille.ino--- This sketch uses is meant for the cap1188 breakout and a corrisponding darlington array
// of pager motors. All in order to feel and transmit brialle. Transmission via bluefruit
#include "buttons.h"
#include "logicBraille.h"

// ---------------Main loops and functions--------------------
void setup()
{ 
  Serial1.begin(9600);
  pagersUp();//set the pager pins as outputs
  buttonUp();//set up the buttons
}

void loop()
{
  mainLoop();
}

void mainLoop()
{  
  if (outputCondition(buttonsSample()) == 128) // default output condition loop
  {
    modeKey(); // creates an alternitive loop base on a "function" or "mode" key
  }
}

byte outputCondition(byte input)
{
  byte actionableSample= ifBraille(input);
  if(actionableSample)//filter the input to valid braille or 0
  {
    hapticResponce(input);
    actionableSample=debouncedInput(actionableSample);//  further filter input to "human intents"
    if(actionableSample==128)
    {
      return 128;
    }
    Serial1.write(actionableSample);// send a keystroke to the bluefruit!
    return actionableSample;
  }// turn the pagers on or off
  hapticResponce(0);
  return 0;
}

void modeKey()
{ // in this game the micro "touches" the user with a message and the user copies 
  while(1)
  {// artificial main loop for game mode
    byte input = outputCondition(buttonsSample());// sample the input for loop condition and responce
    if(input == 128)
    {
      return;
    }
    callAndResponse(input);// display the message to copy and expect its reponse   
  }
}

//----------------------------game-------------------
char gameMessage[] = "aaa";
char failMessage[] = "fail";
char winMessage[] = "win";

byte checkResponse(byte input)
{
  static int place = 0;

  if(input)
  {
    if(input == gameMessage[place])
    {//one letter right case
      place++;
      if(!gameMessage[place])
      {//got to the end of the word!!! success!!!
        place = 0;
        return 1;
      }
    }
    else
    {//fail case
      place = 0;
      return 2;
    }
  }
  return 0;// default pass nor fail mode
}

void callAndResponse(char letter)
{
  static boolean messageFlag = true;

  if(messageFlag)
  {// call mode
    toast(gameMessage);
    messageFlag=false;// message is done, set to response mode      
  }
  else
  {// response mode
    byte result = checkResponse(letter);
    if (result == 2)
    {
      toast(failMessage);
      messageFlag = true;
    }
    else if (result == 1)
    {
      toast(winMessage);
      messageFlag = true;
    };
  };
}

void toast(char message[])
{// message the appears and disapears, like the one in android
  for(int pos=0;message[pos];pos++)
  {
    Serial1.write(message[pos]);
  }
  while(hapticMessage(message) != -128)
  {
    ; 
  }
  for(int i=0;message[i];i++)
  {
    Serial1.write(8);
  }
}


