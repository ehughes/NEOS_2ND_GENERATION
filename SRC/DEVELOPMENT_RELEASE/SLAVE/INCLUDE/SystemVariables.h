#include "DataTypes.h"

#ifndef _DATA_TYPES_H
#define _DATA_TYPES_H


#define INACTIVITYTIMEOUT 6000
#define BUTTON_TIME 10

extern unsigned char MyNodeNumber;	
extern unsigned char OptionJumpers;
extern unsigned char ScoreDisplay;
extern int DiagTentativeAddress;
extern unsigned char DiagAddressSetup;

#define GP_BUFFER_SIZE	512
extern BYTE GeneralPurposeBuffer[GP_BUFFER_SIZE];
extern BOOL GeneralPurposeBufferAcceptanceFlag;
extern DWORD MyUID;


#define CLEAR_WATCHDOG asm("clrwdt")

extern BOOL LEDOverride;

#endif

