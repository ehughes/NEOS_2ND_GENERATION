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

/* EEPROM vars */
/*Declare constants/coefficients/calibration data to be stored in DataEEPROM*/
//int _EEDATA(32) fooArrayInDataEE[] = {0,1,2,3,4,5,6,7,8,9,0xA,0xB,0xC,0xD,0xE,0xF};
/*Declare variables to be stored in RAM*/
//int fooArray1inRAM[] = {0xAAAA, 0xBBBB, 0xCCCC, 0xDDDD, 0xEEEE, 0xFFFF, 0xABCD, 0xBCDE,
//                       0xCDEF, 0xDEFA, 0x0000, 0x1111, 0x2222, 0x3333, 0x4444, 0x5555};
//int fooArray2inRAM[16];


void EEStoreVariable(unsigned int index, unsigned int value)
{
	EraseEE(0x7F,0xFC00+(index*2),1);
	WriteEE(&value,0x7F,0xFC00+(index*2),1);
}


unsigned int EEReadVariable(unsigned int index)
{
	unsigned int value;
	ReadEE(0x7F,0xFC00+(index*2),&value,1);
	return value;
}

void EESaveVariables(void)
{
	//if there is a change & it's not the game count
	if ((GameVariable[0] != 0) && (GameScores[0] != 2) )
	{
		switch (GameScores[0])
		{
			case 0:
				GameScores[1] = Scale100to255(GameScores[1]);
				break;
			case 1:
				GameScores[1] = Scale100to63(GameScores[1]);
				break;
		}

		EEStoreVariable (GameScores[0]+1,GameScores[1]);
		GameVariable[0]=0;
	}
}

void EESaveGameCount(void)
{
	EEStoreVariable (32,(GameCount >>16));
	EEStoreVariable (33,(GameCount & 0xFFFF));
}

void EERecover(void)
{
	AudioGlobalVolume=EERecoverVariable(1);
//	AudioBackgroundVolume=EERecoverVariable(2);
//	NoAnnoy=EERecoverVariable(3) & 0x1;
	RedBrightness=EERecoverVariable(2) & 0x3F;
	GreenBrightness=RedBrightness;
	ScoreBrightness=RedBrightness;
//	ScoreBrightness=EERecoverVariable(6) & 0x3F;
//	DanceFudgeFactor=EERecoverVariable(7) & 0x7F;
	GameCount = ((long)(EEReadVariable(32)) << 16) + (long)(EEReadVariable(33));
	if (GameCount == 0xFFFFFFFF)
	{
		GameCount=0;
		EESaveGameCount();
	}
	
}

unsigned int EERecoverVariable(unsigned int index)
{
	unsigned int eeval;
	eeval = EEReadVariable(index);
	if ((eeval < EELowLimits[index]) || (eeval > EEHighLimits[index]) )
		eeval = EEDefaults[index];
	return (eeval);
}



