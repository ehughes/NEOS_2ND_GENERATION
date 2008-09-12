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
	int i=0;

	ADPCFG=0xFFFF;		//Make all digital inputs or portb can't be read

	//TRISA same for all
	LATA = 0xF000;		// Memory select lines high (off) 
	TRISA = 0x06C0;		/* PA6,7,9,10 option inps, PA12-16 MEM0,1,2,3 outputs */	



//On score display, RB10 connected to RB11. On button, both are NC
//SAFELY detect this without turning on anything else
	TRISB=0xF7FF;		//PB11 an output
	LATB=0;
	i-=2;				//nop delay
	
	if (PORTBbits.RB10 == 0)
	{
		LATB=0x800;		//If RB11 low was read as low on RB10, try high on RB11
		i-=2;				//nop delay
		if (PORTBbits.RB10 == 1)
		{
			ScoreDisplay=1;
		}
	}




		LATD = 0;			/* LEDS all off */
		TRISD = 0;
		LATG = 0x1;		// USB chip reset line high (off)
		TRISG = 0x1002;		
		TRISD=0x0080;		//For ELC0025/26 make RD7 an input to allow piezo switch to work
	
	
	
		LATC = 0; 			/* LEDS on PC 1,2,13,14 off */
		TRISC = 0x8018;		/* PC 1,2,13,14 outputs, PC 3,4,15 inputs */
		LATF = 0;
		TRISF = 0x0091;		
	
		LATB = 0x200;		// ~FORCEOFF must be high
		TRISB = 0x0007;		// All outputs except PB0,1,2

}		


void ReadOptionJumpers(void)
{
	OptionJumpers=((PORTA & 0x600) >> 7) | ((PORTA & 0xC0) >> 6); //Bits 6,7,9,10
}

void RegulatorControl(void)
{
	if (InactivityTimer != 0)
		LATBbits.LATB14 = 1;	//LED & sound voltage regulators on
	else
    	{
			LATBbits.LATB14 = 0;	//turn LED & sound voltage regulators OFF
		}

}



void ResetAll()
{
		ResetCAN();
		ResetAudio();
		ResetLED();
}	


