#include <timer.h>
#include "TimerRoutines.h"
#include "SystemVariables.h"
#include "LED-Display.h"
#include <p30fxxxx.h>
#include "Audio.h"
#include "BoardSupport.h"


unsigned char T210msHeartbeat=0;
unsigned char T2DivideBy45=0;
unsigned char T2PWMCounter=0;
unsigned char T2LED10msRefreshFlag=0;

unsigned long InactivityTimer=INACTIVITYTIMEOUT;

unsigned int ButtonTimer=0;

#define NUM_GP_TIMERS 8

volatile WORD GPTimer[NUM_GP_TIMERS];


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

	if (T210msHeartbeat==1)
	{	
		T210msHeartbeat=0;

	
		if (AudioTimer < 0xFFFF)
		  AudioTimer+=1;
	
		if (InactivityTimer > 0)
			InactivityTimer -= 1;		//Count down inactivity detector

	}
}

/* TIMER 2 INTERRUPT */
//75us duration 223us apart measured 6/21/06 = 34% CPU usage

void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt( void )
{
BYTE i;

	T2DivideBy45+=1;
	if (T2DivideBy45 >=45)
	{
		T2DivideBy45=0;
		T210msHeartbeat=1;
		T2LED10msRefreshFlag=1;
	
		for(i=0;i<NUM_GP_TIMERS;i++)
		{
			if(GPTimer[i]<0xFFFF)
				GPTimer[i]++;	
		}
		
		if (ButtonTimer > 0)
			ButtonTimer-=1;
	}


if(LEDOverride == FALSE)
{
	T2PWMCounter+=1;
	T2PWMCounter &= 0x3F;


	if (LEDColor[0] <= T2PWMCounter)
	{
		LATD &= ~RED_LED_LOCATION;
	}
	else
	{
		LATD |= RED_LED_LOCATION;
	}

	if (LEDColor[1] <= T2PWMCounter)
	{
		LATD &= ~GREEN_LED_LOCATION;
	}
	else
	{
		LATD |= GREEN_LED_LOCATION;
	}
}

 	IFS0bits.T2IF = 0;		/* Reset interrupt flag */

}
