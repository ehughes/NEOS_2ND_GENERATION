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
//			   changes in teh code but the subversion logs should be used instead
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
#include "UARTRoutines.h"
#include "EEPROMRoutines.h"
#include "ADCRoutines.h"
#include "LED-Display.h"
#include "Audio.h"
#include "_GameProcess.h"
#include "SystemUtilities.h"
#include "TimerRoutines.h"
#include "DataEEPROM.h"



/* We have 8.192 MHZ xtal with PLL * 8 for 65MHZ internal, 16MIPS */
/* 
 = 16,384,000 */


_FOSC(CSW_FSCM_OFF & XT_PLL8);  /* Set up for XTxPLL *8 mode since */
_FWDT (WDT_ON & WDTPSA_8 & WDTPSB_16);		//Watchdog now ON (3.9 Hz)
_FBORPOR(MCLR_EN & PWRT_OFF);   /* Enable MCLR reset pin and turn off the power-up timers. */

#ifdef __30F6014A_H
_FGS(GEN_PROT)
#endif


/* Define constants here                            */
#define	FCY		16384000		/* Was 29491200 */
#define	FS		16000			/* Was 48000 */
#define	FCSCK		64*FS		/* 32 bits left & 32 right for 64 total */
#define	LOOPBACKMODE	0



unsigned char Slave;
unsigned char OptionJumpers;
unsigned char ScoreDisplay=0;
unsigned char ELC0025or26=1;	//Set to 1 if 3rd generation board detected

unsigned int BurnInTimer=0;
unsigned char BurnIn=0;

/************* START OF MAIN FUNCTION ***************/

int main (void)
{
	RCONbits.WDTO=0;	//Reset this if wake by watchdog
	int temp = 0;

	PortInit();
	ReadOptionJumpers();

	if (ScoreDisplay==1)		//Use score display jumper as ID not option jumpers
	{
		Slave=1;
		MyNodeNumber=248;
	}
	else if (OptionJumpers == 0xC)	//Master board
	{
		Slave=0;
		MyNodeNumber=247;
		EERecover();		//Recover eeprom
	}
	else			//Slave board, jumpers = 0x4
	{
		Slave=1;
		MyNodeNumber=0;
		ReadEE (0x7F,0xFC00,&temp,1);	//7FFC00 is good in both 5013 & 6014A
		if ( (temp>=0) && (temp<=15) )
			MyNodeNumber=temp;
	}	


	MyAudioStream=0;




#if __dsPIC30F4013__
	ADPCFG |= 0x0F00;
#endif	
	ADPCFG =0xFFFF;

	if (ScoreDisplay != 1)			//All enabled for buttons
	{
		LEDEnable[0] = 0xFFFF;
		LEDEnable[1] = 0xFFFF;
		LEDEnable[2] = 0xFFFF;
	}

	asm("clrwdt");

	CN16Init();
	TimerInit();
	SPIInit();

#ifndef BUTTONDEMO
	CANInit();
	DCIInit();			//Initialize audio Data converter interface 
	UARTInit();			//initialize serial uarts
#endif
	
	ADCInit();

	srand(5647);



	if (Slave==0)		
	{	
		//Maybe doing an 8 byte read here will get the SPI synced so the first read isn't
		//garbage
		SPISendInstruction(0x33 , 0 );		//8 Byte read

		SPISendInstruction(0x03,0);		//256 byte read



//		FillFlashWithTones();
//		AudioPlaySound(1,0); 
//		LEDSendMessage(0,63,0,0,0,63,0,0,200);
//		AudioEnable(0,1,0xFF,0);
//		AudioEnable(1,1,0xFF,0);
//		AudioEnable(2,1,0xFF,0);
	}

//		for (temp=0; temp<256; temp++)
//		{
//			SPITxBuffer[temp] = temp;
//		}
//
//
//		SPISectorErase(0x10000);
///		SPIWritePage(0x10000);
//		SPISendInstruction(0x33 , 0x10000 );		//8 Byte read
//
//		for (temp=0; temp<256; temp++)
//		{
//			SPITxBuffer[temp] = temp/2;
//		}

//		SPISectorErase(0x20000);
//		SPIWritePage(0x20000);
		SPISendInstruction(0x3 , 0x10000 );		//8 Byte read
//
		SPISendInstruction(0x3 , 0x0000 );		//8 Byte read
//
		SPISendInstruction(0x3 , 0x10000 );		//8 Byte read

/* MOVING DCI INIT BEFORE FILLING FLASH BREAKS IT! */
	GameTimer1 = 0;
	GameTimeout1 = 100;		
	GameState=INIT;			
	GameSelected = GAME_ROOT_GAME0;
	
	while (1)
	{	
		asm("clrwdt");
	//Different processing for master & Slave 
		if (Slave != 0)
		{
		}
		else
		{
			rand();					//Always call random generator to include time in randomness
			GameProcess();
			AudioStreamCheck();		// look for time to transmit more audio	
			LEDMaster();
		}

		if ((BurnIn==1) && (ScoreDisplay==1) )
		{
			ScoreBrightness=0x3F;
			LEDScoreFlashEnable[0]=0;
			LEDScoreFlashEnable[1]=0;
	
			if (BurnInTimer>100)
			{
				LEDEnable[0] = 0xFFFF;
				LEDEnable[1] = 0xFFFF;
				LEDEnable[2] = 0xFFFF;		//All lights on
				LEDEnable[3] = 0xFFFF;		//All lights on
			}
			else
			{
				LEDEnable[0]=0;
				LEDEnable[1]=0;
				LEDEnable[2]=0;
				LEDEnable[3]=0;
			}
	
	
			if (BurnInTimer > 200)
				BurnInTimer=0;
		}



		CANTransmitCheck();		//OK See if anything in TX queue can go to CAN Tx registers
		CANRxProcess();		// Check for unprocessed received messages & handle them
		UART1RxProcess();
		UART1ReplyCheck();
		LEDTimerCheck();		//OK on its own
		RegulatorControl();		//OK
		Piezo();
		TimerRefresh();
		AudioTimeoutCheck();
	}

	return 0;
}






