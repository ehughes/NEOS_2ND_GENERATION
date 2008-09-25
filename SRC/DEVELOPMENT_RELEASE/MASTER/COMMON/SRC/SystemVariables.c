#include "SystemVariables.h"
#include "DataTypes.h"

unsigned char OptionJumpers=0;
unsigned char SystemMode = GAME_ACTIVE;

BYTE GeneralPurposeBuffer[GP_BUFFER_SIZE];
BOOL GeneralPurposeBufferAcceptanceFlag = FALSE;
DWORD MyUID;

BOOL InhibitAudio = FALSE;

void InitSystemVariables()
{

SystemMode = GAME_ACTIVE;
	
}	


