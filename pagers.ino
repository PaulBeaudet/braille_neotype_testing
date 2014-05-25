#define UNOPWMPINS 3,5,6,9,10,11,
#define MIRCOPAGER 11,12,9,10,6,5,
byte pagers[]=
{//arangement for the spark neomouse 
  2,3,4,5,6,7// NOTE ---set the desired button pins here--- NOTE
};//these really need to be assigned in corrispondence with the input pin arrangement
//starting from least significant bit on

#define NUMPAGERS sizeof(pagers)

//------------SETUP --------------------------

void pagersUp()
{ //setup the pager motor pins as outputs
  for (byte set=0;set<NUMPAGERS;set++)
  { 
    pinMode(pagers[set], OUTPUT);
  }
}
//-------------- addressing---------------

void patternVibrate(byte pins)//
{
  for (byte i=0; i<NUMPAGERS; i++) // !! only the first 6 bits from the least significant are necisarry !!
  {//!! convert to read from least significant bit!!
    if (pins & (1 << i)) // show desired bit (pin)
    { // imagine incoming byte as an array of 8 bits, one for each pager
      digitalWrite(pagers[i], HIGH);
    }
    else
    {
      digitalWrite(pagers[i], LOW);
    }
  }
}

//------------------haptic logic--------------------
void hapticMessage(char letter) // intializing function
{ // set a letter to be "played"
  timeCheck(HAPTIC, hapticTiming);
  patternVibrate(brailleConvert(letter));
}

boolean hapticMessage() 
{ // updating function 
  static boolean touchPause= 0;
  
  if(timeCheck(HAPTIC))
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
      timeCheck(HAPTIC, hapticTiming/2);
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
    hapticMessage(onLetter);
    possition++;
    return onLetter;
  }
  return 0;
}


char brailleConvert(char letter)
{
  for(byte i=0; i<ENCODEAMT;i++)
  {
    if(letter == pgm_read_byte(&byteToBraille[0][i]))
    {// for a matching letter in the array
      return pgm_read_byte(&byteToBraille[1][i]);
    }// return the corrisponding translation
  }
}






