#include "SystemVariables.h"
#include "DataTypes.h"

BYTE SystemMode = GAME_ACTIVE;
BYTE GeneralPurposeBuffer[GP_BUFFER_SIZE];
BOOL GeneralPurposeBufferAcceptanceFlag = FALSE;
BOOL InhibitAudio = FALSE;

void InitSystemVariables()
{
	SystemMode = GAME_ACTIVE;
}	


