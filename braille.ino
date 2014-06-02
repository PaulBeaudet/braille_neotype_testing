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

bool modeFlag = 0;
void loop()
{
  mainLoop(buttonsSample());
}

void mainLoop(byte input)
{// mainloop is abstracted for testing purposes 
  byte actionableSample= ifBraille(input);
  if(actionableSample)
  {  
    hapticResponce(input);
  }
  else
  {
    hapticResponce(0);
  }
  if(modeFlag)
   {
   game(holdFilter(actionableSample));
   return;
   }
   holdFilter(actionableSample);//  further filter input to "human intents"
}

//----------------------------game-------------------
char gameMessage[] = "bob the crab";
char failMessage[] = "fail ";
char winMessage[] = "win";

void game(char letter)
{ // simon says like typing game
  static int place = 0;//current char that is being attempted

  if(letter)
  {// no input no game
    if (letter==gameMessage[place])
    {//if the input is correct
      place++;//increment the place
      if(!gameMessage[place])
      {//if it was the last place
        place=0;// reset message, user has completed
        rmMessage(gameMessage);//rm user message
        toast(winMessage);// tell user they won!!!
      }
    }
    else
    {// input was and incorect match
      place = 0; // make sure place is set back to zero to start over
      toast(failMessage); // inform user of failure
      toast(gameMessage); // inform user of goal
    }
  }
}

void toast(char message[])
{// message the appears and disapears, like the one in android
  for(int pos=0;message[pos];pos++)
  {
    Serial1.write(message[pos]);
  }
  while(hapticMessage(message) != 128)
  {
    ; 
  }
  rmMessage(message);
}

void rmMessage(char message[])
{//remove a message
  for(int i=0;message[i];i++)
  {
    Serial1.write(8);
  }
}

// ----------------input interpertation-------------

byte holdFilter(byte input)
{
  static byte lastInput = 0;  
  static uint16_t actions[]={
    20,120,300,200,  };
#define ACTIONDELAYS sizeof(actions)
  static byte progress=0;
  static uint32_t timer[2] = {
  };
  static bool hint=0;

  if (input && input == lastInput)
  {
    if(millis() - timer[0] > timer[1])
    {// if the durration has elapsed
      progress++;//increment progress in the actions array
      if(progress==1)
      {//press case, will remove letter until hint flag is falsified
        if(input==128)
        {// toggel the mode give special key case
          modeFlag=!modeFlag;
          return 128;
        }
        hint = true;
        if(input==8)
        {//prevent a double backspace
          hint=false;
        }
        Serial1.write(input);// send a keystroke to the bluefruit!
        return input;
      }
      if(progress==2)
      {// accepted press case
        hint= false;
      }
      if(progress==3)
      {
        Serial1.write(8);//delete currently printed char in preperation for a caps
        return 8;
      }
      if(progress==4)
      { 
        if(input == 128)
        {//if an acception case 
          return 0;
        }
        if(input == 32)
        {
          input = 65;
        }    
        Serial1.write(input-32);//print the upperCase input
        return input-32;
      }
      if(progress==ACTIONDELAYS)
      {
        progress=0;
      }
      timer[0]=millis();  // note the time set
      timer[1]=actions[progress]; //set durration
    }
  }
  else
  {
    if(hint)
    {
      Serial1.write(8);
      return(8);
    }
    hint=false;
    progress=0; //reset progress
    timer[0]=millis();  // note the time set
    timer[1]=actions[progress]; //set durration
    lastInput=input;
  };
  return 0;
}


