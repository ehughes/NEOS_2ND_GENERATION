#include "__Game_Diagnostic.h"
#include "_GameVariables.h"
#include "SystemVariables.h"
#include "_LightShows.h"
#include "SystemUtilities.h"
#include "LED-Display.h"
#include "_RootButtonHandler.h"
#include "CANROutines.h"
#include <stdlib.h>
#include <math.h>
#include "audio.h"




//Table for order of lights in demo
unsigned int __attribute__ ((space(auto_psv))) SelectButtonTable[20][2]=
{
{0x0,0x20},		//L5
{0x0,0x8000},	//L15
{0x0,0x400},	//L10
{0x0,0x40},		//L6
{0x1,0x0},		//L16
{0x0,0x800},	//L11
{0x0,0x80},		//L7
{0x2,0x0},		//L17
{0x0,0x1000},	//L12
{0x0,0x100},	//L8
{0x0,0x200},	//L9
{0x4,0x0},		//L18
{0x0,0x2000},	//L13
{0x0,0x4000},	//L14
{0x8,0x0},		//L19
{0x0,0x1},		//L0
{0x0,0x2},		//L1
{0x0,0x4},		//L2
{0x0,0x8},		//L3
{0x0,0x10}		//L4
};



void GameDiagnostic (void)
{
	//GameScores[0] will be the address being defined
	switch (GameState)
	{
		case 0:
			ResetAudioAndLEDS();			//Turn off everything
			ResetGameVariables();
			GameState+=1;
			GameTimeout1=100;			//Flashing time
			AudioNodeEnable(5,0,0,1,0,50,0xFF,0);	// On for 1 sec
			AudioNodeEnable(9,0,0,1,0,50,0xFF,0);	// On for 1 sec
			AudioPlaySound(299,0);		
			LEDSendScores(248,ScoreBrightness,1000,VERSION);		// Blank if not selected
			GameTimeout1=200;
			GameTimeout2=18000;		//3 min inactivity timer
		break;
		case 1:
			if (GameTimer1 >= GameTimeout1)
				GameState+=1;
		break;
		case 2:

			//Put some scores up & advance
			if (GameScores[0] != 0)
				LEDSendScores(248,ScoreBrightness,GameScores[0],GameScores[1]);
			else
			{
				LEDSegmentPattern[0] = 0x3F3F;		// 00
				LEDSegmentPattern[1] = 0x3F3F;
				LEDSegmentPattern[2] = 0x3F3F;
				CANQueueTxMessage(0x70,248 + (ScoreBrightness << 8),LEDSegmentPattern[0],
				LEDSegmentPattern[1],LEDSegmentPattern[2]);
			}			
			if (GameScores[0] >= 999)
			{
				GameScores[0]=0;
				GameScores[1]=0;
			}
			else
			{
				GameScores[0] /= 111;
				GameScores[0] += 1;
				GameScores[0] *= 111;
				GameScores[1] /= 111;
				GameScores[1] += 1;
				GameScores[1] *= 111;
			}

			//Show a selection light & advance
//			if (GameVariable[2] <= 15)
//			{
//				GameInteger[0] = (1 << GameVariable[2]);
//				GameInteger[1]=0;
//			}
//			else
//			{
//				GameInteger[0]=0;
//				GameInteger[1] = (1 << (GameVariable[2]-16));
//			}

			GameInteger[1]=SelectButtonTable[GameVariable[2]][0];
			GameInteger[0]=SelectButtonTable[GameVariable[2]][1];
			ScoreSendLights(248,GameInteger[0],GameInteger[1]);
			GameVariable[2] += 1;
			GameVariable[2] %= 20;		//Limit value 0-19


			//Light a button & advance
			if (GameVariable[0] == 0)
			{
				LEDSendMessage(GameVariable[1],RED,RED,100,0);	//Flash the button
				GameVariable[0]=1;
			}
			else
			{
				LEDSendMessage(GameVariable[1],GREEN,GREEN,100,0);	//Flash the button
				GameVariable[0]=0;
				GameVariable[1] += 1;			//increment to next button
				GameVariable[1] &= 0x0F;
			}		
	
			GameTimer1=0;
			GameTimeout1=100;
			GameState+=1;

		break;
		case 3:
			GameSwitchCheck();
			MasterSwitchCheck();
			SelectSwitchCheck();
			if (GameTimer1 >= GameTimeout1)
				GameState=2;
			if (GameTimer2 >= GameTimeout2)
			{
				GameState=0;		//Start over at the beginning
				GameSelected=0;		// Go back to selector
			}
		break;

		case 4:
		default:
			GameSelected=GAME_FULL_POWER;
			GameState=0;
		break;
	}
}

void OnMasterSwitchPressDiagnostic(void)
{
	GameState=4;		//This exits the diagnostic
}

void OnButtonPressDiagnostic(unsigned char button)
{
	GameVariable[0]=0;
	GameVariable[1]=button;		//Reset button counter to this button
	//Play a sound
	AudioNodeEnable(button,0,0,1,0,50,0xFF,0);	// On for 1/10 sec
	LEDSendMessage(button,RED,RED,100,0);	//Light the button
	AudioPlaySound(420,0);
	GameScores[0]=button;
	GameScores[1]=button;
	LEDSendScores(248,ScoreBrightness,GameScores[0],GameScores[1]);
	GameTimer1=0;
	GameTimer2=0;
	GameTimeout1=100;
}

void OnSelectPressDiagnostic(unsigned int sel)
{
	AudioNodeEnable(1,0,3,1,0,40,0xFF,0);	// 0.4 sec blip
	AudioNodeEnable(5,0,3,1,0,40,0xFF,0);	// sound on
	AudioNodeEnable(9,0,3,1,0,40,0xFF,0);	// sound on
	AudioPlaySound (22,0);

	GameVariable[2]=sel;
	if (GameVariable[2] <= 15)
	{
		GameInteger[0] = (1 << GameVariable[2]);
		GameInteger[1]=0;
	}
	else
	{
		GameInteger[0]=0;
		GameInteger[1] = (1 << (GameVariable[2]-16));
	}
	ScoreSendLights(248,GameInteger[0],GameInteger[1]);
//	GameScores[0]=sel;
//	GameScores[1]=sel;
//	LEDSendScores(248,ScoreBrightness,GameScores[0],GameScores[1]);
	GameTimer1=0;
	GameTimer2=0;
	GameTimeout1=100;
}
