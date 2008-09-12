#include "__Game_FullPower.h"
#include "_GameVariables.h"
#include "SystemUtilities.h"
#include "SystemVariables.h"
#include "LED-Display.h"
#include "_RootButtonHandler.h"
#include "ADCRoutines.h"
#include "Audio.h"


//Table for order of lights in full power
//Same as table above but the bits stay on
unsigned int __attribute__ ((space(auto_psv))) SelectButtonPowerTable[20][2]=
{
{0x0,0x20},		//L5
{0x0,0x8020},	//L15
{0x0,0x8420},	//L10
{0x0,0x8460},		//L6
{0x1,0x8460},		//L16
{0x1,0x8C60},	//L11
{0x3,0x8CE0},		//L7
{0x3,0x8C60},		//L17
{0x3,0x9CE0},	//L12
{0x3,0x9DE0},	//L8
{0x3,0x9FE0},	//L9
{0x7,0x9FE0},		//L18
{0x7,0xBFE0},	//L13
{0x7,0xFFE0},	//L14
{0xF,0xFFE0},		//L19
{0xF,0xFFE1},		//L0
{0xF,0xFFE3},		//L1
{0xF,0xFFE7},		//L2
{0xF,0xFFEF},		//L3
{0xF,0xFFFF}		//L4
};




void GameFullPower(void)
{
	unsigned int u;
	switch (GameState)
	{
		case 0:
			ResetAudioAndLEDS();			//Turn off everything
			ResetGameVariables();
			AudioNodeEnable(5,0,0,1,0,50,0xFF,0);	// On for 1 sec
			AudioNodeEnable(9,0,0,1,0,50,0xFF,0);	// On for 1 sec
			AudioPlaySound(297,0);		
			GameState+=1;
			GameTimeout2=18000;
		break;
		case 1:
			//4096 = 13.2V so it is 310 counts per volt
			if (GameInteger[2] >= 16)
				GameInteger[3] -= (GameInteger[3]/16);
			if (GameInteger[2] < 16)
				GameInteger[2]+=1;		//Counts entries in the average
			GameInteger[3] += ADCRead();

			u=GameInteger[3]/GameInteger[2];
			GameScores[0] = u/310 ;
			GameScores[1] = ((u % 310) *10) /310;

			//Put some scores up & advance
			LEDSendScores(248,ScoreBrightness,GameScores[0],GameScores[1]);
			//Light a button & advance
			if (GameVariable[1] <= 15)
			{
				LEDSendMessage(GameVariable[1],YELLOW,YELLOW,0,0);
				GameVariable[1] += 1;
				ScoreSendLights(248,0,0);
			}
			else if ( (GameVariable[1]>=16) && (GameVariable[1]<=35) )
			{

			GameInteger[1]=SelectButtonPowerTable[GameVariable[1]-16][0];
			GameInteger[0]=SelectButtonPowerTable[GameVariable[1]-16][1];

			//Game integers keep acculmulating bits for lighted piezo outputs
//				if (GameVariable[1] <32)
//					GameInteger[0] += (1 << (GameVariable[1]-16));
//				else
//					GameInteger[1] += (1 << (GameVariable[1]-32));

				ScoreSendLights(248,GameInteger[0],GameInteger[1]);
				GameVariable[1] +=1;
			}
			GameTimer1=0;
			GameTimeout1=100;
			GameState+=1;
		break;
		case 2:
			GameSwitchCheck();
			MasterSwitchCheck();
			SelectSwitchCheck();
			if (GameTimer1 >= GameTimeout1)
				GameState=1;
			if (GameTimer2 >= GameTimeout2)
			{
				GameState=0;		//Start over at the beginning
				GameSelected=0;		// Go back to selector
			}



		break;
		case 3:
		default:
			
					break;
	}
}
void OnButtonPressFullPower(unsigned char button)
{
	GameState=0;	//Start over
	GameTimer2=0;
}
void OnMasterSwitchPressFullPower(void)
{
	GameState=3;			//Exit
}
void OnSelectPressFullPower(unsigned int sel)
{
	GameTimer2=0;
}
