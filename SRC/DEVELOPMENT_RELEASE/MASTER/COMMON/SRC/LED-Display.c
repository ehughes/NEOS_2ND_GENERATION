#include <p30fxxxx.h>
#include "LED-Display.h"
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "CANRoutines.h"
#include "EEPROMRoutines.h"
#include "DataTypes.h"

WORD LEDSegmentPattern[3];	// encoded 7-segment patterns for 6 digits
WORD DispDefaults[8] = {0,100,100,0,0,0,0,0};	//defaults scaled for display 


#define SCORE_NORMAL     0x0
#define SCORE_FLASHING   0x1
#define SCORE_BLANK      0x2

WORD P1ScoreUpdateTimer;
WORD P2ScoreUpdateTimer;

BYTE P1ScoreDisplayState;
BYTE P2ScoreDisplayState;
WORD P1ScoreOutput;
WORD P2ScoreOutput;
BOOL P1FlashState;
BOOL P2FlashState;

BYTE P1ScoreFlashTime;
BYTE P2ScoreFlashTime;

WORD ScoreManagerTimer;
BOOL ScoreManagerEnabled;
BYTE ScoreManagerUpdateRate;


CHAR __attribute__ ((space(auto_psv))) CHARpatterns[26] =
{
0x77,0x7C,0x58,0x5E,0x79,	//AbcdE
0x71,0x6F,0x74,0x04,0x1E,	//FghiJ
0x78,0x38,0x37,0x54,0x5C,	//-L-no
0x73,0x5d,0x50,0x6d,0x78,	//P-rSt
0x3E,0x1c,0x49,0x76,0x66,	//Uv--y
0x5B};						//z

CHAR __attribute__ ((space(auto_psv)))	digitpatterns[11]={0x3F,0x06,0x5B,0x4F,0x66,0x6D,0x7D,0x07,0x7F,0x6F,0x00};





WORD LEDMake7Seg(WORD value)	
{
	WORD pattern;
	pattern=digitpatterns[value];
	return(pattern);
}



//Handle red/green LEDs on master

// Encode numeric score 0-999 to score 
//1000 or greater are blanked
// LED Decode for score display
// really only needed in master


