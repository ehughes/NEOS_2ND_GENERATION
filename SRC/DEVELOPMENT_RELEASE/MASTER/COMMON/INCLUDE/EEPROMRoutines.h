#ifndef _EEPROMROUTINES_H
#define _EEPROMROUTINES_H


void EEStoreVariable(unsigned int index, int value);
int EEReadVariable(unsigned int index);

void EESaveVariables(void);		//Save for variable adjustment mode
void EESaveGameCount(void);
void EERecover(void);
unsigned int EERecoverVariable(unsigned int index);

extern unsigned int EEDefaults[8] ;
extern unsigned int EELowLimits[8] ;
extern unsigned int EEHighLimits[8];

extern unsigned char MyNodeNumber;		//0-31 number assigned to this node for gameplay
extern unsigned char AudioGlobalVolume;
extern unsigned char AudioBackgroundVolume;
extern unsigned char NoAnnoy;			//When =1 it is silent in idle mode & no snoring
extern unsigned char RedBrightness;
extern unsigned char GreenBrightness;
extern unsigned char ScoreBrightness;

#endif

