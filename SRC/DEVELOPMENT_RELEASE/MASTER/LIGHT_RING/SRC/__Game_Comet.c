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

#define COMET_INITIAL_SPEED	    100
#define COMET_SPEED_INCREMENT	8
#define PHYSICS_UPDATE_TIME 	1
#define COMET_HIT_POINT				10
#define COMET_CLEAR_POINT			25

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define	SFX_STREAM3				0
#define	SFX_STREAM2				1
#define	SFX_STREAM1			    2
#define	BACKGROUND_MUSIC_STREAM 3

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define BUILD_COMET			1
#define COMET_CHASE			2
#define COMET_END			3

//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER			GPTimer[0]
#define PHYSICS_UPDATE_TIMER	GPTimer[1]
#define COMET_FLICKER_TIMER		GPTimer[2]
#define COMET_BUILD_TIMER		GPTimer[3]
#define COMET_MOVE_TIMER		GPTimer[4]

//*************************************************
//*******Game Variables****************************
//*************************************************

BYTE TailLightPosition;
BYTE HeadLightPosition;
BYTE MidLightPosition;
WORD CometSpeed;
BYTE CometDirection;
BYTE CometSize;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void MoveComet();
void InitFirstComet();
BYTE IncWithButtonWrap(BYTE Button);
BYTE DecWithButtonWrap(BYTE Button);
void PlayCometMiss(BYTE button);
void PlayCometHit(BYTE button);
void PlayNewComet(BYTE button);
void GenerateNextComet();
void MoveToCometEnd();


