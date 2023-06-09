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

#define GOOSE_SPEED					50
#define DUCK_DUCK_GAME_TIME			4500
#define DUCK_LIGHT_FLASH_INTERVAL	50

#define DUCK_HIT_POINT	10
#define GOOSE_HIT_POINT	25

#define GOOSE_FLICKER_TIME 5 

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define	HIT_SFX_STREAM				 3
#define	GOOSE_SFX_STREAM 		 	 2
#define	DUCK_SFX_STREAM 			 1
#define	BACKGROUND_MUSIC_STREAM		 0


//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define DUCK_CHASE_INIT	1
#define DUCK_CHASE		2
#define GOOSE_CALL		3
#define GOOSE_CHASE		4
#define DUCK_DUCK_END	5

//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER 	GPTimer[0]
#define DUCK_LIGHT_TIMER 	GPTimer[1]
#define GOOSE_RUN_TIMER		GPTimer[2]
#define GOOSE_MUSIC_TIMER	GPTimer[3]


//*************************************************
//*******Game Variables****************************
//*************************************************

BYTE NumDucks;
BYTE Position;
BYTE Direction;
BYTE MovesUntilSwitch;
BYTE GooseMoves;
WORD DuckAnimationTime;
BYTE DuckLightStartRed=0;
BYTE DuckLightStartGreen=0;
BYTE DuckLightStopRed=0;
BYTE DuckLightStopGreen=0;
BYTE GooseLightState=0;

#define DUCK_DIRECTION  Direction
#define DUCK_POSITION	Position
#define GOOSE_DIRECTION	 Direction
#define GOOSE_POSITION	Position

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void MoveToDuckDuckGooseEnd();
void FlashDuckLight();
void StartDuckChaseMusic();
void PlayDuckHitSound(BYTE button);
void MoveDuck();
void MoveToGooseChase();
void MoveToGooseCall();
void PlayGooseSound(BYTE button);
void MoveGoose();
void LightGoosePosition();
void FlashGooseLight();	
void StartDuckSoundEffect();
void AnimateDuckLight();
void AnimateGooseLight();


void DuckDuckGoose(void)
{
			
	switch (GameState)
	{
		
		case INIT:
	
			MAIN_GAME_TIMER = 0;
			GameState = DUCK_CHASE_INIT;		
			DUCK_POSITION = RandomButton(NO_EXCLUDE, NO_EXCLUDE,NO_EXCLUDE);
		break;
		
		case DUCK_CHASE_INIT:
			LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
				
			DUCK_DIRECTION = SelectRandomDirection();
			NumDucks = 3 + (rand()&0x03);
			DUCK_LIGHT_TIMER = 0xFFFF;
			StartDuckChaseMusic();
			StartDuckSoundEffect();
			GameState = DUCK_CHASE;
			ScoreManagerEnabled = TRUE;
			P1ScoreDisplayState = SCORE_NORMAL;
			P2ScoreDisplayState = SCORE_BLANK;
			GooseMoves = 0;
			
		break;
		
		case DUCK_CHASE:
		
		if(MAIN_GAME_TIMER > DUCK_DUCK_GAME_TIME)
		{
			MoveToDuckDuckGooseEnd();
		}
		
		AnimateDuckLight();
		
		break;
		
		case GOOSE_CALL:
		
		if(MAIN_GAME_TIMER > DUCK_DUCK_GAME_TIME)
		{
			MoveToDuckDuckGooseEnd();
		}
		
		AnimateGooseLight();
		
		break;
	
		case GOOSE_CHASE:
		
			if(GOOSE_MUSIC_TIMER>GOOSECHASEMUSIC_WAV_LENGTH)
			{
				MoveToDuckDuckGooseEnd();
			}
		
			AnimateGooseLight();	
		
			if(GOOSE_RUN_TIMER > GOOSE_SPEED)
			{
				GOOSE_RUN_TIMER = 0;		
			
				GooseMoves++;
				
				if(GooseMoves > 9)
				{
					GameState = DUCK_CHASE_INIT;
		
				}	
				else
				{
					MoveGoose();
					PlayGooseSound(GOOSE_POSITION);
				}
				
			}
			
		break;
	
	
		case DUCK_DUCK_END:
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




void OnButtonPressDuckDuckGoose(unsigned char button)
{
	switch(GameState)
	{
		
		case DUCK_CHASE:
		
			if(button == DUCK_POSITION)
			{
				PlayDuckHitSound(button);
				LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0, 0);
				MoveDuck();
				Player1Score += DUCK_HIT_POINT;
				
				NumDucks--;
				if(NumDucks == 0)
				{
					MoveToGooseCall();
				
				}
				else
				{
					StartDuckSoundEffect();
				}
			}
		
		break;	
		
		case GOOSE_CALL:
		
			if(button == GOOSE_POSITION)
				{
						MoveToGooseChase();
						Player1Score += DUCK_HIT_POINT;
				}
	
		break;
		
		case GOOSE_CHASE:
		
				if(button == GOOSE_POSITION)
				{
					Player1Score += GOOSE_HIT_POINT;
					GameState = DUCK_CHASE_INIT;
					LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
	
				}
		
		
		break;
		
	}


}


void MoveToGooseChase()
{
	GameState = GOOSE_CHASE;
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,NO_TIMEOUT,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,DUCKDUCK_FULLCHASE_WAV);
	GOOSE_RUN_TIMER = 0xFFFF;
	GOOSE_MUSIC_TIMER = 0;
	MovesUntilSwitch = 0;
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0, 0);
	
	GOOSE_DIRECTION  = rand()&0x01;
}	


