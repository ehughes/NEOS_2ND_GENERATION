#include <p30fxxxx.h>
#include <ports.h>
#include <Timer.h>
#include "SystemUtilities.h"
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "Audio.h"
#include "CANRoutines.h"
#include <stdlib.h>
#include <math.h>
#include "DataEEPROM.h"
#include "LED-Display.h"
#include "_RootButtonHandler.h"
#include "CANCommands.h"
#include "GameMapping.h"
#include "config.h"

void SendNodeNOP()
{
CANQueueTxMessage(0x8C,0,0,0,0);
}

unsigned char Scale100to63(unsigned int displayval)
{
	unsigned int roundoff;
	displayval *= 63;		//Range now 0-6300
	roundoff = displayval % 100;
	displayval /= 100;		//Range 0-63
	if (roundoff >=50)
		displayval += 1;
	return ((unsigned char)(displayval));
}

unsigned char Scale100to255(unsigned int displayval)
{
	unsigned int roundoff;
	displayval *= 255;		//Range now 0-25500
	roundoff = displayval % 100;
	displayval /= 100;		//Range 0-255
	if (roundoff >=50)
		displayval += 1;
	return ((unsigned char)(displayval));
}

unsigned int Scale255to100(unsigned char inpval)
{
	unsigned int roundoff,inp;
	inp=inpval * 100;	//Range now 0-25500
	roundoff = inp % 255;
	inp /= 255;		//Range 0-100
	if (roundoff >=128)
		inp += 1;
	return ((unsigned int)(inp));
}

unsigned int Scale63to100(unsigned char inpval)
{
	unsigned int roundoff,inp;
	inp=inpval * 100;	//Range now 0-25500
	roundoff = inp % 63;
	inp /= 63;		//Range 0-100
	if (roundoff >=32)
		inp += 1;
	return ((unsigned int)(inp));
}


void ResetAudioAndLEDS(void)
{
	AudioNodeEnable(0,0,0,0,0,0xFFFF,0xFF,0xFF);		// Master sound off 
	ResetAllSlaves();				//LEDs all sound & LEDs off
	LEDSendMessage(0,0,0,0,0,0,0,0,0);	// Master LEDs off
	ResetLeds();
}

void ResetLeds(void)
{
	int temp;
	for (temp=0; temp <=NUM_BUTTONS; temp+=1)
	{
		LEDSendMessage(temp,0,0,0,0,0,0,0,0);	
	}
}




void PortInit(void)
{

		ADPCFG=0xFFFF;		//Make all digital inputs or portb can't be read
		//TRISA same for all
		LATA = 0xF000;		// Memory select lines high (off) 
		TRISA = 0x06C0;		/* PA6,7,9,10 option inps, PA12-16 MEM0,1,2,3 outputs */	
	

		LATD = 0;			/* LEDS all off */
		TRISD = 0x0080;		//Port D0,D1 has red and green LEd and D7 is piexo input
		
		LATG = 0x1;		// USB chip reset line high (off)
		TRISG = 0x1002;		/* RG12 is grounded so make an input, PG1 switch inp, rest output */
	
		//For ELC0025 & ELC0026, RD7 & RG1 are tied together and connected to
		//piezo switch input. For ELC0019 RD7 is a LED output.
		//Identify ELC0025/26 by verifying this connection
		//This will work unless piezo input is shorted, which is unlikely
	
		LATC = 0; 			/* LEDS on PC 1,2,13,14 off */
		TRISC = 0x8018;		/* PC 1,2,13,14 outputs, PC 3,4,15 inputs */
		LATF = 0;
		TRISF = 0x0091;		
	
		LATB = 0x200;		// ~FORCEOFF must be high
		TRISB = 0x0007;		// All outputs except PB0,1,2

}		


void ReadOptionJumpers(void)
{
	OptionJumpers=((PORTA & 0x600) >> 7) | ((PORTA & 0xC0) >> 6); //Bits 6,7,9,10
}



void ResetAllSlaves(void)
{
	CANQueueTxMessage(RESET_SLAVES,0,0,0,0);
}



#define MAX_RANDOM_RETRIES 254
BYTE PreviousRandomButton1 = 0xFF;
BYTE PreviousRandomButton2 = 0xFF;


void InitRandomButton()
{
	PreviousRandomButton1 = 0xFF;
	PreviousRandomButton2 = 0xFF;
}	

BYTE RandomButton(BYTE Exclude1, BYTE Exclude2)
{
	
	BYTE RandomButton=0;
	BYTE Retries = 0;
	
	//RandomButton=rand()&0x07;
	
	RandomButton = (BYTE)(rand()) % NUM_BUTTONS;
	
	while(((RandomButton == PreviousRandomButton1) || (RandomButton == Exclude1) || (RandomButton == Exclude2)) && (Retries<MAX_RANDOM_RETRIES) )
	{
		Retries++;
		RandomButton = (BYTE)(rand()) % NUM_BUTTONS;
	}
	
	PreviousRandomButton2 = PreviousRandomButton1;
	PreviousRandomButton1 = RandomButton;
	
	return RandomButton;
}

BYTE LGPreviousRandomButton = 0;

BYTE LGRandomButton(BYTE Exclude1, BYTE Exclude2)
{
	
	BYTE RandomButton=0;
	BYTE Retries = 0;
	
//	RandomButton=rand()&0x07;
	RandomButton = (BYTE)(rand()) % NUM_BUTTONS;

	while(((RandomButton == LGPreviousRandomButton) || (RandomButton == Exclude1) || (RandomButton == Exclude2)) && (Retries<MAX_RANDOM_RETRIES) )
	{
		Retries++;
	//	RandomButton=rand()&0x07;
	RandomButton = (BYTE)(rand()) % NUM_BUTTONS;
	
	}
	
	if(Retries == MAX_RANDOM_RETRIES)
	{
		RandomButton = 0;
	}
		
	LGPreviousRandomButton = RandomButton;
	
	return RandomButton;
}

BYTE SelectRandomDirection()
{
	return rand()&0x01;
}	

void ResetToGameSelector()
{
	
GameSelected = 	GAME_ROOT_GAME0;
GameState = INIT;

}	




