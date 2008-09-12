#include <uart.h>
#include "UartRoutines.h"
#include "SystemVariables.h"
#include "SPIRoutines.h"
#include <p30fxxxx.h>



/* UART DATA */

unsigned char UART1TxBuffer[9];
//unsigned char UART1TxDone=0;
unsigned char UART1TxPointer=0;
unsigned char UART1TxLength=0;
unsigned char UART1RxBuffer[137];
unsigned char UARTLastReceived;		//Remembers where the data came from
//unsigned char * Receiveddata = UART1RxBuffer;
unsigned char UART1RxIndex=0;
unsigned char UART1RxTimer=0;		// Looks for end of transmission
unsigned char UART1RxDataReady=0;	// Flag indicates received data ready
unsigned char UART1ReplyPending=0;
unsigned long UARTAddress;
unsigned char SPITxAudioHalt=0;			//Used by UART to halt SPI audio transmissions


void UARTInit(void)
{
	if (Slave ==0)
	{
		ConfigIntUART1(UART_RX_INT_EN & UART_RX_INT_PR2 & 
		UART_TX_INT_EN & UART_TX_INT_PR1);

		ConfigIntUART2(UART_RX_INT_EN & UART_RX_INT_PR2 & 
		UART_TX_INT_EN & UART_TX_INT_PR1); 

		OpenUART1(
			UART_EN &
			UART_IDLE_CON &
			UART_DIS_WAKE &
			UART_DIS_LOOPBACK &
			UART_DIS_ABAUD &
			UART_NO_PAR_8BIT &
			UART_1STOPBIT,
			UART_INT_TX_BUF_EMPTY &
			UART_TX_PIN_NORMAL &
			UART_TX_ENABLE &
			UART_INT_RX_CHAR &
		 	UART_ADR_DETECT_DIS	&
			UART_RX_OVERRUN_CLEAR,
			106 );		// (16384000/(16*9600))	-1 ) for 9600 baud
	
		OpenUART2(
			UART_EN &
			UART_IDLE_CON &
			UART_DIS_WAKE &
			UART_DIS_LOOPBACK &
			UART_DIS_ABAUD &
			UART_NO_PAR_8BIT &
			UART_1STOPBIT,
			UART_INT_TX_BUF_EMPTY &
			UART_TX_PIN_NORMAL &
			UART_TX_ENABLE &
			UART_INT_RX_CHAR &
		 	UART_ADR_DETECT_DIS	&
			UART_RX_OVERRUN_CLEAR,
			7 );		// (16384000/(16*128000))	-1 ) for 128000 baud


	/* Temporary test */
	/* Load transmit buffer and transmit
	encountered */
//	putsUART1 ((unsigned int *)Txdata);
	/* Wait for transmission to complete */
//	while(BusyUART1());
	/* Read all the data remaining in */
//	while(DataRdyUART1())
//	{
//	(*( Receiveddata)++) = ReadUART1();
//	}
	/* Turn off UART1 module */
//	CloseUART1();
	}
}


//void __attribute__((__interrupt__,__auto_psv__)) _U1TXInterrupt(void)
void __attribute__((__interrupt__,__auto_psv__)) _U1TXInterrupt(void)
{
	IFS0bits.U1TXIF = 0;
	if (UART1TxPointer < UART1TxLength)
	{
		WriteUART1 (UART1TxBuffer [UART1TxPointer]);
		UART1TxPointer++;
	}	
//	else
//	UART1TxDone=1;		//flag transmission complete
}

/* This is UART1 receive ISR */
//void __attribute__((__interrupt__,__auto_psv__)) _U1RXInterrupt(void)
void __attribute__((__interrupt__,__auto_psv__)) _U1RXInterrupt(void)
{
	IFS0bits.U1RXIF = 0;
	UARTLastReceived=1;		// indicate reply to uart1
	/* Read the receive buffer till atleast one or more character can be read */
	while( DataRdyUART1())
	{
		if (UART1RxIndex > 136)
			UART1RxIndex=0;
		UART1RxBuffer[UART1RxIndex++] = ReadUART1();
		UART1RxTimer=22;			//233us steps = 5ms
	}
}


//void __attribute__((__interrupt__,__auto_psv__)) _U2TXInterrupt(void)
void __attribute__((__interrupt__,__auto_psv__)) _U2TXInterrupt(void)
{
	IFS1bits.U2TXIF = 0;
	if (UART1TxPointer < UART1TxLength)
	{
		WriteUART2 (UART1TxBuffer [UART1TxPointer]);
		UART1TxPointer++;
	}	
//	else
//	UART1TxDone=1;		//flag transmission complete
}

