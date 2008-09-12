#include "EEPROMRoutines.h"
#include "_GameVariables.h"
#include "SystemVariables.h"
#include "DataEEPROM.h"
#include "SystemUtilities.h"



unsigned int EEDefaults[8] = {0,255,63,0,0,0,0,0};
unsigned int EELowLimits[8] = {0,12,3,0,0,0,0,0};
unsigned int EEHighLimits[8] = {255,255,63,255,255,255,255,255};

//EEPROM VARIABLES
unsigned char MyNodeNumber;		//0-31 number assigned to this node for gameplay
unsigned char AudioGlobalVolume=GLOBALVOLUMEINIT;
unsigned char AudioBackgroundVolume=BACKGROUNDVOLUMEINIT;
unsigned char NoAnnoy=0;			//When =1 it is silent in idle mode & no snoring
unsigned char RedBrightness=63;
unsigned char GreenBrightness=63;
unsigned char ScoreBrightness=63;



void EEStoreVariable(unsigned int index,  int value)
{
	EraseEE(0x7F,0xFC00+(index*2),1);
	WriteEE(&value,0x7F,0xFC00+(index*2),1);
}


int EEReadVariable(unsigned int index)
{
	int value;
	ReadEE(0x7F,0xFC00+(index*2),&value,1);
	return value;
}


void EERecover(void)
{
	AudioGlobalVolume=EERecoverVariable(1);
	RedBrightness=EERecoverVariable(2) & 0x3F;
	GreenBrightness=RedBrightness;
	ScoreBrightness=RedBrightness;
	
}

unsigned int EERecoverVariable(unsigned int index)
{
	unsigned int eeval;
	eeval = EEReadVariable(index);
	if ((eeval < EELowLimits[index]) || (eeval > EEHighLimits[index]) )
		eeval = EEDefaults[index];
	return (eeval);
}



