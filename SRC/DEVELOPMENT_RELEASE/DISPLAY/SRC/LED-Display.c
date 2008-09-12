
#include <p30fxxxx.h>
#include "LED-Display.h"
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "CANRoutines.h"


unsigned int LEDOutputData[4];	/* up to 64 LEDs */
unsigned char LEDColor[3];	/* R,G,B 0-255 color values */
unsigned int LEDEnable[4] = {0,0,0,0}; /* [0] = red LEDs [1] = Green LEDs [2] = Blue LEDs */
//OR in the case of score display LEDENable is the bits to light
unsigned char LEDStartColor[3];	/* R,G,B 0-255 color values */
char LEDColorChange[3]; //SIGNED change values for fading colors


unsigned char LEDScoreOnTime=0;		//Flashing on time in 1/100 sec steps
unsigned char LEDScoreOffTime=0;		//Flashing off time in 1/100 sec steps
unsigned char LEDScoreOnTimer=0;		//Flashing on time in 1/100 sec steps
unsigned char LEDScoreOffTimer=0;		//Flashing off time in 1/100 sec steps
unsigned int LEDScoreFlashEnable[2]={0,0};		//Bit 0 = left flash enable bit 1 = right flash
unsigned int LEDSegmentPattern[3];	// encoded 7-segment patterns for 6 digits

unsigned int DispDefaults[8] = {0,100,100,0,0,0,0,0};	//defaults scaled for display 

int ColorSequence=0;  //Light Show variable
int ColorCounter=0;  //Light Show variable
int	DelayCounter=0;  //Light Show variable

unsigned int LEDTimer=0;	//10ms down counter until LED shutoff 10 BITS LONG
unsigned int LEDFadeTimer=0; //10ms UP counter used for fading 10 BITS LONG
unsigned int LEDFadeTime=0;  //Limit for LED 10 BITS LONG
unsigned char T2LED10msRefreshFlag=0;


unsigned char LEDMasterTimer;


char __attribute__ ((space(auto_psv))) charpatterns[26] =
{
0x77,0x7C,0x58,0x5E,0x79,	//AbcdE
0x71,0x6F,0x74,0x04,0x1E,	//FghiJ
0x78,0x38,0x37,0x54,0x5C,	//-L-no
0x73,0x5d,0x50,0x6d,0x78,	//P-rSt
0x3E,0x1c,0x49,0x76,0x66,	//Uv--y
0x5B};						//z

char __attribute__ ((space(auto_psv)))	digitpatterns[10]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F};


/* LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS */
/* LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS */
/* LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS */
/* LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS */
/* LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS LEDS */

//Handle red/green LEDs on master
void LEDMaster(void)
{
#ifdef ELC0019
	if (Slave==0)
	{
		switch (GameSelected)
		{
			case GAME_BUTTON_ADDRESS:
			case GAME_DIAGNOSTIC:
					case GAME_FULL_POWER:
				if (LEDMasterTimer >= 32)
				{
					LEDColor[0]=0;
					LEDColor[1]=63;			//Green
				}
				else
				{
					LEDColor[0]=63;			//Red
					LEDColor[1]=0;	
				}
			break;
			default:
				LEDColor[0]=0;
				LEDColor[1]=63;			//Green
			break;
		}
	}
#endif
}



