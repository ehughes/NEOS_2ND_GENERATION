#include "__Game_Rodeo.h"
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

#define ELEMENT_DISPLAY_TIME		25
#define INITIAL_ELEMENT_DELAY		25
#define	BONUS_RODEO_HIT	 25
#define	NORMAL_RODEO_HIT 10

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define BACKGROUND_MUSIC_STREAM_A		0x01
#define BACKGROUND_MUSIC_STREAM_B		0x02
#define NEW_PATTERN_STREAM				0x03
#define BONUS_HIT_STREAM				0x04

//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER		 		 GPTimer[0]
#define WAIT_FOR_RESPONSE_TIMER		 GPTimer[1]
#define PATTERN_DISPLAY_TIMER		 GPTimer[2]

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************
#define RODEO									0x01
#define RODEO_DISPLAY_NEXT_PATTERN				0x02
#define RODEO_END								0x03
#define RODEO_LAST_NOTE							0x04
#define RODEO_INIT								0x05


#define PATTERN_TOP_BOTTOM 0
#define PATTERN_SOLID_WALL 1
#define PATTEN_CRISS_CROSS 2

//*************************************************
//*******Game Variables****************************
//*************************************************

static BYTE P1RodeoPattern[4];
static BYTE P2RodeoPattern[4];
static BYTE ElementDisplayTime;
static BYTE P1HitCount;
static BYTE P2HitCount;
static BYTE HitList[9];
static BYTE HitIndex;
static BYTE LastRodeoPattern;
static BYTE CurrentRodeoPattern;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void DisplayRodeoPatterns(BYTE RodeoPattern);
BYTE GenerateRodeoPattern();
BOOL IsPartOfPattern(BYTE button, BYTE * Pattern);
static BYTE PatternDisplayIndex = 0;
BOOL HasBeenHit(BYTE button);
static BYTE LastButtonBeforeNextPattern;
void EnterPatternDisplayState();
void PlayRodeoBonus(BYTE button);
void P1RodeoButtonProcess(BYTE button);
void P2RodeoButtonProcess(BYTE button);
void PlayRodeoHitSound(BYTE button);
BYTE GetRodeoBackGroundStream(BYTE button);
void MoveToRodeoLastNote();
void MoveToRodeoEnd();
//*************************************************
//*******Game Functions****************************
//*************************************************

