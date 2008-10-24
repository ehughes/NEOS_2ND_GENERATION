#include "DataTypes.h"

#ifndef _EEPROMROUTINES_H
#define _EEPROMROUTINES_H


void EEStoreVariable(unsigned int index, int value);
int EEReadVariable(unsigned int index);

void EESaveVariables(void);		//Save for variable adjustment mode
void EESaveGameCount(void);
void EERecover(void);
unsigned int EERecoverVariable(unsigned int index);

extern BYTE EEDefaults[8];
extern BYTE EELowLimits[8];
extern BYTE EEHighLimits[8];

extern BYTE MyNodeNumber;		//0-31 number assigned to this node for gameplay
extern BYTE AudioGlobalVolume;
extern BYTE AudioBackgroundVolume;
extern BYTE NoAnnoy;			//When =1 it is silent in idle mode & no snoring
extern BYTE RedBrightness;
extern BYTE GreenBrightness;
extern BYTE ScoreBrightness;

extern BYTE AudioGlobalVolumeIndex;
extern BYTE VolumeIndexTable[11];

#define AUDIO_VOLUME_INDEX_LOCATION 1

#endif

