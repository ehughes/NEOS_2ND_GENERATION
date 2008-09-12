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
				
			ResetToGameSelector();					
		
		break;
		
	
		default:
			
			GameState = INIT;
		
		break;
			
	}
}




void OnButtonPressDuckDuckGoose(unsigned char button)
{

}

