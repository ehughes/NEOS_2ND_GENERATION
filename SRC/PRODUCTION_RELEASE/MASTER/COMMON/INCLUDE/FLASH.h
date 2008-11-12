#include "DataTypes.h"
#include "BoardSupport.h"
#include <p30fxxxx.h>
#include <spi.h>


void  FlashRead(DWORD StartAddress, WORD Length, BYTE *DataBuffer);
 void BlockUntilFlashOperationComplete(DWORD Address);
 void FlashEnable(DWORD StartAddress);
 void FlashDisable();


void M25PXX_WREN(DWORD Address);
void M25PXX_RDID(DWORD Address, BYTE *ID);
BYTE M25PXX_RDSR(DWORD Address);
void M25PXX_WRSR(DWORD Address,BYTE StatusReg);
void M25PXX_WRSR(DWORD Address,BYTE StatusReg);
void M25PXX_READ(DWORD PageAddress, BYTE * ReadBuffer);
void M25PXX_READ(DWORD PageAddress, BYTE * ReadBuffer);
void M25PXX_SE(DWORD SectorAddress);
void M25PXX_BE(DWORD Address);
void M25PXX_PP(DWORD PageAddress, BYTE *WriteBuffer);


void BlockUntilFlashOperationComplete();

#define SPIRBUF	0x01


#define WAIT_FOR_SPI 	while(!(SPI1STAT& SPIRBUF)){asm("clrwdt");}
#define SPI_WRITE_REG   SPI1BUF 
#define SPI_READ_REG    SPI1BUF