void Comet(void)
{
			
	switch (GameState)
	{
		
		case INIT:
		
			InitFirstComet();
			GameState = BUILD_COMET;
		
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,COMETBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,COMETBACKGROUND_WAV);
			
			Player1Score = 0;
			Player2Score = 0;
								
			P1ScoreDisplayState = SCORE_NORMAL;
			P2ScoreDisplayState = SCORE_BLANK;
			ScoreManagerEnabled = TRUE;	
			
			MAIN_GAME_TIMER = 0;
		
		break;
		
		
		case BUILD_COMET:
		
		if(COMET_BUILD_TIMER > 40)
		{
		COMET_BUILD_TIMER = 0;
			
			switch(CometSize)
			{
				case 0:
				
					CometSize++;
					PlayNewComet(TailLightPosition);
					LEDSendMessage(TailLightPosition,YELLOW,LEDOFF,0,0);
					
				break;
				
				case 1:
					CometSize++;
				
						if(CometDirection)
						{
							MidLightPosition   = IncWithButtonWrap(TailLightPosition);
						}
						else
						{
							MidLightPosition = DecWithButtonWrap(TailLightPosition);
						
						}
		
					PlayNewComet(MidLightPosition);
					LEDSendMessage(MidLightPosition,RED,LEDOFF,0,0);	
			
				break;
				
				case 2:
					CometSize++;
				
					if(CometDirection)
						{
							HeadLightPosition   = IncWithButtonWrap(MidLightPosition);
						}
						else
						{
							HeadLightPosition = DecWithButtonWrap(MidLightPosition);
						}
		
					PlayNewComet(HeadLightPosition);
					LEDSendMessage(HeadLightPosition,RED,LEDOFF,0,0);
					
				break;
				
				case 3:
					GameState = COMET_CHASE;	
					COMET_MOVE_TIMER = 0xFFFF;
				break;
			
			}
		}
		break;
		
		case COMET_CHASE:
		
			if(COMET_MOVE_TIMER >= CometSpeed)
			{
				COMET_MOVE_TIMER = 0;
				MoveComet();
			
			}
			
			if(MAIN_GAME_TIMER > COMETBACKGROUND_WAV_LENGTH)
			{
				MoveToCometEnd();
			}
			
		break;
	
		case COMET_END:
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




void OnButtonPressComet(unsigned char button)
{

	if(button == TailLightPosition)
	{
		PlayCometHit(button);
		
		CometSize--;
	
		Player1Score += COMET_HIT_POINT;
		
		if(CometSize ==0)
		{
			Player1Score += COMET_CLEAR_POINT;
			GameState = BUILD_COMET;
			LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
			COMET_BUILD_TIMER = 0xFFFF;
			GenerateNextComet();
			CometSpeed -= COMET_SPEED_INCREMENT;
		}
	}
	else
	{
		PlayCometMiss(button);
	}
	
}

void GenerateNextComet()
{
	CometDirection = SelectRandomDirection();	
	TailLightPosition = IncWithButtonWrap(TailLightPosition);
	TailLightPosition = IncWithButtonWrap(TailLightPosition);
	TailLightPosition = IncWithButtonWrap(TailLightPosition);
	CometSize = 0;
}	

void InitFirstComet()
{
	CometSpeed = COMET_INITIAL_SPEED;
	CometDirection = SelectRandomDirection();	
	TailLightPosition = RandomButton(NO_EXCLUDE, NO_EXCLUDE);
	CometSize = 0;
	GameState = BUILD_COMET;
}	


void MoveComet()
{

		LEDSendMessage(TailLightPosition,LEDOFF,LEDOFF,0,0);
	
		if(CometDirection)
		{
			TailLightPosition = IncWithButtonWrap(TailLightPosition);
			MidLightPosition  = IncWithButtonWrap(TailLightPosition);
			HeadLightPosition =	IncWithButtonWrap(MidLightPosition);	
		}
		else
		{
			TailLightPosition = DecWithButtonWrap(TailLightPosition);
			MidLightPosition = DecWithButtonWrap(TailLightPosition);
			HeadLightPosition=	DecWithButtonWrap(MidLightPosition);
		}	


	switch(CometSize)
	{	
		case 3:
		
			LEDSendMessage(HeadLightPosition,RED,LEDOFF,0,0);	
			LEDSendMessage(MidLightPosition,RED,LEDOFF,0,0);	
			LEDSendMessage(TailLightPosition,YELLOW,LEDOFF,0,0);	
		
		break;
		
		case 2:
			
			LEDSendMessage(MidLightPosition,RED,LEDOFF,0,0);	
			LEDSendMessage(TailLightPosition,YELLOW,LEDOFF,0,0);	
	
		break;
		
		case 1:
	
			LEDSendMessage(TailLightPosition,YELLOW,LEDOFF,0,0);	

		break;
	
	}
}

BYTE IncWithButtonWrap(BYTE Button)
{
	BYTE t;
			
	if(Button == (NUM_BUTTONS - 1))
	{
		t = 0;
	}
	else
	{
		t=Button+1;
	}

	return t;	
}
		
BYTE DecWithButtonWrap(BYTE Button)
{
	BYTE t;
	
	if(Button == 0)
	{
		t = (NUM_BUTTONS - 1);
	}
	else
	{
		 t = Button -1;
	}
	return t;
}



void PlayCometMiss(BYTE button)
{
	AudioNodeEnable(button,SFX_STREAM1,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT,MISS_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(SFX_STREAM1,MISS_WAV );
}	

void PlayCometHit(BYTE button)
{
	AudioNodeEnable(button,SFX_STREAM2,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT,LASERBLAST_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(SFX_STREAM2,LASERBLAST_WAV );
}	

void PlayNewComet(BYTE button)
{
	AudioNodeEnable(button,SFX_STREAM3,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT,NEWCOMET_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(SFX_STREAM3,NEWCOMET_WAV );
}	

void MoveToCometEnd()
{
	
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
	MAIN_GAME_TIMER = 0;
	GameState = COMET_END;	
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
    AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
	P1ScoreDisplayState = SCORE_FLASHING;
	P2ScoreDisplayState = SCORE_BLANK;
	
		
}	


