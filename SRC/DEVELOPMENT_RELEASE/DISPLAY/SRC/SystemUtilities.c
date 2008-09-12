#include <p30fxxxx.h>
#include <ports.h>
#include <Timer.h>
#include "SystemUtilities.h"
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "Audio.h"
#include "CANRoutines.h"
#include <stdlib.h>
#include <math.h>
#include "DataEEPROM.h"
#include "LED-Display.h"

unsigned int NodeSwitchStates[2];	//32 bits of switch states
unsigned char MasterSwitchState=0;
unsigned char MasterSwitchStateLast=0;
unsigned int NodeSwitchStatesLast[2];  //What they were last time

unsigned char PiezoLastState=0;
unsigned int PiezoTimer=0;

unsigned char Piezos[3];	// status of piezo bits
unsigned char PiezosLast[3];	//Previous state



unsigned char Scale100to63(unsigned int displayval)
{
	unsigned int roundoff;
	displayval *= 63;		//Range now 0-6300
	roundoff = displayval % 100;
	displayval /= 100;		//Range 0-63
	if (roundoff >=50)
		displayval += 1;
	return ((unsigned char)(displayval));
}

unsigned char Scale100to255(unsigned int displayval)
{
	unsigned int roundoff;
	displayval *= 255;		//Range now 0-25500
	roundoff = displayval % 100;
	displayval /= 100;		//Range 0-255
	if (roundoff >=50)
		displayval += 1;
	return ((unsigned char)(displayval));
}

unsigned int Scale255to100(unsigned char inpval)
{
	unsigned int roundoff,inp;
	inp=inpval * 100;	//Range now 0-25500
	roundoff = inp % 255;
	inp /= 255;		//Range 0-100
	if (roundoff >=128)
		inp += 1;
	return ((unsigned int)(inp));
}

unsigned int Scale63to100(unsigned char inpval)
{
	unsigned int roundoff,inp;
	inp=inpval * 100;	//Range now 0-25500
	roundoff = inp % 63;
	inp /= 63;		//Range 0-100
	if (roundoff >=32)
		inp += 1;
	return ((unsigned int)(inp));
}


void ResetAudioAndLEDS(void)
{
	AudioNodeEnable(0,0,0,0,0,0xFFFF,0xFF,0xFF);		// Master sound off 
	ResetAllSlaves();				//LEDs all sound & LEDs off
	LEDSendMessage(0,0,0,0,0,0,0,0,0);	// Master LEDs off
	AudioTimer=0;
	AudioTimeout=0xFFFF;
}

void ResetLeds(void)
{
	int temp;
	for (temp=0; temp <=15; temp+=1)
	{
		LEDSendMessage(temp,0,0,0,0,0,0,0,0);	
	}
}




void ResetGameVariables (void)
{
	int temp;	
	GameTimer1=0;
	GameTimeout1=0;			// No timeout unless this is nonzero
	GameTimer2=0;
	GameTimeout2=0;
	GameTimer3=0;

	GameScores[0]=0;
	GameScores[1]=0;

	GameShowTimer=0;
	GameShowIndex=0;

	GamePhase=0;
	GameVariable[0]=0;
	GameVariable[1]=0;
	GameVariable[2]=0;
	GameVariable[3]=0;
	GameVariable[4]=0;
	GameVariable[5]=0;

	GameInteger[0]=0;
	GameInteger[1]=0;
	GameInteger[2]=0;
	GameInteger[3]=0;

//		GameShowEnd=0;
	for (temp=0; temp <=15; temp++)
	{
		GameNodeTimer[temp]=0;
		GameNodeState[temp]=0;
		GameArray[temp]=0;
	}
	GameActiveButton[0]=0xFF;
	GameActiveButton[1]=0xFF;
	GameActiveButton[2]=0xFF;
	GameActiveButton[3]=0xFF;
	GameActiveButton[4]=0xFF;
	GameActiveButton[5]=0xFF;

	NodeSwitchStates[0]=0;			//Clear all switch states
	NodeSwitchStates[1]=0;
	NodeSwitchStatesLast[0]=0;
	NodeSwitchStatesLast[1]=0;

	Piezos[0]=0;
	Piezos[1]=0;
	Piezos[2]=0;
	PiezosLast[0]=0;
	PiezosLast[1]=0;
	PiezosLast[2]=0;

}

unsigned char SelectRandomButton(unsigned char * group)
{
	int temp= -1;
	int tries = 0;
	unsigned char button, cnt, doagain;	
	//This can get stuck if there is no available button - maybe limit it to 32 tries?
	do
	{
		tries +=1;
		doagain=0;
		temp=rand() & 0x0F ;				// Make number 0-31
		button = group[temp];	//Get the corresponding button number in this group
		for (cnt=0; cnt<6; cnt++)	
		{
			if ((button==GameActiveButton[cnt]) || (button== 0xFF) )
				doagain=1;
		}
	} while ((doagain != 0) && (tries < 200));

	if (tries>=200)
		button=group[0];		//if it failed to find one don't hang, return First one
	return(button);
}


