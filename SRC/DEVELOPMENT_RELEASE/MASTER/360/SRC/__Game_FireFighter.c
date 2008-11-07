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
#include "config.h"

//*************************************************
//*******Game Parameters***************************
//*************************************************

#define FIRE_FIGHTER_TIME_OUT 		DDBACKGROUND_WAV_LENGTH
#define FIRE_ANIMATION_TIME				5
#define FIRE_HIT_POINT	2
#define FIRE_OUT_POINT	10
#define INTENSITY_START 2
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
#define FIRE_START_TIMER_P1    			GPTimer[2]
#define FIRE_START_TIMER_P2    			GPTimer[3]
#define FIRE_GROW_TIMER	    			GPTimer[4]


//*************************************************
//*******Game Variables****************************
//*************************************************

static WORD NextFireStartTime_P1;
static WORD NextFireStartTime_P2;

static BYTE FireGrowTime;					
static BYTE MaxFires;						
static BYTE SpawnLimit;					
static BYTE FireAnimationState;

 typedef struct {

	BYTE GrowTimer;
	BYTE Intensity;
	BYTE Owner;
	
} Fire;

static Fire MyFires[8];
static BYTE NumFires_P1 = 0;
static BYTE NumFires_P2 = 0;

static BYTE LastFireLocation_P1 = 0;
static BYTE LastFireLocation_P2 = 0;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void StartNewFire(BYTE Player);
void PlayWaterMiss(BYTE button);
void PlayWaterHit(BYTE button);
void PlayFireStart(BYTE button);
void PlayFireOut(BYTE button);
void MoveToFireFighterEnd();
void ResetFires();
void GrowFires();
void MoveToFireFighterLastNote();
void StartNewFires();
void AnimateFires();

//*************************************************
//*******Game Functions****************************
//*************************************************

void FireFighter(void)
{

	switch (GameState)
	{
		case INIT:
			ResetAllSlaves();
			MAIN_GAME_TIMER = 0;
			GameState = FIREFIGHTER_INIT;
			SendNodeNOP();	
			SendNodeNOP();	
			SendNodeNOP();				
		break;

		case FIREFIGHTER_INIT:

			if(MAIN_GAME_TIMER>GLOBAL_INIT_TIME)
			{
				AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,FF_BACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
				SendNodeNOP();	
				EAudioPlaySound(BACKGROUND_MUSIC_STREAM,FF_BACKGROUND_WAV);
				GameState = FIREFIGHTER;
				ResetFires();
				
				FireGrowTime = 100;					
				MaxFires = 4;						
				SpawnLimit = 3;
				
				if(GamePlayers == 1)
				{
					NextFireStartTime_P1 = 300 + (rand()&0xff);
					FIRE_START_TIMER_P1 = 0;
			
					StartNewFire(PLAYER_1);	
					
					P1ScoreDisplayState = SCORE_NORMAL;
					P2ScoreDisplayState = SCORE_BLANK;
				}
				else
				{
					NextFireStartTime_P1 = 300 + (rand()&0xff);
					NextFireStartTime_P2 = 300 + (rand()&0xff);
					
					FIRE_START_TIMER_P1 = 0;
					FIRE_START_TIMER_P2 = 0;
					StartNewFire(PLAYER_1);
					StartNewFire(PLAYER_2);	
					
					P1ScoreDisplayState = SCORE_NORMAL;
					P2ScoreDisplayState = SCORE_NORMAL;		
				}
				
			
				FIRE_ANIMATION_TIMER = 0;
				MAIN_GAME_TIMER = 0;
			
				ScoreManagerEnabled = TRUE;
		 	}
		break;

		case FIREFIGHTER:
		
			if(MAIN_GAME_TIMER>FIRE_FIGHTER_TIME_OUT)
			{
				 MoveToFireFighterLastNote();
				 LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
			}
		
			GrowFires();
			StartNewFires();
			AnimateFires();
			
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
		break;


		default:
		GameState = INIT;
		break;

		}
}




