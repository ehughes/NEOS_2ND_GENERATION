#include <p30fxxxx.h>
#include <ports.h>
#include <Timer.h>
#include "BoardSupport.h"
#include "SystemVariables.h"
#include "Audio.h"
#include "CANRoutines.h"
#include <stdlib.h>
#include <math.h>
#include "EEPROM_Access.h"
#include "LED-Display.h"
#include "TimerRoutines.h"

void PortInit(void)
{
	ADPCFG=0xFFFF;		//Make all digital inputs or portb can't be read

	TRISB &= ~(CAN_HS_LOC | VREG_ENABLE_LOC);
	TRISB |= VIN_SENSE_ANALOG;
	
	CAN_TRANSCIEVER_ENABLE;
	VOLTAGE_REGULATOR_ENABLE;
	
	LED_OFF;
	TRISD &= ~(GREEN_LED_LOCATION | RED_LED_LOCATION);
	TRISD |=  BUTTON_LOCATION_OLD;
	
	TRISG |= (GROUNDED_IO_LOC | BUTTON_LOCATION); 		
	TRISG &= ~(AUDIO_SHUTDOWN_LOC);
	
	AUDIO_ENABLE;

}		


void RegulatorControl(void)
{
	if (InactivityTimer != 0)
	{
		 VOLTAGE_REGULATOR_ENABLE;
		 AUDIO_ENABLE;
	}	 
	else
   	{
		 VOLTAGE_REGULATOR_DISABLE;
		 AUDIO_DISABLE;
	}
}

void ResetAll()
{
		ResetCAN();
		ResetAudio();
		ResetLED();
}	


