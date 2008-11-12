#include "DataTypes.h"



#ifndef _LED_DISPLAY_H
#define _LED_DISPLAY_H

void LEDTimerCheck(void);
void ResetLED();

extern WORD LEDTimer;	//10ms down counter until LED shutoff 10 BITS LONG
extern WORD LEDFadeTimer; //10ms UP counter used for fading 10 BITS LONG
extern WORD LEDFadeTime;  //Limit for LED 10 BITS LONG

extern BYTE LEDColor[3];	/* R,G,B 0-255 color values */
extern BYTE LEDStartColor[3];	/* R,G,B 0-255 color values */
extern SIGNED_BYTE LEDColorChange[3]; //SIGNED change values for fading colors

#endif

