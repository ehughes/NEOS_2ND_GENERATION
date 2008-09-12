#include <timer.h>
#include "TimerRoutines.h"
#include "_GameVariables.h"
#include "SystemVariables.h"
#include "LED-Display.h"
#include <p30fxxxx.h>



unsigned char T210msHeartbeat=0;
unsigned char T2DivideBy45=0;
unsigned char T2PWMCounter=0;
unsigned char MasterButtonTimer=0;
unsigned long InactivityTimer=INACTIVITYTIMEOUT;

volatile WORD GPTimer[NUM_GP_TIMERS];


void TimerInit (void)
{
	
	BYTE i;
	
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
	
		for(i=0;i<NUM_GP_TIMERS;i++)
		{
			if (GPTimer[i] < 0xFFFF)
		 		 GPTimer[i]=0;
		}
}



void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt( void )
{
	BYTE i;
	
	T2DivideBy45+=1;
	if (T2DivideBy45 >=45)
	{
		T2DivideBy45=0;
//		T210msHeartbeat=1;
//		T210msHeartbeat=0;

		for(i=0;i<NUM_GP_TIMERS;i++)
		{
			if (GPTimer[i] < 0xFFFF)
		 		 GPTimer[i]++;
		}
	  
	  	if (P1ScoreUpdateTimer < 0xFFFF)
		  P1ScoreUpdateTimer+=1;
	  	if (P2ScoreUpdateTimer < 0xFFFF)
		  P2ScoreUpdateTimer+=1;
		  
		if (ScoreManagerTimer < 0xFFFF)
		  ScoreManagerTimer+=1;  
	  
	  
		if (InactivityTimer > 0)
			InactivityTimer -= 1;		//Count down inactivity detector



		if(	MasterButtonTimer>0)
			MasterButtonTimer-=1;
			
	}

	T2PWMCounter+=1;
	T2PWMCounter &= 0x3F;

 	IFS0bits.T2IF = 0;		/* Reset interrupt flag */
}
