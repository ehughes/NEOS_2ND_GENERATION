#include "__Game_Double_Dots.h"
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "_RootButtonHandler.h"
#include "_GameProcess.h"
#include <stdlib.h>
#include "LED-Display.h"
#include "SystemUtilities.h"
#include "Audio.h"
#include "CanRoutines.h"
#include "DataTypes.h"
#include "SoundMapping.h"
#include "GameMapping.h"
#include "InternalButtonSoundMapping.h"
#include "TimerRoutines.h"
//*************************************************
//*******Game Parameters***************************
//*************************************************

#define DOUBLE_DOTS_TRANSITION_TIME 100
#define HIT_THRESHOLD	50
#define MAX_HIT_TIME	75
#define DOUBLE_DOTS_POINT_1P	25
#define DOUBLE_DOTS_POINT_2P	25
#define DOUBLE_DOTS_POINT_LEVEL4	50
#define DOUBLE_DOTS_POINT_LEVEL3	25
#define DOUBLE_DOTS_POINT_LEVEL2	10
#define DOUBLE_DOTS_POINT_LEVEL1	5
#define DOUBLE_DOTS_LEVEL4_TIME	(HIT_THRESHOLD>>4)
#define DOUBLE_DOTS_LEVEL3_TIME	(HIT_THRESHOLD>>2)
#define DOUBLE_DOTS_LEVEL2_TIME	(HIT_THRESHOLD>>1)
#define DOUBLE_DOTS_LEVEL1_TIME	(HIT_THRESHOLD)
#define DOUBLE_DOTS_TIME_OUT 		DDBACKGROUND_WAV_LENGTH
//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define BACKGROUND_MUSIC_STREAM				0x00
#define CORRECT_SOUND_EFFECT_STREAM_1P    	0x01
#define CORRECT_SOUND_EFFECT_STREAM_2P    	0x02
#define FX_STREAM    						0x03

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define DOUBLE_DOTS					0x01
#define DOUBLE_DOTS_END				0x02
#define DOUBLE_DOTS_TRANSITION		0x03
#define DOUBLE_DOTS_LAST_NOTE		0x04

//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER		 			GPTimer[0]
#define HIT_TIMER_P1				    GPTimer[1]
#define HIT_TIMER_P2				    GPTimer[2]
#define DOUBLE_DOTS_TRANSITION_TIMER    GPTimer[3]
#define PIXIE_TIMER					    GPTimer[4]

//*************************************************
//*******Game Variables****************************
//*************************************************

static BYTE Dots[4];
static BOOL DotHit[4];


static BYTE PixieTime;
static BYTE PixieStartRed;
static BYTE PixieStartGreen;
static BYTE PixieStopRed;
static BYTE PixieStopGreen;
static BYTE SeperationTime_1P = 0;
static BYTE SeperationTime_2P = 0;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void GetNextDots1P();
void GetNextDots2P(BYTE Player);
	
void MoveToDoubleDotsEnd();
void GetNextPixieColor();
void DoubleDotAccepted(BYTE Player);
void DoubleDotNotAccepted(BYTE Player);
void DoubleDotsPlayCorrect(BYTE Player);
void MoveToDoubleDotsLastNote();
void TimeoutDots();
void CheckForDoubleDotHit(BYTE button);

