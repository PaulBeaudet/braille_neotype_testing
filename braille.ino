//braille.ino--- Utilizes an array of 8 buttons and a corrisponding darlington array of 6 pagers
//Copyright Paul C Beaudet -braile_spark_testing GPL-See LICENSE
// With the goal of feeling and transmit brialle by Bluetooth transmission w/ Bluefruit EZ-Key
#include "hardware.h" // abstracts low level hardware
#include "spark_disable_cloud.h"//needs to be include for the folowing undef
#undef SPARK_WLAN_ENABLE //disable wifi by defaults in order to have an offline option

#define ENCODEAMT 33 // size is defined to structure iteration amount
byte byteToBraille [2][ENCODEAMT] // brialle convertion array
{
  { // input in characters
    ' ','t','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','u','v','w','x','y','z', 8, 128,'-',';','.', '?', }
  ,
  { //corrisponding braille binary output in decimal form, read from least significant bit
    32, 30, 1 , 5 , 3 , 11, 9 , 7 , 15, 13, 6 , 14, 17, 21, 19, 27, 25, 23, 31, 29, 22, 49, 53, 46, 51, 59, 57 ,64, 128, 48, 40, 34,  43, } 
};//each bit in the corrisponding bytes represents a "bump" state

int HAPTICTIMING = 1400; //ms, controls haptic display durration, Future; will be user adjustable
byte PWMintensity = 200; // Adjusts the intensity of the pwm

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
  if(actionableSample){patternVibrate(input, PWMintensity);}//fire the assosiated pagers! given action
  else{patternVibrate(0, 0);}//otherwise be sure the pagers are off
  actionableSample = holdFilter(actionableSample);//  further filter input to "human intents"
  if(actionableSample){Serial1.write(actionableSample);}//print the filter output 
}
//-----------braille checking and convertion----------------
byte brailleConvert(byte letter, boolean convert)
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
// ----------------input interpertation-------------

byte holdTimer(byte reset)
{
    #define DELAYTIME 1 //the delay time corisponds to action values
    #define TIMESTARTED 0 // Denotes when each action starts
    static uint16_t actions[]={2,150,300,200,300}; //actions progres as timer is held at 0
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

/**************************
hold flow
1. register- Print key hinting, remove given no follow thru
2. Debounce/conglomerate- Accept valid chord, turn off hinting
3. Shift-up- remove char in preperation of upper case
4. Capitilize- print upper case chare
5. Special Cases- Programed 'command' cases for special features 
**************************/

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
				    specialCommands(input);//turns various input into commands
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

void specialCommands(byte input)
{// specialCommands occure on long holds of particular letters
    Serial1.write(8);//removes key letter
	switch(input)//takes in key letter
	{
		case 104://'h' or HINT; sings the haptic alphabet
		for(byte i=97;i<123;i++)
		{//interate through all the letters in the alphabet
			hapticMessage(i);//ask for a letter in the alphabet
			Serial1.write(i);//write the letter
			while(!hapticMessage()){;}//wait for the char to finish
			Serial1.write(8);//remove letter
		}
		break;
		case 114://'r'
		break;
		case 115://'s' case changes speed off haptic display
		//btMessage("spd#");
		//rmMessage("spd#");
		break;
		case 116://'t'
		break;
	}
}

void toast(char message[])
{// message the appears and disapears, just like "toast" in android
  btMessage(message);//print the message
  while(hapticMessage(message) != 128){;}//wait for haptic message to finish
  rmMessage(message);// remove message
}

void btMessage(char message[])
{
  for(int pos=0;message[pos];pos++){Serial1.write(message[pos]);}//print message
}

void rmMessage(char message[])
{//remove a message
  for(int i=0;message[i];i++){Serial1.write(8);}
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

void hapticMessage(byte letter) // intializing function
{ // set a letter to be "played"
  ptimeCheck(HAPTICTIMING);
  patternVibrate(brailleConvert(letter, 1), PWMintensity);
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
      patternVibrate(0, 0);//stop the message
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

boolean checkMatch(char input[], char target[])
{
  for(byte i=0;target[i];i++)
  {
    if(input[i]!=target[i])
    {
      return false;
    }
  }
  return true;
}
