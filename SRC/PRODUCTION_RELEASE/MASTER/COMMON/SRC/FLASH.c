/********************************************************
* 
*                MP25xX Flash Access Routines
* 
* 
*                      Eli Hughes  V0.2
*
*********************************************************/

#include "FLASH.h"
#include "DataTypes.h"
#include "SystemVariables.h"

 
#define WIP 0x01
#define WREN  0x06
#define WRDI 0x04
#define RDID 0x9f 
#define RDSR 0x05
#define WRSR 0x01 
#define READ 0x03 
#define FAST_READ 0x0B 
#define PP 0x02 
#define SE 0xD8 
#define BE 0xC7 
#define DP 0xB9 
#define RES 0xAB

 
//This function selects the FLASH chip on the NEOS board.
//There are 4 MP25P64 Devices.  
//Address						Chip #			Port Connection
//0x0000000	- 0x07FFFFF			0				PORT A.12
//0x0800000 - 0x0FFFFFF			1				PORT A.13
//0x1000000 - 0x17FFFFF			2				PORT A.14
//0x1800000 - 0x1FFFFFF			3				PORT A.15


//0x0000000	- 0x00			0				PORT A.12
//0x0800000 - 0x01			1				PORT A.13
//0x1000000 - 0x10			2				PORT A.14
//0x1800000 - 0x11			3				PORT A.15


void FlashEnable(DWORD StartAddress)
{
	switch((StartAddress>>23))
	{
		case 0:
					
			MEM0_ENABLE;
			MEM1_DISABLE;
			MEM2_DISABLE;
			MEM3_DISABLE;

		break;
		
		case 1:
		
			MEM0_DISABLE
			MEM1_ENABLE;
			MEM2_DISABLE;
			MEM3_DISABLE;
			
		break;
		
		case 2:
					
			MEM0_DISABLE
			MEM1_DISABLE;
			MEM2_ENABLE;
			MEM3_DISABLE;
			
		break;
		
		case 3:
		
			MEM0_DISABLE
			MEM1_DISABLE;
			MEM2_DISABLE;
			MEM3_ENABLE;
			                                                         ;
		break;
	}
}	


 void FlashDisable()
{
		MEM0_DISABLE
		MEM1_DISABLE;
		MEM2_DISABLE;
		MEM3_DISABLE;
			
}

//A high level function to read up to 65k bytes from the Flash device

void FlashRead(DWORD StartAddress, WORD Length, BYTE *DataBuffer)
{
	WORD k;
	FlashEnable(StartAddress);
	//send out Read Instruction
	SPI_WRITE_REG = READ;

	WAIT_FOR_SPI;

    DataBuffer[0] = SPI_READ_REG; //First Byte back is Junk
		
	//send out Address
	SPI_WRITE_REG = (BYTE)(StartAddress>>16);
	WAIT_FOR_SPI;
	k=SPI_READ_REG;//Junk
	SPI_WRITE_REG = (BYTE)(StartAddress>>8);
	WAIT_FOR_SPI;
	k=SPI_READ_REG;//Junk
	SPI_WRITE_REG = (BYTE)(StartAddress);
	WAIT_FOR_SPI;
	k=SPI_READ_REG; //Junk
	
	for(k=0;k<Length;k++)
	{
		SPI_WRITE_REG = (BYTE)(0xFF);
		WAIT_FOR_SPI;
	  	DataBuffer[k]= SPI_READ_REG;
	}
	
	FlashDisable();
	
}	


void BlockUntilFlashOperationComplete(DWORD Address)
{
	BYTE FLASHStatusByte;
	
	FLASHStatusByte = M25PXX_RDSR(Address);
	
		while((FLASHStatusByte&WIP) > 0)
			{
				FLASHStatusByte = M25PXX_RDSR(Address);
				asm("clrwdt");
			}	
}



void M25PXX_WREN(DWORD Address)
{
	
	BYTE i;
	
	//First Bring chip select Low
	
	FlashEnable(Address);
	
	//send out Write Enable INStruction.
	
	
	SPI_WRITE_REG = WREN;
	WAIT_FOR_SPI;
	i = SPI_READ_REG; //First Byte back is Junk
	
	FlashDisable();

	
}

//THis function acquires the identification from the M25PX device (BLocking SPI)
void M25PXX_RDID(DWORD Address, BYTE *ID)
{
	BYTE i;
	
	//First Bring chips select LOw
	
	FlashEnable(Address);
	
	//send out ID instruction
	SPI_WRITE_REG = RDID;
	WAIT_FOR_SPI;
	
	i = SPI_READ_REG; //First Byte back is Junk
	
	//Now, send three dummy bytes
	
	for(i=0;i<3;i++)
	{
		SPI_WRITE_REG = 0xff;
		WAIT_FOR_SPI;
		ID[i] = SPI_READ_REG; //First Byte back is Junk
	}		
	
	FlashDisable();
	
}

