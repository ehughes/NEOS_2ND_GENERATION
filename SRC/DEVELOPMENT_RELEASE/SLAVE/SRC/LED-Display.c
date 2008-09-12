#include <p30fxxxx.h>
#include "LED-Display.h"
#include "SystemVariables.h"
#include "CANRoutines.h"
#include "BoardSupport.h"
#include "TimerRoutines.h"

unsigned char LEDColor[3];	/* R,G,B 0-255 color values */
unsigned char LEDStartColor[3];	/* R,G,B 0-255 color values */
char LEDColorChange[3]; //SIGNED change values for fading colors

unsigned int LEDTimer=0;	//10ms down counter until LED shutoff 10 BITS LONG
unsigned int LEDFadeTimer=0; //10ms UP counter used for fading 10 BITS LONG
unsigned int LEDFadeTime=0;  //Limit for LED 10 BITS LONG



void ResetLED()
{
	LEDColor[0]=0;		//LEDs off
	LEDColor[1]=0;
	LEDColor[2]=0;
	LEDTimer=0;
	LEDFadeTimer=0;
	LEDFadeTime=0;

}	



void LEDTimerCheck(void)
{
	long fadetemp;
	if (T2LED10msRefreshFlag != 0)
	{
		
		if ((LEDFadeTime >0)	&& (LEDFadeTimer < LEDFadeTime))		//See if a fade is in place
		{
			LEDFadeTimer+=1;		//Increment first so that the last time through these two are equal
			fadetemp=(LEDFadeTimer*0x100) /LEDFadeTime;	//shifted up 8 to increase precision
			fadetemp *= LEDColorChange[0];
			fadetemp /= 0x100;
			LEDColor[0] = (LEDStartColor[0] + fadetemp) & 0x3F;

			fadetemp=(LEDFadeTimer*0x100) /LEDFadeTime;	//shifted up 8 to increase precision
			fadetemp *= LEDColorChange[1];
			fadetemp /= 0x100;
			LEDColor[1] = (LEDStartColor[1] + fadetemp) & 0x3F;

			fadetemp=(LEDFadeTimer*0x100) /LEDFadeTime;	//shifted up 8 to increase precision
			fadetemp *= LEDColorChange[2];
			fadetemp /= 0x100;
			LEDColor[2] = (LEDStartColor[2] + fadetemp) & 0x3F;

		}
		if (LEDTimer > 0)			//If timer is nonzero it is running
		{
			LEDTimer-=1;
			if (LEDTimer == 0)		//if timer transitions to 0 turn them all off
			{
				LEDColor[0]=0;
				LEDColor[1]=0;
				LEDColor[2]=0;
			}
		}
		T2LED10msRefreshFlag=0;
	}
}
	


