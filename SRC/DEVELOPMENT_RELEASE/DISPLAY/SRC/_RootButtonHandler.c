#include "SystemVariables.h"
#include "_GameVariables.h"
#include "_RootButtonHandler.h"
#include "__Game_RootGame0.h"
#include "__Game_AddressSetup.h"
#include "__Game_Diagnostic.h"
#include "__Game_FullPower.h"

void OnButtonPress(unsigned char button)
{
	switch(GameSelected)
	{
		case GAME_ROOT_GAME0:
			OnButtonPressRootGame0(button);
			break;

		case GAME_BUTTON_ADDRESS:
			OnButtonPressAddressSetup(button);
		break;
		case GAME_DIAGNOSTIC:
			OnButtonPressDiagnostic(button);
		break;
		case GAME_FULL_POWER:
			OnButtonPressFullPower(button);
		break;
	
		default:
			
			break;

	}  
}


void OnSelectPress(unsigned int button)
{
	
	switch(GameSelected)
	{
		case GAME_ROOT_GAME0:
			OnSelectPressRootGame0(button);
			break;

		case GAME_BUTTON_ADDRESS:
			OnSelectPressAddressSetup(button);
		break;
		case GAME_DIAGNOSTIC:
			OnSelectPressDiagnostic(button);
		break;
		case GAME_FULL_POWER:
			OnSelectPressFullPower(button);
		break;
	
		default:
			
			break;

	}  
}


void OnMasterSwitchPress(void)
{
	switch(GameSelected)
	{
		case GAME_ROOT_GAME0:
	    	OnMasterSwitchPressRootGame0();
			break;
		case GAME_BUTTON_ADDRESS:
			OnMasterSwitchPressAddressSetup();
		break;
		case GAME_DIAGNOSTIC:
			OnMasterSwitchPressAddressSetup();
		break;
		case GAME_FULL_POWER:
			OnMasterSwitchPressFullPower();
		break;
		
	}
}





//Low Level routine ******************************
//************************************************




void OnButtonRelease(unsigned char button)
{
	switch(GameSelected)
	{
		default:
			OnButtonReleaseNull(button);
			break;
	}
}

void OnButtonReleaseNull(unsigned char button)
{
}



void MasterSwitchCheck(void)
{
	if (MasterSwitchState != MasterSwitchStateLast)
	{
		if (MasterSwitchState==0) //If state currently off		
		{
			MasterSwitchStateLast=0;
		}
		else
		{
			MasterSwitchStateLast=1;
			OnMasterSwitchPress();
		}
	}
}


void GameSwitchCheck(void)
{
	unsigned int temp,temp2,mask;

	for (temp=0; temp <=1; temp+=1)
	{
		mask=0x0001;

		for (temp2=0; temp2<=15; temp2+=1)
		{
			if ( (NodeSwitchStates[temp] & mask) != (NodeSwitchStatesLast[temp] & mask) )
			{
				if ((NodeSwitchStates[temp] & mask)==0) //If state currently off		
				{
					NodeSwitchStatesLast[temp] &= (mask ^ 0xFFFF);	// Zero out a bit
					OnButtonRelease(temp*16+temp2);
				}
				else
				{
					NodeSwitchStatesLast[temp] |= mask;	// Set a bit
					OnButtonPress(temp*16+temp2);
				}
			}
			mask = mask <<1;
		}
	}
}



void SelectSwitchCheck(void)
{
	unsigned int temp,temp2,mask;
	for (temp=0; temp <=2; temp+=1)
	{
		mask=0x0001;
		for (temp2=0; temp2<=7; temp2+=1)
		{
//			if ( (Piezos[temp] & mask) != (PiezosLast[temp] & mask) )
//			{
				if ((Piezos[temp] & mask) > 0) //If state currently on
				{
//					PiezosLast[temp] |= mask;
					Piezos[temp] &= (mask ^ 0xFF);	//zero this switch because it was processed
					OnSelectPress(temp*8+temp2);
					//GameTimeout2 = temp*8+temp2;	//  bad -temporary
				}
//				else
//				{
//					PiezosLast[temp] &= (mask ^ 0xFF);	// Zero out a bit
// (really the score display doesn't send a release message)
//					OnSelectRelease(temp*8+temp2);
//				}
//			}
			mask = mask <<1;
		}
	}
}





