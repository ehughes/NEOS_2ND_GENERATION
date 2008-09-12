#include <timer.h>
#include "TimerRoutines.h"
#include "_GameVariables.h"
#include "SystemVariables.h"
#include "LED-Display.h"
#include <p30fxxxx.h>


unsigned char T210msHeartbeat=0;
unsigned char T2DivideBy45=0;
unsigned char T2PWMCounter=0;

unsigned long InactivityTimer=INACTIVITYTIMEOUT;


void TimerInit (void)
{
/* Want 64 brightness levels * 70 Hz = 4480 ints/sec
16,384,000 / 4480 = 3657.14 instructions between ints */

	CloseTimer2();	
	ConfigIntTimer2 (T2_INT_PRIOR_4 & T2_INT_ON);
	/* Timer 2 is 16 bit*/
	/* 16,384,000 / 1 / 3657 = 4480.175Hz */
	/* that is equiv of being off by 3.3 seconds per day */
	OpenTimer2 (T2_ON & T2_GATE_OFF & T2_IDLE_STOP &
		T2_PS_1_1 & T2_32BIT_MODE_OFF &
		T2_SOURCE_INT, 3657);
	EnableIntT2;
}



void TimerRefresh(void)
{
	int temp;
	if (T210msHeartbeat==1)
	{	
		T210msHeartbeat=0;
		if (GameTimer1 < 0xFFFF)
		  GameTimer1+=1;
		if (GameTimer2 < 0xFFFF)
		  GameTimer2+=1;
		if (GameTimer3 < 0xFFFF)
		  GameTimer3+=1;
		if (GameShowTimer < 0xFFFF)
		  GameShowTimer+=1;

		if (BurnInTimer < 0xFFFF)
			BurnInTimer+=1;


		if (AudioTimer < 0xFFFF)
		  AudioTimer+=1;

		for (temp=0; temp <=15; temp++)
		{
			if (GameNodeTimer[temp]>0)
				GameNodeTimer[temp]-=1;
		}					

		if (InactivityTimer > 0)
			InactivityTimer -= 1;		//Count down inactivity detector

	}
}

/* TIMER 2 INTERRUPT */
//75us duration 223us apart measured 6/21/06 = 34% CPU usage

