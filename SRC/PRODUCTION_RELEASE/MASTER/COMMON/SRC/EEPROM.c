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

DWORD GameCount;

void RecallGameCount()
{
	WORD t1,t2;
	
	t1 = EEReadVariable(GAME_COUNT_LOW_LOCATION);
	t2 = EEReadVariable(GAME_COUNT_HIGH_LOCATION);

	GameCount = (((DWORD)(t2))<<16)	+ ((DWORD)(t1));
	
	if(GameCount >= 1000000)
	{
		GameCount = 0;		
		StoreGameCount();
	}	
}	

void StoreGameCount()
{
	EEStoreVariable(GAME_COUNT_LOW_LOCATION,  (WORD)(GameCount));
	EEStoreVariable(GAME_COUNT_HIGH_LOCATION, (WORD)(GameCount>>16));
}	



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
	RecallGameCount();
}

WORD EERecoverVariable(WORD index)
{
	WORD eeval;
	eeval = EEReadVariable(index);
	if ((eeval < EELowLimits[index]) || (eeval > EEHighLimits[index]) )
		eeval = EEDefaults[index];
	return (eeval);
}



