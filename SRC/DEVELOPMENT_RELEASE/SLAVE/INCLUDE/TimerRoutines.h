#include "Datatypes.h"

void TimerInit (void);
void TimerRefresh(void);
void __attribute__((__interrupt__)) _T2Interrupt( void );


extern unsigned char T210msHeartbeat;
extern unsigned char T2DivideBy45;
extern unsigned char T2PWMCounter;
extern unsigned long InactivityTimer;
extern unsigned int ButtonTimer;
extern unsigned char T2LED10msRefreshFlag;




#define NUM_GP_TIMERS 8

extern volatile WORD GPTimer[NUM_GP_TIMERS];
