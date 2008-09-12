


void EEStoreVariable(unsigned int index,  int value);
unsigned int EEReadVariable(unsigned int index);

void EESaveVariables(void);		//Save for variable adjustment mode
void EESaveGameCount(void);
void EERecover(void);
unsigned int EERecoverVariable(unsigned int index);




extern unsigned char AudioGlobalVolume;
extern unsigned char AudioBackgroundVolume;
