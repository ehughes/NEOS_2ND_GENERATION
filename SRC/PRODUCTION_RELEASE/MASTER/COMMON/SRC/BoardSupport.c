#include <p30fxxxx.h>
#include "BoardSupport.h"


void PortInit(void)
{
		ADPCFG=0xFFFF;		//Make all digital inputs or portb can't be read
	
		MEM0_DISABLE;
		MEM1_DISABLE;
		MEM2_DISABLE;
		MEM3_DISABLE;

		TRISA &= ~(MEM0_LOC | MEM1_LOC | MEM2_LOC | MEM3_LOC);	

		TRISB &= ~(MEM_WRITE_LOC | CAN_HS_LOC | VREG_ENABLE_LOC);
		TRISB |= VIN_SENSE_ANALOG;

		CAN_TRANSCIEVER_ENABLE;
		MEM_WRITE_ENABLE;
		
		GREEN_LED_OFF;
		RED_LED_OFF;
		
		TRISD &= ~(GREEN_LED_LOCATION | RED_LED_LOCATION);
		TRISD |=  BUTTON_LOCATION_OLD;
	
		USB_RESET_DISABLE;
		TRISG |= (GROUNDED_IO_LOC | BUTTON_LOCATION); 		
		TRISG &= ~(AUDIO_SHUTDOWN_LOC | USB_RESET_LOC);
			
}		
