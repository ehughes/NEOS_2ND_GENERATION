#ifndef _SPIROUTINES_H
#define _SPIROUTINES_H


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


extern unsigned char SPIInstBuffer[5];		// Up to 8 pending inst, 3* addr, 1* chip number
extern unsigned char SPITxBuffer[256];	// 128 words read from Serial Flash
extern unsigned char SPIRxBuffer[256];		//Data coming in from SPI (Was [260] for interrupt version)
extern unsigned int SPIDataCount;		//Number of data bytes required for instruction */
extern unsigned char SPIInstrCount;			//Counts SPI transmit bytes


#endif

