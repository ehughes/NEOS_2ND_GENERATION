#include "DataTypes.h"

#ifndef _SYSTEM_VARIABLES_H
#define _SYSTEM_VARIABLES_H

#define INACTIVITYTIMEOUT 6000
#define BUTTON_TIME 10

extern BYTE MyNodeNumber;	
extern SIGNED_WORD DiagTentativeAddress;
extern BYTE DiagAddressSetup;

#define GP_BUFFER_SIZE	512
extern BYTE GeneralPurposeBuffer[GP_BUFFER_SIZE];
extern BOOL GeneralPurposeBufferAcceptanceFlag;
extern DWORD MyUID;

#define CLEAR_WATCHDOG asm("clrwdt")

extern BOOL LEDOverride;

#endif

