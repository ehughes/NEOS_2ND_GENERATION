#include <p30fxxxx.h>
#include <ports.h>
#include <Timer.h>
#include "BoardSupport.h"
#include "CANRoutines.h"
#include "TimerRoutines.h"
#include "DataTypes.h"

void PortInit(void)
{
	ADPCFG=0xFFFF;		//Make all digital inputs or portb can't be read
	
	//Setup FIFO status lines and data lines to inputs, 
	TRISD |= FT245_RXF | FT245_TXE | FT245_DATA;
	TRISB &=  ~CAN_HS;
	//Other control lines are outputs
	TRISD &= ~(FT245_RD | FT245_WR);
	TRISF &= ~(CAN_TERMINATION_CTRL);
	TRISG &= ~(RX_LED | TX_LED | FT245_RESET);
		
	FT245_SWITCH_TO_READ;
	CAN_TERMINATION_OFF;
	TX_LED_OFF;
	RX_LED_OFF;
	FT245_RD_OFF;
	FT245_WR_OFF;
	
	FT245_DATA_LATCH &= ~(FT245_DATA&0x00);
}		

BYTE Test1;
WORD Test2;

BYTE ReadFT245()
{
//Note - No Delays need.  Setup/Hold times well within
//FT245 SPEC.   Read cycle rime is on the order of 400nS (scope measure)

BYTE DataReadFT245;
//Make sure Data port is are inputs
FT245_SWITCH_TO_READ;
//Make sure Read Strobe is is inactive State
FT245_RD_OFF;

//Strobe to get the next Data point
FT245_RD_ON;

//Grab the Data
DataReadFT245 = (BYTE)(FT245_DATA_PORT); 
DataReadFT245 = (BYTE)(FT245_DATA_PORT); 

 
//Stop the read cycle
FT245_RD_OFF;

//All Done!
return DataReadFT245;
	
}

void WriteFT245(BYTE DataOut)
{
	

//Note - No Delays need.  Setup/Hold times well within
//FT245 SPEC.   Read cycle rime is on the order of 400nS (scope measure)

//Make sure Data port is are Outputs
FT245_SWITCH_TO_WRITE;
//Make sure WRITE Strobe is is inactive State
FT245_WR_OFF;

//Put the Data on the port
FT245_DATA_LATCH  = (FT245_DATA_PORT & 0xFF00) | DataOut;

//Strobe to get the next Data point
FT245_WR_ON;
 
//Stop the read cycle
FT245_WR_OFF;

//Make sure Data port is are Outputs
FT245_SWITCH_TO_READ;

//All Done!

}

void WriteBlockFT245(BYTE *DataOut,WORD Length)
{
	WORD i;
	
	for(i=0 ; i<Length ; i++)
	{
		while(!FT245_TRANSMIT_READY)
		{
		}
	
		WriteFT245(DataOut[i]);
	}
}	
	


