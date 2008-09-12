#include "SystemVariables.h"
#include "_GameVariables.h"
#include "_RootButtonHandler.h"
#include "__Game_RootGame0.h"
#include "__Game_AddressSetup.h"
#include "__Game_Diagnostic.h"
#include "__Game_FullPower.h"

void GameProcess(void)
{
	switch (GameSelected)
	{
		case GAME_ROOT_GAME0 :
			Root_Game0();
			break;
		case GAME_BUTTON_ADDRESS:
			GameAddressSetup();
			break;
		case GAME_DIAGNOSTIC:
			GameDiagnostic();
			break;
		case GAME_FULL_POWER:
			GameFullPower();
			break;
	

		default:
		
		break;
	}
}

