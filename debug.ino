#define LED 13

void debugSetup()
{
  pinMode(LED, OUTPUT);
  useSerial();
}

void blinkDebug(int time)
{
  digitalWrite(LED, HIGH);
  delay(time);
  digitalWrite(LED, LOW);
  delay(time);
}

void useSerial()//consolidate to a function to comment out easily
{
  Serial.begin(9600);//open the connection 
  while(!Serial)//wait for the monitor to come up
  {
    ;
  }
}

void byteOutSerial(byte pins)
{
  byte flag=false;
  Serial.println(pins);
  
  for (byte i=0; i<8; i++) // !! only the first 6 bits from the least significant are necisarry !!
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

void newLetterSerial(char letter)
{
  static char lastLetter = 0;
  
  if( letter != lastLetter)
  {
    Serial.println(letter);
  }
  
  lastLetter = letter;
}

