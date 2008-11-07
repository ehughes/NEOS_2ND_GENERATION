#include "DataTypes.h"
#include <p30fxxxx.h>

#ifndef _SYSTEMUTILITIES_H
#define _SYSTEMUTILITIES_H

#define NO_EXCLUDE  0xFF
#define RIGHT		0x00
#define LEFT	    0x01

void ResetAllSlaves(void);
void ResetAudioAndLEDS(void);
void ResetLeds(void);
void ResetGameVariables (void);
BYTE RandomButton(BYTE Exclude1, BYTE Exclude2, BYTE Exclude3);
BYTE RandomButton4Exclude(BYTE Exclude1, BYTE Exclude2, BYTE Exclude3,BYTE Exclude4);
void InitRandomButton();
void ResetToGameSelector();
BYTE SelectRandomDirection();
void SendNodeNOP();

#endif

