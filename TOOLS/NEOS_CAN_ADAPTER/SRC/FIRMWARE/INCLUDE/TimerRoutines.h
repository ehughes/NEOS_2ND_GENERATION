#include "DataTypes.h"

#ifndef _TIMERROUTINES_H
#define _TIMERROUTINES_H

void TimerInit (void);
void __attribute__((__interrupt__)) _T2Interrupt( void );

extern WORD GPTimer0;

#endif

