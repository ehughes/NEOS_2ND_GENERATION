//Variables used System wide


#define VERSION 128	//100 for version 1.00, etc.

//#define	FASTSLEEP	//THis makes it go to sleep fast to test wake up


//***********************************************************************
//Main Function
//***********************************************************************
extern unsigned char Slave; 
extern unsigned char OptionJumpers;
extern unsigned char ScoreDisplay;
extern unsigned char ELC0025or26;
extern unsigned long InactivityTimer;


extern unsigned int BurnInTimer;
extern unsigned char BurnIn;

extern int DiagTentativeAddress;
extern unsigned char DiagAddressSetup;

extern unsigned int NodeSwitchStates[2];	//32 bits of switch states
extern unsigned char MasterSwitchState;
extern unsigned char MasterSwitchStateLast;
extern unsigned int NodeSwitchStatesLast[2];  //What they were last time
extern unsigned char PiezoLastState;
extern unsigned int PiezoTimer;

extern unsigned char Piezos[3];	// status of piezo bits
extern unsigned char PiezosLast[3];	//Previous state


// EEPROM VARIABLES  EEPROM VARIABLES  EEPROM VARIABLES EEPROM VARIABLES
// EEPROM VARIABLES  EEPROM VARIABLES  EEPROM VARIABLES EEPROM VARIABLES
// EEPROM VARIABLES  EEPROM VARIABLES  EEPROM VARIABLES EEPROM VARIABLES

extern unsigned char MyNodeNumber;		//0-31 number assigned to this node for gameplay
extern unsigned char AudioGlobalVolume;
extern unsigned char AudioBackgroundVolume;
extern unsigned char NoAnnoy;			//When =1 it is silent in idle mode & no snoring
extern unsigned char RedBrightness;
extern unsigned char GreenBrightness;
extern unsigned char ScoreBrightness;



//AUDIO STREAM  AUDIO STREAM  AUDIO STREAM  AUDIO STREAM AUDIO STREAM  AUDIO STREAM AUDIO STREAM AUDIO STREAM
//AUDIO STREAM  AUDIO STREAM  AUDIO STREAM  AUDIO STREAM AUDIO STREAM  AUDIO STREAM AUDIO STREAM AUDIO STREAM
//AUDIO STREAM  AUDIO STREAM  AUDIO STREAM  AUDIO STREAM AUDIO STREAM  AUDIO STREAM AUDIO STREAM AUDIO STREAM

extern unsigned char AudioStreamTransmitFlag[4];  //Set to 1 after every 128 words are sent to codec 
extern unsigned char AudioStreamCounter[4];		// Counts to 3F in interrupt
//Audio buffer contains 128 upper & 128 lower words
//While one half is being played the other half is being written by CAN data
extern char AudioBuffer[256];	/* 256 samples at 16000 hz = 16ms of audio */
extern unsigned int AudioWriteIndex ;		/* Addr to write incoming data */
extern unsigned int AudioReadIndex;		/* Addr to read stuff out to codec */

//unsigned char AudioReadIndexResetStream=0;	// Stream # to look for to reset index
extern unsigned char AudioReadIndexReset;		// Indicates a stream reset is needed

//Start read index 1/2 buffer out of phase with the stream counter
extern unsigned char AudioOn;	/* 0 for no sound, 1 for sound */
extern unsigned char MyAudioStream;	//Stream 0,1,2 or 3 that THIS NODE is playing
extern unsigned char MyAudioVolume;

extern unsigned char AudioTimeoutVolume;	//Volume AFTER timeout
extern unsigned char AudioTimeoutStatus;	//on/off status after timeout
extern unsigned char AudioTimeoutStream;	//Stream AFTER timeout
extern unsigned int AudioTimer;			//Count up at 1/100 sec rate to time audio switchover
extern unsigned int AudioTimeout;			//Value to look for for audio switchover

extern unsigned long AudioStreamAddress[4];	//moving addresses for the 4 current sounds
extern unsigned long AudioStreamStartAddress[4];	//Starting addresses for the 4 current sounds
extern unsigned long AudioStreamLength[4];		//and their lengths
//unsigned char AudioStreamVolume[4];
//long DCILast=0;				// Value for interpolating 16KHZ audio from 8KHz


//LED LED LED LED LED LED LED LED
//LED LED LED LED LED LED LED LED
//LED LED LED LED LED LED LED LED

extern unsigned int LEDOutputData[4];	/* up to 64 LEDs */
extern unsigned char LEDColor[3];	/* R,G,B 0-255 color values */
extern unsigned int LEDEnable[4] ; /* [0] = red LEDs [1] = Green LEDs [2] = Blue LEDs */
//OR in the case of score display LEDENable is the bits to light
extern unsigned char LEDStartColor[3];	/* R,G,B 0-255 color values */
extern char LEDColorChange[3]; //SIGNED change values for fading colors



