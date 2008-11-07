#include "DataTypes.h"

#ifndef _TIMER_ROUTINES_H
#define _TIMER_ROUTINES_H

#define NUM_GP_TIMERS	12

void TimerInit (void);
void __attribute__((__interrupt__)) _T2Interrupt( void );

extern BYTE MasterButtonTimer;
extern BYTE T210msHeartbeat;
extern BYTE T2DivideBy45;
extern BYTE T2PWMCounter;
extern BYTE MasterButtonTimer;
extern DWORD InactivityTimer;
extern volatile WORD GPTimer[NUM_GP_TIMERS];

#endif