// LED refresh for score display
void LEDRefresh(void)
{

	if (ScoreDisplay==1)
	{
		unsigned int count;
		for (count = 0; count <=3; count++)
		{
			TRISD = 0x0000;		//Data bus all outputs
			PORTD = (LATD & 0xFF00) + (LEDOutputData[count] & 0xFF);				// put LSB on PORTD parallel bus 
	//		LATD |= 0xFF;			// ALL ON TO TEST
	//		LATD = (LATD & 0xFF00) + (Piezos[1] & 0xFF);
			LATBbits.LATB6 = 0;						// LSB of demultiplexer address
			LATBbits.LATB7 = count & 0x1;			// 2SB of demultiplexer address
			LATBbits.LATB12 = (count >> 1) & 0x1;		// 3rd bit of demultiplexer select
			LATDbits.LATD14 = 0;				// clock low to idle
			LATDbits.LATD14 = 1;				// Clock the data on rising edge
	
			PORTD = (LATD & 0xFF00) + ((LEDOutputData[count] & 0xFF00) >> 8);		// put MSB on PORTD parallel bus 
	//		LATD |= 0xFF;			// ALL ON TO TEST
	//		PORTD = (LATD & 0xFF00) + ((Piezos[1] & 0xFF00) >> 8);		// put MSB on PORTD parallel bus 
			LATBbits.LATB6 = 1;				//LSB of demultiplexer address is now a 1
			LATDbits.LATD14 = 0;				// clock low to idle
			LATDbits.LATD14 = 1;				// Clock the data on rising edge
	
			TRISD = 0x00FF;		//make data lines inputs
			LATGbits.LATG1 = 0;	// enable LSB
			TRISD = 0x00FF;		//make data lines inputs  DUMMY INSTR NECESSARY TO GET DATA
			Piezos[0] = PORTD & 0xFF;
			LATGbits.LATG1 = 1;	// enable LSB
			
			LATGbits.LATG2 = 0;	// enable LSB
			TRISD = 0x00FF;		//make data lines inputs DUMMY INSTR NECESSARY TO GET DATA	 	Piezos[1] = PORTD & 0xFF; 
		 	Piezos[1] = PORTD & 0xFF; 
		 	LATGbits.LATG2 = 1;	// enable next 8
				
			Piezos[2] = ((PORTC & 0x6)>>1) + ((PORTC & 0x6000) >> 11);
		}
		TRISD = 0x0000;		//Data bus all outputs
		LATDbits.LATD15=0;		// ENABLE LEDS
	}
	else
	{

#ifdef ELC0019
	unsigned int count;
	unsigned long tempdata; /* 32 bits */
	LATD = LEDOutputData[0];
	LATCbits.LATC1 = LEDOutputData[1] & 0x1;	// LED 16
	LATCbits.LATC2 = (LEDOutputData[1] >> 1) & 0x1;	// LED 17
	LATCbits.LATC13 = (LEDOutputData[2] >> 2) & 0x1;   // LED 35
	LATCbits.LATC14 = (LEDOutputData[2] >> 3) & 0x1;	// LED 36

	// 12 additional lights are needed in ELC0019
	count = (LEDOutputData[2] & 0b0011111111110000) >> 1;  //Leds 37 through 46
	LATB |= count;		//set the bits that are high
//	count = ~count;			//1's compliment
	count |= 0b1110000000000111;		//don't clear these bits not associated with leds
	LATB &= count;		//clear led bits that are low
	LATFbits.LATF2 = (LEDOutputData[2] >> 14) & 0x1;	//LED47
	LATFbits.LATF3 = (LEDOutputData[2] >> 15) & 0x1;	//LED48

	tempdata = (unsigned long) ((LEDOutputData[1] & 0xFFFC) >> 2 );			/* first 14 bits */
	tempdata += (unsigned long)((LEDOutputData[2] & 0x0003) << 14);  /* next 2 bits LED 33, 34*/

	/* Send data to shift registers, MSB first */
	LATGbits.LATG7 = 1;			/* CLR signal off */
	for (count=0; count <32; count+=1)
	{
		LATGbits.LATG8 = (tempdata & 0x80000000) ? 1 : 0 ; /* set data line, true or fasle */
		tempdata = tempdata << 1;	/* shift a bit left for next time */
		LATGbits.LATG6 = 0; /* Shift register clock low */
		LATGbits.LATG6 = 1; /* Shift register clock high, data ten this edge */	
	}
	LATGbits.LATG9 = 0;
	LATGbits.LATG9 = 1;	/* Data is sent to latches on rising edge */
#endif

#ifdef ELC0002
	unsigned int count;
	unsigned long tempdata; /* 32 bits */
	LATD = LEDOutputData[0];
	LATCbits.LATC1 = LEDOutputData[1] & 0x1;	// LED 16
	LATCbits.LATC2 = (LEDOutputData[1] >> 1) & 0x1;	// LED 17
	LATCbits.LATC13 = (LEDOutputData[2] >> 2) & 0x1;   // LED 35
	LATCbits.LATC14 = (LEDOutputData[2] >> 3) & 0x1;	// LED 36

	tempdata = (unsigned long) ((LEDOutputData[1] & 0xFFFC) >> 2 );			/* first 14 bits */
	tempdata += (unsigned long)((LEDOutputData[2] & 0x0003) << 14);  /* next 2 bits LED 33, 34*/

	/* Send data to shift registers, MSB first */
	LATGbits.LATG7 = 1;			/* CLR signal off */
	for (count=0; count <32; count+=1)
	{
		LATGbits.LATG8 = (tempdata & 0x80000000) ? 1 : 0 ; /* set data line, true or fasle */
		tempdata = tempdata << 1;	/* shift a bit left for next time */
		LATGbits.LATG6 = 0; /* Shift register clock low */
		LATGbits.LATG6 = 1; /* Shift register clock high, data ten this edge */	
	}
	LATGbits.LATG9 = 0;
	LATGbits.LATG9 = 1;	/* Data is sent to latches on rising edge */
#endif
	}

}


