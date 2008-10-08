#include "DataTypes.h"

#ifndef _SYSTEMVARIABLES_H
#define _SYSTEMVARIABLES_H

#define INACTIVITYTIMEOUT 6000;	//1 minute

#define	FCY		16384000		/* Was 29491200 */
#define	FS		16000			/* Was 48000 */
#define	FCSCK		64*FS		/* 32 bits left & 32 right for 64 total */

#define MAX_FLASH_RETRIES 8

#define GLOBAL_INIT_TIME 5
extern BYTE OptionJumpers;
extern BYTE SystemMode;
extern BOOL InhibitAudio;
#define MASTER_NODE_ADDRESS ((BYTE)(247))

//Main Processing loop States
#define GAME_ACTIVE 				0x00
#define USB_FLASH_UPDATE			0x01
#define SYSTEM_IDLE					0x02
#define SYSTEM_SOFTWARE_RESET		0x03
#define SYSTEM_DIAGNOSTICS			0x04
#define SYSTEM_BOOT					0xFE


#define GP_BUFFER_SIZE	512
extern BYTE GeneralPurposeBuffer[GP_BUFFER_SIZE];
extern BOOL GeneralPurposeBufferAcceptanceFlag;
extern DWORD MyUID;



#define DIAGNOSTIC_INIT	0x00
#define ALL_NODES		0xFF


#define CLEAR_WATCHDOG 	asm("clrwdt");



void InitSystemVariables();

#endif




