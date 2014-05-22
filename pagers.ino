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

void patternSerial(byte pins)
{
  byte flag=false;
  Serial.println(pins);
  
  for (byte i=0; i<NUMPAGERS; i++) // !! only the first 6 bits from the least significant are necisarry !!
  {//!! convert to read from least significant bit!!
    if (pins & (1 << i)) // show desired bit (pin)
    { // imagine incoming byte as an array of 8 bits, one for each pager
      Serial.write('1');
    }
    else
    {
      Serial.write('0');
    }
    if(flag)
    {
      Serial.println();
    }
    flag=!flag;
  }
  Serial.println();
}

boolean hapticMessage(char message[], int mSpeed)
{//returns a true when message is done
  static int pos = 0;
  static boolean switchFlag = false;
  static boolean startChar = true;

  if (!mSpeed)//default behavior when set to zero
  {// default arguments cause copilation errors in the arduino ide
    mSpeed = 50;//in this way 75 ms is the durration of a char 'buzz'
  } 

  if (message[pos])
  {//given there is a char in the array
    if(startChar)
    {
      //Serial.print(message[pos]);
      bluePrint(message[pos]);
      patternVibrate(brailleConvert(message[pos]));
      //patternSerial(brailleConvert(message[pos]));
      startChar=false;
    }
    else if(switchFlag)
    {// have we got to the pause yet
      if (timer(mSpeed/2))//wait for the end of the pause phase
      {
        pos++;//increment to the char in the message
        switchFlag = false; //change the flag for the next phase
        startChar = true; 
      }
    }
    else if(timer(mSpeed))//wait for the end of the message phase
    {
      patternVibrate(0); // init pause phase
      switchFlag = true; // signify pause phase
    };
    return false;
  }
  else
  {
    pos = 0; // reinstate pos for another message
    return true; // signify message is done
  }
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
  Serial.print("err");//testing case there is no pass thru
  //return letter;//pass through non translating chars
}

//--------------------------------    testing 

void anotherTest(int time)
{
  for(byte i=0;i<NUMPAGERS;i++)
  {
    digitalWrite(pagers[i], HIGH);
    delay(time);
    digitalWrite(pagers[i], LOW);
  }
}