void DoubleDots(void)
{
			
	switch (GameState)
	{
		
		case INIT:
			//ResetAudioAndLEDS();
	
		
		
			if(GamePlayers == 1)
			{
				GetNextDots1P();
			
				Player1Score = 0;
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_BLANK;
			}
			else
			{
				GetNextDots2P(PLAYER_1);
				GetNextDots2P(PLAYER_2);
			
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_NORMAL;
			}
			
			GetNextPixieColor();
	
			ScoreManagerEnabled = TRUE;
			GameState = DOUBLE_DOTS;

			HIT_TIMER_P1 = 0x0;
			HIT_TIMER_P2 = 0x0;
			
			MAIN_GAME_TIMER = 0x0;
			
			SendNodeNOP();	
			SendNodeNOP();	
			SendNodeNOP();	
		//	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,DDBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,DDBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,DDBACKGROUND_WAV);
			
		break;

		
		case DOUBLE_DOTS:
						
			if(MAIN_GAME_TIMER>DOUBLE_DOTS_TIME_OUT)
			{
				 MoveToDoubleDotsLastNote();
			}
			
			if( PIXIE_TIMER >= PixieTime-1)
			{
				PIXIE_TIMER = 0;
				GetNextPixieColor();
			}
			
			TimeoutDots();
			
		break;
		
		case DOUBLE_DOTS_END:
		if(MAIN_GAME_TIMER>ENDING_WAV_LENGTH)
			{
				ResetToGameSelector();
			}
		break;
		
		
		case DOUBLE_DOTS_LAST_NOTE:
		if(MAIN_GAME_TIMER>DDEND_WAV_LENGTH)
			{
				MoveToDoubleDotsEnd();
			}
		break;

		default:
		GameState = INIT;
		break;

		}
}



void OnButtonPressDoubleDots( BYTE  button)
{
	switch(GameState)
	{
		case DOUBLE_DOTS:
		case DOUBLE_DOTS_LAST_NOTE:
					
		CheckForDoubleDotHit(button);
		
		break;		
	}
}


void CheckForDoubleDotHit(BYTE button)
{

		
			if((DotHit[0] == FALSE) && (DotHit[1] == FALSE))
			{
				if(button == Dots[0])
				{
					DotHit[0] = TRUE;
					HIT_TIMER_P1 = 0;	
				}
				else if(button == Dots[1])
				{
					DotHit[1] = TRUE;
					HIT_TIMER_P1 = 0;	
				}
			}
			else if((DotHit[0] == TRUE) || (DotHit[1] == TRUE))
			{
				if((button == Dots[0]) && (DotHit[0] == FALSE))	
				{
					SeperationTime_1P = HIT_TIMER_P1;
					if(SeperationTime_1P < HIT_THRESHOLD)
					{	
						DoubleDotAccepted(PLAYER_1);
					}
					else
					{
						DoubleDotNotAccepted(PLAYER_1);	
					}
				}
				else if((button == Dots[1]) && (DotHit[1] == FALSE))	
				{
					if(HIT_TIMER_P1 < HIT_THRESHOLD)
					{	
						SeperationTime_1P = HIT_TIMER_P1;
						DoubleDotAccepted(PLAYER_1);
					}
					else
					{
						DoubleDotNotAccepted(PLAYER_1);	
					}
				}
			}
		
	
	
	if(GamePlayers == 2)
	{
			
			if((DotHit[2] == FALSE) && (DotHit[3] == FALSE))
			{
				if(button == Dots[2])
				{
					DotHit[2] = TRUE;
					HIT_TIMER_P2 = 0;	
				}
				else if(button == Dots[1])
				{
					DotHit[3] = TRUE;
					HIT_TIMER_P2 = 0;	
				}
			}
			else if((DotHit[2] == TRUE) || (DotHit[3] == TRUE))
			{
				if((button == Dots[2]) && (DotHit[2] == FALSE))	
				{
					SeperationTime_2P = HIT_TIMER_P2;
					if(SeperationTime_2P < HIT_THRESHOLD)
					{	
						DoubleDotAccepted(PLAYER_2);
				
					}
					else
					{
						DoubleDotNotAccepted(PLAYER_2);	
					}
				}
				else if((button == Dots[3]) && (DotHit[3] == FALSE))	
				{
					if(HIT_TIMER_P2 < HIT_THRESHOLD)
					{	
						SeperationTime_2P = HIT_TIMER_P2;
						DoubleDotAccepted(PLAYER_2);
					}
					else
					{
						DoubleDotNotAccepted(PLAYER_2);	
					}
				}
			}
		
	}	

}	


