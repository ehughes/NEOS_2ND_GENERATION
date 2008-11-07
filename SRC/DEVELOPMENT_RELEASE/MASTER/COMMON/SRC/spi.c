#include <spi.h>
#include "SPIRoutines.h"
#include "SystemVariables.h"
#include <p30fxxxx.h>
#include "DataTypes.h"
#include "BoardSupport.h"


BYTE SPIInstBuffer[5];		// Up to 8 pending inst, 3* addr, 1* chip number
BYTE SPITxBuffer[256];		// 128 words read from Serial Flash
BYTE SPIRxBuffer[256];		//Data coming in from SPI (Was [260] for interrupt version)
WORD SPIDataCount;			//Number of data bytes required for instruction */
BYTE SPIInstrCount;			//Counts SPI transmit bytes


//Note reading 256 bytes from flash takes 1.2ms

void SPIInit(void)
{
	CloseSPI1();	/* turn off in case it was reset hot */

		ConfigIntSPI1(SPI_INT_DIS & SPI_INT_PRI_5);	
		
		WORD config1 = 
		FRAME_ENABLE_OFF &
		FRAME_SYNC_OUTPUT &
		ENABLE_SDO_PIN &
		SPI_MODE16_OFF &
		SPI_SMP_ON &			//Tried OFF 8/7/06 - sorta worked
//Was using CLK_POL_ACTIVE_LOW and SPI_CKE_OFF up until 8/8/06
//tried HIGH and ON since the chip allows this - right away it seemed 
//to work OK
		SPI_CKE_ON &			// WAS OFF - changed to ON/HIGH 
		SLAVE_ENABLE_OFF &
		CLK_POL_ACTIVE_HIGH & 	//Was LOW
		MASTER_ENABLE_ON &
		SEC_PRESCAL_1_1	&
		PRI_PRESCAL_1_1;		/* Change to 1:1 */
		WORD config2 =
		SPI_ENABLE &
		SPI_IDLE_STOP &
		SPI_RX_OVFLOW_CLR;
		OpenSPI1 (config1,config2);	/* turn SPI back on */

}




void SPISendInstruction(BYTE instruction, DWORD address)
{
	WORD temp,temp2;

	//Empty SPI garbage with a dummy read
	temp = SPI1BUF;	
	
	SPIInstBuffer[0] = instruction;
	SPIInstBuffer[1] = ((address & 0xFF0000) >> 16);
	SPIInstBuffer[2] = ((address & 0xFF00) >> 8);
	SPIInstBuffer[3] = address & 0xFF;
	SPIInstBuffer[4] = ((address & 0xFF000000) >> 24);

		//Enable the appropriate IC 
		if ( (SPIInstBuffer[4] == 0) && (SPIInstBuffer[1] < 0x80) )
		{
			MEM0_ENABLE;
			MEM1_DISABLE;
			MEM2_DISABLE;
			MEM3_DISABLE;
		}
		else if ( (SPIInstBuffer[4] == 0) && (SPIInstBuffer[1] >= 0x80) )
		{
			MEM0_DISABLE
			MEM1_ENABLE;
			MEM2_DISABLE;
			MEM3_DISABLE;
		}	
		else if ( (SPIInstBuffer[4] == 1) && (SPIInstBuffer[1] < 0x80) )
		{
			MEM0_DISABLE
			MEM1_DISABLE;
			MEM2_ENABLE;
			MEM3_DISABLE;
		}
		else
		{
			MEM0_DISABLE
			MEM1_DISABLE;
			MEM2_DISABLE;
			MEM3_ENABLE;
		}

		MEM_WRITE_ENABLE;
		SPIDataCount =0;		//Assume 0 data until told otherwise
		SPIInstrCount=1;			//Assume 0 address required too


		SPIInstBuffer[1] &= 0x7F;	//Zap the 24th bit of address, always 0
		temp =SPIInstBuffer[0];		//Get the instruction
		
		switch (temp)
		{
			case 0x2:				//read
			case 0x3:				//write
				SPIInstrCount=4;
				SPIDataCount=256;
				break;
			case 0xD8:				/* Sector erase instr, addr but no data */
				SPIInstrCount=4;
				break;
			case 0x01:				//Status read
			case 0x05:				//Status write
				SPIDataCount=1;
				break;
			case 0x9F:				// read ID
				SPIDataCount=3;
				break;
			case 0x33:				//256 byte read
				SPIInstrCount=4;
				SPIDataCount=8;
				SPIInstBuffer[0]=0x3;	
				break;
			case 0x43:				// 128 byte read
				SPIInstrCount=4;
				SPIDataCount=128;
				SPIInstBuffer[0]=0x3;	
				break;
		}

	// Loop through the instruction bytes
	for (temp=0; temp<SPIInstrCount; temp+=1)
	{
		temp2=asmSPITransfer(SPIInstBuffer[temp]);	// Call assy language transfer
	}

	//Loop through data bytes, if any (for loop won't run if none needed)
	for (temp=0; temp<SPIDataCount; temp+=1)
	{

		SPIRxBuffer[temp]=asmSPITransfer(SPITxBuffer[temp]);	// Call assy language transfer
	}
	
		MEM0_DISABLE
		MEM1_DISABLE;
		MEM2_DISABLE;
		MEM3_DISABLE;
}

void SPIWritePage(DWORD address)
{
	SPISendInstruction(0x06,address);		//Write enable
	SPISendInstruction(0x02,address);		//Write page
	do	
	{
		SPISendInstruction(0x05,address);		//Read status
	} while (SPIRxBuffer[0] & 0x1);
}

void SPIWritePageNoWait(DWORD address)
{
	SPISendInstruction(0x06,address);		//Write enable
	SPISendInstruction(0x02,address);		//Write page
}

void SPISectorErase(DWORD address)
{
	SPISendInstruction(0x06,address);		//Write enable
	SPISendInstruction(0xD8,address);		//Sector Erase
	do	
	{
		SPISendInstruction(0x05,address);		//Read status
	} while (SPIRxBuffer[0] & 0x1);
}

void SPISectorEraseNoWait(DWORD address)
{
	SPISendInstruction(0x06,address);		//Write enable
	SPISendInstruction(0xD8,address);		//Sector Erase
}


/* Busy check returns 1 if busy & 0 if not */
WORD SPIBusyCheck(DWORD address)
{	
	SPISendInstruction(0x05,address);		//Read status
	return (SPIRxBuffer[0] & 0x1);
}







