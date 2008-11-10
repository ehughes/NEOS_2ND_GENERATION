#include "EEPROMRoutines.h"
#include "_GameVariables.h"
#include "SystemVariables.h"
#include "DataEEPROM.h"
#include "SystemUtilities.h"
#include "DataTypes.h"

BYTE EEDefaults[8] = {0,GLOBAL_VOLUME_INDEX_DEFAULT,63,0,0,0,0,0};
BYTE EELowLimits[8] = {0,GLOBAL_VOLUME_INDEX_MIN,3,0,0,0,0,0};
BYTE EEHighLimits[8] = {255,GLOBAL_VOLUME_INDEX_MAX,63,255,255,255,255,255};
BYTE VolumeIndexTable[11] = {0,25,51,76,102,127,153,178,204,229,255};

BYTE MyNodeNumber;		
BYTE AudioGlobalVolume;
BYTE AudioGlobalVolumeIndex;

void EEStoreVariable(WORD index,  int value)
{
	EraseEE(0x7F,0xFC00+(index*2),1);
	WriteEE((int *)(&value),0x7F,0xFC00+(index*2),1);
}


int EEReadVariable(WORD index)
{
	SIGNED_WORD  value;
	ReadEE(0x7F,0xFC00+(index*2),(int *)&value,1);
	return value;
}


void EERecover(void)
{
	AudioGlobalVolumeIndex=EERecoverVariable(AUDIO_VOLUME_INDEX_LOCATION);
	AudioGlobalVolume = VolumeIndexTable[AudioGlobalVolumeIndex];
}

WORD EERecoverVariable(WORD index)
{
	WORD eeval;
	eeval = EEReadVariable(index);
	if ((eeval < EELowLimits[index]) || (eeval > EEHighLimits[index]) )
		eeval = EEDefaults[index];
	return (eeval);
}



