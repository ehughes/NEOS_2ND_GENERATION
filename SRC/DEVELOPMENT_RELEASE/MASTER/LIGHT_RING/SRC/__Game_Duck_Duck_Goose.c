#include "SystemVariables.h"
#include "_GameVariables.h"
#include "_RootButtonHandler.h"
#include "_GameProcess.h"
#include <stdlib.h>
#include "LED-Display.h"
#include "SystemUtilities.h"
#include "Audio.h"
#include "CanRoutines.h"
#include "DataTypes.h"
#include "SoundMapping.h"
#include "GameMapping.h"
#include "InternalButtonSoundMapping.h"
#include "TimerRoutines.h"
#include "config.h"

//*************************************************
//*******Game Parameters***************************
//*************************************************

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define	BACKGROUND_MUSIC_STREAM 3


//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

//*************************************************
//******** System Timer MACROS*********************
//*************************************************

//*************************************************
//*******Game Variables****************************
//*************************************************

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************


void DuckDuckGoose(void)
{
			
	switch (GameState)
	{
		
		case INIT:
				
		//	ResetToGameSelector();					
		
			GPTimer[0] = 0;
			GameState = 2;
		break;
		
		case 1:
		if(GPTimer[0] >200)
		{
			LEDSendMessage(0xFF,GREEN,GREEN,0,0);
			GameState = 2;	
			GPTimer[0] = 0;	
		}
		
		break;
		
			case 2:
		if(GPTimer[0] >200)
		{
			LEDSendMessage(0xFF,RED,RED,0,0);
			GameState = 1;	
			GPTimer[0] = 0;	
		}
		
		break;
	
		default:
			
			GameState = INIT;
		
		break;
			
	}
}




void OnButtonPressDuckDuckGoose(unsigned char button)
{

}

