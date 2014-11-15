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
#include "__GameMapping.h"
#include "SystemUtilities.h"
#include "__Game_Shrek.h"

void GameProcess(void)
{
	switch (GameSelected)
	{			
		case GAME_SHREK:
			Shrek();
		break;

		default:
			Shrek();
		break;
	}
}