void MoveToGooseCall()
{
	GameState = GOOSE_CALL;
	AudioNodeEnable(GOOSE_POSITION,GOOSE_SFX_STREAM,GOOSE_SFX_STREAM,AUDIO_ON_BEFORE_TIMEOUT,NO_TIMEOUT,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(GOOSE_SFX_STREAM,GOOSECALL_WAV);
	DUCK_LIGHT_TIMER = 0xFFFF;
	GOOSE_RUN_TIMER = 0xFFFF;
}

	
void MoveToDuckDuckGooseEnd()
{
	MAIN_GAME_TIMER = 0;
	GameState = DUCK_DUCK_END;	
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
    AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
	P1ScoreDisplayState = SCORE_FLASHING;
	P2ScoreDisplayState = SCORE_BLANK;
}

void FlashDuckLight()
{
	
	LEDSendMessage(DUCK_POSITION,YELLOW,LEDOFF,DUCK_LIGHT_FLASH_INTERVAL>>1,0);
	
}	

void FlashGooseLight()
{
	
	LEDSendMessage(GOOSE_POSITION,RED,LEDOFF,DUCK_LIGHT_FLASH_INTERVAL>>1,0);
	
}

void LightGoosePosition()
{
	LEDSendMessage(GOOSE_POSITION,LEDOFF,RED,GOOSE_SPEED, GOOSE_SPEED/8);
}	

void StartDuckChaseMusic()
{
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,NO_TIMEOUT,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,DUCKDUCK_BACKGROUND_NO_QUACKS_WAV);
}		

void PlayDuckHitSound(BYTE button)
{
	AudioNodeEnable(button,HIT_SFX_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT,DUCKHIT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(HIT_SFX_STREAM,DUCKHIT_WAV  );
}	

void PlayGooseSound(BYTE button)
{
	AudioNodeEnable(button,HIT_SFX_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT,GOOSE_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(HIT_SFX_STREAM,GOOSE_WAV );
	
}	


void StartDuckSoundEffect()
{
	AudioNodeEnable(DUCK_POSITION,DUCK_SFX_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT,NO_TIMEOUT,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(DUCK_SFX_STREAM,DUCK_WAV );
}	


void MoveDuck()
{
	if(DUCK_DIRECTION)
	{
		if(DUCK_POSITION == (NUM_BUTTONS-1))
		{
			DUCK_POSITION = 0;	
		}
		else
		{
			DUCK_POSITION++;	
		}
	}
	else
	{
		if(DUCK_POSITION == 0)
		{
			DUCK_POSITION = (NUM_BUTTONS-1);	
		}
		else
		{
			DUCK_POSITION--;	
		}
	}
}	
	
void MoveGoose()
{
	LEDSendMessage(GOOSE_POSITION,LEDOFF,LEDOFF,0, 0);
	
	if(GOOSE_DIRECTION)
	{
		if(GOOSE_POSITION == (NUM_BUTTONS-1))
		{
			GOOSE_POSITION = 0;	
		}
		else
		{
			GOOSE_POSITION++;	
		}
	}
	else
	{
		if(GOOSE_POSITION == 0)
		{
			GOOSE_POSITION = (NUM_BUTTONS-1);	
		}
		else
		{
			GOOSE_POSITION--;	
		}
	}
	
	LEDSendMessage(GOOSE_POSITION,RED,RED,0, 0);
	LEDSendMessage(GOOSE_POSITION,RED,RED,0, 0);
}	



void AnimateDuckLight()
{
	if(DUCK_LIGHT_TIMER>DuckAnimationTime)
	{
		DUCK_LIGHT_TIMER = 0;

		DuckAnimationTime = 32 +(rand()&0x1F);
		DuckLightStartRed = DuckLightStopRed;
		DuckLightStartGreen = DuckLightStopGreen;
	
		DuckLightStopRed = 32+(rand()&0x1F);
		DuckLightStopGreen = DuckLightStopRed;
		
		LEDSendMessage(DUCK_POSITION,DuckLightStartRed,DuckLightStartGreen,0,DuckLightStopRed,DuckLightStopGreen,0,DuckAnimationTime,DuckAnimationTime);
	
	}
	
}

void AnimateGooseLight()
{
	if(DUCK_LIGHT_TIMER>GOOSE_FLICKER_TIME)
	{
		DUCK_LIGHT_TIMER = 0;
	
		switch(GooseLightState)
		{
			case 0:
				LEDSendMessage(GOOSE_POSITION,RED,LEDOFF,GOOSE_FLICKER_TIME,GOOSE_FLICKER_TIME);
				GooseLightState = 1;
			break;
			
			case 1:
				LEDSendMessage(GOOSE_POSITION,LEDOFF,LEDOFF,GOOSE_FLICKER_TIME,GOOSE_FLICKER_TIME);
				GooseLightState = 2;
			break;

			case 2:
				LEDSendMessage(GOOSE_POSITION,GREEN,LEDOFF,GOOSE_FLICKER_TIME,GOOSE_FLICKER_TIME);
				GooseLightState = 3;
			break;
			
			case 3:
				LEDSendMessage(GOOSE_POSITION,LEDOFF,LEDOFF,GOOSE_FLICKER_TIME,GOOSE_FLICKER_TIME);
				GooseLightState = 0;
			break;
		
			default:
			GooseLightState = 0;
			break;
			
		}		
	
	
	}	
	
}


