#define NEEDEDTIMERS 5 //List the amount of timers that will be needed here
//multiply this number by 8 bytes to understand ram footprint

unsigned long timers[2][NEEDEDTIMERS] = {};// create global timers to modify 

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
