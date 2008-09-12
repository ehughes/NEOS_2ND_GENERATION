#include "__Game_AddressSetup.h"
#include "_GameVariables.h"
#include "SystemUtilities.h"
#include "SystemVariables.h"
#include "_RootButtonHandler.h"
#include "LED-Display.h"
#include "CANRoutines.h"
#include "Audio.h"

void GameAddressSetup (void)
{
	//GameScores[0] will be the address being defined
	switch (GameState)
	{
		case 0:
			ResetAudioAndLEDS();			//Turn off everything
			ResetGameVariables();
			AudioNodeEnable(5,0,0,1,0,50,0xFF,0);	// On for 1 sec
			AudioNodeEnable(9,0,0,1,0,50,0xFF,0);	// On for 1 sec
			AudioPlaySound(293,0);		
			GameState+=1;
			GameTimeout1=100;			//Flashing time
		break;
		case 1:
			LEDSendScoreAddress(248,ScoreBrightness,GameScores[0],GameScores[0]);
			GameState+=1;
		break;
		case 2:
			//If a node (or nodes!) already has this address, light it
			LEDSendMessage(GameScores[0],RED,RED,100,0);		//Red if already addressed
			//Send out message to grab this address
			CANQueueTxMessage(0xE3,GameScores[0],0,0,0);	//Send message to grab address
			GameTimer1=0;
			GameState+=1;
		break;
		case 3:
			//Flash light appropriately			
			if (GameTimer1 >= GameTimeout1)
			{
				GameTimer1=0;
				LEDSendMessage(GameScores[0],RED,RED,50,0);
			}

			//Wait for somebody to claim this address - Game state will increment
			GameSwitchCheck();
			MasterSwitchCheck();
			SelectSwitchCheck();
		break;

		case 4:
			//Wait for acknowledge beep/flash to end
			if (GameTimer2 >= GameTimeout2)
			{
				if (GameScores[0] >15)
					GameState+=1;
				else
				GameState=2;		//Go back & do another light
			}
		break;

		case 5:
		default:
			CANQueueTxMessage(0xE4,0,0,0,0);	//Make sure they are out of setup!
			GameSelected=0;
			GameState=0;
		break;
	}
}

//When a button press comes back acknowledging address, 
//Flash an acknowledge & move on to the next

void OnButtonPressAddressSetup(unsigned char button)
{
	// DISABLE Address setup
	CANQueueTxMessage(0xE4,0,0,0,0);
	GameScoreAdd(0,1);	//Add 1 to score to go to next button
	LEDSendScoreAddress(248,ScoreBrightness,GameScores[0],GameScores[0]);

	//Light the node GREEN
	LEDSendMessage(button,GREEN,GREEN,50,0);
	//Audio feedback
	AudioNodeEnable(button,2,3,1,0,50,0xFF,0);	// On for 1/10 sec
	AudioPlaySound(423,2);

	GameTimer2=0;
	GameTimeout2=50;
	GameState+=1;
}

//Select button advances without defining an address
void OnSelectPressAddressSetup(unsigned int sel)
{
	CANQueueTxMessage(0xE4,0,0,0,0);
	LEDSendMessage(GameScores[0],LEDOFF,LEDOFF,50,0);		//turn off current button
	GameScoreAdd(0,1);	//Add 1 to score to go to next button
	LEDSendScoreAddress(248,ScoreBrightness,GameScores[0],GameScores[0]);
	GameTimer2=0;
	GameTimeout2=0;
	GameState+=1;
}

//Master button Exits
void OnMasterSwitchPressAddressSetup(void)
{
	GameState=5;
}
