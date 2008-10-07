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

#define BACKGROUND_MUSIC_STREAM			0x00
#define CORRECT_SOUND_EFFECT_STREAM    	0x01
#define FX_STREAM    		0x02

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
#define HIT_TIMER					    GPTimer[1]
#define DOUBLE_DOTS_TRANSITION_TIMER    GPTimer[2]
#define PIXIE_TIMER					    GPTimer[3]

//*************************************************
//*******Game Variables****************************
//*************************************************

static BYTE Dots[2];
static BOOL DotHit[2];
static BYTE PixieTime;
static BYTE PixieStartRed;
static BYTE PixieStartGreen;
static BYTE PixieStopRed;
static BYTE PixieStopGreen;
static BYTE SeperationTime = 0;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void GetNextDots();
void MoveToDoubleDotsEnd();
void GetNextPixieColor();
void DoubleDotAccepted();
void DoubleDotNotAccepted();
void DoubleDotsPlayCorrect();
void MoveToDoubleDotsLastNote();

void DoubleDots(void)
{
			
	switch (GameState)
	{
		
		case INIT:
			ResetAudioAndLEDS();
			LEDSendScores(248, 	ScoreBrightness, 0,BLANK_SCORE);
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,DDBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,DDBACKGROUND_WAV);
			GameState = DOUBLE_DOTS;
			ScoreManagerEnabled = FALSE;
			HIT_TIMER = 0xFFFF;
			GetNextDots();
			GetNextPixieColor();
			Player1Score = 0;
			Player2Score = 0;
			
			P1ScoreDisplayState = SCORE_NORMAL;
			P2ScoreDisplayState = SCORE_BLANK;
			ScoreManagerEnabled = TRUE;
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
			if(((DotHit[0] == TRUE) || (DotHit[1] == TRUE)) && (HIT_TIMER > MAX_HIT_TIME))
			{
				DotHit[0] = FALSE;
				DotHit[1] = FALSE;
			}
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



void OnButtonPressDoubleDots(unsigned char button)
{
	switch(GameState)
	{
		case DOUBLE_DOTS:
		case DOUBLE_DOTS_LAST_NOTE:
		if((DotHit[0] == FALSE) && (DotHit[1] == FALSE))
		{
			
			if(button == Dots[0])
			{
				DotHit[0] = TRUE;
				HIT_TIMER = 0;	
			}
			else if(button == Dots[1])
			{
				DotHit[1] = TRUE;
				HIT_TIMER = 0;	
			}
		}
		else if((DotHit[0] == TRUE) || (DotHit[1] == TRUE))
		{
			if((button == Dots[0]) && (DotHit[0] == FALSE))	
			{
				SeperationTime = HIT_TIMER;
				if(SeperationTime < HIT_THRESHOLD)
				{	
					DoubleDotAccepted();
				}
				else
				{
					DoubleDotNotAccepted();	
				}
			}
			else if((button == Dots[1]) && (DotHit[1] == FALSE))	
			{
				if(HIT_TIMER < HIT_THRESHOLD)
				{	
					SeperationTime = HIT_TIMER;
					DoubleDotAccepted();
				}
				else
				{
					DoubleDotNotAccepted();	
				}
			}
		}
		break;		
	}
}

void DoubleDotAccepted()
{
	DotHit[0] = FALSE;
	DotHit[1] = FALSE;
	DoubleDotsPlayCorrect();
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
	GetNextDots();
	
		 if(SeperationTime < DOUBLE_DOTS_LEVEL4_TIME) {Player1Score +=  DOUBLE_DOTS_POINT_LEVEL4;}
	else if(SeperationTime < DOUBLE_DOTS_LEVEL3_TIME) {Player1Score +=  DOUBLE_DOTS_POINT_LEVEL3;}
	else if(SeperationTime < DOUBLE_DOTS_LEVEL2_TIME) {Player1Score +=  DOUBLE_DOTS_POINT_LEVEL2;}
	else 											   {Player1Score +=  DOUBLE_DOTS_POINT_LEVEL1;}
  
}	


void DoubleDotNotAccepted()
{
	DotHit[0] = FALSE;
	DotHit[1] = FALSE;	
}	

void MoveToDoubleDotsEnd()
{
		GameState = DOUBLE_DOTS_END;
		MAIN_GAME_TIMER = 0;
		AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
		SendNodeNOP();	
		EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
		LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
		P1ScoreDisplayState = SCORE_FLASHING;
		P2ScoreDisplayState = SCORE_BLANK;
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




void GetNextDots()
{
	if(GamePlayers == 1)
	{
		Dots[0] = RandomButton(NO_EXCLUDE, NO_EXCLUDE, NO_EXCLUDE);
		Dots[1] = (Dots[0]+1)&0x07;
	}
	else
	{
		Dots[0] = RandomButton(NO_EXCLUDE, NO_EXCLUDE, NO_EXCLUDE);
		Dots[1] = (Dots[0]+4)&0x07;	
	}
	DotHit[0] = FALSE;
	DotHit[1] = FALSE;
}

void GetNextPixieColor()
{
	PixieTime = 65 +(rand()&0x1F);
	PixieStartRed = PixieStopRed;
	PixieStartGreen = PixieStopGreen;
	PixieStopRed = rand()&0x2f;
	PixieStopGreen = rand()&0x2f;
	LEDSendMessage(Dots[0],PixieStartRed,PixieStartGreen,0,PixieStopRed,PixieStopGreen,0,PixieTime,PixieTime);
	LEDSendMessage(Dots[1],PixieStartRed,PixieStartGreen,0,PixieStopRed,PixieStopGreen,0,PixieTime,PixieTime);
}	

void DoubleDotsPlayCorrect()
{
	AudioNodeEnable(Dots[0],CORRECT_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,DDCORRECT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable(Dots[1],CORRECT_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,DDCORRECT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(CORRECT_SOUND_EFFECT_STREAM,DDCORRECT_WAV);

}




	
	
	
	