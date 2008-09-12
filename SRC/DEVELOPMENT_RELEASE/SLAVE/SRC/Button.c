#include "Button.h"
#include <p30fxxxx.h>
#include "SystemVariables.h"
#include "EEPROM_Access.h"
#include "TimerRoutines.h"
#include "CANRoutines.h"
#include "BoardSupport.h"
#include "SystemVariables.h"
#include "CANCommands.h"
#include "DataTypes.h"


unsigned char ButtonLastState=0;

CANMsg OutgoingMsg;

void ButtonCheck(void)
{
	unsigned int ButtonState;

	ButtonState = PORTG & BUTTON_LOCATION;
	
	if ((ButtonTimer==0) && (ButtonState) && (ButtonLastState))
	{
		InitCANMsg(&OutgoingMsg);
		OutgoingMsg.SID = SLAVE_BUTTON_RELEASED;
		OutgoingMsg.Data[0] = MyNodeNumber;
		CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&OutgoingMsg);	//Send a switch off message to everybody
		ButtonLastState=0;
	}

	if (!ButtonState)
	{
		ButtonTimer=BUTTON_TIME;		
		if (!ButtonLastState)
		{
			InitCANMsg(&OutgoingMsg);
			OutgoingMsg.SID = SLAVE_BUTTON_PRESSED;
			OutgoingMsg.Data[0] = MyNodeNumber;
			CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&OutgoingMsg);
			
				if (DiagAddressSetup==TRUE)
				{
					EraseEE(0x7F,0xFC00,1);
					WriteEE(&DiagTentativeAddress,0x7F,0xFC00,1);
					MyNodeNumber=DiagTentativeAddress;
					DiagAddressSetup=FALSE;
				}

				ButtonLastState=1;			
		}
	}	

}