// Encode numeric score 0-999 to score 
//1000 or greater are blanked
// LED Decode for score display
// really only needed in master
void LEDEncode(unsigned int score1, unsigned int score2)
{
	unsigned int temp1,temp2;
		
	if (score2 <= 999)
	{
		//Start on right with LSD of score2
		LEDSegmentPattern[2] = (LEDMake7Seg (score2 % 10) << 8);  // in MSB of pattern 2
		temp1 = LEDMake7Seg ((score2 / 10) % 10);		
		temp2 = LEDMake7Seg ((score2/100) % 10);		//MSD of score2
		if (temp2 == 0x3F)					//Do lead zero blanking
			temp2=0;
		if ( (temp2==0) && (temp1==0x3F) )	//2nd zero blanking
			temp1=0;
		LEDSegmentPattern[2] += temp1;	//Store 2SD or blank
		LEDSegmentPattern[1] = temp2 << 8; 	//Store MSD or blank
	}
	else
	{
		LEDSegmentPattern[2]=0;		//Blank all 3
		LEDSegmentPattern[1]=0;
	}

	if (score1 <= 999)
	{
		//Now Score 1
		LEDSegmentPattern[1] += LEDMake7Seg (score1 % 10);
		temp1 = LEDMake7Seg ((score1 / 10) % 10);		
		temp2 = LEDMake7Seg ((score1 / 100) % 10);		//MSD of score1
		if (temp2 == 0x3F)					//Do lead zero blanking
			temp2=0;
		if ( (temp2==0) && (temp1==0x3F) )	//2nd zero blanking
			temp1=0;
		LEDSegmentPattern[0] = (temp1<<8) + temp2;
	}
	else
	{
		LEDSegmentPattern[1] &= 0xFF00;		//Blank all 3
		LEDSegmentPattern[0]=0;
	}
	
}

unsigned int LEDMake7Seg(unsigned int value)	
{
	unsigned int pattern;
	pattern=digitpatterns[value];
	return(pattern);
}



//this sends either "P1" or "P1  P2" to the score display during game selection
void LEDSendScoreP1P2(unsigned int node, unsigned int brightness, unsigned int players)
{
	LEDSegmentPattern[0] = 	0x0673;		// "P1"	in leftmost 2 digits
	LEDSegmentPattern[1] = 0;			//center 2 digits blank
	if (players==2)
		LEDSegmentPattern[2] = 0x5B73;	//"P2" in rightmost 2 digits
	else
		LEDSegmentPattern[2]=0;
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}

//Send numeric data to score display
//Values of 1000 or greater are blanked
void LEDSendScores(unsigned int node, unsigned int brightness, unsigned int score1, unsigned int score2)
{
	LEDEncode(score1, score2);
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}

//This writes "Adr" in left digits & shows score in right
void LEDSendScoreAddress(unsigned int node, unsigned int brightness, unsigned int score1,unsigned int score2)
{
	LEDEncode(score1, score2);
	LEDSegmentPattern[0] = 	0x5E77;		// "Ad"	in leftmost 2 digits
	LEDSegmentPattern[1] &= 0xFF00;		//Blank the 3rd dight
	LEDSegmentPattern[1] |= 0x50;		//"r"
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}