void DoubleDotAccepted(BYTE Player)
{
	
	if(Player == PLAYER_1)
	{
		DotHit[0] = FALSE;
		DotHit[1] = FALSE;
		DoubleDotsPlayCorrect(PLAYER_1);
	
		LEDSendMessage(Dots[0],LEDOFF,LEDOFF,0,0);
		LEDSendMessage(Dots[1],LEDOFF,LEDOFF,0,0);
		
		if(GamePlayers == 1)
		{
			GetNextDots1P();
			GetNextPixieColor();
		}
		else
		{	
			GetNextDots2P(PLAYER_1);
			GetNextPixieColor();
		}
				 if(SeperationTime_1P < DOUBLE_DOTS_LEVEL4_TIME) {Player1Score +=  DOUBLE_DOTS_POINT_LEVEL4;}
			else if(SeperationTime_1P < DOUBLE_DOTS_LEVEL3_TIME) {Player1Score +=  DOUBLE_DOTS_POINT_LEVEL3;}
			else if(SeperationTime_1P < DOUBLE_DOTS_LEVEL2_TIME) {Player1Score +=  DOUBLE_DOTS_POINT_LEVEL2;}
			else 											     {Player1Score +=  DOUBLE_DOTS_POINT_LEVEL1;}
 	}
 	else
 	{
	 	DotHit[2] = FALSE;
		DotHit[3] = FALSE;
		DoubleDotsPlayCorrect(PLAYER_2);
		
		LEDSendMessage(Dots[2],LEDOFF,LEDOFF,0,0);
		LEDSendMessage(Dots[3],LEDOFF,LEDOFF,0,0);
		
		GetNextDots2P(PLAYER_2);
		GetNextPixieColor();
		
				 if(SeperationTime_2P < DOUBLE_DOTS_LEVEL4_TIME) {Player2Score +=  DOUBLE_DOTS_POINT_LEVEL4;}
			else if(SeperationTime_2P < DOUBLE_DOTS_LEVEL3_TIME) {Player2Score +=  DOUBLE_DOTS_POINT_LEVEL3;}
			else if(SeperationTime_2P < DOUBLE_DOTS_LEVEL2_TIME) {Player2Score +=  DOUBLE_DOTS_POINT_LEVEL2;}
			else 											  	 {Player2Score +=  DOUBLE_DOTS_POINT_LEVEL1;}	
	 	
	}	
 		 
}	


void DoubleDotNotAccepted(BYTE Player)
{
	if(Player == PLAYER_1)
	{
		DotHit[0] = FALSE;
		DotHit[1] = FALSE;	
	}
	else
	{
		DotHit[2] = FALSE;
		DotHit[3] = FALSE;
	}	
}	

void TimeoutDots()
{
	if(((DotHit[0] == TRUE) || (DotHit[1] == TRUE)) && (HIT_TIMER_P1 > MAX_HIT_TIME))
		{
			DotHit[0] = FALSE;
			DotHit[1] = FALSE;
		}
			
	if(GamePlayers == 2)
		{
			if(((DotHit[2] == TRUE) || (DotHit[3] == TRUE)) && (HIT_TIMER_P2 > MAX_HIT_TIME))
				{
					DotHit[2] = FALSE;
					DotHit[3] = FALSE;
				}
		}
}	

void MoveToDoubleDotsEnd()
{
		GameState = DOUBLE_DOTS_END;
		MAIN_GAME_TIMER = 0;
		AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
		SendNodeNOP();	
		EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
		LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
	
		if(GamePlayers == 1)
		{
			P1ScoreDisplayState = SCORE_FLASHING;
			P2ScoreDisplayState = SCORE_BLANK;
		}
		
		else
		{
		  SyncDisplayFlashing();
			if(Player1Score > Player2Score)
				{
					P1ScoreDisplayState = SCORE_FLASHING;
					P2ScoreDisplayState = SCORE_NORMAL;
				}
			else if(Player2Score > Player1Score)
				{
						P2ScoreDisplayState = SCORE_FLASHING;
						P1ScoreDisplayState = SCORE_NORMAL;
				}
			else 
				{
						P2ScoreDisplayState = SCORE_FLASHING;
						P1ScoreDisplayState = SCORE_FLASHING;
				}
		}
			
}	



