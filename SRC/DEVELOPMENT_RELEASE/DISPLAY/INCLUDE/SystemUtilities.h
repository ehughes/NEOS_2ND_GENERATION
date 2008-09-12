void PortInit(void);
void ReadOptionJumpers(void);

unsigned char Scale100to63(unsigned int displayval);	//Conversions for diagnostic adjustments
unsigned char Scale100to255(unsigned int displayval);
unsigned int Scale255to100(unsigned char inpval);
unsigned int Scale63to100(unsigned char inpval);

unsigned char SelectRandomButton(unsigned char* group);
void ResetAllSlaves(void);
void RegulatorControl(void);

void CN16Init(void);

void Piezo(void);
//void FillFlashWithTones(void);
//void RecordMusic1(unsigned long address);
//void RecordMusic2(unsigned long address);
void ResetAudioAndLEDS(void);
void ResetLeds(void);
void ResetGameVariables (void);

/* ReadEErow prototype:
--------------------
extern int ReadEE(int Page, int Offset, int* DataOut, int Size);

Parameters Definition:
Page:        is the 8 most significant bits of the source address in EEPROM
Offset:      is 16 least significant bits of the source address in EEPROM
DataOut:     is the 16-bit address of the destination RAM location or array
Size:        is the number of words to read from EEPROM and is a value of 1 or
             16

Return Value: Function returns ERROREE (or -1) if Size is invalid


EraseEErow prototype:
---------------------
extern int EraseEE(int Page, int Offset, int Size);

Parameters Definition:
Page:        is the 8 most significant bits of the address in EEPROM to be
             erased
Offset:      is 16 least significant bits of the address in EEPROM to be erased
Size:        is the number of words to read from EEPROM and is a value of 1, 16
             or 0xFFFF (for erasing ALL EEPROM memory)

Return Value:
Function returns ERROREE (or -1) if Size is invalid


WriteEErow prototype:
---------------------
extern int WriteEE(int* DataIn, int Page, int Offset, int Size);

Parameters Definition:
Page:        is the 8 most significant bits of the destination address in EEPROM
Offset:      is 16 least significant bits of the destination address in EEPROM
DataIn:      is the 16-bit address of the source RAM location or array
Size:        is the number of words to read from EEPROM and is a value of 1 or
             16

Return Value:
Function returns ERROREE (or -1) if Size is invalid */

