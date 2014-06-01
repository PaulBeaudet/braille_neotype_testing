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
  mainLoop(buttonsSample());
}

void mainLoop(byte input)
{// mainloop is abstracted for testing purposes 
  static bool modeFlag = 0;

  byte actionableSample= ifBraille(input);  
  if(actionableSample)//filter the input to valid braille or 0
  {
    hapticResponce(input);
    actionableSample=holdFilter(actionableSample);//  further filter input to "human intents"
    if(actionableSample==128)
    {
      modeFlag=!modeFlag;
      return;
    }   
    Serial1.write(actionableSample);// send a keystroke to the bluefruit!
  }// turn the pagers off given no actionable sample
  else{
    hapticResponce(0);
  };
  if(modeFlag)
  {
    game(actionableSample);
  }  
}

//----------------------------game-------------------
char gameMessage[] = "aaa";
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
  while(hapticMessage(message) != -128)
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