void OnButtonPressFireFighter(BYTE  button)
{

	switch(GameState)
	{
	
		case FIREFIGHTER:
		case FIREFIGHTER_LAST_NOTE:
		
			if(MyFires[button].Intensity > 0)
			{
				PlayWaterHit(button);	
				MyFires[button].Intensity--;
				
				if(MyFires[button].Owner == PLAYER_1)
				{
					Player1Score += FIRE_HIT_POINT;
					LEDSendMessage(button,YELLOW,(MyFires[button].Intensity*8)-1,0,0,30,30);
				
				}
				else
				{
					Player2Score += FIRE_HIT_POINT;
					LEDSendMessage(button,YELLOW,0,(MyFires[button].Intensity*8)-1,0,30,30);
				}		
				
				
				if(MyFires[button].Intensity == 0)
				{
					PlayFireOut(button);
					
					if(MyFires[button].Owner == PLAYER_1)
					{
						if(NumFires_P1 < SpawnLimit)
						{
							StartNewFire(PLAYER_1);	
						}
						
						Player1Score += FIRE_OUT_POINT;
						NumFires_P1--;
					}
					else
					{
						if(NumFires_P2 < SpawnLimit)
						{
							StartNewFire(PLAYER_2);	
						}
						
						Player2Score += FIRE_OUT_POINT;
						NumFires_P2--;
					}		
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


void StartNewFires()
{
		if(FIRE_START_TIMER_P1 > NextFireStartTime_P1)
			{
				FIRE_START_TIMER_P1 = 0;
				NextFireStartTime_P1 = 300 + (rand()&0xff);	
				StartNewFire(PLAYER_1);
			}
			
		if(GamePlayers == 2)
		{
			if(FIRE_START_TIMER_P2 > NextFireStartTime_P2)
			{
				FIRE_START_TIMER_P2 = 0;
				NextFireStartTime_P2 = 300 + (rand()&0xff);	
				StartNewFire(PLAYER_2);
			}
		}		
}	


void AnimateFires()
{
	BYTE i;
	
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
							if(MyFires[i].Owner == PLAYER_1)
								LEDSendMessage(i,RED,RED,0,0);	
							else
								LEDSendMessage(i,GREEN,GREEN,0,0);
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
							if(MyFires[i].Owner == PLAYER_1)
								LEDSendMessage(i,48,24,0,24,24,0,0,0);	
							else
								LEDSendMessage(i,24,48,0,24,24,0,0,0);	
							
						}
						else
						{
							LEDSendMessage(i,LEDOFF,LEDOFF,0,0);
						}
					}
					
					break;
					
					
						
				}
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
		
}	



void PlayFireStart(BYTE button)
{
	PlayInternalNodeSound(button,INTERNAL_SOUND_FF_FIRE_APPEAR,CurrentGameSettings.GameSoundEffectVolume,1,BACKGROUND_MUSIC_STREAM,CurrentGameSettings.GameBackgroundMusicVolume,1);
}	

void PlayFireOut(BYTE button)
{
	PlayInternalNodeSound(button,INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,BACKGROUND_MUSIC_STREAM,CurrentGameSettings.GameBackgroundMusicVolume,1);
}	

void PlayWaterHit(BYTE button)
{
	PlayInternalNodeSound(button,INTERNAL_SOUND_FF_WATER_HIT,CurrentGameSettings.GameSoundEffectVolume,1,BACKGROUND_MUSIC_STREAM,CurrentGameSettings.GameBackgroundMusicVolume,1);
}	

void PlayWaterMiss(BYTE button)
{
}	



void StartNewFire(BYTE Player)
{
	BYTE NewLocation,i;
	i=0;	

	if(Player == PLAYER_1)
	{
			if(NumFires_P1 >= MaxFires)
			{
				return;
			}
		
			NumFires_P1++;
		
			NewLocation = RandomButton(LastFireLocation_P1,NO_EXCLUDE,NO_EXCLUDE);
	
			while( (MyFires[NewLocation].Intensity > 0) && (i <64) )
			{
				i++;
				NewLocation = RandomButton(LastFireLocation_P1,NO_EXCLUDE,NO_EXCLUDE);	
				CLEAR_WATCHDOG;	
			}
		
			if(i >= 64)
			{
				for(i=0;i<NUM_BUTTONS;i++)
				{
					if(MyFires[i].Intensity == 0)
					{
						LastFireLocation_P1 = i;
						MyFires[i].Intensity = INTENSITY_START;
						PlayFireStart(i);	
						MyFires[i].GrowTimer = 0;
						MyFires[i].Owner = PLAYER_1;
						break;
					}	 
				}	
			}
			else
			{
				LastFireLocation_P1 = NewLocation;
				MyFires[NewLocation].Intensity = INTENSITY_START;
				PlayFireStart(NewLocation);	
				MyFires[NewLocation].GrowTimer = 0;
				MyFires[NewLocation].Owner = PLAYER_1;
			}
				
	}
	else
	{
			if(NumFires_P2 >= MaxFires)
			{
				return;
			}
		
			NumFires_P2++;
			NewLocation = RandomButton(LastFireLocation_P2,NO_EXCLUDE,NO_EXCLUDE);
	
			while( (MyFires[NewLocation].Intensity > 0) && (i <64) )
			{
				i++;
				NewLocation = RandomButton(LastFireLocation_P2,NO_EXCLUDE,NO_EXCLUDE);	
				CLEAR_WATCHDOG;	
			}
		
			if(i >= 64)
			{
				for(i=0;i<NUM_BUTTONS;i++)
				{
					if(MyFires[i].Intensity == 0)
					{
						LastFireLocation_P2 = i;
						MyFires[i].Intensity = INTENSITY_START;
						PlayFireStart(i);	
						MyFires[i].GrowTimer = 0;
						MyFires[i].Owner = PLAYER_2;
						break;
					}	 
				}	
			}
			else
			{
				LastFireLocation_P2 = NewLocation;
				MyFires[NewLocation].Intensity = INTENSITY_START;
				PlayFireStart(NewLocation);	
				MyFires[NewLocation].GrowTimer = 0;
				MyFires[NewLocation].Owner = PLAYER_2;
			}
		
	}		
	
}	





void ResetFires()
{
	BYTE i;
	for(i=0;i<8;i++)
	{
		MyFires[i].Intensity=0;
		MyFires[i].Owner = PLAYER_1;
		MyFires[i].GrowTimer = 0;
	}	
		
		NumFires_P1 = 0;
		NumFires_P2 = 0;
		
		LastFireLocation_P1 = 0xFF;
		LastFireLocation_P2 = 0xFF;
}	



void GrowFires()
{
	BYTE i;
	
	if(FIRE_GROW_TIMER>1)
		{
			FIRE_GROW_TIMER = 0;
	
			for(i=0;i<8;i++)
			{
				if(MyFires[i].Intensity>0)
				{
					MyFires[i].GrowTimer++;
					if(	MyFires[i].GrowTimer > FireGrowTime)
					{
						MyFires[i].GrowTimer = 0;
						if(MyFires[i].Intensity < 5)
						{
							MyFires[i].Intensity++;	
						}
					}
				}
			}
		}
	
}	