void MoveToDoubleDotsLastNote()
{
		GameState = DOUBLE_DOTS_LAST_NOTE;
		MAIN_GAME_TIMER = 0;
		AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,DDEND_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
		SendNodeNOP();	
		EAudioPlaySound(BACKGROUND_MUSIC_STREAM,DDEND_WAV);		
		LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
}	




void GetNextDots1P()
{
		Dots[0] = RandomButton(Dots[0], ((Dots[0] + 4) &0x07), NO_EXCLUDE);
		Dots[1] = (Dots[0]+4)&0x07;
		DotHit[0] = FALSE;
		DotHit[1] = FALSE;
}


void GetNextDots2P(BYTE Player)
{
		if(Player == PLAYER_1)
		{
			Dots[0] = RandomButton4Exclude(Dots[0],Dots[1] ,Dots[2],Dots[3]);
			Dots[1] = (Dots[0]+4)&0x07;
			DotHit[0] = FALSE;
			DotHit[1] = FALSE;
		}
		else
		{
			Dots[2] = RandomButton4Exclude(Dots[2],Dots[3], Dots[0],Dots[1]);
			Dots[3] = (Dots[2]+4)&0x07;
			DotHit[2] = FALSE;
			DotHit[3] = FALSE;
		}		
}

void GetNextPixieColor()
{
	PixieTime = 65 +(rand()&0x1F);
	PixieStartRed = PixieStopRed;
	PixieStartGreen = PixieStopGreen;

	PixieStopRed = 31+(rand()&0x1f);
	PixieStopGreen = 31+(rand()&0x1f);
	
	if(GamePlayers == 1)
	{
		LEDSendMessage(Dots[0],PixieStartRed,PixieStartGreen,0,PixieStopRed,PixieStopGreen,0,PixieTime,PixieTime);
		LEDSendMessage(Dots[1],PixieStartRed,PixieStartGreen,0,PixieStopRed,PixieStopGreen,0,PixieTime,PixieTime);
	}	
	
	else
	{
	
		LEDSendMessage(Dots[0],PixieStartRed,0,0,PixieStopRed,0,0,PixieTime,PixieTime);
		LEDSendMessage(Dots[1],PixieStartRed,0,0,PixieStopRed,0,0,PixieTime,PixieTime);
		
		LEDSendMessage(Dots[2],0,PixieStartGreen,0,0,PixieStopGreen,0,PixieTime,PixieTime);
		LEDSendMessage(Dots[3],0,PixieStartGreen,0,0,PixieStopGreen,0,PixieTime,PixieTime);
		
	}	

}	

void DoubleDotsPlayCorrect(BYTE Player)
{
	if(Player == PLAYER_1)
	{
		AudioNodeEnable(Dots[0],CORRECT_SOUND_EFFECT_STREAM_1P,BACKGROUND_MUSIC_STREAM,1,1,DDCORRECT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		AudioNodeEnable(Dots[1],CORRECT_SOUND_EFFECT_STREAM_1P,BACKGROUND_MUSIC_STREAM,1,1,DDCORRECT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		SendNodeNOP();	
		EAudioPlaySound(CORRECT_SOUND_EFFECT_STREAM_1P,DDCORRECT_WAV);
	}
	else
	{
		AudioNodeEnable(Dots[2],CORRECT_SOUND_EFFECT_STREAM_2P,BACKGROUND_MUSIC_STREAM,1,1,DDCORRECT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		AudioNodeEnable(Dots[3],CORRECT_SOUND_EFFECT_STREAM_2P,BACKGROUND_MUSIC_STREAM,1,1,DDCORRECT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		SendNodeNOP();	
		EAudioPlaySound(CORRECT_SOUND_EFFECT_STREAM_2P,DDCORRECT_WAV);
		
	}	
}




	
	
	
	


