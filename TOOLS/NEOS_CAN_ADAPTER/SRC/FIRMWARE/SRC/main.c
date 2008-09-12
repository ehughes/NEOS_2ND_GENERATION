/*****************************************************************/
//      Playworld Systems
//
//		Neos USB to Can Adapter
//      Development Release
//		-Eli Hughes
//
//
/*****************************************************************/

#include <p30fxxxx.h>
#include <Timer.h>
#include <math.h>
#include <ports.h>

#include "CANRoutines.h"
#include "TimerRoutines.h"
#include "BoardSupport.h"
#include "PacketDetect.h"
#include "SystemVariables.h"

/* We have 8.192 MHZ xtal with PLL * 8 for 65MHZ internal, 16MIPS */
/* 
 = 16,384,000 */


//Device Configuration Bits
_FOSC(CSW_FSCM_OFF & XT_PLL8);  /* Set up for XTxPLL *8 mode since */
_FBORPOR(MCLR_EN & PWRT_OFF);   /* Enable MCLR reset pin and turn off the power-up timers. */
_FGS(GEN_PROT)

/* Define constants here                            */
#define	FCY		16384000		/* Was 29491200 */

CANMsg Eli;


int main (void)
{
  SoftwareResetEntry:

	ResetCAN();
	PortInit();
	TimerInit();
	CANInit();
	CAN_HS_ON;
	CAN_TERMINATION_OFF;
    InitPacketDetect();
	SystemMode = CAN_INTERFACE;


	while (TRUE)
	{	
		
	
		if(SystemMode == RESET)
		{
			goto SoftwareResetEntry;

		}

		switch(SystemMode)
		{
			case CAN_INTERFACE:
		
				
				CANTransmitCheck();	 	//OK See if anything in TX queue can go to CAN Tx registers
				
				CANRxProcess();		   // Check for unprocessed received messages & handle them
       
       				ScanForIncomingMessages();	
        				
        	break;
        	
        	default:
        		goto SoftwareResetEntry;
        	break;
        }

	}

	return 0;
}