void LEDSendCharsandVar(unsigned int node, unsigned int brightness, unsigned int score1,unsigned int score2,unsigned char c1, unsigned char c2, unsigned char c3)
{
	LEDEncode(score1, score2);
	LEDSegmentPattern[0] = 	charpatterns[c1-'A'] + (charpatterns[c2-'A'] <<8);		// "Ad"	in leftmost 2 digits
	LEDSegmentPattern[1] &= 0xFF00;		//Blank the 3rd dight
	LEDSegmentPattern[1] |= charpatterns[c3-'A'];
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}

void LEDSendChars(unsigned int node, unsigned int brightness, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4, unsigned char c5, unsigned char c6)
{
	LEDSegmentPattern[0] = charpatterns[c1-'A'] + (charpatterns[c2-'A'] <<8);		// "Ad"	in leftmost 2 digits
	LEDSegmentPattern[1] = charpatterns[c3-'A'] + (charpatterns[c4-'A'] <<8);
	LEDSegmentPattern[2] = charpatterns[c5-'A'] + (charpatterns[c6-'A'] <<8);
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}



void LEDCANCommand(void)
{
	if ( (CANRxBuff[CANRxNextToProcess][4] >> 8) == MyNodeNumber)
	{
	LEDColor[0] = CANRxBuff[CANRxNextToProcess][1] & 0x3F;
	LEDColor[1] = (CANRxBuff[CANRxNextToProcess][1] & 0x0FC0) >> 6;
	LEDColor[2] = ((CANRxBuff[CANRxNextToProcess][1] & 0xF0) >> 12) + 
		((CANRxBuff[CANRxNextToProcess][2] & 0x03) << 4);

	LEDStartColor[0]=LEDColor[0];
	LEDStartColor[1]=LEDColor[1];
	LEDStartColor[2]=LEDColor[2];

	LEDColorChange[0] = ((CANRxBuff[CANRxNextToProcess][2] & 0x00FC) >> 2 ) - LEDColor[0];
	LEDColorChange[1] = ((CANRxBuff[CANRxNextToProcess][2] & 0x3F00) >> 8) - LEDColor[1];
	LEDColorChange[2] = ( ((CANRxBuff[CANRxNextToProcess][2] & 0xC000) >> 14) +
		((CANRxBuff[CANRxNextToProcess][3] & 0x000F) << 2) ) - LEDColor[2];

	LEDTimer = (CANRxBuff[CANRxNextToProcess][3] & 0x3FF0) >> 4;	
	LEDFadeTime = ((CANRxBuff[CANRxNextToProcess][3] & 0xC000) >> 14) +
		((CANRxBuff[CANRxNextToProcess][4] & 0xFF) << 2);
	LEDFadeTimer= 0;
	}
}



void LEDTimerCheck(void)
{
	long fadetemp;
	if (T2LED10msRefreshFlag != 0)
	{

	LEDMasterTimer+=1;
	LEDMasterTimer &= 0x3F;		//Counts 0-63
		
	if (ScoreDisplay==1)		//score flashing timers
	{
		if (LEDScoreOffTimer>0)
		{
			LEDScoreOffTimer-=1;
			if (LEDScoreOffTimer==0)				
			{
				LEDScoreOnTimer=LEDScoreOnTime;
			}
		}
		if (LEDScoreOnTimer>0)
		{
			LEDScoreOnTimer-=1;
			if (LEDScoreOnTimer==0)				
			{
				LEDScoreOffTimer=LEDScoreOffTime;
			}
		}
	}

		if ((LEDFadeTime >0)	&& (LEDFadeTimer < LEDFadeTime))		//See if a fade is in place
		{
			LEDFadeTimer+=1;		//Increment first so that the last time through these two are equal
			fadetemp=(LEDFadeTimer*0x100) /LEDFadeTime;	//shifted up 8 to increase precision
			fadetemp *= LEDColorChange[0];
			fadetemp /= 0x100;
			LEDColor[0] = (LEDStartColor[0] + fadetemp) & 0x3F;

			fadetemp=(LEDFadeTimer*0x100) /LEDFadeTime;	//shifted up 8 to increase precision
			fadetemp *= LEDColorChange[1];
			fadetemp /= 0x100;
			LEDColor[1] = (LEDStartColor[1] + fadetemp) & 0x3F;

			fadetemp=(LEDFadeTimer*0x100) /LEDFadeTime;	//shifted up 8 to increase precision
			fadetemp *= LEDColorChange[2];
			fadetemp /= 0x100;
			LEDColor[2] = (LEDStartColor[2] + fadetemp) & 0x3F;

//			LEDColor[0] = (LEDStartColor[0] + ((LEDColorChange[0] * LEDFadeTimer)/LEDFadeTime)) & 0x3F;
//			LEDColor[1] = (LEDStartColor[1] + ((LEDColorChange[1] * LEDFadeTimer)/LEDFadeTime)) & 0x3F;
//			LEDColor[2] = (LEDStartColor[2] + ((LEDColorChange[2] * LEDFadeTimer)/LEDFadeTime)) & 0x3F;
		}
		if (LEDTimer > 0)			//If timer is nonzero it is running
		{
			LEDTimer-=1;
			if (LEDTimer == 0)		//if timer transitions to 0 turn them all off
			{
				LEDColor[0]=0;
				LEDColor[1]=0;
				LEDColor[2]=0;
			}
		}
		T2LED10msRefreshFlag=0;
	}
}
	
