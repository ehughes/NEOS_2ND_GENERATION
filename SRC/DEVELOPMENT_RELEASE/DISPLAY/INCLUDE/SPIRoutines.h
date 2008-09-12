void SPIInit(void);
void __attribute__ ((__interrupt__)) _SPI1Interrupt(void);
//void SPISendInstrCheck(void);
//void SPIQueueInstruction(unsigned char instruction, unsigned long address);
void SPISendInstruction(unsigned char instruction, unsigned long address);
void SPIWritePage(unsigned long address);
void SPISectorErase(unsigned long address);
//void SPIWriteTone(unsigned int frequency, unsigned long address, unsigned long length);
//void SPIQueueIndex(unsigned long address, unsigned long length, unsigned int index);
//void SPIWriteTest(unsigned long address, unsigned long length);
void SPISectorEraseNoWait(unsigned long address);
void SPIWritePageNoWait(unsigned long address);
int SPIBusyCheck(unsigned long address);

extern unsigned int asmSPITransfer(unsigned int dat);


