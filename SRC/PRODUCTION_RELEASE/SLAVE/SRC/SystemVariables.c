#include "SystemVariables.h"
#include "DataTypes.h"

BYTE MyNodeNumber;	

SIGNED_WORD DiagTentativeAddress;
BYTE DiagAddressSetup;

BOOL LEDOverride =  FALSE;

BYTE GeneralPurposeBuffer[GP_BUFFER_SIZE];
BOOL GeneralPurposeBufferAcceptanceFlag = FALSE;

DWORD MyUID;