void LEDSendMessage(unsigned char nodenumber, unsigned char redbrt, unsigned char greenbrt, unsigned char bluebrt,
unsigned char redfade, unsigned char greenfade, unsigned char bluefade,
unsigned int ledtime, unsigned int fadetime)
{
	unsigned int word1,word2,word3,word4;
		
	unsigned int i;

#ifdef ROWFAKE
	if (nodenumber==4)
		nodenumber=1;
	else if (nodenumber==8)
		nodenumber=2;
	else if (nodenumber==12)
		nodenumber=3;
	else if (nodenumber==1)
		nodenumber=4;
	else if (nodenumber==2)
		nodenumber=8;
	else if (nodenumber==3)
		nodenumber=12;
#endif

	i=redbrt*(RedBrightness+1);
	redbrt = (i >> 6);
	i=redfade*(RedBrightness+1);
	redfade = (i >> 6);
	i=greenbrt*(GreenBrightness+1);
	greenbrt = (i >> 6);
	i=greenfade*(GreenBrightness+1);
	greenfade = (i >> 6);
	
	word1 = (redbrt & 0x3f) + ((greenbrt & 0x3f)<<6) + ((bluebrt & 0x0f)<<12);
	word2 = ((bluebrt & 0x30)>>4) + ((redfade & 0x3f)<<2) + 
		((greenfade & 0x3F)<<8) + ((bluefade & 0x3) <<14);
	word3 = ((bluefade & 0x3C)>>2) + ((ledtime & 0x3FF)<<4) + ((fadetime & 0x3)<<14);
	word4 = ((fadetime & 0x3FC)>>2) + ((nodenumber & 0xFF)<<8);
	CANQueueTxMessage(0x010,word1,word2,word3,word4);
}

void ScoreSendLights(unsigned char nodenumber, unsigned int Data0to15, unsigned int Data16to19)
{
	CANQueueTxMessage(0x72,nodenumber,Data0to15,Data16to19,0);
}




void LEDSendVariable(unsigned int indx, unsigned int value)
{
	char c1,c2,c3;
	switch(indx)
	{
	case 0:
		c1='V';
		c2='O';
		c3='L';
		break;
//	case 1:
//		c1='V';
//		c2='B';
//		c3='G';
//		break;
//	case 2:
//		c1='N';
//		c2='A';
//		c3='N';
//		break;
	case 1:
		c1='B';
		c2='R';
		c3='I';
		break;
//	case 4:
//		c1='B';
//		c2='G';
//		c3='R';
//		break;
//	case 5:
//		c1='B';
//		c2='S';
//		c3='D';
//		break;
//	case 6:
//		c1='D';
//		c2='F';
//		c3='F';
//		break;
	}
	LEDSendCharsandVar(248,ScoreBrightness, 0, value, c1,c2,c3);
}
