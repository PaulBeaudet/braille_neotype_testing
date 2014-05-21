//braille.ino--- This sketch uses is meant for the cap1188 breakout and a corrisponding darlington array
// of pager motors. All in order to feel and transmit brialle. Transmission via bluefruit
#include<avr/pgmspace.h>//explicitly stated read only memory

// brialle convertion array
#define ENCODEAMT 27

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
  capActuation();
}

void capActuation()
{
  byte state= capState();//abstract state once to avoid registering second states over time.
  char letter= ifBraille(state);//fill this var given a matching mapping
  
  blueDebounce(letter);//clasifies human intention, varifying keystroke and sending over bluetooth
  //newLetterSerial(letter);//prints the letter to the serial monitor if its a new one
  
  if(letter)
  {//only actuate when given data
    patternVibrate(state);//provides haptic feedback for keystroke
    //bluePrint(letter);//send letter over bluetooth
    newLetterSerial(letter);//prints the letter to the serial monitor if its a new one
  }
  else
  {
    patternVibrate(0);
  };
}





