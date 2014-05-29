// logic for understanding braille convertion
#include "pagers.h"
#include "bluefruit.h"
// brialle convertion array
#define ENCODEAMT 28 // size is defined to structure iteration amount
byte byteToBraille [2][ENCODEAMT] // 
{
  { // input in characters
    ' ','t','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','u','v','w','x','y','z', 8,                       }
  ,
  { //corrisponding braille binary output in decimal form, read from least significant bit
    32, 30, 1 , 5 , 3 , 11, 9 , 7 , 15, 13, 6 , 14, 17, 21, 19, 27, 25, 23, 31, 29, 22, 49, 53, 46, 51, 59, 57 ,64,                      } 
};//each bit in the corrisponding bytes represents a "bump" state


byte ifBraille(byte combination)
{//checks for valid usage in the character map and converts to a char
  if (combination==128)
  {
    return 128;
  }
  for(byte i=0; i<ENCODEAMT;i++)
  {
    if(combination == (byteToBraille[1][i]))
    {// for a corrisponding translation
      return (byteToBraille[0][i]);
    }// return the matching letter in the array
  }
  return 0; // no matches try again
}

char brailleConvert(char letter)
{
  for(byte i=0; i<ENCODEAMT;i++)
  {
    if(letter == (byteToBraille[0][i]))
    {// for a matching letter in the array
      return (byteToBraille[1][i]);
    }// return the corrisponding translation
  }
  return 0;
}

void hapticResponce(byte input)
{
  if(input) // given the input is in the braille char map
  {
    patternVibrate(input);//provide haptic feedback for keystroke  
  }
  else
  {
    patternVibrate(0); // if not stop previous stroke
  };
}

byte blueResponce(byte input)
{
  if(input)// given one time intent has been discerned 
  {
    if(input==128)
    {
      return 128;
    }
    bluePrint(input);// send a keystroke to the bluefruit!
    return input;
  }
  return 0;
}

//------------------haptic logic--------------------
void hapticMessage(char letter) // intializing function
{ // set a letter to be "played"
  ptimeCheck(HAPTIC, hapticTiming);
  patternVibrate(brailleConvert(letter));
}

boolean hapticMessage() 
{ // updating function 
  static bool touchPause= 0;
  
  if(ptimeCheck(HAPTIC))
  {//time to "display" a touch has elapsed
    if(touchPause)
    {//this case allows for a pause after "display"
      touchPause=!touchPause;
      return true;
    }
    else
    {
      touchPause=!touchPause;
      patternVibrate(0);//stop the message
      ptimeCheck(HAPTIC, hapticTiming/2);
    }
  }
  return false;
}

char hapticMessage(char message[])
{ 
  static byte possition = 0;

  char onLetter = message[possition];

  if(!onLetter)
  {
    possition = 0;
    while (!hapticMessage())
    {//finish last "touch"
      ; //figure out how to get rid of this pause latter
    }
    return -128;//signal the message is done
  }
  
  if (hapticMessage())//refresh display
  {
    hapticMessage(brailleConvert(onLetter));
    possition++;
    return onLetter;
  }
  return 0;
}

//----------------------------game-------------------
char gameMessage[] = "aaa ";
char failMessage[] = "fail";
char winMessage[] = "win";

void toast(char tmessage[])
{// message the appears and disapears, like the one in android
  bluePrint(tmessage);
  while(hapticMessage(tmessage) != -128)
  {
    ; 
  }
  int i=0;
  while(tmessage[i])
  {
    bluePrint(8);
    i++;
  }
}

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

void callAndResponse(byte letter)
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

/*
void blueBraille(byte pads)
{
  for(byte i=0; i<ENCODEAMT; i++)
  {
    if(pads == (byteToBraille[1][i]))
    {
      Serial1.write((byteToBraille[0][i])); 
    }
  }
}*/
