#include "__Game_RootGame0.h"
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "_RootButtonHandler.h"
#include "_GameProcess.h"
#include "_LightShows.h"
#include <stdlib.h>
#include "LED-Display.h"
#include "EEPROMRoutines.h"
#include "SystemUtilities.h"


unsigned char EasterEggTable[16][4]= {
{3,2,1,0},
{7,6,5,4},
{11,10,9,8},
{15,14,13,12},		//First 4 in order for firefly up

{13,9,5,1},			//These 3 horiz lines for firefly Jr
{14,10,6,2},
{15,11,7,3},
{12,8,4,0},

{12,13,14,15},
{8,9,10,11},
{4,5,6,7},
{0,1,2,3},		//verticals

{0,4,8,12},		//Then 4 that are horizontal
{1,5,9,13},
{2,6,10,14},
{3,7,11,15}
};


#define WAIT1 0x01
#define COUNT 0x02
unsigned short eli=0;


void Root_Game0 (void)
{

	GameSwitchCheck();
			MasterSwitchCheck();
			SelectSwitchCheck();
	switch (GameState)
	{
		
		case INIT:
			ResetAudioAndLEDS();
			GameState = WAIT1;
		break;
		
		case WAIT1:
			if(GameTimer1>100)
			{
			   GameTimer1 = 0;
			   GameState = COUNT;	
			}
		break;
		
		case COUNT:
		
			if(GameTimer1>10)
			{
	   			 LEDSendScores(248, 	ScoreBrightness, eli,eli);
	   			 eli++;
	   			 if(eli>999)
	   			 	eli = 0;
	   			 	
	   			
	   			 GameTimer1 = 0;
	   		 }
	   		 
	   			 
		break;
		
		
	}
}




void OnButtonPressRootGame0(unsigned char button)
{
	AudioNodeEnable(0,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(1,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(2,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(3,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(4,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(5,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(6,3,1,1,0,50,0xFF,0x00);
	
	
	AudioNodeEnable(7,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(8,3,1,1,0,50,0xFF,0x00);
	
	AudioPlaySound(96,3);
}

void OnMasterSwitchPressRootGame0(void)
{
	AudioNodeEnable(0,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(1,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(2,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(3,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(4,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(5,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(6,3,1,1,0,50,0xFF,0x00);
	
	
	AudioNodeEnable(7,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(8,3,1,1,0,50,0xFF,0x00);
	
	AudioPlaySound(96,3);
}

void OnSelectPressRootGame0(button)
{
	AudioNodeEnable(0,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(1,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(2,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(3,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(4,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(5,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(6,3,1,1,0,50,0xFF,0x00);
	
	
	AudioNodeEnable(7,3,1,1,0,50,0xFF,0x00);
	AudioNodeEnable(8,3,1,1,0,50,0xFF,0x00);
	
	AudioPlaySound(96,3);	
}	
