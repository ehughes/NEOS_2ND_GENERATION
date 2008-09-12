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
#include "SystemUtilities.h"

void GameProcess(void)
{
	switch (GameSelected)
	{
		case GAME_ROOT_GAME0:
			Root_Game0();
		break;
			
		case GAME_LIGHT_GRABBER:
			LightGrabber();
		break;
		
		case GAME_DOUBLE_DOTS:
			DoubleDots();
		break;
		
		case GAME_SURROUND_SOUND:
			SurroundSound();
		break;
		
		case GAME_RODEO:
			Rodeo();
		break;
		
		case GAME_MARATHON:
			Marathon();
		break;
		
		case GAME_ZIGZAG:
			ZigZag();
		break;
		
		case GAME_NINJA:
			Ninja();
		break;
		
		case GAME_FIRE_FIGHTER:
			FireFighter();
		break;
			
		default:
			ResetToGameSelector();
		break;
	}
}