BYTE M25PXX_RDSR(DWORD Address)
{
	BYTE i;
	//First Bring chip select LOw
	
	FlashEnable(Address);
	//send out read status register instruction
	SPI_WRITE_REG = RDSR;
	WAIT_FOR_SPI;

    i = SPI_READ_REG; //First Byte back is Junk
	
	//Send out Dummy Byte to get status reg
	SPI_WRITE_REG = 0xFF;
	WAIT_FOR_SPI;
	
	FlashDisable();
	
	i = SPI_READ_REG;
	return i;
}



void M25PXX_WRSR(DWORD Address,BYTE StatusReg)
{
	BYTE i;
	
	

	M25PXX_WREN(Address);
	
		
	FlashEnable(Address);

	//send out Write Status Reg instruction
	SPI_WRITE_REG = WRSR;
	
	WAIT_FOR_SPI;
    i = SPI_READ_REG; //First Byte back is Junk
	
	//Send out New Status Register value
	SPI_WRITE_REG = StatusReg;
	WAIT_FOR_SPI;
	
	FlashDisable();
	i = SPI_READ_REG;

}



void M25PXX_READ(DWORD PageAddress, BYTE * ReadBuffer)
{
	
	
	WORD k;
	
	FlashEnable(PageAddress);
	//send out Read Instruction
	SPI_WRITE_REG = READ;
	
	WAIT_FOR_SPI;

    ReadBuffer[0] = SPI_READ_REG; //First Byte back is Junk
		
	//send out Address
	SPI_WRITE_REG = (BYTE)(PageAddress>>16);
	WAIT_FOR_SPI;
    ReadBuffer[0] = SPI_READ_REG; //junk
		//send out Address
	SPI_WRITE_REG = (BYTE)(PageAddress>>8);
	WAIT_FOR_SPI;
    ReadBuffer[0] = SPI_READ_REG; //junk
	
			//All reads are sector aligned
	SPI_WRITE_REG = (BYTE)(0);
	WAIT_FOR_SPI;
    ReadBuffer[0] = SPI_READ_REG; //junk
	
	
	for(k=0;k<256;k++)
	{
		SPI_WRITE_REG = (BYTE)(0xFF);
		WAIT_FOR_SPI;

		ReadBuffer[k]= SPI_READ_REG;
	}
	
	FlashDisable();
}

void M25PXX_PP(DWORD PageAddress, BYTE *WriteBuffer)
{
	
	BYTE i;
	WORD k;

	M25PXX_WREN(PageAddress);
	
	FlashEnable(PageAddress);
	//send out Read Instruction
	SPI_WRITE_REG = PP;
	
	WAIT_FOR_SPI;

    i = SPI_READ_REG; //First Byte back is Junk
		
	//send out Address
	SPI_WRITE_REG = (BYTE)(PageAddress>>16);
	WAIT_FOR_SPI;
    i = SPI_READ_REG; //junk
		//send out Address
	SPI_WRITE_REG = (BYTE)(PageAddress>>8);
	WAIT_FOR_SPI;
    i = SPI_READ_REG; //junk
	
	
	//LSB must be page aligned (ELi's Rule);
	SPI_WRITE_REG = 0;
	WAIT_FOR_SPI;
    i = SPI_READ_REG; //junk
	
	i=0;
	for(k=0;k<256;k++)
	{
		
		SPI_WRITE_REG = WriteBuffer[k];		
		WAIT_FOR_SPI;
		i= SPI_READ_REG;
	}
	
	FlashDisable();
		
}


void M25PXX_SE(DWORD SectorAddress)
{
	
	BYTE i;

	
	M25PXX_WREN(SectorAddress);
	FlashEnable(SectorAddress);
	
	//send out Read Instruction
	SPI_WRITE_REG = SE;
	
	WAIT_FOR_SPI;

    i = SPI_READ_REG; //First Byte back is Junk
		
	//send out Address
	SPI_WRITE_REG = (BYTE)(SectorAddress>>16);
	WAIT_FOR_SPI;
    i = SPI_READ_REG; //junk
		//MB must be sector aligned
	SPI_WRITE_REG = 0;
	WAIT_FOR_SPI;
    i = SPI_READ_REG; //junk
	
	//LSB must be sector Aligned
	SPI_WRITE_REG = 0;
	WAIT_FOR_SPI;
    i = SPI_READ_REG; //junk
			
	FlashDisable();
		
}



void M25PXX_BE(DWORD Address)
{
	
	BYTE i;
	
	M25PXX_WREN(Address);
	
	FlashEnable(Address);
	//send out Read Instruction
	SPI_WRITE_REG = BE;
	
	WAIT_FOR_SPI;

    i = SPI_READ_REG; //First Byte back is Junk
	
			
	FlashDisable();
		
}






