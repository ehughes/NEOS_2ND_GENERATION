#include <spi.h>
#include "SPIRoutines.h"
#include "SystemVariables.h"
#include <p30fxxxx.h>


unsigned char SPIInstBuffer[5];		// Up to 8 pending inst, 3* addr, 1* chip number
unsigned char SPITxBuffer[256];	// 128 words read from Serial Flash
unsigned char SPIRxBuffer[256];		//Data coming in from SPI (Was [260] for interrupt version)
unsigned int SPIDataCount;		//Number of data bytes required for instruction */
unsigned char SPIInstrCount;			//Counts SPI transmit bytes
//unsigned long SPIAddress;
//unsigned char SPINextToSend=0;	//Next instruction to send to SPI
//unsigned char SPINextAvailable=0;	//Next available location for SPI instruction
//unsigned int SPIRxIndex;
//unsigned char SPIStatus;
//unsigned char SPIPending;		// Indicates that interrupt has more work to do
//unsigned char SPITestFlag;		// TEMPORARY




//Note reading 256 bytes from flash takes 1.2ms
//This is with timer & audio ints enabled but no CAN traffic when measured

void SPIInit(void)
{
	CloseSPI1();	/* turn off in case it was reset hot */

		ConfigIntSPI1(SPI_INT_DIS & SPI_INT_PRI_5);	/* disable interrupt */

		unsigned int config1 = 
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
		unsigned int config2 =
		SPI_ENABLE &
		SPI_IDLE_STOP &
		SPI_RX_OVFLOW_CLR;
		OpenSPI1 (config1,config2);	/* turn SPI back on */

}


// Was sampled at SPI_SMP_ON for middle of time 

//void __attribute__ ((__interrupt__)) _SPI1Interrupt(void)
//{
//	IFS0bits.SPI1IF=0;		// Clear the interrupt request
//}


void SPISendInstruction(unsigned char instruction, unsigned long address)
{
	unsigned int temp,temp2;

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
			LATAbits.LATA12=0;
			LATAbits.LATA13=1;
			LATAbits.LATA14=1;
			LATAbits.LATA15=1;
		}
		else if ( (SPIInstBuffer[4] == 0) && (SPIInstBuffer[1] >= 0x80) )
		{
			LATAbits.LATA12=1;
			LATAbits.LATA13=0;
			LATAbits.LATA14=1;
			LATAbits.LATA15=1;
		}	
		else if ( (SPIInstBuffer[4] == 1) && (SPIInstBuffer[1] < 0x80) )
		{
			LATAbits.LATA12=1;
			LATAbits.LATA13=1;
			LATAbits.LATA14=0;
			LATAbits.LATA15=1;
		}
		else
		{
			LATAbits.LATA12=1;
			LATAbits.LATA13=1;
			LATAbits.LATA14=1;
			LATAbits.LATA15=0;
		}

		LATBbits.LATB15=0;		//Enable writes 
		SPIDataCount=0;		//Assume 0 data until told otherwise
		SPIInstrCount=1;			//Assume 0 address required too
//		SPIRxIndex=0;

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
//  		SPI1STATbits.SPIROV=0;
//		SPI1BUF=SPIInstBuffer[temp];		//Send out a data byte
//		while (SPI1STATbits.SPIRBF == 0);		//Wait for rx full
//		temp2 = SPI1BUF;		//Get RX data
//		while (SPI1STATbits.SPITBF);		// Make sure TX is available 
		temp2=asmSPITransfer(SPIInstBuffer[temp]);	// Call assy language transfer
	}

	//Loop through data bytes, if any (for loop won't run if none needed)
	for (temp=0; temp<SPIDataCount; temp+=1)
	{
//		SPI1STATbits.SPIROV=0;
//		SPI1BUF=SPITxBuffer[temp];		//Send out a data byte
//		while (SPI1STATbits.SPIRBF == 0);		//Wait for rx full
//		SPIRxBuffer[temp] = SPI1BUF;		//Get RX data
//		while (SPI1STATbits.SPITBF);		// Make sure TX is available 
		SPIRxBuffer[temp]=asmSPITransfer(SPITxBuffer[temp]);	// Call assy language transfer
	}
	LATAbits.LATA12=1;		//turn off all enables
	LATAbits.LATA13=1;		//Can't turn off yet, byte may not have gone!
	LATAbits.LATA14=1;
	LATAbits.LATA15=1;
}

void SPIWritePage(unsigned long address)
{
	SPISendInstruction(0x06,address);		//Write enable
	SPISendInstruction(0x02,address);		//Write page
	do	
	{
		SPISendInstruction(0x05,address);		//Read status
	} while (SPIRxBuffer[0] & 0x1);
}

void SPIWritePageNoWait(unsigned long address)
{
	SPISendInstruction(0x06,address);		//Write enable
	SPISendInstruction(0x02,address);		//Write page
}

void SPISectorErase(unsigned long address)
{
	SPISendInstruction(0x06,address);		//Write enable
	SPISendInstruction(0xD8,address);		//Sector Erase
	do	
	{
		SPISendInstruction(0x05,address);		//Read status
	} while (SPIRxBuffer[0] & 0x1);
}

void SPISectorEraseNoWait(unsigned long address)
{
	SPISendInstruction(0x06,address);		//Write enable
	SPISendInstruction(0xD8,address);		//Sector Erase
}


/* Busy check returns 1 if busy & 0 if not */
int SPIBusyCheck(unsigned long address)
{	
	SPISendInstruction(0x05,address);		//Read status
	return (SPIRxBuffer[0] & 0x1);
}