void PortInit(void)
{
	int i=0;

	ADPCFG=0xFFFF;		//Make all digital inputs or portb can't be read

	//TRISA same for all
	LATA = 0xF000;		// Memory select lines high (off) 
	TRISA = 0x06C0;		/* PA6,7,9,10 option inps, PA12-16 MEM0,1,2,3 outputs */	

//	OptionJumpers=((PORTA & 0x600) >> 7) | ((PORTA & 0xC0) >> 6); //Bits 6,7,9,10
//
//	if (OptionJumpers==0)		//Score Display
//		ScoreDisplay=1;

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


//Further initialization for score display only
	if (ScoreDisplay==1)
	{
		LATC = 0; 			/* LEDS on PC 1,2,13,14 off */
		TRISC = 0xE006;		/* PC 1,2,13,14 switch inputs, PC 15 osc input */
	
		LATD = 0x8000;		/* LED_OE high for LEDs off */
		TRISD = 0x00FF;		// Drivers on bits 8-15 outputs, data on 0-7 inputs
	
		LATF = 0;
		TRISF = 0x0095;		
	
		LATG = 0x7;		// USB chip reset line high (off) & switch inps off
		TRISG = 0x1000;		/* RG12 is grounded so make an input, rest output */
	
	//New V1.24 - check for PB0 connected to PB1 by a jumper in the programming
	//connector. If shorted, set BURNIN to flash score display lights
		TRISB=0x42E;		//PB0 an output
		LATB = 0x200;
		i-=2;				//nop delay
		if (PORTBbits.RB1 == 0)
		{
			LATB=0x201;		//If RB0 low was read as low on RB1, try high on RB0
			i-=2;				//nop delay
			if (PORTBbits.RB1 == 1)
			{
				BurnIn=1;
			}
		}
		TRISB = 0x042F;	
		LATB=0x200;
	}

	else		//else it is a button board
	{
	#ifdef ELC0019 
		LATD = 0;			/* LEDS all off */
		TRISD = 0;
		LATG = 0x1;		// USB chip reset line high (off)
		TRISG = 0x1002;		/* RG12 is grounded so make an input, PG1 switch inp, rest output */
	
		//For ELC0025 & ELC0026, RD7 & RG1 are tied together and connected to
		//piezo switch input. For ELC0019 RD7 is a LED output.
		//Identify ELC0025/26 by verifying this connection
		//This will work unless piezo input is shorted, which is unlikely
		if (PORTGbits.RG1 == 0)		//Must be low with RD7 low
		{
			PORTDbits.RD7=1;		//Set the output high
			LATG = 0x1;		// Dummy 
			TRISG = 0x1002;		
			if (PORTGbits.RG1 == 1)
			{
				TRISD=0x0080;		//For ELC0025/26 make RD7 an input to allow piezo switch to work
				ELC0025or26=1;		//ID the board
			}
		}
		else
			ELC0025or26=0;
	
		LATC = 0; 			/* LEDS on PC 1,2,13,14 off */
		TRISC = 0x8018;		/* PC 1,2,13,14 outputs, PC 3,4,15 inputs */
		LATF = 0;
		TRISF = 0x0091;		
	
		LATB = 0x200;		// ~FORCEOFF must be high
		TRISB = 0x0007;		// All outputs except PB0,1,2
	#endif


	#ifdef ELC0002
		LATC = 0; 			/* LEDS on PC 1,2,13,14 off */
		TRISC = 0x8018;		/* PC 1,2,13,14 outputs, PC 3,4,15 inputs */
		LATD = 0;			/* LEDS all off */
		TRISD = 0x0000;			/* PORTD all outputs for LEDs */
		LATF = 0;
		TRISF = 0x0095;		
		LATG = 0x1;		// USB chip reset line high (off)
		TRISG = 0x1000;		/* RG12 is grounded so make an input, rest output */
	
		LATB = 0x200;		// ~FORCEOFF must be high
		TRISB = 0x00FF;
	
	#endif
	}
}		//End of PORTINIT routine




void ReadOptionJumpers(void)
{
	OptionJumpers=((PORTA & 0x600) >> 7) | ((PORTA & 0xC0) >> 6); //Bits 6,7,9,10
}



void ResetAllSlaves(void)
{
	CANQueueTxMessage(0xE0,0,0,0,0);
}



