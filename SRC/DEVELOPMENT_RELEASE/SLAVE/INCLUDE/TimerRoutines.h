#include "Datatypes.h"

void TimerInit (void);
void TimerRefresh(void);
void __attribute__((__interrupt__)) _T2Interrupt( void );


extern BYTE T210msHeartbeat;
extern BYTE T2DivideBy45;
extern BYTE T2PWMCounter;
extern DWORD InactivityTimer;
extern WORD ButtonTimer;
extern BYTE T2LED10msRefreshFlag;

#define NUM_GP_TIMERS 8

extern volatile WORD GPTimer[NUM_GP_TIMERS];
