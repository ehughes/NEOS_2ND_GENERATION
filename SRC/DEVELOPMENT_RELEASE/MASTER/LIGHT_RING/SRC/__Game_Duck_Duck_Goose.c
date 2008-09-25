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


//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define	HIT_SFX_STREAM 2
#define	BACKGROUND_MUSIC_STREAM 3


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

void DuckDuckGoose(void)
{
			
	switch (GameState)
	{
		
		case INIT:
	
			MAIN_GAME_TIMER = 0;
			GameState = DUCK_CHASE_INIT;		
	
		break;
		
		case DUCK_CHASE_INIT:
		
			DUCK_POSITION = RandomButton(NO_EXCLUDE, NO_EXCLUDE);	
			DUCK_DIRECTION = SelectRandomDirection();
			NumDucks = 3 + (rand()&0x03);
			DUCK_LIGHT_TIMER = 0xFFFF;
			StartDuckChaseMusic();
			GameState = DUCK_CHASE;
			ScoreManagerEnabled = TRUE;
			P1ScoreDisplayState = SCORE_NORMAL;
			P2ScoreDisplayState = SCORE_BLANK;
			
		break;
		
		case DUCK_CHASE:
		
		if(MAIN_GAME_TIMER > DUCK_DUCK_GAME_TIME)
		{
			MoveToDuckDuckGooseEnd();
		}
		
		if(DUCK_LIGHT_TIMER>DUCK_LIGHT_FLASH_INTERVAL)
		{
			DUCK_LIGHT_TIMER = 0;
			FlashDuckLight();	
			
		}
		
		case GOOSE_CALL:
		
		if(MAIN_GAME_TIMER > DUCK_DUCK_GAME_TIME)
		{
			MoveToDuckDuckGooseEnd();
		}
		
		if(DUCK_LIGHT_TIMER>DUCK_LIGHT_FLASH_INTERVAL)
		{
			DUCK_LIGHT_TIMER = 0;
			FlashGooseLight();	
		}
		
		break;
	
		case GOOSE_CHASE:
		
			if(GOOSE_MUSIC_TIMER>GOOSECHASEMUSIC_WAV_LENGTH)
			{
				MoveToDuckDuckGooseEnd();
			}
		
			if(GOOSE_RUN_TIMER > GOOSE_SPEED)
			{
				GOOSE_RUN_TIMER = 0;		
			
				
				
				MoveGoose();
				PlayGooseSound(GOOSE_POSITION);
				
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
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,GOOSECHASEMUSIC_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,GOOSECHASEMUSIC_WAV);
	GOOSE_RUN_TIMER = 0;
	GOOSE_MUSIC_TIMER = 0;
	MovesUntilSwitch = 0;
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0, 0);
}	


void MoveToGooseCall()
{
	GameState = GOOSE_CALL;
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,GOOSECALL_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,GOOSECALL_WAV);
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
	
	LEDSendMessage(DUCK_POSITION,YELLOW,LEDOFF,DUCK_LIGHT_FLASH_INTERVAL,0);
	
}	

void FlashGooseLight()
{
	
	LEDSendMessage(GOOSE_POSITION,RED,LEDOFF,DUCK_LIGHT_FLASH_INTERVAL,0);
	
}

void LightGoosePosition()
{
	LEDSendMessage(GOOSE_POSITION,LEDOFF,RED,GOOSE_SPEED, GOOSE_SPEED/8);
}	

void StartDuckChaseMusic()
{
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,NO_TIMEOUT,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,DUCKCHASEMUSIC_WAV);
			
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
	
	if(MovesUntilSwitch == 0)
	{
		MovesUntilSwitch = (rand()&0x3);		
		GOOSE_DIRECTION = SelectRandomDirection();
	}
	else
	{
		MovesUntilSwitch--;	
	}
	
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


