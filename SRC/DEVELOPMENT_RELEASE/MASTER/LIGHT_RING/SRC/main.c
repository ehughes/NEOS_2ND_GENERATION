/*****************************************************************/
//      Playworld Systems
//
//		Neos All Around
//      Development Release
//
//		-Eli Hughes
//
//
//		Note:  This source was derived from V1.28 of the Original
//			   NEOS code.  All comments about bugs and version changes
//			   have been removed as we are starting from a working copy
//			   This Release will start from version 1.   Version Changes
//			   are control by Subversion.   There will be comments reflecting
//			   changes in the code but the subversion logs should be used instead
//
/*****************************************************************/

#include <p30fxxxx.h>
#include <Timer.h>
#include <stdlib.h>
#include <math.h>
#include <ports.h>
#include "_GameVariables.h"
#include "SPIRoutines.h"
#include "SystemVariables.h"
#include "CANRoutines.h"
#include "Diagnostics.h"
#include "EEPROMRoutines.h"
#include "ADCRoutines.h"
#include "LED-Display.h"
#include "Audio.h"
#include "_GameProcess.h"
#include "SystemUtilities.h"
#include "TimerRoutines.h"
#include "DataEEPROM.h"
#include "_RootButtonHandler.h"
#include "SystemVariables.h"
#include "PacketProcess.h"
#include "GameMapping.h"
#include "ErrorCodes.h"
#include "USB.h"
#include "FLASH.h"
#include "Diagnostics.h"


/* We have 8.192 MHZ xtal with PLL * 8 for 65MHZ internal, 16MIPS */
/* 
 = 16,384,000 */

_FOSC(CSW_FSCM_OFF & XT_PLL8);  /* Set up for XTxPLL *8 mode since */
_FWDT (WDT_ON & WDTPSA_8 & WDTPSB_16);		//Watchdog now ON (3.9 Hz)
_FBORPOR(MCLR_EN & PWRT_OFF);   /* Enable MCLR reset pin and turn off the power-up timers. */
_FGS(GEN_PROT)

#define BOOT_TIMER GPTimer[0]

int main (void)
{

	void (*JumpToResetVector)() = 0x0;
	
	RCONbits.WDTO=0;	//Reset this if wake by watchdog
	PortInit();
	ReadOptionJumpers();

	MyNodeNumber=MASTER_NODE_ADDRESS;
	EERecover();		//Recover eeprom

	 InitSystemVariables();
	ADPCFG =0xFFFF;

	CLEAR_WATCHDOG;

	TimerInit();
	SPIInit();
	CANInit();
	DCIInit();			//Initialize audio Data converter interface 

	InitUSB();
	InitPacketDetect();
	InitPacketProcess();
	InitScoreDisplayerManager();
	ADCInit();
	srand(ADCRead());
	ResetAudioAndLEDS();

	SystemMode = SYSTEM_BOOT;
	
	while (1)
	{	
		CLEAR_WATCHDOG;
		
		switch(SystemMode)
		{
			
			case SYSTEM_BOOT:
			
			if(BOOT_TIMER > 200)
			{
				
				SystemMode = SYSTEM_DIAGNOSTICS;
				DiagnosticsState = INIT;
				
			}
			
			ProcessIncomingUSBMessages();
			
			break;
			
			
			case SYSTEM_DIAGNOSTICS:
				rand();				
				CANTransmitCheck();	
				CANRxProcess();
				AudioStreamCheck();	
				SystemsDiagnostics();
				MasterButtonCheck();
			break;
			
		
			case GAME_ACTIVE:
			
				rand();					//Always call random generator to include time in randomness
				CANTransmitCheck();		//OK See if anything in TX queue can go to CAN Tx registers
				CANRxProcess();		// Check for unprocessed received messages & handle them
				MasterButtonCheck();
				AudioStreamCheck();	
				ScoreDisplayManager();
				ProcessIncomingUSBMessages();
				GameProcess();
			
			break;
				
			case USB_FLASH_UPDATE:
				rand();
				ProcessIncomingUSBMessages();
			break;
			
			case SYSTEM_IDLE:
			default:
				
				rand();
				ProcessIncomingUSBMessages();
				CANTransmitCheck();		//OK See if anything in TX queue can go to CAN Tx registers
				CANRxProcess();		// Check for unprocessed received messages & handle them
			
			break;

			case SYSTEM_SOFTWARE_RESET:
				JumpToResetVector();
			break;
			
		}
	
	}

	return 0;
}




