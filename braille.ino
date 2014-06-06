//braille.ino--- Utilizes an array of 8 buttons and a corrisponding darlington array of 6 pagers
// With the goal of feeling and transmit brialle by Bluetooth transmission w/ Bluefruit EZ-Key
#include "buttons.h" // button logic, to be consolidated to hardware in near future
#include "logicBraille.h" // convertion and haptic logic
#include "spark_disable_cloud.h"//needs to be include for the folowing undef
#undef SPARK_WLAN_ENABLE //disable wifi by defaults in order to have an offline option

char compareBuffer[15] = {};//buffer to compare user input to game message
char gameMessage[] = "aaa"; //experimental simon says message to learn the keys

byte modeFlag = 0;// please find a way to get rid of this global var

// ---------------Main loops and functions--------------------
void setup()
{ 
  Serial1.begin(9600);//Establish communication with EZ-Key
  pagersUp();//set the pager pins as outputs
  buttonUp();//set up the buttons
  if(!buttonSample())//given no button holding on start-up; will connect to wifi
  {// connect the spark core in the default button state
    Spark.connect();
	  toast("wifi");//testing
  }
}

void loop()
{
	if(Spark.connected()){SPARK_WLAN_Loop();}//spark connect called in setup; run wlan loop
        mainLoop(buttonSample());//abstracted main loop for testing purposes
}

void mainLoop(byte input)
{// mainloop is abstracted for testing purposes 
  byte actionableSample= brailleConvert(input, 0);// 0 parameter denotes reverse lookup
  if(actionableSample){hapticResponce(input);}//fire the assosiated pagers! given action
  else{hapticResponce(0);}//otherwise be sure the pagers are off
  actionableSample = holdFilter(actionableSample);//  further filter input to "human intents"
  if(actionableSample){Serial1.write(actionableSample);}//print the filter output 
}

// ----------------input interpertation-------------

byte holdTimer(byte reset)
{
    #define DELAYTIME 1 //the delay time corisponds to action values
    #define TIMESTARTED 0 // Denotes when each action starts
    static uint16_t actions[]={30,100,300,200,}; //actions progres as timer is held at 0
    #define ACTIONDELAYS sizeof(actions) //note sizeof() counts bytes /2 + 1 for correct value
    static uint32_t timer[2] = {};// holds time started and delay time
    static byte progress=0; //keeps the progress of the actions 
    
    if(reset)
    {
	progress=0;//set everything back to the begining
        timer[DELAYTIME]=actions[progress]; //set the intial timing
	timer[TIMESTARTED]=millis();  // note the start time of the transition
    }
    else if(millis() - timer[TIMESTARTED] > timer[DELAYTIME])
    { 
	progress++;//increment the progress of the time table
	if(progress==ACTIONDELAYS/2+1){progress=0;}//correct time table if it has been overrun
	timer[DELAYTIME]=actions[progress]; //set durration baseded on progress level
	timer[TIMESTARTED]=millis();  // note the start time of the transition
	return progress; //return which level of progress has ellapsed
    }
    return 0;// in most cases this function is called, time will yet to be ellapsed 
}

byte holdFilter(byte input)
{//debounces input and interprets hold states for capitilization and other functions
	static byte hint=0;// holds whether char falshing is occuring
	static byte lastInput=0;//remembers last entry to debounce
	
	if(input && input== lastInput)
	{//Given values and the fact values are the same as the last
		if( byte progress = holdTimer(0) )
		{//check the timer to see if a step has been made
			switch(progress)// I dislike swich cases but here we go
			{//given how long the input has been held
				case 1://printable case 5-200ms
					if(input==128){break;}// nonprintable
					hint = 1;//the first case where the leter prints is just a hint
					if(input==8){hint=0;}//prevent a double backspace
					return input;//return fruitful output 
				case 2://validation checkpoint; letter stays printable
					if(input==128){break;}// register mode intention
					hint = 0;//Now press counts as a real press and will retain
					return 0;// no output just a checkponit
				case 3://hold check point
					if(input==128){break;}// register mode intention
					hint = 2; // given user want lower they can release deletion happen
					if(input==8){hint=0;}//prevent a double backspace
					return 8;//delete currently printed char in preperation for a caps
				case 4://printable hold case 300-1000ms
					hint = 0; // removes hinting for capitilization 
					if(input == 128){ break;}// register mode intention
                                        if(input == 32){input = 65;}//space turns to excliamation    
                                        return input-32;//subtract 32 to get caps; how convienient 
				case 5://special commands
					break;
			}
		}
		return 0;//if the timer returns no action: typical case
	}
	else// no input or input was inequal to last
	{
		lastInput=input; // hold the place of the current value for next loop
		holdTimer(1);//reset the timer
		byte holdReturn =0;// hold the return char to reduce repition
		switch(hint)
		{
			case 1://input was released before being accepted; speed press for hint
				holdReturn=8;//delete the hint
			        break;
			case 2:// leter was removed for caps, user let go before upper case
				holdReturn=input;//places the letter back
		}
		hint = 0;
		return holdReturn;
	}
}

//------------------------messaging functions----------------------------------

void toast(char message[])
{// message the appears and disapears, just like "toast" in android
  for(int pos=0;message[pos];pos++){Serial1.write(message[pos]);}//print message
  while(hapticMessage(message) != 128){;}//wait for haptic message to finish
  rmMessage(message);// remove message
}

void rmMessage(char message[])
{//remove a message
  for(int i=0;message[i];i++)
  {
    Serial1.write(8);
  }
}