/* This is UART1 receive ISR */
//void __attribute__((__interrupt__,__auto_psv__)) _U2RXInterrupt(void)
void __attribute__((__interrupt__,__auto_psv__)) _U2RXInterrupt(void)
{
	IFS1bits.U2RXIF = 0;
	UARTLastReceived=2;			// indicate reply to uart 2
	/* Read the receive buffer till atleast one or more character can be read */
	while( DataRdyUART2())
	{
		if (UART1RxIndex > 136)
			UART1RxIndex=0;
		UART1RxBuffer[UART1RxIndex++] = ReadUART2();
		UART1RxTimer=108;			//233us steps = 5ms
	}
}


/* Format a transmit reply */
void UART1TxFormat (unsigned long addr, unsigned char command)
{
	UART1TxBuffer[0]=0x01;		//SOH
	UART1TxBuffer[1]=9;		// Byte count
	UART1TxBuffer[2]= command;		// Command
	UART1TxBuffer[3] = addr & 0xFF;		//LSB
	UART1TxBuffer[4] = (addr >> 8) & 0xFF;
	UART1TxBuffer[5] = (addr >> 16) & 0xFF;
	UART1TxBuffer[6] = (addr >> 24) & 0xFF;   //MSB
	UARTMakeChecksum (UART1TxBuffer);
	UART1TxLength=9;
	UART1TxPointer=1;		//interrupt will continue from 2nd byte
	// Start the appropriate UART transmission
	if (UARTLastReceived==2)
		WriteUART2 (UART1TxBuffer [0]);  // Start the first byte
	else
		WriteUART1 (UART1TxBuffer [0]);  // Start the first byte
}

	
void UARTMakeChecksum (unsigned char array[] )
{
	unsigned int checksum=0;
	unsigned int loopcount;
	if (array[1] >= 9)			// byte count minimum 3
	{
		for (loopcount=0; loopcount <= (array[1]-3); loopcount++)
		{
			checksum += array[loopcount];
		}
		array[loopcount] = (checksum >> 8) & 0xFF;
		array[loopcount+1] = checksum & 0xFF;
	}
}

int UARTRxVerify( unsigned char array[] )
{
	int pass=1;
	unsigned int loopcount,checksum=0;
	
	if (UART1RxBuffer[5]==0x1)		
		pass=1;

	if (array[0] != 0x1)
		pass = 0;

	if (array[1] >=5)
	{
		for (loopcount=0; loopcount <= (array[1]-3); loopcount++)
		{
			checksum += array[loopcount];
		}
		if ( (((checksum >> 8 ) & 0xFF) != array[loopcount]) ||
			((checksum & 0xFF) != array[loopcount+1]) )
			pass=0;
	}
	return (pass);
}

//Check Rx buffer to see if there is anything to process 
void UART1RxProcess (void)
{
	int i;
	if (UART1RxDataReady) 
	{	
		UART1RxDataReady=0;
		if (UARTRxVerify (UART1RxBuffer))
		{
			//Look for restore Audio activity command
			if ((UART1RxBuffer[2] == 0x82) && (UART1RxBuffer[1] == 0x5) )
			{
				UART1ReplyPending=1;
				SPITxAudioHalt=0;
			}
	
			//Look for write address command
			if ((UART1RxBuffer[2] == 0x81) && (UART1RxBuffer[1] == 137)
			&& ((UART1RxBuffer[3] & 0x7F)==0)  && (UART1RxBuffer[6] <= 0x1) )
			{
				for (i=0; i<128; i++)
				{
					SPITxBuffer[i+(UART1RxBuffer[3] & 0x80)] = UART1RxBuffer[i+7];
				}
				UARTAddress = ((unsigned long)UART1RxBuffer[6] <<24) + ((unsigned long)UART1RxBuffer[5] <<16) + ((unsigned long)UART1RxBuffer [4] <<8) + (unsigned long)UART1RxBuffer[3];
				UART1ReplyPending=1;
				SPITxAudioHalt=1;
				// if address is 0xxx0000, erase the sector
				if  ((UARTAddress & 0xFFFF) == 0)
				{
					SPISectorEraseNoWait(UARTAddress & 0xFFFF0000);
				}
				//If address is 0xxxxx80, write 256 byte page
				if ((UART1RxBuffer[3] & 0x80) != 0)
				{
					SPIWritePageNoWait(UARTAddress & 0xFFFFFF00);			//Write page if upper half
				}
			}
		}
	}
}


void UART1ReplyCheck(void)
{	
	if (UART1ReplyPending)			//See if a erply is pending
	{
		if (!SPIBusyCheck(UARTAddress & 0xFFFFFF00))		//see if the RAM is still writing
		{
			UART1TxFormat(UARTAddress,0x6);		//send ACK
			UART1ReplyPending=0;		
		}
	}
}






