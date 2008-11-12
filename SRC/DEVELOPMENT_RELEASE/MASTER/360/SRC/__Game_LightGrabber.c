#include "__Game_LightGrabber.h"
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "_RootButtonHandler.h"
#include "_GameProcess.h"
#include <stdlib.h>
#include "LED-Display.h"
#include "EEPROMRoutines.h"
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

#define POINTS_TO_GET_TO_BONUS_LEVEL	450
#define LIGHT_GRAB_POINT				10
#define LIGHT_GRAB_BONUS_POINT			25

#define NUMBER_OF_BONUS_LIGHTS_1P			0x4
#define BONUS_POINT_INTERVAL_1P			(LIGHT_GRABBER_GAME_TIME/NUMBER_OF_BONUS_LIGHTS_1P)
#define BONUS_TIMEOUT_1P					BONUSSOUND_WAV_LENGTH*3

#define NUMBER_OF_BONUS_LIGHTS_2P			0x8
#define BONUS_POINT_INTERVAL_2P		  	(LIGHT_GRABBER_GAME_TIME/NUMBER_OF_BONUS_LIGHTS_2P)
#define BONUS_TIMEOUT2_P					BONUSSOUND_WAV_LENGTH*3

#define LIGHT_TO_GRAB_FLASH_INTERVAL	50
#define BONUS_FLASH_INTERVAL			25

#define LIGHT_GRABBER_GAME_TIME 		LIGHTGRABBERBACKGROUNDMUSIC_WAV_LENGTH
#define LIGHT_GRABBER_BONUS_GAME_TIME   LIGHTGRABBERBACKGROUNDMUSIC_BONUS_WAV_LENGTH

#define NODE_0_AUDIO_REPEAT_TIME	110
#define NODE_1_AUDIO_REPEAT_TIME	110	
#define NODE_2_AUDIO_REPEAT_TIME	110
#define NODE_3_AUDIO_REPEAT_TIME	110
#define NODE_4_AUDIO_REPEAT_TIME	110
#define NODE_5_AUDIO_REPEAT_TIME	110
#define NODE_6_AUDIO_REPEAT_TIME	110
#define NODE_7_AUDIO_REPEAT_TIME	110


//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define BACKGROUND_MUSIC_STREAM			0x00
#define P1_NODE_SOUND_EFFECT_STREAM		0x01
#define P2_NODE_SOUND_EFFECT_STREAM		0x02
#define BONUS_STREAM					0x03

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define LIGHT_GRABBER_INIT					0x01
#define LIGHT_GRABBER_1P					0x02
#define LIGHT_GRABBER_END					0x03
#define LIGHT_GRABBER_2P				 	0x04
#define LIGHT_GRABBER_LAST_NOTE				0x05

//*************************************************
//******** System Timer MACROS*********************
//*************************************************


#define MAIN_GAME_TIMER					GPTimer[0]	
#define BONUS_INTERVAL_TIMER			GPTimer[1]
#define BONUS_LED_FLASH_TIMER			GPTimer[2]
#define P1_LIGHT_GRAB_FLASH_TIMER		GPTimer[3]
#define P1_NODE_AUDIO_REPEAT_TIMER		GPTimer[4]
#define P2_LIGHT_GRAB_FLASH_TIMER		GPTimer[5]
#define P2_NODE_AUDIO_REPEAT_TIMER		GPTimer[6]

//*************************************************
//*******Game Variables****************************
//*************************************************

static WORD NodeAudioRepeatTime[2] = {NODE_0_AUDIO_REPEAT_TIME,NODE_0_AUDIO_REPEAT_TIME};
static WORD Num1Hits = 0;
static WORD Num2Hits = 0;
static BYTE P1_CurrentLightToGrab=0;
static BYTE P2_CurrentLightToGrab=0;
static BYTE BonusLocation=0;
static BYTE BonusOwner=PLAYER_1;
static BOOL BonusActive = FALSE;
static BOOL BonusLevelActive = FALSE;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void LightGrabberPlayNodeSound(BYTE Player,BYTE button, BOOL Loop);
void LightGrabberPlayButtonFeebackSound(BYTE button);
void FlashP1LightToGrab(BYTE Node);
void FlashP2LightToGrab(BYTE Node);
void LightGrabberPlayBonusSound(BYTE P1_BonusLocation);
void FlashBonus(BYTE Owner, BYTE Node);
void MoveToLightGrabberEnd();
void MoveToLightGrabberLastNote();
void MoveToLightGrabberBonusLevel();

