#include "DataTypes.h"
#include <p30fxxxx.h>

#ifndef _BOARD_SUPPORT_H
#define _BOARD_SUPPORT_H


void PortInit(void);

BYTE ReadFT245();

void WriteFT245(BYTE DataOut);
void WriteBlockFT245(BYTE *DataOut,WORD Length);

#define RX_LED 					0x4000
#define TX_LED 					0x8000
#define CAN_TERMINATION_CTRL	0x0008
#define CAN_HS					0x2000

#define FT245_RXF				0x0200
#define FT245_TXE				0x0400
#define FT245_RD				0x0800
#define FT245_WR				0x1000
#define FT245_DATA				0x00FF
#define FT245_RESET				0x2000

#define FT245_DATA_LATCH				LATD
#define FT245_DATA_PORT					PORTD


#define CAN_HS_OFF			LATB|=CAN_HS;
#define CAN_HS_ON		LATB&=~CAN_HS;
	

#define USB_RESET_ON 				LATG|=FT245_RESET
#define USB_RESET_OFF 				LATG&=~FT245_RESET
#define USB_RESET_TOGGLE			LATG^=FT245_RESET
	
#define RX_LED_ON 					LATG|=RX_LED 
#define RX_LED_OFF					LATG&=~RX_LED 
#define RX_LED_TOGGLE 				LATG^=RX_LED 

#define TX_LED_ON 					LATG|=TX_LED 
#define TX_LED_OFF					LATG&=~TX_LED 
#define TX_LED_TOGGLE 				LATG^=TX_LED 

#define CAN_TERMINATION_ON			LATF|=CAN_TERMINATION_CTRL
#define CAN_TERMINATION_OFF			LATF&=~CAN_TERMINATION_CTRL
#define CAN_TERMINATION_TOGGLE		LATF^=CAN_TERMINATION_CTRL

#define FT245_RD_ON 			    LATD&=~FT245_RD 
#define FT245_RD_OFF				LATD|=FT245_RD 	
#define FT245_RD_TOGGLE 			LATD^=FT245_RD

#define FT245_WR_ON 				LATD|=FT245_WR 
#define FT245_WR_OFF				LATD&=~FT245_WR 
#define FT245_WR_TOGGLE 			LATD^=FT245_WR

#define FT245_TRANSMIT_READY		(!(FT245_DATA_PORT&FT245_TXE))
#define FT245_DATA_AVAILABLE		(!(FT245_DATA_PORT&FT245_RXF))

#define FT245_SWITCH_TO_WRITE		TRISD&=~FT245_DATA	
#define FT245_SWITCH_TO_READ		TRISD|=FT245_DATA


#endif

