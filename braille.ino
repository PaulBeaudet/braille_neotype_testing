//braille.ino--- Utilizes an array of 8 buttons and a corrisponding darlington array of 6 pagers
//Copyright Paul C Beaudet -braile_spark_testing GPL-See LICENSE
// With the goal of feeling and transmit brialle by Bluetooth transmission w/ Bluefruit EZ-Key
#include "hardware.h" // abstracts low level hardware
#include "spark_disable_cloud.h"//needs to be include for the folowing undef
#undef SPARK_WLAN_ENABLE //disable wifi by defaults in order to have an offline option

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
  byte actionableSample= patternToChar(input);// 0 parameter denotes reverse lookup
  if(actionableSample){patternVibrate(input, PWMintensity);}//fire the assosiated pagers! given action
  else{patternVibrate(0, 0);}//otherwise be sure the pagers are off
  actionableSample = holdFilter(actionableSample);//  further filter input to "human intents"
  if(actionableSample){Serial1.write(actionableSample);}//print the filter output 
}
//-----------braille checking and convertion----------------
byte chordPatterns[] {1,5,48,56,2,24,33,6,4,14,28,12,40,30,7,18,31,3,16,32,51,45,8,35,54,49,};
  #define PATTERNSIZE sizeof(chordPatterns)

byte patternToChar(byte base)
{
  if(base == 128){return 8;}//Express convertion: Backspace // Backspace doubles as second level shift for special chars
  if(base == 64){return 32;}//Express convertion: Space // Space also doubles as the first shift in a chord
  
  for (byte i=0; i<PATTERNSIZE; i++)   
  {// for all of the key mapping   
    if ( (base & 63) == chordPatterns[i] ) 
    {//patern match regardless most significant 2 bits // 63 = 0011-1111 // mask the 6th and 7th bit out
      if ((base & 192) == 192){break;}//third level shift *combination holding space and backspace
      if (base & 64)//first level shift *combination with space
      {// 64 = 0100-0000 // if( 6th bit is fliped high )
        //if(lower shift, less than 10th result) {return corrisponding number}
        if(i<10){return '0' + i;} //a-j cases (ascii numbers)
        if(i<25){return 23 + i;}  //k-y cases ( !"#$%&'()*+'-./ )
        if(i==26){break;}         //z case (unassigned)
      } 
      if (base & 128)//second level shift *combination with backspace
      {//128 = 1000-0000 // if(7th bit is high) 
        if(i<7){return ':' + i;}//a-g cases ( :;<=>?@ )
        if(i<13){return 84 + i;}//h-m cases ( [\]^_`  )
        if(i<17){return 110 + i;}//n-q cases( {|}~    ) 
        break;                   //other casses unassigned
      }
      return 'a' + i;// return plain char based on possition in the array given no shift
    }
  }
  return 0;
}

byte charToPattern(byte letter)
{
  if(letter == 32){return 64;}//Express convertion: Space // Space also doubles as the first shift in a chord
  
  for (byte i=0; i<PATTERNSIZE; i++)   
  {// for all of the key mapping
    if ( letter == ('a'+ i) ){return chordPatterns[i];}//return typicall letter patterns
    if ( letter < 58 && letter == ('0' + i) ) {return chordPatterns[i] | 64;} // in numbers shift case return pattern with 6th bit shift
    if ( letter > 32 && letter < 48 && letter == (23 + i) ) {return chordPatterns[i] | 64;}//k-y cases ( !"#$%&'()*+'-./ )return 6th bit shift
    if ( letter < 65 && letter == (':' + i) ) {return chordPatterns[i] | 128;}//               a-g cases  (:;<=>?@ ), return 7th bit shift
    if ( letter > 90 && letter < 97 && letter == (84 + i) ) {return chordPatterns[i] | 128;}// h-m cases  ([\]^_`  ), return 7th bit shift
    if ( letter > 122 && letter < 127 && letter == (110 + i) ) {return chordPatterns[i] | 128;}//n-q cases( {|}~   ), return 7th bit shift
  }
  return 0;
}
// ----------------input interpertation-------------

byte holdTimer(byte reset)
{
    #define DELAYTIME 1 //the delay time corisponds to action values
    #define TIMESTARTED 0 // Denotes when each action starts
    static uint16_t actions[]={2,100,300,200,300}; //actions progres as timer is held at 0
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
					hint = 1;//the first case where the leter prints is just a hint
					if(input==8 || input==32){hint=0;break;}//prevent a double backspace or space hinting
					return input;//return fruitful output 
				case 2://validation checkpoint; letter stays printable
					hint = 0;//Now press counts as a real press and will retain
					if(input==8 || input==32){return input;}//be sure of printability for back and space before executing
					return 0;// no output just a checkponit
				case 3://hold check point
					hint = 2; // given user want lower they can release deletion happen
					if(input==8 || input== 32){hint=0;}//prevent a double backspace or space hinting
					if(input > 32 && input < 97 || input > 122 && input < 127){hint=0;break;}//in special char cases
					return 8;//delete currently printed char in preperation for a caps
				case 4://printable hold case 300-1000ms
					hint = 0; // removes hinting for capitilization 
					if(input == 8 || input > 32 && input < 97 || input > 122 && input < 127){break;}//exept for backspace and special cases
                    if(input == 32){input = 45;}//space turns to cariage return    
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
		//'i'
		//'j'
		//'k'
		//'l'
		case 109://'m'
		  toast("this is a message");
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
  patternVibrate(charToPattern(letter), PWMintensity);
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