//*************************************************
//*******Game Functions****************************
//*************************************************

void LightGrabber(void)
{
			
	switch (GameState)
	{
		
		case INIT:
		
			ResetAudioAndLEDS();
			ScoreManagerEnabled = FALSE;
			GameState = LIGHT_GRABBER_INIT;	
			MAIN_GAME_TIMER = 0;
			
		break;
		
		case LIGHT_GRABBER_INIT:
	
		if(MAIN_GAME_TIMER>GLOBAL_INIT_TIME)
		{
			if(GamePlayers == 1)
			{
				
				
				P1_CurrentLightToGrab = RandomButton(NO_EXCLUDE,NO_EXCLUDE, NO_EXCLUDE);
		    	LightGrabberPlayNodeSound(PLAYER_1,P1_CurrentLightToGrab,FALSE);
		  	    FlashP1LightToGrab(P1_CurrentLightToGrab);
		  	    GameState = LIGHT_GRABBER_1P;
				
				BonusLevelActive = FALSE;
		
				P1_LIGHT_GRAB_FLASH_TIMER=0;		
				BONUS_LED_FLASH_TIMER = 0;
				P1_NODE_AUDIO_REPEAT_TIMER = 0;	
			
				Player1Score = 0;
				Player2Score = 0;
								
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_BLANK;
				
			
			}
			else
			{
				P1_CurrentLightToGrab = RandomButton(NO_EXCLUDE,NO_EXCLUDE, NO_EXCLUDE);
		    	P2_CurrentLightToGrab = RandomButton(P1_CurrentLightToGrab,NO_EXCLUDE, NO_EXCLUDE);
		    		    	
		    	LightGrabberPlayNodeSound(PLAYER_1,P1_CurrentLightToGrab,FALSE);
		  	    LightGrabberPlayNodeSound(PLAYER_2,P2_CurrentLightToGrab,FALSE);
		  	    
		  	    FlashP1LightToGrab(P1_CurrentLightToGrab);
		  	    FlashP2LightToGrab(P2_CurrentLightToGrab);
		  	    
		  	    GameState = LIGHT_GRABBER_2P;
										
				P1_LIGHT_GRAB_FLASH_TIMER=0;		
				P1_NODE_AUDIO_REPEAT_TIMER = 0;	
				P2_LIGHT_GRAB_FLASH_TIMER=0;		
				P2_NODE_AUDIO_REPEAT_TIMER = 0;	
				
				BonusOwner = PLAYER_2;
				
				Player1Score = 0;
				Player2Score = 0;
				
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_NORMAL;
			
			}
		
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,LIGHTGRABBERBACKGROUNDMUSIC_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,LIGHTGRABBERBACKGROUNDMUSIC_WAV);
			
			Player1Score=0;
			Player2Score=0;
			Num1Hits=0;
			Num2Hits=0;
			
			MAIN_GAME_TIMER=0;	
			BONUS_INTERVAL_TIMER=0;					
			BONUS_LED_FLASH_TIMER = 0;
			BonusActive = FALSE;
			ScoreManagerEnabled = TRUE;
		}
					
		break;
		
				
		case LIGHT_GRABBER_1P:
	
		if(BonusLevelActive == FALSE)
		{
			if(MAIN_GAME_TIMER>LIGHT_GRABBER_GAME_TIME)
			{
				MAIN_GAME_TIMER= 0;
				
				if(Player1Score > POINTS_TO_GET_TO_BONUS_LEVEL)
				{
				
					MoveToLightGrabberBonusLevel();
					
				}
				else
				
				{
					MoveToLightGrabberLastNote();
				}
			    
			}
		}
		else if(MAIN_GAME_TIMER>LIGHT_GRABBER_BONUS_GAME_TIME ) 
		{
				MoveToLightGrabberEnd();
		}
				
				
		if(P1_LIGHT_GRAB_FLASH_TIMER>LIGHT_TO_GRAB_FLASH_INTERVAL)
			{
				 FlashP1LightToGrab(P1_CurrentLightToGrab);
			     P1_LIGHT_GRAB_FLASH_TIMER = 0;	
			}
			
		if(P1_NODE_AUDIO_REPEAT_TIMER>NodeAudioRepeatTime[PLAYER_1])
			{
				 LightGrabberPlayNodeSound(PLAYER_1,P1_CurrentLightToGrab,FALSE);
				 P1_NODE_AUDIO_REPEAT_TIMER=0;
			}
			
		if(BONUS_INTERVAL_TIMER > BONUS_POINT_INTERVAL_1P)
			{
				BonusLocation = RandomButton(P1_CurrentLightToGrab,NO_EXCLUDE, NO_EXCLUDE);
				BonusActive = TRUE;	
				FlashBonus(PLAYER_1,BonusLocation);
				LightGrabberPlayBonusSound(BonusLocation);
				BONUS_INTERVAL_TIMER = 0;
			}
			
		if(BonusActive == TRUE)
			{
				if(BONUS_INTERVAL_TIMER > BONUS_TIMEOUT_1P)
				{
					BonusActive = FALSE;
					AudioNodeEnable(BonusLocation,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,LIGHTGRABBERBACKGROUNDMUSIC_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
					SendNodeNOP();	
				}
				
				else if(BONUS_LED_FLASH_TIMER>BONUS_FLASH_INTERVAL)
				{
					BONUS_LED_FLASH_TIMER = 0;
					FlashBonus(PLAYER_1,BonusLocation);
				}
			}
		
		break;
		
		
		case LIGHT_GRABBER_2P:
		
		if(MAIN_GAME_TIMER>LIGHT_GRABBER_GAME_TIME)
		 {
			MAIN_GAME_TIMER= 0;
				
			MoveToLightGrabberLastNote();
		
					
							    
			}
		
						
		if(P1_LIGHT_GRAB_FLASH_TIMER>LIGHT_TO_GRAB_FLASH_INTERVAL)
			{
				 FlashP1LightToGrab(P1_CurrentLightToGrab);
			     P1_LIGHT_GRAB_FLASH_TIMER = 0;	
			}
			
		if(P1_NODE_AUDIO_REPEAT_TIMER>NodeAudioRepeatTime[PLAYER_1])
			{
				 LightGrabberPlayNodeSound(PLAYER_1,P1_CurrentLightToGrab,FALSE);
				 P1_NODE_AUDIO_REPEAT_TIMER=0;
			}
			
		
		if(P2_LIGHT_GRAB_FLASH_TIMER>LIGHT_TO_GRAB_FLASH_INTERVAL)
			{
				 FlashP2LightToGrab(P2_CurrentLightToGrab);
			     P2_LIGHT_GRAB_FLASH_TIMER = 0;	
			}
			
		if(P2_NODE_AUDIO_REPEAT_TIMER>NodeAudioRepeatTime[PLAYER_2])
			{
				 LightGrabberPlayNodeSound(PLAYER_2,P2_CurrentLightToGrab,FALSE);
				 P2_NODE_AUDIO_REPEAT_TIMER=0;
			}
			
				
					
		if(BONUS_INTERVAL_TIMER > BONUS_POINT_INTERVAL_1P)
			{
				if(BonusOwner == PLAYER_1)
				{
					BonusOwner = PLAYER_2;	
				}
				else
				{
					BonusOwner = PLAYER_1;
				}
				BonusLocation = RandomButton(P1_CurrentLightToGrab,P2_CurrentLightToGrab, NO_EXCLUDE);
				BonusActive = TRUE;	
				FlashBonus(BonusOwner,BonusLocation);
				LightGrabberPlayBonusSound(BonusLocation);
				
				BONUS_INTERVAL_TIMER = 0;
			}
			
		if(BonusActive == TRUE)
			{
				if(BONUS_INTERVAL_TIMER > BONUS_TIMEOUT_1P)
				{
					BonusActive = FALSE;
					AudioNodeEnable(BonusLocation,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,LIGHTGRABBERBACKGROUNDMUSIC_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
					SendNodeNOP();	
				}
				
				else if(BONUS_LED_FLASH_TIMER>BONUS_FLASH_INTERVAL)
				{
					BONUS_LED_FLASH_TIMER = 0;
					FlashBonus(BonusOwner,BonusLocation);
				}
			}
		
		break;
		
		case LIGHT_GRABBER_END:
			if(MAIN_GAME_TIMER>ENDING_WAV_LENGTH)
			{
					ResetToGameSelector();
			}
		break;
		
		case LIGHT_GRABBER_LAST_NOTE:
			if(MAIN_GAME_TIMER>LG_END_WAV_LENGTH)
			{
					MoveToLightGrabberEnd();
			}
		break;
		
		
		default:
		GameState = INIT;
		break;

		}
}




void OnButtonPressLightGrabber( BYTE  button)
{
	switch(GameState)
	{
		
		case LIGHT_GRABBER_1P:
			if(button == P1_CurrentLightToGrab)
				{
					LightGrabberPlayButtonFeebackSound(button);
				   
				    LEDSendMessage(button,LEDOFF,LEDOFF,0,0);
				
					if(BonusActive == TRUE)
					{
						P1_CurrentLightToGrab = RandomButton(BonusLocation,NO_EXCLUDE,button);
					}
					else
					{
						P1_CurrentLightToGrab = RandomButton(button,NO_EXCLUDE, NO_EXCLUDE);
					}
					
				    LightGrabberPlayNodeSound(PLAYER_1,P1_CurrentLightToGrab , FALSE);
				    FlashP1LightToGrab(P1_CurrentLightToGrab);
					P1_LIGHT_GRAB_FLASH_TIMER = 0;
					P1_NODE_AUDIO_REPEAT_TIMER = 0;
					Player1Score+=LIGHT_GRAB_POINT;
				
					
				}
			if(BonusActive == TRUE)
			{
				if(button == BonusLocation)
				{
				    LEDSendMessage(button,LEDOFF,LEDOFF,0,0);
					BonusActive = FALSE;
					LightGrabberPlayButtonFeebackSound(button);
				    Player1Score+=LIGHT_GRAB_BONUS_POINT;
					
	
				}
			}
		break;
		
		case LIGHT_GRABBER_2P:
			if(button == P1_CurrentLightToGrab)
				{
					LightGrabberPlayButtonFeebackSound(button);
				    LEDSendMessage(button,LEDOFF,LEDOFF,0,0);
				
					if(BonusActive == TRUE)
					{
						P1_CurrentLightToGrab = RandomButton(P2_CurrentLightToGrab,BonusLocation,button);
					}
					else
					{
						P1_CurrentLightToGrab = RandomButton(P2_CurrentLightToGrab,NO_EXCLUDE,button);
					}
					
				  LightGrabberPlayNodeSound(PLAYER_1,P1_CurrentLightToGrab , FALSE);
				  FlashP1LightToGrab(P1_CurrentLightToGrab);
				  P1_LIGHT_GRAB_FLASH_TIMER = 0;
				  P1_NODE_AUDIO_REPEAT_TIMER = 0;
				  Player1Score+=LIGHT_GRAB_POINT;
				
					
				}
				
			if((BonusActive == TRUE) && (button == BonusLocation))
			{
			
				    LEDSendMessage(button,LEDOFF,LEDOFF,0,0);
					BonusActive = FALSE;
					LightGrabberPlayButtonFeebackSound(button);
					if(BonusOwner == PLAYER_1)
					{
				    	Player1Score+=LIGHT_GRAB_BONUS_POINT;
					}
					else
					{
						Player2Score+=LIGHT_GRAB_BONUS_POINT;
					}
	  		
	
			}
			
		
		
			if(button == P2_CurrentLightToGrab)
				{
					LightGrabberPlayButtonFeebackSound(button);
				    LEDSendMessage(button,LEDOFF,LEDOFF,0,0);
				
					if(BonusActive == TRUE)
					{
						P2_CurrentLightToGrab = RandomButton(P1_CurrentLightToGrab,BonusLocation,button);
					}
					else
					{
						P2_CurrentLightToGrab = RandomButton(P1_CurrentLightToGrab,NO_EXCLUDE,button);
					}
					
				  LightGrabberPlayNodeSound(PLAYER_2,P2_CurrentLightToGrab , FALSE);
				  FlashP2LightToGrab(P2_CurrentLightToGrab);
				  P2_LIGHT_GRAB_FLASH_TIMER = 0;
				  P2_NODE_AUDIO_REPEAT_TIMER = 0;
				  Player2Score+=LIGHT_GRAB_POINT;
				  
					
				}
	
		break;
		
	}
	
}


void LightGrabberPlayBonusSound(BYTE button)
{
	AudioNodeEnable(button,BONUS_STREAM,BACKGROUND_MUSIC_STREAM	,1,1,BONUSSOUND_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(BONUS_STREAM,BONUSSOUND_WAV);
}

void LightGrabberPlayButtonFeebackSound(BYTE button)
{

  	PlayInternalNodeSound(button,INTERNAL_SOUND_POSITIVE_FEEDBACK,CurrentGameSettings.GameSoundEffectVolume,1,BACKGROUND_MUSIC_STREAM,CurrentGameSettings.GameBackgroundMusicVolume,1);

}	

void LightGrabberPlayNodeSound(BYTE Player, BYTE button, BYTE Loop)
{
	
	AudioReSync(button);
	
	switch(button)
	{
		case 0:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE0_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NODE0_WAV);
				NodeAudioRepeatTime[PLAYER_1] = NODE_0_AUDIO_REPEAT_TIME;
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE0_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NODE0_WAV);
				NodeAudioRepeatTime[PLAYER_2] = NODE_0_AUDIO_REPEAT_TIME;
			}
		break;
		
		case 1:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE1_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NODE1_WAV);
				NodeAudioRepeatTime[PLAYER_1] = NODE_1_AUDIO_REPEAT_TIME;
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE1_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NODE1_WAV);
				NodeAudioRepeatTime[PLAYER_2] = NODE_1_AUDIO_REPEAT_TIME;
			}
		break;
	
		case 2:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE2_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NODE2_WAV);
				NodeAudioRepeatTime[PLAYER_1] = NODE_2_AUDIO_REPEAT_TIME;
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE2_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NODE2_WAV);
				NodeAudioRepeatTime[PLAYER_2] = NODE_2_AUDIO_REPEAT_TIME;
			}
		break;
	
		case 3:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE3_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NODE3_WAV);
				NodeAudioRepeatTime[PLAYER_1] = NODE_3_AUDIO_REPEAT_TIME;
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE3_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NODE3_WAV);
				NodeAudioRepeatTime[PLAYER_2] = NODE_3_AUDIO_REPEAT_TIME;
			}
		break;
	
		case 4:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE4_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NODE4_WAV);
				NodeAudioRepeatTime[PLAYER_1] = NODE_4_AUDIO_REPEAT_TIME;
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE4_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NODE4_WAV);
				NodeAudioRepeatTime[PLAYER_2] = NODE_4_AUDIO_REPEAT_TIME;
			}
		break;
	
		case 5:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE5_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NODE5_WAV);
				NodeAudioRepeatTime[PLAYER_1] = NODE_5_AUDIO_REPEAT_TIME;
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE5_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NODE5_WAV);
				NodeAudioRepeatTime[PLAYER_2] = NODE_5_AUDIO_REPEAT_TIME;
			}
		break;
	
		case 6:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE6_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NODE6_WAV);
				NodeAudioRepeatTime[PLAYER_1] = NODE_6_AUDIO_REPEAT_TIME;
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE6_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NODE6_WAV);
				NodeAudioRepeatTime[PLAYER_2] = NODE_6_AUDIO_REPEAT_TIME;
			}
		break;
	
		case 7:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE7_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NODE7_WAV);
				NodeAudioRepeatTime[PLAYER_1] = NODE_7_AUDIO_REPEAT_TIME;
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NODE0_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NODE0_WAV);
				NodeAudioRepeatTime[PLAYER_2] = NODE_0_AUDIO_REPEAT_TIME;
			}
		break;
	
		default:
		break;
	}
	
}	


