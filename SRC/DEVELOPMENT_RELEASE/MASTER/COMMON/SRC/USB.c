#include <uart.h>
#include <p30fxxxx.h>
#include "DataTypes.h"
#include "SystemVariables.h"
#include "USB.h"

struct   {
  					BYTE *TxBuf; //This is the pointer for the next byte to go out
					WORD TxCnt;  //This keeps track of how many bytes are left to send in the specified buffer
		        				 //When this is at zero, there isn't any more data to send
							
	    		 	BYTE RxBuf[RX_BUFFER_SIZE];   //THis is a circular FIFO buffer that stores
								 																	 //data from the serial port.
					WORD RxBufBasePtr; //This keeps a pointer to the base of the ,recieve FIFO
					WORD RxCnt; //This is the number bytes that are waiting to
																//be read

			} Uart0; 


void InitUSB(void)
{
WORD i;

	
ConfigIntUART2(UART_RX_INT_EN & UART_RX_INT_PR2 & 
		UART_TX_INT_EN & UART_TX_INT_PR1); 

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
	

//Initialize the software structure:
Uart0.RxBufBasePtr = 0;
Uart0.RxCnt = 0;
for(i=0;i<RX_BUFFER_SIZE;i++)
{
	Uart0.RxBuf[i] = 0;
}

Uart0.TxCnt = 0;

// initialize the software transmit info
	Uart0.TxBuf = 0;
	Uart0.TxCnt = 0;


}



void __attribute__((__interrupt__,__auto_psv__)) _U2TXInterrupt(void)
{
	if(Uart0.TxCnt != 0)
	{
		Uart0.TxCnt--;
		WriteUART2(*(Uart0.TxBuf))			; //send out the next byte
		Uart0.TxBuf++;
	 }
   
 else
   {
      	//if we are done, do nothing
    }
    
		IFS1bits.U2TXIF = 0;


}

void __attribute__((__interrupt__,__auto_psv__)) _U2RXInterrupt(void)
{

	while( DataRdyUART2())
	{
			Uart0.RxBuf[(Uart0.RxBufBasePtr + Uart0.RxCnt) & (RX_BUFFER_SIZE - 1)] = ReadUART2();
			Uart0.RxCnt++;
	}
	
	IFS1bits.U2RXIF = 0;
}


WORD UartTx(BYTE *Buf, WORD len)
{


	if(Uart0.TxCnt != 0)
	  {
		return Uart0.TxCnt;
	   }
   
	else
		{
     		 
	       Uart0.TxCnt = len - 1;
	  	   Uart0.TxBuf = Buf + 1;
		 // lets send out the first byte, The interrupt routine will do the rest
		 // in the background

		 //send out the first byte
		   WriteUART2(*Buf);	
				 
		}

	return 0;
}






WORD CopyFromRxBuf(BYTE *Buf, WORD Cnt)
{
	WORD i;

//If more bytes are request than available, return the number of bytes available	
	if(Cnt > Uart0.RxCnt)
	{
		return Uart0.RxCnt;
	}
	else
		{
			
			for(i=0;i<Cnt;i++)
				{
					Buf[i] = Uart0.RxBuf[Uart0.RxBufBasePtr];
					Uart0.RxBufBasePtr++; 
					//Make sure to perform circular wrap
					Uart0.RxBufBasePtr = Uart0.RxBufBasePtr & (RX_BUFFER_SIZE-1);
					
					Uart0.RxCnt--;	
				}

		}

	return 0;
}

BYTE GrabSingleRxByte()
{
	BYTE RetVal;
		
	RetVal = Uart0.RxBuf[Uart0.RxBufBasePtr];
	Uart0.RxBufBasePtr++; 
	//Make sure to perform circular wrap
	Uart0.RxBufBasePtr = Uart0.RxBufBasePtr & (RX_BUFFER_SIZE-1);
	Uart0.RxCnt--;
	
	return RetVal;
}

WORD RxAvailable()
{
	return Uart0.RxCnt;
}

WORD TxBusy()
{
	return Uart0.TxCnt;
}

void FlushRxBuf()
{
	Uart0.RxCnt = 0;
	Uart0.RxBufBasePtr = 0;
}

void FlushTxBuf()
{
	Uart0.TxCnt = 0;
}



// Calculate the CRC for a packet using
// the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1)

WORD CalcCRC(BYTE *data, WORD length)
{
	WORD i,crc;
	crc = 0xffff;
	for (i=0;i<length;i++)
	{
		crc = (BYTE)(crc >> 8) | (crc << 8); 
		crc ^= data[i];
		crc ^= (BYTE)(crc & 0xff) >> 4;
		crc ^= (crc << 8) << 4;
		crc ^= ((crc & 0xff) << 4) << 1;
	}
	return crc;
}



