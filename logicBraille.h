// logic for understanding braille convertion
#include "pagers.h"
// brialle convertion array
#define ENCODEAMT 33 // size is defined to structure iteration amount
byte byteToBraille [2][ENCODEAMT] // 
{
  { // input in characters
    ' ','t','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','u','v','w','x','y','z', 8, 128,'-',';','.', '?',                      }
  ,
  { //corrisponding braille binary output in decimal form, read from least significant bit
    32, 30, 1 , 5 , 3 , 11, 9 , 7 , 15, 13, 6 , 14, 17, 21, 19, 27, 25, 23, 31, 29, 22, 49, 53, 46, 51, 59, 57 ,64, 128, 48, 40, 34, 43,                     } 
};//each bit in the corrisponding bytes represents a "bump" state

//-----------braille checking and convertion----------------
byte brailleConvert(byte letter, bool convert)
{
  for(byte i=0; i<ENCODEAMT;i++)
  {
    if(letter == (byteToBraille[!convert][i]))
    {// for a matching letter in the array
      return (byteToBraille[convert][i]);
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

//----------------------haptic logic----------------------------
boolean ptimeCheck(uint32_t durration)
{//used for checking an setting timer
  static uint32_t ptimer[2] = { };// create timer to modify
  if(durration)
  {
    ptimer[1]=durration; //set durration
    ptimer[0]=millis();  // note the time set
  }
  else if(millis() - ptimer[0] > ptimer[1])
  {// if the durration has elapsed
    return true;
  }
  return false;
}

#define HAPTICTIMING 800 //ms, controls haptic display durration, Future; will be user adjustable 

void hapticMessage(byte letter) // intializing function
{ // set a letter to be "played"
  ptimeCheck(HAPTICTIMING);
  patternVibrate(brailleConvert(letter, 1));
}

boolean hapticMessage() 
{ // updating function 
  static boolean touchPause= 0;

  if(ptimeCheck(0))
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
      ptimeCheck(HAPTICTIMING/2);
    };
  }
  return false;
}

byte hapticMessage(char message[])
{ 
  static byte possition = 0;
  byte onLetter = message[possition];

  if(!onLetter)
  {
    possition = 0;
    while (!hapticMessage())
    {//finish last "touch"
      ; //figure out how to get rid of this pause latter
    }
    return 128;//signal the message is done
  }
  if (hapticMessage())//refresh display
  {
    hapticMessage(onLetter);
    possition++;
    return onLetter;
  }
  return 0;
}

