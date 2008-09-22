#ifndef _SYSTEMUTILITIES_H
#define _SYSTEMUTILITIES_H

#include "DataTypes.h"
#include <p30fxxxx.h>

void PortInit(void);
void ReadOptionJumpers(void);

unsigned char Scale100to63(unsigned int displayval);	//Conversions for diagnostic adjustments
unsigned char Scale100to255(unsigned int displayval);
unsigned int Scale255to100(unsigned char inpval);
unsigned int Scale63to100(unsigned char inpval);

unsigned char SelectRandomButton(unsigned char* group);
void ResetAllSlaves(void);

void ResetAudioAndLEDS(void);
void ResetLeds(void);
void ResetGameVariables (void);

BYTE RandomButton(BYTE Exclude1, BYTE Exlude2);

void InitRandomButton();
	
#define NO_EXCLUDE 0xFF

void ResetToGameSelector();


#define RIGHT	0x00
#define LEFT	0x01

BYTE SelectRandomDirection();
void SendNodeNOP();


#define GREEN_LED_LOC	0x02
#define RED_LED_LOC		0x01


#define GREEN_LED_ON	LATD |= GREEN_LED_LOC
#define GREEN_LED_OFF	LATD &= ~GREEN_LED_LOC

#define RED_LED_ON		LATD |= RED_LED_LOC	
#define RED_LED_OFF		LATD &= ~RED_LED_LOC



#endif

