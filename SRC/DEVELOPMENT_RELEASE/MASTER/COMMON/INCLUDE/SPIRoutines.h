#include "DataTypes.h"

#ifndef _SPIROUTINES_H
#define _SPIROUTINES_H

void SPIInit(void);
void __attribute__ ((__interrupt__)) _SPI1Interrupt(void);
void SPISendInstruction(BYTE instruction, DWORD address);
void SPIWritePage(DWORD address);
void SPISectorErase(DWORD address);
void SPISectorEraseNoWait(DWORD address);
void SPIWritePageNoWait(DWORD address);
WORD SPIBusyCheck(DWORD address);

extern WORD asmSPITransfer(WORD dat);
extern BYTE SPIInstBuffer[5];		// Up to 8 pending inst, 3* addr, 1* chip number
extern BYTE SPITxBuffer[256];	// 128 words read from Serial Flash
extern BYTE SPIRxBuffer[256];		//Data coming in from SPI (Was [260] for interrupt version)
extern WORD SPIDataCount;		//Number of data bytes required for instruction */
extern BYTE SPIInstrCount;			//Counts SPI transmit bytes


#endif

