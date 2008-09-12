#include "__Game_FireFighter.h"
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

#define FIRE_FIGHTER_TIME_OUT 		DDBACKGROUND_WAV_LENGTH
#define FIRE_ANIMATION_TIME				5
#define FIRE_HIT_POINT	0
#define FIRE_OUT_POINT	10

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define BACKGROUND_MUSIC_STREAM			0x00
#define NEW_FIRE_STREAM			    	0x01
#define FX_STREAM1    					0x02
#define FX_STREAM2    					0x03


//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define FIREFIGHTER					0x01
#define FIREFIGHTER_END				0x02
#define FIREFIGHTER_INIT			0x03
#define FIREFIGHTER_LAST_NOTE		0x04


//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER		 			GPTimer[0]
#define FIRE_ANIMATION_TIMER			GPTimer[1]
#define FIRE_START_TIMER    			GPTimer[2]
#define FIRE_GROW_TIMER	    			GPTimer[3]


//*************************************************
//*******Game Variables****************************
//*************************************************

static WORD NextFireStartTime;
static BYTE FireGrowTime;					
static BYTE MaxFires;						
static BYTE SpawnLimit;					
static BYTE FireAnimationState;

 typedef struct {

	BYTE GrowTimer;
	BYTE Intensity;
	
} Fire;

static Fire MyFires[8];
static BYTE NumFires = 0;
static BYTE LastFireLocation = 0;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void StartNewFire();
void PlayWaterMiss(BYTE button);
void PlayWaterHit(BYTE button);
void PlayFireStart(BYTE button);
void MoveToFireFighterEnd();
void ResetFires();
void GrowFires();
void MoveToFireFighterLastNote();

//*************************************************
//*******Game Functions****************************
//*************************************************

void FireFighter(void)
{
	BYTE i;
	switch (GameState)
	{
		case INIT:
			ResetAllSlaves();
			MAIN_GAME_TIMER = 0;
			GameState = FIREFIGHTER_INIT;
			
		break;

		case FIREFIGHTER_INIT:

			if(MAIN_GAME_TIMER>GLOBAL_INIT_TIME)
			{
				AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,FF_BACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
				SendNodeNOP();	
				EAudioPlaySound(BACKGROUND_MUSIC_STREAM,FF_BACKGROUND_WAV);
				GameState = FIREFIGHTER;
				ResetFires();
				if(GamePlayers == 1)
				{
					NextFireStartTime = 300 + (rand()&0xff);
					FireGrowTime = 200;					
					MaxFires = 4;						
					SpawnLimit = 3;
				}
				else
				{
					NextFireStartTime = 150 + (rand()&0xff);
					FireGrowTime = 200;					
					MaxFires = 6;						
					SpawnLimit = 5;
					StartNewFire();			
				}
				
				StartNewFire();
				FIRE_ANIMATION_TIMER = 0;
				FIRE_START_TIMER = 0;
				MAIN_GAME_TIMER = 0;
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_BLANK;
				ScoreManagerEnabled = TRUE;
		 	}
		break;

		case FIREFIGHTER:
		
			if(MAIN_GAME_TIMER>FIRE_FIGHTER_TIME_OUT)
			{
				 MoveToFireFighterLastNote();
				 
				 P1ScoreDisplayState = SCORE_FLASHING;
				 P2ScoreDisplayState = SCORE_BLANK;
				 LEDSendMessage(0xFF,LEDOFF,LEDOFF,0,0);
			}
			if(FIRE_START_TIMER > NextFireStartTime)
			{
				FIRE_START_TIMER = 0;
				if(GamePlayers == 1)
				{
					NextFireStartTime = 300 + (rand()&0xff);	
				}
				else
				{
					NextFireStartTime = 150 + (rand()&0xff);	
				}
				StartNewFire();
			}
			if(FIRE_GROW_TIMER>1)
			{
				FIRE_GROW_TIMER = 0;
				GrowFires();	
			}
			
			
			
			if(FIRE_ANIMATION_TIMER > FIRE_ANIMATION_TIME)
			{
				FIRE_ANIMATION_TIMER = 0;
				FireAnimationState++;
				FireAnimationState &= 0x03;
				switch(FireAnimationState)
				{
					case 0:
					default:
					for(i=0;i<8;i++)
					{
						if(MyFires[i].Intensity>0)
						{
							LEDSendMessage(i,LEDOFF,LEDOFF,0,0);	
						}
						else
						{
							LEDSendMessage(i,LEDOFF,LEDOFF,0,0);
						}
					}
					break;
					
					case 1:
					for(i=0;i<8;i++)
					{
						if(MyFires[i].Intensity>0)
						{
							LEDSendMessage(i,RED,RED,0,0);	
						}
						else
						{
							LEDSendMessage(i,LEDOFF,LEDOFF,0,0);
						}
					}
					break;
					
					case 2:
								
					for(i=0;i<8;i++)
					{
						if(MyFires[i].Intensity>0)
						{
							LEDSendMessage(i,LEDOFF,LEDOFF,0,0);	
						}
						else
						{
							LEDSendMessage(i,LEDOFF,LEDOFF,0,0);
						}
					}
					
					break;
					
					case 3:
								
					for(i=0;i<8;i++)
					{
						if(MyFires[i].Intensity>0)
						{
								LEDSendMessage(i,24,24,0,24,24,0,0,0);	
						}
						else
						{
							LEDSendMessage(i,LEDOFF,LEDOFF,0,0);
						}
					}
					
					break;
					
					
						
				}
			}
						
		
		
		break;
		
		case FIREFIGHTER_END:
			if(MAIN_GAME_TIMER>ENDING_WAV_LENGTH)
			{
				ResetToGameSelector();
			}
		break;

		case FIREFIGHTER_LAST_NOTE:
			if(MAIN_GAME_TIMER>FF_END_WAV_LENGTH)
			{
				MoveToFireFighterEnd();
			}
		break;


		default:
		GameState = INIT;
		break;

		}
}




