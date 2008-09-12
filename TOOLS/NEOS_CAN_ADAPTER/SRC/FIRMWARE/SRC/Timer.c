#include <timer.h>
#include "TimerRoutines.h"
#include <p30fxxxx.h>

#include "BoardSupport.h"

BYTE T2Ticks=0;
BYTE LEDOffTicks = 0;
BYTE OneSecondTick=0;

WORD GPTimer0;


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




/* TIMER 2 INTERRUPT */


void __attribute__((__interrupt__,__auto_psv__)) _T2Interrupt( void )
{

	LEDOffTicks++;
	if(LEDOffTicks > 20)
	{
		LEDOffTicks = 0;
		TX_LED_OFF;
		RX_LED_OFF;
	}
		
	T2Ticks+=1;
	if (T2Ticks >=45)
	{
		T2Ticks=0;
		GPTimer0++;
	//10mSec update Rate
	OneSecondTick++;
	if(OneSecondTick>25)
	{
		OneSecondTick=0;
		
	}
	}

 	IFS0bits.T2IF = 0;		/* Reset interrupt flag */

}
