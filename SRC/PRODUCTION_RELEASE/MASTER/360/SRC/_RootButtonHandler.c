#include <p30fxxxx.h>
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "_RootButtonHandler.h"
#include "__Game_RootGame0.h"
#include "__Game_LightGrabber.h"
#include "__Game_FireFighter.h"
#include "__Game_Marathon.h"
#include "__Game_Ninja.h"
#include "__Game_Rodeo.h"
#include "__Game_Zig_Zag.h"
#include "__Game_SurroundSound.h"
#include "__Game_Double_Dots.h"
#include "GameMapping.h"
#include "BoardSupport.h"
#include "TimerRoutines.h"
#include "SystemUtilities.h"
#include "Diagnostics.h"

BYTE ScoreBoardButtons[3];	// status of piezo bits
BYTE ScoreBoardButtonsLast[3];	//Previous state
BYTE MasterButtonLastState=0;


void OnButtonPress(BYTE button)
{
	
	switch(SystemMode)
	{
		case SYSTEM_DIAGNOSTICS:
			OnButtonPressDiagnosticsMode(button);
		break;
		
		default:
		case GAME_ACTIVE:
			switch(GameSelected)
			{
				case GAME_ROOT_GAME0:
					OnButtonPressRootGame0(button);
				break;
		
				case GAME_LIGHT_GRABBER:
					OnButtonPressLightGrabber(button);
				break;
				
				case GAME_DOUBLE_DOTS:
					OnButtonPressDoubleDots(button);
				break;
				
				case GAME_SURROUND_SOUND:
					OnButtonPressSurroundSound(button);
				break;
				
				case GAME_RODEO:
					OnButtonPressRodeo(button);
				break;
				
				case GAME_MARATHON:
					OnButtonPressMarathon(button);
				break;
				
				case GAME_ZIGZAG:
					OnButtonPressZigZag(button);
				break;
				
				case GAME_NINJA:
					OnButtonPressNinja(button);
				break;
				
				case GAME_FIRE_FIGHTER:
					OnButtonPressFireFighter(button);
				break;
				
				default:
				break;
			}  
		break;
	}
}


void OnSelectPress(WORD button)
{
	
	switch(SystemMode)
	{
		case SYSTEM_DIAGNOSTICS:
			OnSelectPressDiagnosticsMode(button);
		break;
	
		default:
		case GAME_ACTIVE:	
			switch(GameSelected)
			{
				case GAME_ROOT_GAME0:
					OnSelectPressRootGame0(button);
				break;
			
				default:
				break;
			}  
		break;	
	
	}
}


void OnMasterSwitchPress(void)
{
	
	switch(SystemMode)
	{
		case SYSTEM_DIAGNOSTICS:
			DiagnosticsState = INIT;
		break;
		
		default:
		case GAME_ACTIVE:
			switch(GameSelected)
			{
				case GAME_ROOT_GAME0:
			   		 EnterAddressingMode();
				break;
				
				default:
					GameState = BOOT;
					ResetToGameSelector();
				break;
				
			}
		break;
	}
}


void MasterButtonCheck(void)
{
	WORD MasterButtonState;

	MasterButtonState = PORTG & BUTTON_LOCATION;
	
	if ((MasterButtonTimer==0) && (MasterButtonState) && (MasterButtonLastState))
	{
		MasterButtonLastState=0;
	}

	if (!MasterButtonState)
	{
		MasterButtonTimer=BUTTON_TIME;		
		if (!MasterButtonLastState)
		{

				OnMasterSwitchPress();
				MasterButtonLastState=1;			
		}
	}	

}





void SelectSwitchCheck(void)
{
	WORD temp,temp2,mask;
	for (temp=0; temp <=2; temp+=1)
	{
		mask=0x0001;
		for (temp2=0; temp2<=7; temp2+=1)
		{
				if ((ScoreBoardButtons[temp] & mask) > 0) //If state currently on
				{
					ScoreBoardButtons[temp] &= (mask ^ 0xFF);	//zero this switch because it was processed
					OnSelectPress(temp*8+temp2);
				}
			mask = mask <<1;
		}
	}
}





