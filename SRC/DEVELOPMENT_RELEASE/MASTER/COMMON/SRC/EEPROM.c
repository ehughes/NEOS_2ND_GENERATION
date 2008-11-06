#include "EEPROMRoutines.h"
#include "_GameVariables.h"
#include "SystemVariables.h"
#include "DataEEPROM.h"
#include "SystemUtilities.h"

//#define GLOBAL_VOLUME_INDEX_MAX 0	
//#define GLOBAL_VOLUME_INDEX_MIN	10

 BYTE EEDefaults[8] = {0,GLOBAL_VOLUME_INDEX_MAX,63,0,0,0,0,0};
 BYTE EELowLimits[8] = {0,GLOBAL_VOLUME_INDEX_MIN,3,0,0,0,0,0};
 BYTE EEHighLimits[8] = {255,GLOBAL_VOLUME_INDEX_MAX,63,255,255,255,255,255};

BYTE VolumeIndexTable[11] = {0,25,51,76,102,127,153,178,204,229,255};


//BYTE VolumeIndexTable[11] = {0,7,17,28,43,61,84,114,150,197,255};



//EEPROM VARIABLES
unsigned char MyNodeNumber;		//0-31 number assigned to this node for gameplay
BYTE AudioGlobalVolume=GLOBALVOLUMEINIT;
BYTE AudioBackgroundVolume=BACKGROUNDVOLUMEINIT;
BYTE NoAnnoy=0;			//When =1 it is silent in idle mode & no snoring
BYTE RedBrightness=63;
BYTE GreenBrightness=63;
BYTE ScoreBrightness=63;

BYTE AudioGlobalVolumeIndex = 10;

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
	AudioGlobalVolumeIndex=EERecoverVariable(AUDIO_VOLUME_INDEX_LOCATION);
	AudioGlobalVolume = VolumeIndexTable[AudioGlobalVolumeIndex];
	
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



