#include "DataTypes.h"

#ifndef _TIMER_ROUTINES_H
#define _TIMER_ROUTINES_H

void TimerInit (void);
//void TimerRefresh(void);
void __attribute__((__interrupt__)) _T2Interrupt( void );


extern unsigned char MasterButtonTimer;

extern unsigned char T210msHeartbeat;
extern unsigned char T2DivideBy45;
extern unsigned char T2PWMCounter;
extern unsigned char MasterButtonTimer;
extern unsigned long InactivityTimer;



#define NUM_GP_TIMERS	12
extern volatile WORD GPTimer[NUM_GP_TIMERS];



#endif