void Rodeo(void)
{
	BYTE k=0;
	BYTE Sound;
	WORD Length;
			
	switch (GameState)
	{
		
		case INIT:
			ResetAudioAndLEDS();
			SendNodeNOP();
			SendNodeNOP();
			GameState = RODEO_INIT;
			MAIN_GAME_TIMER = 0;
		   						
		break;

		case RODEO_INIT:
		
		if(MAIN_GAME_TIMER>1)
		{
			MAIN_GAME_TIMER = 0;
			
		    AudioNodeEnable(0,BACKGROUND_MUSIC_STREAM_A,BACKGROUND_MUSIC_STREAM_A,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
		    AudioNodeEnable(1,BACKGROUND_MUSIC_STREAM_A,BACKGROUND_MUSIC_STREAM_A,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
		    AudioNodeEnable(4,BACKGROUND_MUSIC_STREAM_A,BACKGROUND_MUSIC_STREAM_A,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
		    AudioNodeEnable(5,BACKGROUND_MUSIC_STREAM_A,BACKGROUND_MUSIC_STREAM_A,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
		
		    AudioNodeEnable(2,BACKGROUND_MUSIC_STREAM_B,BACKGROUND_MUSIC_STREAM_B,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
		    AudioNodeEnable(3,BACKGROUND_MUSIC_STREAM_B,BACKGROUND_MUSIC_STREAM_B,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
		    AudioNodeEnable(6,BACKGROUND_MUSIC_STREAM_B,BACKGROUND_MUSIC_STREAM_B,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
		    AudioNodeEnable(7,BACKGROUND_MUSIC_STREAM_B,BACKGROUND_MUSIC_STREAM_B,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM_A,RODEOBACKGROUND_A_WAV);
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM_B,RODEOBACKGROUND_B_WAV);
		
			GameState = RODEO;
		
			P1HitCount = 0;
			P2HitCount = 0;
			Player1Score = 0;
			Player2Score = 0;
			LastRodeoPattern = 0;
			HitIndex=0;	
			PATTERN_DISPLAY_TIMER = 0xFFFF;
			PatternDisplayIndex = 0;
			GenerateRodeoPattern();
			EnterPatternDisplayState();
			MAIN_GAME_TIMER = 0;
						
			if(GamePlayers == 1)
			{
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_BLANK;
			}
			else
			{
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_NORMAL;
			}
				ScoreManagerEnabled = TRUE;
			
		}	
		
		
		break;
		
		case RODEO:
		
		if(MAIN_GAME_TIMER>RODEOBACKGROUND_A_WAV_LENGTH)
		{
			MoveToRodeoLastNote();	
		}
				
		break;
		
		
		case RODEO_LAST_NOTE:
		
		if(MAIN_GAME_TIMER>RODEOEND_WAV_LENGTH)
		{
			MoveToRodeoEnd();	
			
		}
		
		break;
		
		case RODEO_DISPLAY_NEXT_PATTERN:
		
		if(PATTERN_DISPLAY_TIMER > ElementDisplayTime)
		{
			PATTERN_DISPLAY_TIMER = 0;
			
			switch(	PatternDisplayIndex)
			{
				case 0:
					switch(CurrentRodeoPattern)
						{
						case	PATTERN_TOP_BOTTOM:
							Sound  = RODEO_TOPBOTTOM_01_WAV;
							Length = RODEO_TOPBOTTOM_01_WAV_LENGTH; 
						break;
						
						case	PATTERN_SOLID_WALL:
							Sound  = RODEO_SOLIDWALL_01_WAV;
							Length = RODEO_SOLIDWALL_01_WAV_LENGTH; 
						break;
						
						case	PATTEN_CRISS_CROSS:
						default:
							Sound  = RODEO_CRISSCROSS_01_WAV;
							Length = RODEO_CRISSCROSS_01_WAV_LENGTH;
						break;
						}
				
					for(k=0;k<4;k++)
					{
						BYTE Stream;
							Stream = GetRodeoBackGroundStream(1);
  				    		AudioNodeEnable(1,NEW_PATTERN_STREAM,Stream,AUDIO_ON_BEFORE_AFTER_TIMEOUT,Length,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
							Stream = GetRodeoBackGroundStream(3);
  				    		AudioNodeEnable(3,NEW_PATTERN_STREAM,Stream,AUDIO_ON_BEFORE_AFTER_TIMEOUT,Length,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
							Stream = GetRodeoBackGroundStream(5);
  				    		AudioNodeEnable(5,NEW_PATTERN_STREAM,Stream,AUDIO_ON_BEFORE_AFTER_TIMEOUT,Length,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
							Stream = GetRodeoBackGroundStream(7);
  				    		AudioNodeEnable(7,NEW_PATTERN_STREAM,Stream,AUDIO_ON_BEFORE_AFTER_TIMEOUT,Length,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
						
					
					}
					SendNodeNOP();	
					EAudioPlaySound(NEW_PATTERN_STREAM,Sound);
																	   	
			   		PatternDisplayIndex++;
					ElementDisplayTime = 0;	
				break;
				
				case 1:
					for(k=0;k<4;k++)
							{
			  			   		LEDSendMessage(P1RodeoPattern[k],LEDOFF,RED,0,75);
			  			   		if(GamePlayers == 2)
								{
									LEDSendMessage(P2RodeoPattern[k],LEDOFF,GREEN,0,75);
								}
							}
		   				PatternDisplayIndex=2;
		   				ElementDisplayTime = ELEMENT_DISPLAY_TIME;	
			   break;			
						
				case 2:
					GameState = RODEO;
				break;	
				
				default:
				break;		
			}
			
		}
		break;
		
		
			
		case RODEO_END:
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




void OnButtonPressRodeo(BYTE button)
{
switch(GamePlayers)
{
	
	case 1:
	
		switch(GameState)
		{	
			
			case RODEO:
			if(HasBeenHit(button) == FALSE)
		    {
			    HitList[HitIndex] = button;
				HitIndex++;
				
				if(IsPartOfPattern(button, &P1RodeoPattern[0]))
				{
					P1RodeoButtonProcess(button);	
				}
			
			}
			break;
			
			default:
			break;
		}
	break;
	
	case 2:

		switch(GameState)
			{	
				
				case RODEO:
					if(HasBeenHit(button) == FALSE)
				    {
					    HitList[HitIndex] = button;
						HitIndex++;
						
						if(IsPartOfPattern(button, &P1RodeoPattern[0]))
						{
							P1RodeoButtonProcess(button);	
						}
					
						else if(IsPartOfPattern(button, &P2RodeoPattern[0]))
						{
							P2RodeoButtonProcess(button);	
						}
					
					}
					break;
			
				default:
				break;
			}
		break;	

	default:
	break;
	
}	
	
}


void P1RodeoButtonProcess(BYTE button)
{
		    	P1HitCount++;
				LEDSendMessage(button,RED,LEDOFF,0,10);	
		 		
		 		if(P1HitCount == 4)
		 		{
			 		Player1Score += BONUS_RODEO_HIT;
			 		GenerateRodeoPattern();
			 		EnterPatternDisplayState();
			 		ElementDisplayTime = INITIAL_ELEMENT_DELAY;
			 		LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
					P1HitCount = 0;
					P2HitCount = 0;
					HitIndex = 0;
					LastButtonBeforeNextPattern = button;		
					PlayRodeoHitSound(button);
			 	}
			 	else
			 	{
					Player1Score += NORMAL_RODEO_HIT;
					PlayRodeoHitSound(button);
		 		}
		 
	
}	

void P2RodeoButtonProcess(BYTE button)
{
				P2HitCount++;
				LEDSendMessage(button,GREEN,LEDOFF,0,10);	
		 		
		 		if(P2HitCount == 4)
		 		{
			 		Player2Score += BONUS_RODEO_HIT;
			 		GenerateRodeoPattern();
			 		EnterPatternDisplayState();
			 		LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
			 		ElementDisplayTime = INITIAL_ELEMENT_DELAY;
					P1HitCount = 0;
					P2HitCount = 0;
					HitIndex = 0;
					LastButtonBeforeNextPattern = button;
					
						PlayRodeoHitSound(button);
			 	}
			 	else
			 	{
					Player2Score += NORMAL_RODEO_HIT;
					PlayRodeoHitSound(button);
		 		}
		 		
	
}	

void EnterPatternDisplayState()
{
		GameState = RODEO_DISPLAY_NEXT_PATTERN;
		PATTERN_DISPLAY_TIMER = 0;
		PatternDisplayIndex = 0;
}	

BYTE GenerateRodeoPattern()
{
	
	BYTE i=0;
	BYTE RandomOrientation = rand() & 0x07;

	LastRodeoPattern = CurrentRodeoPattern;
	CurrentRodeoPattern = rand() & 0x3;
	
	while(((CurrentRodeoPattern > 2) || (CurrentRodeoPattern == LastRodeoPattern)) && (i<128))
	{
		CurrentRodeoPattern = rand() & 0x3;
		i++;		
	}
	
	if((i>127) || (CurrentRodeoPattern == LastRodeoPattern))
	{
			
		if(CurrentRodeoPattern == 2)
		{
			CurrentRodeoPattern = 0;
		}
		else
		{
			CurrentRodeoPattern++;	
		}
	}
	
	switch(CurrentRodeoPattern)
	{
		case PATTERN_TOP_BOTTOM:
		
			if(rand()&0x01)
			{
			P1RodeoPattern[0] = 0;
			P1RodeoPattern[1] = 2;
			P1RodeoPattern[2] = 4;
			P1RodeoPattern[3] = 6;
			P2RodeoPattern[0] = 1;
			P2RodeoPattern[1] = 3;
			P2RodeoPattern[2] = 5;
			P2RodeoPattern[3] = 7;
			}
			else
    		{
	
			P2RodeoPattern[0] = 0;
			P2RodeoPattern[1] = 2;
			P2RodeoPattern[2] = 4;
			P2RodeoPattern[3] = 6;
			P1RodeoPattern[0] = 1;
			P1RodeoPattern[1] = 3;
			P1RodeoPattern[2] = 5;
			P1RodeoPattern[3] = 7;
					
			}
					
		break;
			
	
		case PATTERN_SOLID_WALL:
		
			P1RodeoPattern[0] = 0;
			P1RodeoPattern[1] = 1;
			P1RodeoPattern[2] = 2;
			P1RodeoPattern[3] = 3;
			P2RodeoPattern[0] = 4;
			P2RodeoPattern[1] = 5;
			P2RodeoPattern[2] = 6;
			P2RodeoPattern[3] = 7;
 			
 			for(i=0;i<4;i++)
 			{
	 			P1RodeoPattern[i] += RandomOrientation;
		 		P1RodeoPattern[i] &= 0x07;
		 		P2RodeoPattern[i] += RandomOrientation;
	 			P2RodeoPattern[i] &= 0x07;
	 		}
		
		break;
		
		case	PATTEN_CRISS_CROSS:
		
			P1RodeoPattern[0] = 0;
			P1RodeoPattern[1] = 1;
			P1RodeoPattern[2] = 4;
			P1RodeoPattern[3] = 5;
			P2RodeoPattern[0] = 2;
			P2RodeoPattern[1] = 3;
			P2RodeoPattern[2] = 6;
			P2RodeoPattern[3] = 7;

			for(i=0;i<4;i++)
 			{
	 			P1RodeoPattern[i] += RandomOrientation;
		 		P1RodeoPattern[i] &= 0x07;
		 		P2RodeoPattern[i] += RandomOrientation;
	 			P2RodeoPattern[i] &= 0x07;
	 		}
		


		default:
		
		
		break;
		
	}

	return 	CurrentRodeoPattern;
}	


BOOL IsPartOfPattern(BYTE button, BYTE * Pattern)
{
	BYTE i;
	
	for(i=0;i<4;i++)
	{
		if(button == Pattern[i])
		{
			return TRUE;	
		}
	}
	
	return FALSE;
}	

BOOL HasBeenHit(BYTE button)
{
	
	BYTE i;
	
	if(HitIndex == 0)
	{
		return FALSE;	
	}
	else
	{
		for(i=0;i<HitIndex;i++)
		{
			if(button == HitList[i])
			{
				return TRUE;	
			}
			
		}
	}
		
	
	return FALSE;
	
}	


void PlayRodeoBonus(BYTE button)
{
	
		BYTE Stream;
	
		Stream = GetRodeoBackGroundStream(button);
	
		AudioNodeEnable(button,BONUS_HIT_STREAM,Stream,AUDIO_ON_BEFORE_AFTER_TIMEOUT, BONUSSOUND_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		  	
		SendNodeNOP();	
		EAudioPlaySound(BONUS_HIT_STREAM,BONUSSOUND_WAV);
				
}


void PlayRodeoHitSound(BYTE button)
{

	BYTE Stream;
	
	Stream = GetRodeoBackGroundStream(button);
	
	PlayInternalNodeSound(button,INTERNAL_SOUND_POSITIVE_FEEDBACK,CurrentGameSettings.GameSoundEffectVolume,1,Stream,CurrentGameSettings.GameBackgroundMusicVolume,1);
		
	
}	

BYTE GetRodeoBackGroundStream(BYTE button)
{
	BYTE Stream;
	
	switch(button)
		{
			case 0:
			case 1:
			case 4:
			case 5:
			default:
				Stream = BACKGROUND_MUSIC_STREAM_A;
		 			
			break;
					
			case 2:
			case 3:
			case 6:		
			case 7:
			 Stream = 	BACKGROUND_MUSIC_STREAM_B;
		
			
			break;
		}
	
	return Stream;
	
}	


void InitRodeoStereoStreams()
{
 BYTE k,Stream;
 
 for(k=0;k<8;k++)
 {
	Stream = GetRodeoBackGroundStream(k);
	AudioNodeEnable(k,Stream,Stream,AUDIO_ON_BEFORE_TIMEOUT,RODEOBACKGROUND_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
 }
 	
 	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM_A,RODEOBACKGROUND_A_WAV);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM_B,RODEOBACKGROUND_B_WAV);	
	
}	




void MoveToRodeoEnd()
{	
			GameState = RODEO_END;
			MAIN_GAME_TIMER = 0;
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM_A,BACKGROUND_MUSIC_STREAM_A,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM_A,ENDING_WAV);		
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

void MoveToRodeoLastNote()
	{
			GameState = RODEO_LAST_NOTE;
			MAIN_GAME_TIMER = 0;
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM_A,BACKGROUND_MUSIC_STREAM_A,AUDIO_ON_BEFORE_TIMEOUT,RODEOEND_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM_A,RODEOEND_WAV);		
			LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
			
	}
