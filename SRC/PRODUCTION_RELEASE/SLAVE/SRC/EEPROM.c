#include "EEPROMRoutines.h"
#include "SystemVariables.h"
#include "EEPROM_Access.h"
#include "SystemUtilities.h"

//EEPROM VARIABLES
unsigned char MyNodeNumber;		//0-31 number assigned to this node for gameplay
unsigned char AudioGlobalVolume=GLOBAL_VOLUME_INIT;
unsigned char AudioBackgroundVolume=BACKGROUND_VOLUME_INIT;

void EEStoreVariable(unsigned int index, int value)
{
	EraseEE(0x7F,0xFC00+(index*2),1);
	WriteEE(&value,0x7F,0xFC00+(index*2),1);
}


unsigned int EEReadVariable(unsigned int index)
{
	int value;
	ReadEE(0x7F,0xFC00+(index*2),&value,1);
	return value;
}