extern unsigned char LEDScoreOnTime;		//Flashing on time in 1/100 sec steps
extern unsigned char LEDScoreOffTime;		//Flashing off time in 1/100 sec steps
extern unsigned char LEDScoreOnTimer;		//Flashing on time in 1/100 sec steps
extern unsigned char LEDScoreOffTimer;		//Flashing off time in 1/100 sec steps
extern unsigned int LEDScoreFlashEnable[2];		//Bit 0 = left flash enable bit 1 = right flash
extern unsigned int LEDSegmentPattern[3];	// encoded 7-segment patterns for 6 digits


extern int ColorSequence;  //Light Show variable
extern int ColorCounter;  //Light Show variable
extern int	DelayCounter;  //Light Show variable

extern unsigned int LEDTimer;	//10ms down counter until LED shutoff 10 BITS LONG
extern unsigned int LEDFadeTimer; //10ms UP counter used for fading 10 BITS LONG
extern unsigned int LEDFadeTime;  //Limit for LED 10 BITS LONG
extern unsigned char T2LED10msRefreshFlag;

extern unsigned char T210msHeartbeat;
extern unsigned char T2DivideBy45;
extern unsigned char T2PWMCounter;

//MISC MISC MISC MISC MISC MISC
//MISC MISC MISC MISC MISC MISC
//MISC MISC MISC MISC MISC MISC
//MISC MISC MISC MISC MISC MISC
//MISC MISC MISC MISC MISC MISC


//#define ELC0002		// define this for old style button
#define ELC0019			//Define this for ELC0019 or ELC0020 versions
//DO NOT TURN ON BOTH ELC0002 & ELC0019!
//ELC0002 probably doesn't work any more because of memory problems as of v1.2x
//OK to leave ELC0019 on even if it's a score display
//No! it;'s not OK to leave ELC0019 on for scoredisplay
//OK I changed this now - ELC0019 ON doesn't affect score display
//Except for the clock options, which I intend to fix with next batch
//of scoreboards using 8mhz crystals

//SCOREDISPLAY replaced by automatic ScoreDisplay variable
//I may still need this define because the clock oscillator settings are different
//#define SCOREDISPLAY		//enable this for alternate score display compile

#define PIEZO 0	//Set to 1 to enable piezo button connected to programming connector
//PIEZO=1 is only necessary for ELC0002! ELC0019 uses different port

//#define BUTTONDEMO		// Standalone light demo - make slave=1 too!
//#define BUTTONTEST		//Test for buttons during game0
//#define ROWFAKE		//Makes 1st column act like a row for debugging

#define LASTBUTTON 15		//Highest numbered button in game array

#define PIEZOTIME 50	//Piezo switch forced dwell time in 1/100 sec steps

//#define AUDIO16		//Set to indicate 16 bit / 16KHz data is received in serial download from PC


#ifdef FASTSLEEP
#define INACTIVITYTIMEOUT 200;	// 2 sec
#else
#define INACTIVITYTIMEOUT 6000;	//1 minute
#endif


//NOANNOY was originally a define but then changed to a variable adjustable in diagnostics
//But then in V1.23 production version adjustability was removed, so I brought 
//back the define option for V1.24!
//#define NOANNOY



//***********************************************************************
//SPI Related 
//***********************************************************************

extern unsigned char SPIInstBuffer[5];		// Up to 8 pending inst, 3* addr, 1* chip number
extern unsigned char SPITxBuffer[256];	// 128 words read from Serial Flash
extern unsigned char SPIRxBuffer[256];		//Data coming in from SPI (Was [260] for interrupt version)
extern unsigned int SPIDataCount;		//Number of data bytes required for instruction */
extern unsigned char SPIInstrCount;			//Counts SPI transmit bytes




//***********************************************************************
/// UART DATA 
//***********************************************************************

extern unsigned char UART1TxBuffer[9];
//unsigned char UART1TxDone=0;
extern unsigned char UART1TxPointer;
extern unsigned char UART1TxLength;
extern unsigned char UART1RxBuffer[137];
extern unsigned char UARTLastReceived;		//Remembers where the data came from
//unsigned char * Receiveddata = UART1RxBuffer;
extern unsigned char UART1RxIndex;
extern unsigned char UART1RxTimer;		// Looks for end of transmission
extern unsigned char UART1RxDataReady;	// Flag indicates received data ready
extern unsigned char UART1ReplyPending;
extern unsigned long UARTAddress;
extern unsigned char SPITxAudioHalt;			//Used by UART to halt SPI audio transmissions


//***********************************************************************
/// EEPROM
//***********************************************************************


extern unsigned int EEDefaults[8] ;
extern unsigned int EELowLimits[8] ;
extern unsigned int EEHighLimits[8];

extern unsigned char MyNodeNumber;		//0-31 number assigned to this node for gameplay
extern unsigned char AudioGlobalVolume;
extern unsigned char AudioBackgroundVolume;
extern unsigned char NoAnnoy;			//When =1 it is silent in idle mode & no snoring
extern unsigned char RedBrightness;
extern unsigned char GreenBrightness;
extern unsigned char ScoreBrightness;

//**************************************
// LED  /  DISPLAY
//************************************88

extern unsigned int DispDefaults[8];	//defaults scaled for display 