void LEDEncode(WORD score1, WORD score2)
{
	WORD temp1,temp2;
		
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



//this sends either "P1" or "P1  P2" to the score display during game selection
void LEDSendScoreP1P2(WORD node, WORD brightness, WORD players)
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
void LEDSendScores(WORD node, WORD brightness, WORD score1, WORD score2)
{
	LEDEncode(score1, score2);
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}

//This writes "Adr" in left digits & shows score in right
void LEDSendScoreAddress(WORD node, WORD brightness, WORD score1,WORD score2)
{
	LEDEncode(score1, score2);
	LEDSegmentPattern[0] = 	0x5E77;		// "Ad"	in leftmost 2 digits
	LEDSegmentPattern[1] &= 0xFF00;		//Blank the 3rd dight
	LEDSegmentPattern[1] |= 0x50;		//"r"
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}

void LEDSendCHARsandVar(WORD node, WORD brightness, WORD score1,WORD score2,BYTE  c1, BYTE  c2, BYTE  c3)
{
	LEDEncode(score1, score2);
	LEDSegmentPattern[0] = 	CHARpatterns[c1-'A'] + (CHARpatterns[c2-'A'] <<8);		// "Ad"	in leftmost 2 digits
	LEDSegmentPattern[1] &= 0xFF00;		//Blank the 3rd dight
	LEDSegmentPattern[1] |= CHARpatterns[c3-'A'];
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}

void LEDSendCHARs(WORD node, WORD brightness, BYTE  c1, BYTE  c2, BYTE  c3, BYTE  c4, BYTE  c5, BYTE  c6)
{
	LEDSegmentPattern[0] = CHARpatterns[c1-'A'] + (CHARpatterns[c2-'A'] <<8);		// "Ad"	in leftmost 2 digits
	LEDSegmentPattern[1] = CHARpatterns[c3-'A'] + (CHARpatterns[c4-'A'] <<8);
	LEDSegmentPattern[2] = CHARpatterns[c5-'A'] + (CHARpatterns[c6-'A'] <<8);
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}

void LEDSendMessage(BYTE  nodenumber, BYTE  redbrt, BYTE  greenbrt, BYTE  bluebrt,
BYTE  redfade, BYTE  greenfade, BYTE  bluefade,
WORD ledtime, WORD fadetime)
{
	WORD word1,word2,word3,word4;

	word1 = (redbrt & 0x3f) + ((greenbrt & 0x3f)<<6) + ((bluebrt & 0x0f)<<12);
	word2 = ((bluebrt & 0x30)>>4) + ((redfade & 0x3f)<<2) + 
		((greenfade & 0x3F)<<8) + ((bluefade & 0x3) <<14);
	word3 = ((bluefade & 0x3C)>>2) + ((ledtime & 0x3FF)<<4) + ((fadetime & 0x3)<<14);
	word4 = ((fadetime & 0x3FC)>>2) + ((nodenumber & 0xFF)<<8);
	CANQueueTxMessage(0x010,word1,word2,word3,word4);
}

void ScoreSendLights(BYTE  nodenumber, WORD Data0to15, WORD Data16to19)
{
	CANQueueTxMessage(0x72,nodenumber,Data0to15,Data16to19,0);
}




void LEDSendVariable(WORD indx, WORD value)
{
	CHAR c1,c2,c3;
	
	switch(indx)
	{
	case DISPLAY_VOL:
		c1='V';
		c2='O';
		c3='L';
		break;

	case DISPLAY_BRI:
		c1='B';
		c2='R';
		c3='I';
		break;

	case DISPLAY_ERR:
		c1='E';
		c2='R';
		c3='R';
	break;
	
	case DISPLAY_DIA:
		c1='D';
		c2='I';
		c3='A';
		
	break;	
		
	case DISPLAY_VER:
		c1='V';
		c2='E';
		c3='R';
	
	break;

	}
	LEDSendCHARsandVar(248,0xFF, 0, value, c1,c2,c3);
}


void LEDSendDigits(WORD node, BYTE brightness, BYTE d1, BYTE d2, BYTE d3, BYTE d4, BYTE d5, BYTE d6)
{
	LEDSegmentPattern[0] = digitpatterns[d1] + (digitpatterns[d2] <<8);		
	LEDSegmentPattern[1] = digitpatterns[d3] + (digitpatterns[d4] <<8);
	LEDSegmentPattern[2] = digitpatterns[d5] + (digitpatterns[d6] <<8);
	CANQueueTxMessage(0x70,node + (brightness << 8),LEDSegmentPattern[0],
	LEDSegmentPattern[1],LEDSegmentPattern[2]);
}


void DisplayGameCount()
{
	BYTE Digit[6];
	
	Digit[5] = ((GameCount %10));
	Digit[4] = ((GameCount / 10) % 10);
	Digit[3] = ((GameCount / 100) % 10);
	Digit[2] = ((GameCount / 1000) % 10);
	Digit[1] = ((GameCount / 10000) % 10);
	Digit[0] = ((GameCount / 100000) % 10);
	 
	LEDSendDigits(DISPLAY_ADDRESS,0xFF, Digit[0], Digit[1], Digit[2], Digit[3], Digit[4], Digit[5]);
}






void InitScoreDisplayerManager()
{
P1ScoreUpdateTimer=0;
P2ScoreUpdateTimer=0;

P1ScoreDisplayState = SCORE_NORMAL;
P2ScoreDisplayState = SCORE_NORMAL;
P1ScoreOutput = 0;
P2ScoreOutput = 0;
P1FlashState = FALSE;
P2FlashState = FALSE;
	
P1ScoreFlashTime = 25;
P2ScoreFlashTime = 25;	
ScoreManagerUpdateRate = 1;
ScoreManagerTimer = 0;
ScoreManagerEnabled = FALSE;
}	

void ScoreDisplayManager()
{

if(ScoreManagerEnabled == TRUE)
	{	
		switch(P1ScoreDisplayState)
		{
			case SCORE_NORMAL:
			default:
			
			if(Player1Score > 999)
			{ 
				Player1Score = 999;
				P1ScoreOutput = 999;
			}
			else
			{
				P1ScoreOutput = Player1Score;
			}
			
			
			break;
				
			case SCORE_FLASHING:
			
			if(P1ScoreUpdateTimer > P1ScoreFlashTime)
			{
				P1ScoreUpdateTimer = 0;
				
				if(P1FlashState == FALSE)
				{
					P1FlashState = TRUE;
					P1ScoreOutput = Player1Score;
				}
				else
				{
					P1FlashState = FALSE;
					P1ScoreOutput = BLANK_SCORE;
				}
				
			}
			break;
			
			case SCORE_BLANK:
				P1ScoreOutput = BLANK_SCORE;
			break;
		}
		
		
		switch(P2ScoreDisplayState)
		{
			case SCORE_NORMAL:
			default:
			
			if(Player2Score > 999)
			{ 
				Player2Score = 999;
				P2ScoreOutput = 999;
			}
			else
			{
				P2ScoreOutput = Player2Score;
			}
			
			
			break;
				
			case SCORE_FLASHING:
			
			if(P2ScoreUpdateTimer > P1ScoreFlashTime)
			{
				P2ScoreUpdateTimer = 0;
				
				if(P2FlashState == FALSE)
				{
					P2FlashState = TRUE;
					P2ScoreOutput = Player2Score;
				}
				else
				{
					P2FlashState = FALSE;
					P2ScoreOutput = BLANK_SCORE;
				}
				
			}
			break;
			
			case SCORE_BLANK:
				P2ScoreOutput = BLANK_SCORE;
			break;
		}
		
		if(ScoreManagerTimer > ScoreManagerUpdateRate)
		{
			ScoreManagerTimer = 0;
			LEDSendScores(DISPLAY_ADDRESS, 0xFF, P1ScoreOutput, P2ScoreOutput);
			
		}
	}
	
}	

void SyncDisplayFlashing()
{
	P1ScoreUpdateTimer  = 0;
	P2ScoreUpdateTimer  = 0;
	P1FlashState = FALSE;
	P2FlashState = FALSE;
}	


