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


void ResetAudioAndLEDS(void)
{
	AudioNodeEnable(0,0,0,0,0,0xFFFF,0xFF,0xFF);		// Master sound off 
	ResetAllSlaves();				//LEDs all sound & LEDs off
	LEDSendMessage(0,0,0,0,0,0,0,0,0);	// Master LEDs off
	ResetLeds();
}

void ResetLeds(void)
{
	WORD temp;
	for (temp=0; temp <=NUM_BUTTONS; temp+=1)
	{
		LEDSendMessage(temp,0,0,0,0,0,0,0,0);	
	}
	
	CANQueueTxMessage(0x70,DISPLAY_ADDRESS,0,0,0);
	ScoreSendLights(DISPLAY_ADDRESS,  0,0x0);	
		
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

BYTE RandomButton(BYTE Exclude1, BYTE Exclude2, BYTE Exclude3)
{
	
	BYTE RandomButton=0;
	BYTE Retries = 0;
	BYTE i;
	
	RandomButton = (BYTE)(rand()) % NUM_BUTTONS;
	
	while(((RandomButton == PreviousRandomButton1) || (RandomButton == Exclude1) || (RandomButton == Exclude2) || (RandomButton == Exclude3)) && (Retries<MAX_RANDOM_RETRIES) )
	{
		Retries++;
		RandomButton = (BYTE)(rand()) % NUM_BUTTONS;
	}
	
	if(Retries >= MAX_RANDOM_RETRIES)
	{
		for(i=0;i<NUM_BUTTONS;i++)
		{
			if((i!=PreviousRandomButton1)&&(i!=Exclude1)&&(i!=Exclude2)&&(i!=Exclude3))
			{
			 	RandomButton = i;
			 	break;
			}
		}
		
	}
	
	PreviousRandomButton1 = RandomButton;
	
	return RandomButton;
}


BYTE RandomButton4Exclude(BYTE Exclude1, BYTE Exclude2, BYTE Exclude3,BYTE Exclude4)
{
	
	BYTE RandomButton=0;
	BYTE Retries = 0;
	BYTE i;
	
	RandomButton = (BYTE)(rand()) % NUM_BUTTONS;
	
	while(((RandomButton == Exclude1) || (RandomButton == Exclude2) || (RandomButton == Exclude3) || (RandomButton == Exclude4)) && (Retries<MAX_RANDOM_RETRIES) )
	{
		Retries++;
		RandomButton = (BYTE)(rand()) % NUM_BUTTONS;
	}
	
	if(Retries >= MAX_RANDOM_RETRIES)
	{
		for(i=0;i<NUM_BUTTONS;i++)
		{
			if((i!=Exclude1)&&(i!=Exclude2)&&(i!=Exclude3)&&(i!=Exclude4))
			{
			 	RandomButton = i;
			 	break;
			}
		}
		
	}
	
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


	


