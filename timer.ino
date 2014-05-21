boolean timer(unsigned long time)
{
  static unsigned long startTime = millis();
  static unsigned long setTime;

  if (setTime == time)//check the clock
  {//in this way a different durration is need to reset the clock
    // when in progress
    if ( millis() - startTime > setTime)
    {
      setTime=0;//ready the function for next timed action
      //this makes it posible to repeat the same durration 
      return true;// the time is up!
    }//given durration is still in progress fall thru to false case
  }
  else
  {
    setTime = time; //set the clock
    startTime = millis();//track what the start time was
  }
  return false;//fall thru case
}

boolean timer0(unsigned long time, boolean reset)
{
  static unsigned long startTime = millis();
  static unsigned long setTime;

  if (reset)//reset the clock
  {
    setTime = time; //set the clock
    startTime = millis();//track what the start time was
  }
  else
  {
    if ( millis() - startTime > setTime)
    {
      setTime=0;//ready the function for next timed action
      //this makes it posible to repeat the same durration 
      return true;// the time is up!
    }//given durration is still in progress fall thru to false case
  }
  return false;//fall thru case
}

boolean timer1(unsigned long time, boolean reset)
{
  static unsigned long startTime = millis();
  static unsigned long setTime;

  if (reset)//reset the clock
  {
    setTime = time; //set the clock
    startTime = millis();//track what the start time was
  }
  else
  {
    if ( millis() - startTime > setTime)
    {
      setTime=0;//ready the function for next timed action
      //this makes it posible to repeat the same durration 
      return true;// the time is up!
    }//given durration is still in progress fall thru to false case
  }
  return false;//fall thru case
}

//----------------------------- experimental timers 
unsigned long timers[2][4] = {};// create 4 global timers to modify among two functions
//set timer
void timeCheck(byte whichTimer, unsigned long durration)
{
  timers[1][whichTimer]=durration;
  timers[0][whichTimer]=millis();
}
//check timer
boolean timeCheck(byte whichTimer)
{
  if(millis() - timers[0][whichTimer] > timers[1][whichTimer])
  {
    return true;
  }
  else
  {
    return false;
  }
}