void FlashP1LightToGrab(BYTE Node)
{
	 LEDSendMessage(Node,RED,RED,0,0);
}	 

void FlashP2LightToGrab(BYTE Node)
{
	 LEDSendMessage(Node,GREEN,GREEN,0,0);
}	 


void FlashBonus(BYTE Owner,BYTE Node)
{
	if(Owner == PLAYER_1)
	{
	 LEDSendMessage(Node,RED,RED,BONUS_FLASH_INTERVAL>>1,0);
	}
	else
	{
	 LEDSendMessage(Node,GREEN,GREEN,BONUS_FLASH_INTERVAL>>1,0);	
	}
}



void MoveToLightGrabberEnd()
{
	MAIN_GAME_TIMER = 0;
	GameState = LIGHT_GRABBER_END;	
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
    AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
				
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

void MoveToLightGrabberLastNote()
{
	MAIN_GAME_TIMER = 0;
	GameState = LIGHT_GRABBER_LAST_NOTE;	
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
    AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,LG_END_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,LG_END_WAV);		
	
}		


void MoveToLightGrabberBonusLevel()
{
	P1_CurrentLightToGrab = RandomButton(NO_EXCLUDE,NO_EXCLUDE,NO_EXCLUDE);
	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
	MAIN_GAME_TIMER = 0;
	GameState = LIGHT_GRABBER_1P;
	BonusLevelActive = TRUE;
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,LIGHTGRABBERBACKGROUNDMUSIC_BONUS_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM,LIGHTGRABBERBACKGROUNDMUSIC_BONUS_WAV );	
	

	
}		


