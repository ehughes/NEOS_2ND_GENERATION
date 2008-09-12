#include "SystemVariables.h"
#include "DataTypes.h"

unsigned char MyNodeNumber;	
unsigned char OptionJumpers;
unsigned char ScoreDisplay;
int DiagTentativeAddress;
unsigned char DiagAddressSetup;

BOOL LEDOverride =  FALSE;


BYTE GeneralPurposeBuffer[GP_BUFFER_SIZE];
BOOL GeneralPurposeBufferAcceptanceFlag = FALSE;

DWORD MyUID;
