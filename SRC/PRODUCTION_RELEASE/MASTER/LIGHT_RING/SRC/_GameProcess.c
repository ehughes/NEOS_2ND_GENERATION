#include "SystemVariables.h"
#include "_GameVariables.h"
#include "_RootButtonHandler.h"
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
		
		case GAME_JAM_CIRCLE:
			JamCircle();
		break;
		
		case GAME_TEATHERBALL:
			TeatherBall();
		break;
		
		case GAME_DUCKDUCKGOOSE:
			DuckDuckGoose();
		break;
		
		
		case GAME_FLIP_FLOP:
			FlipFlop();
		break;
		default:
			ResetToGameSelector();
		break;
	}
}

