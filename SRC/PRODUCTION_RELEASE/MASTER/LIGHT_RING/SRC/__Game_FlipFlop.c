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
#include "config.h"


//*************************************************
//*******Game Parameters***************************
//*************************************************

#define FLIP_FLOP_SCORE_INCREMENT 1


//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define	FLIP_SFX_STREAM 1
#define	FLOP_SFX_STREAM 2

#define	BACKGROUND_MUSIC_STREAM 3


//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define FLIP_FLOP		1
#define FLIP_FLOP_END	2

#define RED_STATE      0
#define GREEN_STATE    1


//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER 				GPTimer[0]
#define BUTTON_LED_REFRESH_TIMER 		GPTimer[1]
#define FLIPFLOP_SCORE_UPDATE_TIMER 	GPTimer[2]


//*************************************************
//*******Game Variables****************************
//*************************************************


BYTE ButtonFlipFlopState[NUM_BUTTONS];

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void InitButtonFlipFlopState();
void PlayFlip(BYTE button);
void PlayFlop(BYTE button);
void MoveToFlipFlopEnd();
void StartMainFlipFlopThemeMusic();
void RefreshFlipFlopLEDs();
void FlipFlopScoring();


void FlipFlop(void)
{
			
	switch (GameState)
	{
		
		case INIT:
	
			MAIN_GAME_TIMER = 0;
			StartMainFlipFlopThemeMusic();
			
			P1ScoreDisplayState = SCORE_NORMAL;
			P2ScoreDisplayState = SCORE_NORMAL;
			ScoreManagerEnabled = TRUE;
			InitButtonFlipFlopState();
			BUTTON_LED_REFRESH_TIMER = 0xFFFF;
			GameState = FLIP_FLOP;
	
		break;
		
		case FLIP_FLOP:
			if(MAIN_GAME_TIMER>FLIPFLOPBACKGROUND_WAV_LENGTH)
			{
					MoveToFlipFlopEnd();
			}
			if(BUTTON_LED_REFRESH_TIMER > 10)
			{
				BUTTON_LED_REFRESH_TIMER = 0;
				RefreshFlipFlopLEDs();
				RefreshFlipFlopLEDs();
			}
			if( FLIPFLOP_SCORE_UPDATE_TIMER > 10)
			{
				FLIPFLOP_SCORE_UPDATE_TIMER = 0;
				FlipFlopScoring();
			}
		break;
		
		
	
		case FLIP_FLOP_END:
			if(MAIN_GAME_TIMER>ENDING_WAV_LENGTH)
			{
					ResetToGameSelector();
			}
		break;
		
		
	
		default:
			
			GameState = INIT;
		
		break;
			
	}
}


void OnButtonPressFlipFlop(unsigned char button)
{
	switch(GameState)
	{
		
		case FLIP_FLOP:
			if(ButtonFlipFlopState[button] == RED_STATE)
			{
				ButtonFlipFlopState[button] = GREEN_STATE;
				PlayFlip(button);
				RefreshFlipFlopLEDs();
			}
			else
			{
				ButtonFlipFlopState[button] = RED_STATE;
				PlayFlop(button);
				RefreshFlipFlopLEDs();
			}
		
		break;
		default:
		break;
	}
		
}


void MoveToFlipFlopEnd()
{
			GameState = FLIP_FLOP_END;
			MAIN_GAME_TIMER = 0;
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
			LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
			
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

void StartMainFlipFlopThemeMusic()
{
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT, FLIPFLOPBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,FLIPFLOPBACKGROUND_WAV);
	MAIN_GAME_TIMER = 0;
}	


void FlipFlopScoring()
{
	
	BYTE RedCount,GreenCount = 0;
	BYTE i;
	BYTE ScoreTemp;
	
	
	RedCount = 0;
	GreenCount = 0;
	
	for(i=0;i<NUM_BUTTONS;i++)
	{
		if(ButtonFlipFlopState[i] == RED_STATE)
			RedCount++;
		else
			GreenCount++;	
	}
	
	if(RedCount != GreenCount)
	{
		if(RedCount > GreenCount)
		{
			
			ScoreTemp = (RedCount - GreenCount);
			
			ScoreTemp=ScoreTemp>>1;
			
			Player2Score += FLIP_FLOP_SCORE_INCREMENT * ScoreTemp;
		
			
		
		}
		else if (GreenCount > RedCount)
		{
			ScoreTemp = (GreenCount - RedCount);
			
			ScoreTemp=ScoreTemp>>1;
			Player1Score += FLIP_FLOP_SCORE_INCREMENT * ScoreTemp;	
		}
	}
}	

void InitButtonFlipFlopState()
{
	if(rand()&0x01)
	{
		//Even/odd mode	
		BYTE i;
		
		for(i=0;i<NUM_BUTTONS;i++)
		{
			if(i&0x01)
			{
				 ButtonFlipFlopState[i] = RED_STATE;
			}
			else
			{
				ButtonFlipFlopState[i] = GREEN_STATE;	
				
			}
		}
		
		if(rand()&0x01)
		{
			for(i=0;i<NUM_BUTTONS;i++)
			{
				if(ButtonFlipFlopState[i] == RED_STATE)
				{
					ButtonFlipFlopState[i] = GREEN_STATE;
				}
				else
				{
					ButtonFlipFlopState[i] = RED_STATE;
				}
			}
		}
	}
	else
	{
		BYTE StartLocation = rand()%NUM_BUTTONS;
		BYTE i;
		
		for(i=0;i<(NUM_BUTTONS>>1);i++)
		{
			ButtonFlipFlopState[StartLocation] = RED_STATE;
			StartLocation++;
			if(StartLocation == NUM_BUTTONS)
			{
				StartLocation = 0;	
			}
		}
		
		for(i=0;i<(NUM_BUTTONS>>1);i++)
		{
			ButtonFlipFlopState[StartLocation] = GREEN_STATE;
			StartLocation++;
			if(StartLocation == NUM_BUTTONS)
			{
				StartLocation = 0;	
			}
		}
		
	}
}
	
	
void RefreshFlipFlopLEDs()
{
	BYTE i;
	
	for(i=0;i<NUM_BUTTONS;i++)
	{
		if(ButtonFlipFlopState[i] == RED_STATE)
		{
			LEDSendMessage(i,RED,RED,0,0);	
		}
		else
		{
			LEDSendMessage(i,GREEN,GREEN,0,0);	
		}
		
	}
	
}	
	
void PlayFlip(BYTE button)
{
	AudioNodeEnable(button,FLIP_SFX_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT, FLOP1_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(FLIP_SFX_STREAM ,FLOP1_WAV );
}
	
void PlayFlop(BYTE button)
{
	AudioNodeEnable(button,FLOP_SFX_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT, FLOP2_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(FLOP_SFX_STREAM,FLOP2_WAV );
}	

