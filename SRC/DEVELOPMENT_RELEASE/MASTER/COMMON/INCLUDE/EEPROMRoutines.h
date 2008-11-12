#include "DataTypes.h"

#ifndef _EEPROMROUTINES_H
#define _EEPROMROUTINES_H

int EEReadVariable(WORD index);
WORD EERecoverVariable(WORD index);
void EERecover(void);
void EEStoreVariable(WORD index, int value);
void RecallGameCount();
void StoreGameCount();

extern DWORD GameCount;		//Total number of games played

extern BYTE EEDefaults[8];
extern BYTE EELowLimits[8];
extern BYTE EEHighLimits[8];

extern BYTE MyNodeNumber;		//0-31 number assigned to this node for gameplay
extern BYTE AudioGlobalVolume;
extern BYTE AudioGlobalVolumeIndex;
extern BYTE VolumeIndexTable[11];

#define AUDIO_VOLUME_INDEX_LOCATION 1
#define GAME_COUNT_LOW_LOCATION 	2
#define GAME_COUNT_HIGH_LOCATION 	3



#endif