void RegulatorControl(void)
{
//For score display burn in keep the regulators alive
	if (BurnIn==1)
	{
		LATBbits.LATB14 = 1;	//LED & sound voltage regulators on
	}
	else
	{
	//	TRISB = 0x40FF;
		if (InactivityTimer != 0)
			LATBbits.LATB14 = 1;	//LED & sound voltage regulators on
		else
		{
			LATBbits.LATB14 = 0;	//turn LED & sound voltage regulators OFF

			//for score display turn off 12V drivers too
			if (ScoreDisplay==1)
			{
				LEDEnable[0] &= 0x7F7F;		
				LEDEnable[1] &= 0x7F7F;
				LEDEnable[2] &= 0x7F7F;
				LEDEnable[3]=0;
			}

			if ((Slave != 0) && (ScoreDisplay !=1))   //slaves only except score display
			{
				//turn off ints that I do NOT want to wake from idle
				DisableIntT2;
//				DCIDisable();
				LATGbits.LATG3=1;	//Audio amp off
				LATBbits.LATB13=1;	//CAN to listen only mode
	
				//Going into idle automatically clears watchdog
				//go back into idle if wake up is due to watchdog
				do
				{
					Idle();			//New V1.17 Idle on inactivity, slaves only
				}while ((RCONbits.WDTO==1) && (RCONbits.IDLE==1));
				RCONbits.WDTO=0;	//Reset this if wake by watchdog

				//Re-enable interrupts disabled for idle
				EnableIntT2;
//				DCIInit();
				LATGbits.LATG3=0;	//Audio amp on
				LATBbits.LATB13=0;	//CAN transciever on
			}
		}
	}
}

void CN16Init(void)
{
	ConfigIntCN(CHANGE_INT_ON & CHANGE_INT_PRI_3 &	0xFF010000);		//Enable CN16
	EnableCN16;			//Macro enables interrupt on CN16/PD7
}

//void __attribute__((__interrupt__,__auto_psv__)) _CNInterrupt( void )
void __attribute__((__interrupt__,__auto_psv__)) _CNInterrupt( void )
{
 	IFS0bits.CNIF = 0;		/* Reset interrupt flag */
}




void Piezo(void)
{
	unsigned int temp1,temp2,temp3;

	if (ScoreDisplay==1)
	{
	temp1 = Piezos[0] & (PiezosLast[0] ^ 0xFF);	//get bits that changed from 0 to 1
	temp2 = Piezos[1] & (PiezosLast[1] ^ 0xFF);	//get bits that changed from 0 to 1
	temp3 = Piezos[2] & (PiezosLast[2] ^ 0xFF);	//get bits that changed from 0 to 1
	
	if ((temp1+temp2+temp3) > 0)		//if anything is set need to send a message
		{
			CANQueueTxMessage(0xF3,MyNodeNumber,temp1+(temp2<<8),temp3,0);	//send a message to everybody else
		}
	PiezosLast[0] = Piezos[0];
	PiezosLast[1] = Piezos[1];
	PiezosLast[2] = Piezos[2];
	}

	else
	{

#ifdef ELC0002
	temp1 = PORTB;
	if ((PiezoTimer==0) && ((temp1 & 0x1) == 1) && (PIEZO==1) && (PiezoLastState==1) )
	{
		CANQueueTxMessage(0xF1,MyNodeNumber,0,0,0);	//Send a switch off message to everybody
		PiezoLastState=0;
	}

	if ( ((temp1 & 0x1) == 0) && (PIEZO==1))
	{
		PiezoTimer=PIEZOTIME;			//Set timer at 1/2 sec always when button is ON
		if (PiezoLastState==0)
		{
			#ifdef BUTTONDEMO
				GameVariable[0] +=1;
				if (GameVariable[0] > 2)
					GameVariable[0] =0;
//				GameVariable[0] +=1;
//				GameVariable[0] %=8;
			#else
				CANQueueTxMessage(0xF0,MyNodeNumber,0,0,0);	//send a message to everybody else
				//Upon button press in Address setup mode, save to EEProm
				if (DiagAddressSetup==1)
				{
					EraseEE(0x7F,0xFC00,1);
					WriteEE(&DiagTentativeAddress,0x7F,0xFC00,1);
					MyNodeNumber=DiagTentativeAddress;
					DiagAddressSetup=0;
				}
			#endif
			PiezoLastState=1;			//Piezo is ON
		}
	}	
#endif


#ifdef ELC0019
	temp1 = PORTG;
	if ((PiezoTimer==0) && ((temp1 & 0x2) == 2) && (PiezoLastState==1) )
	{
		CANQueueTxMessage(0xF1,MyNodeNumber,0,0,0);	//Send a switch off message to everybody
		PiezoLastState=0;
	}

	if ((temp1 & 0x2) == 0)
	{
		PiezoTimer=PIEZOTIME;			//Set timer at 1/2 sec always when button is ON
		if (PiezoLastState==0)
		{
			#ifdef BUTTONDEMO
				GameVariable[0] +=1;
				if (GameVariable[0] > 2)
					GameVariable[0] =0;

//				GameVariable[0] +=1;
//				GameVariable[0] %=8;
			#else
				CANQueueTxMessage(0xF0,MyNodeNumber,0,0,0);	//send a message to everybody else
				if (DiagAddressSetup==1)
				{
					EraseEE(0x7F,0xFC00,1);
					WriteEE(&DiagTentativeAddress,0x7F,0xFC00,1);
					MyNodeNumber=DiagTentativeAddress;
					DiagAddressSetup=0;
				}

			#endif
			PiezoLastState=1;			//Piezo is ON
		}
	}	
#endif
	}
}