void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt( void )
{
	if (UART1RxTimer)
	{
		UART1RxTimer--;			//Decrement if nonzero
		if (UART1RxTimer==0)
		{
			UART1RxDataReady=1;		// Set flag on transition from 1 to 0
//			Receiveddata = UART1RxBuffer;  //Reset rx index to beginning
			UART1RxIndex=0;
		}
	}	

//	PORTAbits.RA14=1;  //Temporary interrupt testing
	T2DivideBy45+=1;
	if (T2DivideBy45 >=45)
	{
		T2DivideBy45=0;
		T210msHeartbeat=1;
		T2LED10msRefreshFlag=1;

		if (PiezoTimer > 0)
			PiezoTimer-=1;
	}

	T2PWMCounter+=1;
	T2PWMCounter &= 0x3F;

#ifdef BUTTONDEMO
		if (MyNodeNumber==247)
			LEDEnable[0]=0xFFFF;
#endif


	if (ScoreDisplay ==1)
	{
		LEDOutputData[0]=LEDEnable[0];
		LEDOutputData[1]=LEDEnable[1];
		LEDOutputData[2]=LEDEnable[2];
		LEDOutputData[3]=LEDEnable[3];		//Need all 4 bytes
	
	//	LEDOutputData[0]=0xFFFF;		//force ON
	//	LEDOutputData[1]=0xFFFF;
	//	LEDOutputData[2]=0xFFFF;
	//	LEDOutputData[3]=0xFFFF;		//Need all 4 bytes
	
		if (LEDScoreOffTimer)
		{
			if (LEDScoreFlashEnable[0] & 0x1)
			{
				LEDOutputData[0] &= 0x8080;		// blank out 1st 3 digits
				LEDOutputData[1] &= 0xFF80;
			}
			if (LEDScoreFlashEnable[0] & 0x2)
			{
				LEDOutputData[1] &= 0x80FF;		// blank out last 3 digits
				LEDOutputData[2] &= 0x8080;
			}
	//		if (LEDScoreFlashEnable[0] & 0x100)
	//			LEDOutputData[0] &= 0xFF7F;
	//		if (LEDScoreFlashEnable[0] & 0x200)
	//			LEDOutputData[0] &= 0x7FFF;
	//		if (LEDScoreFlashEnable[1] & 0x400)
	//			LEDOutputData[0] &= 0xFF7F;
	//		if (LEDScoreFlashEnable[1] & 0x800)
	//			LEDOutputData[0] &= 0x7FFF;
	//		LEDOutputData[3] &= (LEDScoreFlashEnable[1]==0);	// zero any leds set here too
		}
	
		if (ScoreBrightness <= T2PWMCounter)	//dim score display
		{		
			LEDOutputData[0] &= 0x8080;
			LEDOutputData[1] &= 0x8080;
			LEDOutputData[2] &= 0x8080;
		}
	}

	
//following used to run for score display AND buttons AND master
//but I don't think it's needed for score
//the PWM probably jusr re-does what was already done
//and I think it would interfere with score flash function which is probably unused
//anyway now i'll try making it an ELSE
	else
	{

#ifdef ELC0019
// Sort LED colors into LED output map 
// Only for button display - score display controls LedOutputData directly

	LEDOutputData[0]=LEDEnable[0];
	LEDOutputData[1]=LEDEnable[1];
	LEDOutputData[2]=LEDEnable[2];
	if (LEDColor[0] <= T2PWMCounter)
	{
		LEDOutputData[0] &= 0b0100100100100100;		//zero the reds
		LEDOutputData[1] &= 0b0010010010010010;
		LEDOutputData[2] &= 0b1111111111111001;		//48 bits total
	}
	if (LEDColor[1] <= T2PWMCounter)
	{
		LEDOutputData[0] &= 0b1011011011011011;		//Zero the greens
		LEDOutputData[1] &= 0b1101101101101101;
		LEDOutputData[2] &= 0b0000000000000110;		// 48 bits total
	}

	//For ELC0025, Bit 0 is red & Bit 1 is green - not in pairs of 2
	if (ELC0025or26 !=0) 
	{
		if ((LEDOutputData[0] & 0x4) == 4)
			LEDOutputData[0] |= 2;		//Green now on bit 1
		else
			LEDOutputData[0] &= 0xFFFD;		//if green =0 clear bit 1
	}
#endif


#ifdef ELC0002
// Sort LED colors into LED output map 
// Only for button display - score display controls LedOutputData directly
	LEDOutputData[0]=LEDEnable[0];
	LEDOutputData[1]=LEDEnable[1];
	LEDOutputData[2]=LEDEnable[2];
	if (LEDColor[0] <= T2PWMCounter)
	{
		LEDOutputData[0] &= 0b0110110110110110;
		LEDOutputData[1] &= 0b1011011011011011;
		LEDOutputData[2] &= 0b0000000000001101;
	}
	if (LEDColor[1] <= T2PWMCounter)
	{
		LEDOutputData[0] &= 0b1101101101101101;
		LEDOutputData[1] &= 0b0110110110110110;
		LEDOutputData[2] &= 0b0000000000001011;
	}
	if (LEDColor[2] <= T2PWMCounter)
	{
		LEDOutputData[0] &= 0b1011011011011011;
		LEDOutputData[1] &= 0b1101101101101101;
		LEDOutputData[2] &= 0b0000000000000110;
	}
#endif
	}

	LEDRefresh();
 	IFS0bits.T2IF = 0;		/* Reset interrupt flag */
//	PORTAbits.RA14=0;  //Temporary interrupt testing
}
