/*****************************************************************/
//      Playworld Systems
//
//		Neos All Around
//      Development Release
//		Button Firmware
//		-Eli Hughes
//
//
//		Note:  This source was derived from V1.28 of the Original
//			   NEOS code.  All comments about bugs and version changes
//			   have been removed as we are starting from a working copy
//			   This Release will start from version 1.   Version Changes
//			   are control by Subversion.   There will be comments reflecting
//			   changes in teh code but the subversion logs should be used instead
//
//
//			   The All Around version of the NEOS will have the Button/Master/Display functionality segregated.	
//			   Each will have seperate firmware to simply code and make room for future enhancements.
/*****************************************************************/



#include <p30fxxxx.h>
#include <Timer.h>
#include <stdlib.h>
#include <math.h>
#include <ports.h>

#include "CANRoutines.h"
#include "EEPROM_Access.h"
#include "LED-Display.h"
#include "Audio.h"
#include "TimerRoutines.h"
#include "Button.h"
#include "BoardSupport.h"
#include "SystemVariables.h"
#include "InternalSounds.h"
#include "ADCRoutines.h"

/* We have 8.192 MHZ xtal with PLL * 8 for 65MHZ internal, 16MIPS */
/* 
 = 16,384,000 */


//Device Configuration Bits
_FOSC(CSW_FSCM_OFF & XT_PLL8);  /* Set up for XTxPLL *8 mode since */
//_FWDT (WDT_ON & WDTPSA_8 & WDTPSB_16);		//Watchdog now ON (3.9 Hz)
_FBORPOR(MCLR_EN & PWRT_OFF);   /* Enable MCLR reset pin and turn off the power-up timers. */
_FGS(GEN_PROT)


//7497

/* Define constants here                            */
#define	FCY		16384000		/* Was 29491200 */
#define	FS		16000			/* Was 48000 */
#define	FCSCK		64*FS		/* 32 bits left & 32 right for 64 total */
#define	LOOPBACKMODE	0


int main (void)
{
 int temp = 0;

	RCONbits.WDTO=0;	//Reset this if wake by watchdog
	PortInit();
	ReadOptionJumpers();

	//Read in Node Number

		MyNodeNumber=0;
		ReadEE (0x7F,0xFC00,&temp,1);	//7FFC00 is good in both 5013 & 6014A
		if ( (temp>=0) && (temp<=15) )
			MyNodeNumber=temp;


	ADCInit();
	TimerInit();
	CANInit();
	DCIInit();			//Initialize audio Data converter interface 
	ResetAll();  //Reset all of our background functions. I.E. Can buffers, audio, etc.
	srand(ADCRead());
   

	while (1)
	{	
		CLEAR_WATCHDOG;
		rand();
		CANTransmitCheck();	 	//OK See if anything in TX queue can go to CAN Tx registers
		CANRxProcess();		   // Check for unprocessed received messages & handle them
       	LEDTimerCheck();		//Perform LED commands/Fading
		RegulatorControl();		//Shut down High power stuff if inactive
		ButtonCheck();			//Check the status of the Button
		TimerRefresh();			//Update Timer Flag
		AudioTimeoutCheck();    //Handle Audio Timeout Function
	
	}

	return 0;
}






