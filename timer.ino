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


//----------------------------- experimental timers 

unsigned long timers[2][4] = {};// create 4 global timers to modify among two functions

void timeCheck(byte whichTimer, unsigned long durration)
{//used for setting the durration of the timer
  timers[1][whichTimer]=durration; //set durration
  timers[0][whichTimer]=millis();  // note the time set
}

boolean timeCheck(byte whichTimer)
{//used for checking the timer
  if(millis() - timers[0][whichTimer] > timers[1][whichTimer])
  {// if the durration has elapsed
    return true;
  }
  else
  {
    return false;
  }
}