void OnButtonPressFireFighter(unsigned char button)
{

	switch(GameState)
	{
	
		case FIREFIGHTER:
		
			if(MyFires[button].Intensity > 0)
			{
				PlayWaterHit(button);	
				MyFires[button].Intensity--;
				Player1Score += FIRE_HIT_POINT;
				LEDSendMessage(button,YELLOW,(MyFires[button].Intensity*8)-1,0,0,30,30);
				if(MyFires[button].Intensity == 0)
				{
					if(NumFires < SpawnLimit)
					{
						StartNewFire();	
					}
					Player1Score += FIRE_OUT_POINT;
					NumFires--;
				}
				
				
			}
			else
			{
				PlayWaterMiss(button);	
			}
		break;
		
		default:
		break;	
		
	}
}	

void MoveToFireFighterEnd()
{
		GameState = FIREFIGHTER_END;
		MAIN_GAME_TIMER = 0;
		AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
		SendNodeNOP();	
		EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
		LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
}	


void MoveToFireFighterLastNote()
{
		GameState = FIREFIGHTER_LAST_NOTE;
		MAIN_GAME_TIMER = 0;
		AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,FF_END_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
		SendNodeNOP();	
		EAudioPlaySound(BACKGROUND_MUSIC_STREAM,FF_END_WAV);		
		//LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
}	



void PlayFireStart(BYTE button)
{
	PlayInternalNodeSound(button,INTERNAL_SOUND_FF_FIRE_ON,CurrentGameSettings.GameSoundEffectVolume,1,BACKGROUND_MUSIC_STREAM,CurrentGameSettings.GameBackgroundMusicVolume,1);
}	

void PlayWaterHit(BYTE button)
{
	PlayInternalNodeSound(button,INTERNAL_SOUND_FF_WATER_HIT,CurrentGameSettings.GameSoundEffectVolume,1,BACKGROUND_MUSIC_STREAM,CurrentGameSettings.GameBackgroundMusicVolume,1);
}	

void PlayWaterMiss(BYTE button)
{
	PlayInternalNodeSound(button,INTERNAL_SOUND_FF_WATER_MISS,CurrentGameSettings.GameSoundEffectVolume,1,BACKGROUND_MUSIC_STREAM,CurrentGameSettings.GameBackgroundMusicVolume,1);
}	



void StartNewFire()
{
	BYTE NewLocation,i;

	i=0;	
	if(NumFires >= MaxFires)
	{
		return;
	}

	NumFires++;

	if(NumFires < 3)
	{
		NewLocation = rand() & 0x06;
	}
	else
	{
		NewLocation = rand() & 0x07;
			
	}
	while( ((MyFires[NewLocation].Intensity > 0) || (NewLocation == LastFireLocation)) && (i <64))
	{
		i++;
			if(NumFires < 3)
			{
			NewLocation = rand() & 0x06;
			}
			else
			{
			NewLocation = rand() & 0x07;
					
			}
		asm("clrwdt");	
	}
	
	if(i >= 64)
	{
			return;	
	}
	else
	{
		LastFireLocation = NewLocation;
		MyFires[NewLocation].Intensity = 1;
		PlayFireStart(NewLocation);	
		MyFires[NewLocation].GrowTimer = 0;
	}
	
}	

void ResetFires()
{
	BYTE i;
	for(i=0;i<8;i++)
	{
		MyFires[i].Intensity=0;
	}		
		NumFires = 0;
		LastFireLocation = 0xFF;
}	



void GrowFires()
{
	BYTE i;
	
	for(i=0;i<8;i++)
	{
		if(MyFires[i].Intensity>0)
		{
			MyFires[i].GrowTimer++;
			if(	MyFires[i].GrowTimer > FireGrowTime)
			{
				MyFires[i].GrowTimer = 0;
				if(MyFires[i].Intensity < 8)
				{
					MyFires[i].Intensity++;	
				}
			}
		}
	}
}	





