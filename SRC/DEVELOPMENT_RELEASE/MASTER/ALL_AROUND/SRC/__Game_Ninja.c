#include "__Game_Ninja.h"
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

#define SHOW_PATTERN_INTERVAL				 50
#define RESPONSE_TIME						 400
#define PAUSE_BEFORE_NEXT_PATTERN_TIME 		 75
#define NINJA_HIT						  	 10
#define NINJA_HIT_BONUS						 25
#define NINJA_HIT_BONUS_FIRST				 35
#define MAX_PATTERN_ELEMENTS 				 4
#define MIN_PATTERN_ELEMENTS 				 2

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define BACKGROUND_MUSIC_STREAM			0x00
#define P1_NODE_SOUND_EFFECT_STREAM		0x01
#define P2_NODE_SOUND_EFFECT_STREAM		0x02
#define HUGE_HIT_STREAM					0x03

//*************************************************
//******** System Timer MACROS*********************
//*************************************************


#define MAIN_GAME_TIMER		 		GPTimer[0]
#define SHOW_PATTERN_TIMER			 GPTimer[1]
#define WAIT_FOR_RESPONSE_TIMER		 GPTimer[2]

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define NINJA_1P			0x01
#define NINJA_2P			0x02
#define NINJA_END			0x03

#define NINJA_SHOW_PATTERN  0x00
#define NINJA_WAIT_FOR_RESPONSE  0x01
#define PAUSE_BEFORE_NEXT_PATTERN 0x02


//*************************************************
//*******Game Variables****************************
//*************************************************

static BYTE NinjaSubState = NINJA_SHOW_PATTERN;
static BOOL PatternElementDisplayed = FALSE;
static BYTE CurrentPatternElementDisplay;
static BYTE P1HitCount;
static BYTE P2HitCount;
static volatile BOOL P1PatternFinished;
static volatile BOOL P2PatternFinished;
static BYTE RandomStartPosition;
static const BYTE NinjaPatternBuffer[87] = {
							 
							 
							2, 1, 2,
							2, 6, 5,
							3, 1, 2, 3,
							3, 7, 6, 5,
							2, 1, 0,
							2, 4, 5,
							3, 1, 0, 7,
							3, 3, 4, 5,
							3, 1, 7, 0,
							3, 4, 2, 3,
							3, 7, 5, 6,
							3, 2, 0, 1,
							3, 5, 2, 7,
							3, 3, 0, 5,
							3, 1, 3, 2,
							3, 6, 0, 7,
							3, 3, 5, 4,
							3, 0, 2, 1,
							3, 5, 0, 3,
							3, 7, 2, 5,
							4, 1, 7, 0, 6,
							4, 2, 4, 3, 5,

							0xFF
							};
					
	
static WORD CurrentPatternBufferPosition = 0;

typedef struct{
				BYTE PatternLength;
				BYTE Pattern[MAX_PATTERN_ELEMENTS];
			} NinjaPattern;
		  		

NinjaPattern CurrentNinjaPatternP1;
NinjaPattern UserEnteredNinjaPatternP1;
NinjaPattern CurrentNinjaPatternP2;
NinjaPattern UserEnteredNinjaPatternP2;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void GetRandomNinjaPattern(NinjaPattern *NewNinjaPattern);
void NinjaPlayNodeSound(BYTE Player, BYTE button);
void NinjaPlayBadGong();
void NinjaPlayQuickHit(BYTE Player, BYTE button);
void NinjaPlayHugeHit(BYTE button);
BOOL PatternsEqual(NinjaPattern *Pattern1,NinjaPattern *Pattern2);
BOOL IsPartofPattern(BYTE button,NinjaPattern *PatternToCheck);
BOOL PatternsEqual(NinjaPattern *Pattern1,NinjaPattern *Pattern2);
BOOL IsPartofPatternSubset(BYTE button,NinjaPattern *PatternToCheck, BYTE SubSetLength);
BOOL GetNextNinjaPattern(BYTE *PatternBuffer, NinjaPattern * PatternToPopulate);
void Generate2PPattern(NinjaPattern * P1Pattern,NinjaPattern * P2Pattern);
void ResetNinjaPatternStateVariables();

//*************************************************
//*******Game Functions****************************
//*************************************************

void Ninja(void)
{
	switch (GameState)
	{
		
		case INIT:
			ResetAudioAndLEDS();
		
		
			if(GamePlayers == 1)
			{
				AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,NINJABACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
				SendNodeNOP();	
				EAudioPlaySound(BACKGROUND_MUSIC_STREAM,NINJABACKGROUND_WAV);
			
				GameState = NINJA_1P;
				
				CurrentPatternBufferPosition = 0;
				
				ResetNinjaPatternStateVariables();
				GetNextNinjaPattern((BYTE *)(&NinjaPatternBuffer[0]),&CurrentNinjaPatternP1);
				
				ScoreManagerEnabled = FALSE;
				Player1Score = 0;
				Player2Score = 0;
				RandomStartPosition = RandomButton(NO_EXCLUDE, NO_EXCLUDE);
					
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_BLANK;
				
				
			}
			else
			{
			
		    	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,NINJABACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
				SendNodeNOP();	
				EAudioPlaySound(BACKGROUND_MUSIC_STREAM,NINJABACKGROUND_WAV);
				GameState = NINJA_2P;
				MAIN_GAME_TIMER = 0;
				CurrentPatternBufferPosition = 0;
				ResetNinjaPatternStateVariables();
				GetNextNinjaPattern((BYTE *)NinjaPatternBuffer,&CurrentNinjaPatternP1);
				Generate2PPattern(&CurrentNinjaPatternP1,&CurrentNinjaPatternP2);
				
				Player1Score = 0;
				Player2Score = 0;
				
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_NORMAL;
				RandomStartPosition = RandomButton(NO_EXCLUDE, NO_EXCLUDE);
			}
			
			ScoreManagerEnabled = TRUE;
			
								
		break;


		case NINJA_1P:
		
		if(MAIN_GAME_TIMER>NINJABACKGROUND_WAV_LENGTH)
		{
			GameState = NINJA_END;
			MAIN_GAME_TIMER = 0;
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
			LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,50);
			
			P1ScoreDisplayState = SCORE_FLASHING;
			P2ScoreDisplayState = SCORE_BLANK;
		}
		
						
		switch(NinjaSubState)
				{
					case NINJA_SHOW_PATTERN:
					
					if(PatternElementDisplayed==FALSE)						
						{
								LEDSendMessage(CurrentNinjaPatternP1.Pattern[CurrentPatternElementDisplay],RED,RED,1000,0);	
								LEDSendMessage(CurrentNinjaPatternP1.Pattern[CurrentPatternElementDisplay],RED,RED,1000,0);
								NinjaPlayNodeSound(PLAYER_1,CurrentNinjaPatternP1.Pattern[CurrentPatternElementDisplay]);
								PatternElementDisplayed=TRUE;
								SHOW_PATTERN_TIMER = 0;
						}
						
					else if(SHOW_PATTERN_TIMER>SHOW_PATTERN_INTERVAL)
					{
						SHOW_PATTERN_TIMER = 0;
						
						CurrentPatternElementDisplay++;
						if(CurrentPatternElementDisplay == CurrentNinjaPatternP1.PatternLength)
						{
							NinjaSubState = NINJA_WAIT_FOR_RESPONSE;
							PatternElementDisplayed=FALSE;
							WAIT_FOR_RESPONSE_TIMER = 0;
						}
						else
						{
						    PatternElementDisplayed=FALSE;
						    SHOW_PATTERN_TIMER = 0;
						}
						
					}
					break;
					
					case NINJA_WAIT_FOR_RESPONSE:
					
					
					if(P1PatternFinished==TRUE)
					{
						WAIT_FOR_RESPONSE_TIMER = 0;
						NinjaSubState = PAUSE_BEFORE_NEXT_PATTERN;
					}
					
					if(WAIT_FOR_RESPONSE_TIMER > RESPONSE_TIME)
					{
						WAIT_FOR_RESPONSE_TIMER = 0;
						NinjaPlayBadGong();
					    NinjaSubState = PAUSE_BEFORE_NEXT_PATTERN;
					    LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
					   
					}
					
					break;	
					
					case PAUSE_BEFORE_NEXT_PATTERN:
					
					if(WAIT_FOR_RESPONSE_TIMER > PAUSE_BEFORE_NEXT_PATTERN_TIME)
					{
					    ResetNinjaPatternStateVariables();
					    GetNextNinjaPattern((BYTE *)&NinjaPatternBuffer[0],&CurrentNinjaPatternP1);
						LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
					}
					break;
					
					default:
					break;
				}
		
		break;

		
		case NINJA_2P:
		
		if(MAIN_GAME_TIMER>NINJABACKGROUND_WAV_LENGTH)
		{
			GameState = NINJA_END;
			MAIN_GAME_TIMER = 0;
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);		
			LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,50);
			
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
		
			switch(NinjaSubState)
				{
					case NINJA_SHOW_PATTERN:
					
					if(PatternElementDisplayed==FALSE)						
						{
								LEDSendMessage(CurrentNinjaPatternP1.Pattern[CurrentPatternElementDisplay],LEDOFF,RED,1000,10);
								LEDSendMessage(CurrentNinjaPatternP2.Pattern[CurrentPatternElementDisplay],LEDOFF,GREEN,1000,10);
									
								NinjaPlayNodeSound(PLAYER_1,CurrentNinjaPatternP1.Pattern[CurrentPatternElementDisplay]);
								NinjaPlayNodeSound(PLAYER_2,CurrentNinjaPatternP2.Pattern[CurrentPatternElementDisplay]);
								
								PatternElementDisplayed=TRUE;
								SHOW_PATTERN_TIMER = 0;
						}
						
					if(SHOW_PATTERN_TIMER>SHOW_PATTERN_INTERVAL)
					{
						SHOW_PATTERN_TIMER = 0;
						
						CurrentPatternElementDisplay++;
						if(CurrentPatternElementDisplay == CurrentNinjaPatternP1.PatternLength)
						{
							NinjaSubState = NINJA_WAIT_FOR_RESPONSE;
							PatternElementDisplayed=FALSE;
							WAIT_FOR_RESPONSE_TIMER = 0;
						}
						else
						{
						    PatternElementDisplayed=FALSE;
						    SHOW_PATTERN_TIMER = 0;
						}
						
					}
					break;
					
					case NINJA_WAIT_FOR_RESPONSE:
					
					if(P1PatternFinished==TRUE && P2PatternFinished==TRUE)
					{
						WAIT_FOR_RESPONSE_TIMER = 0;
						NinjaSubState = PAUSE_BEFORE_NEXT_PATTERN;
					}
					
					if(WAIT_FOR_RESPONSE_TIMER > RESPONSE_TIME)
					{
						WAIT_FOR_RESPONSE_TIMER = 0;
						NinjaPlayBadGong();
					    NinjaSubState = PAUSE_BEFORE_NEXT_PATTERN;
					    LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
					   
					}
					
					break;	
					
					case PAUSE_BEFORE_NEXT_PATTERN:
					
					if(WAIT_FOR_RESPONSE_TIMER > PAUSE_BEFORE_NEXT_PATTERN_TIME)
					{
						ResetNinjaPatternStateVariables();
						GetNextNinjaPattern((BYTE *)&NinjaPatternBuffer[0],&CurrentNinjaPatternP1);
						Generate2PPattern(&CurrentNinjaPatternP1,&CurrentNinjaPatternP2);
						LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
					}
					break;
					
					default:
					break;
				}
		
		break;
		
		
		break;
		
		
		
		case NINJA_END:
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



void ResetNinjaPatternStateVariables()
{
	NinjaSubState = NINJA_SHOW_PATTERN;
    CurrentPatternElementDisplay=0;	
	SHOW_PATTERN_TIMER = 0;
	PatternElementDisplayed=FALSE;	
	CurrentPatternElementDisplay=0;	
	P1HitCount = 0;
	P2HitCount = 0;
	P1PatternFinished=FALSE;
	P2PatternFinished=FALSE;
	
}	


void OnButtonPressNinja(unsigned char button)
{
	switch(GameState)
	{
		case NINJA_1P:
			switch(NinjaSubState)
			{
				
				case NINJA_SHOW_PATTERN:
				case NINJA_WAIT_FOR_RESPONSE:
				
				 if(IsPartofPatternSubset(button,&CurrentNinjaPatternP1,CurrentPatternElementDisplay))
					{	
						UserEnteredNinjaPatternP1.Pattern[P1HitCount] = button;
						
						P1HitCount++;
										
						if(P1HitCount == (CurrentNinjaPatternP1.PatternLength))
						{
								
							NinjaPlayHugeHit(button);
							LEDSendMessage(button,RED,LEDOFF,5,5);
						
							UserEnteredNinjaPatternP1.PatternLength = P1HitCount;
							
							P1PatternFinished=TRUE;
							
							if(PatternsEqual(&UserEnteredNinjaPatternP1,&CurrentNinjaPatternP1))
							{
								Player1Score += NINJA_HIT_BONUS; 
							}
							else
							{
								Player1Score += NINJA_HIT; 	
								
							}
						
						}
						else
						{
							Player1Score += NINJA_HIT;
							
							NinjaPlayQuickHit(PLAYER_1,button);
							LEDSendMessage(button,RED,LEDOFF,5,5);	
						}
				}
				break;
				
				default:
				break;			
			}
			
			break;
			
		case NINJA_2P:
			switch(NinjaSubState)
			{
				
				case NINJA_SHOW_PATTERN:
				case NINJA_WAIT_FOR_RESPONSE:
				
				 if(IsPartofPatternSubset(button,&CurrentNinjaPatternP1,CurrentPatternElementDisplay))
					{	
						UserEnteredNinjaPatternP1.Pattern[P1HitCount] = button;
						
						P1HitCount++;
										
						if(P1HitCount == (CurrentNinjaPatternP1.PatternLength))
						{
								
							NinjaPlayHugeHit(button);
							LEDSendMessage(button,RED,LEDOFF,5,5);
							UserEnteredNinjaPatternP1.PatternLength = P1HitCount;
							
													
							if(PatternsEqual(&UserEnteredNinjaPatternP1,&CurrentNinjaPatternP1))
							{
								if(P2PatternFinished == TRUE)
								{
									Player1Score += NINJA_HIT_BONUS; 
								}
								else
								{
									Player1Score += NINJA_HIT_BONUS_FIRST; 
									
								}
							}
							else
							{
								Player1Score += NINJA_HIT; 	
								
							}
						
							P1PatternFinished=TRUE;
						
						}
						else
						{
							Player1Score += NINJA_HIT;
							
							NinjaPlayQuickHit(PLAYER_1,button);
							LEDSendMessage(button,RED,LEDOFF,5,5);	
						}
						
					}
					
					else if(IsPartofPatternSubset(button,&CurrentNinjaPatternP2,CurrentPatternElementDisplay))
					{	
						UserEnteredNinjaPatternP2.Pattern[P2HitCount] = button;
						
						P2HitCount++;
										
						if(P2HitCount == (CurrentNinjaPatternP2.PatternLength))
						{
								
							NinjaPlayHugeHit(button);
							LEDSendMessage(button,GREEN,LEDOFF,5,5);
							
							UserEnteredNinjaPatternP2.PatternLength = P2HitCount;
														
							if(PatternsEqual(&UserEnteredNinjaPatternP2,&CurrentNinjaPatternP2))
							{
								if(P1PatternFinished == TRUE)
								{
									Player2Score += NINJA_HIT_BONUS; 
								}
								else
								{
									Player2Score += NINJA_HIT_BONUS_FIRST; 
									
								}
							}
							else
							{
								Player2Score += NINJA_HIT; 	
								
							}
							
							P2PatternFinished=TRUE;
						
						}
						else
						{
							Player2Score += NINJA_HIT;
							
							NinjaPlayQuickHit(PLAYER_2,button);
						
							LEDSendMessage(button,GREEN,LEDOFF,5,5);	
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






void NinjaPlayNodeSound(BYTE Player, BYTE button)
{
	switch(button)
	{
		case 0:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_01_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_01_WAV);
				
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_01_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_01_WAV);
			
			}
		break;
		
		case 1:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_02_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_02_WAV);
			
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_02_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_02_WAV);
			
			}
		break;
	
		case 2:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_03_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_03_WAV);
			
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_03_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_03_WAV);
			
			}
		break;
	
		case 3:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_04_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_04_WAV);
			
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_04_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_04_WAV);
			
			}
		break;
	
		case 4:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_05_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_05_WAV);
			
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_05_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_05_WAV);
				
			}
		break;
	
		case 5:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_06_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_06_WAV);
			
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_06_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_06_WAV);
				
			}
		break;
	
		case 6:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_07_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_07_WAV);
				
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_07_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_07_WAV);
				
			}
		break;
	
		case 7:
			if(Player == PLAYER_1)
			{
				AudioNodeEnable(button,P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_08_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_08_WAV );
				
			}
			else
			{
				AudioNodeEnable(button,P2_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1,NINJA_BUTTON_08_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
				SendNodeNOP();	
				EAudioPlaySound(P2_NODE_SOUND_EFFECT_STREAM,NINJA_BUTTON_08_WAV);
				
			}
		break;
	
		default:
		break;
	}
	
}	



void NinjaPlayBadGong()
{
		BYTE i;
	
	
		for(i=0;i<CurrentNinjaPatternP1.PatternLength;i++)
		{
			AudioNodeEnable(CurrentNinjaPatternP1.Pattern[i],P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, NINJA_BADGONG_01_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		
			if(GameState == NINJA_2P)
			{
				AudioNodeEnable(CurrentNinjaPatternP2.Pattern[i],P1_NODE_SOUND_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, NINJA_BADGONG_01_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			
			}
		}
		
		SendNodeNOP();	
		EAudioPlaySound(P1_NODE_SOUND_EFFECT_STREAM, NINJA_BADGONG_01_WAV);
}



void NinjaPlayQuickHit(BYTE Player, BYTE button)
{

	BYTE RandomHit;
	
	BYTE Sound;
	WORD SoundLength;
	BYTE Stream;
	
	RandomHit = rand() & 0x7;
	
	if(RandomHit>4)
	{
		RandomHit -= 5;	
	}
	
	switch(RandomHit)
	{
		case 0:
			SoundLength =  NINJA_QUICKHIT_01_WAV_LENGTH;
			Sound = NINJA_QUICKHIT_01_WAV;
		break;	
		
		case 1:
			SoundLength =  NINJA_QUICKHIT_02_WAV_LENGTH;
			Sound = NINJA_QUICKHIT_02_WAV;
		break;	
		
		
		case 2:
			SoundLength =  NINJA_QUICKHIT_03_WAV_LENGTH;
			Sound = NINJA_QUICKHIT_03_WAV;
		break;	
				
		case 3:
			SoundLength =  NINJA_QUICKHIT_04_WAV_LENGTH;
			Sound = NINJA_QUICKHIT_04_WAV;
		break;	
		
		
		case 4:
			SoundLength =  NINJA_QUICKHIT_05_WAV_LENGTH;
			Sound = NINJA_QUICKHIT_05_WAV;
		break;	
			
		
		default:
		break;
		
	}
	
	
	if(Player == PLAYER_1)
	{
		Stream = P1_NODE_SOUND_EFFECT_STREAM;
	}
	else
	{
		Stream = P2_NODE_SOUND_EFFECT_STREAM;	
	}
	
	AudioNodeEnable(button,Stream,BACKGROUND_MUSIC_STREAM,1,1, SoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable((button+1)&0x07,Stream,BACKGROUND_MUSIC_STREAM,1,1, SoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable((button-1)&0x07,Stream,BACKGROUND_MUSIC_STREAM,1,1, SoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	
	SendNodeNOP();	
	EAudioPlaySound(Stream, Sound);
	
}


void NinjaPlayHugeHit(BYTE button)
{

	BYTE RandomHit;
	
	RandomHit = rand() & 0xF;
	
	BYTE Sound;
	WORD SoundLength;
	
	if(RandomHit>9)
	{
		RandomHit -= 10;	
	}
	
	switch(RandomHit)
	{
		case 0:
			SoundLength =  NINJA_HUGEHIT_01_WAV_LENGTH;
			Sound = NINJA_HUGEHIT_01_WAV;
		break;	
		
		case 1:
			SoundLength =  NINJA_HUGEHIT_02_WAV_LENGTH;
			Sound = NINJA_HUGEHIT_02_WAV;
		break;	
		
		
		case 2:
			SoundLength =  NINJA_HUGEHIT_03_WAV_LENGTH;
			Sound = NINJA_HUGEHIT_03_WAV;
		break;	
				
		case 3:
			SoundLength =  NINJA_HUGEHIT_04_WAV_LENGTH;
			Sound = NINJA_HUGEHIT_04_WAV;
		break;	
		
		
		case 4:
			SoundLength =  NINJA_HUGEHIT_05_WAV_LENGTH;
			Sound = NINJA_HUGEHIT_05_WAV;
		break;	
			

		case 5:
			SoundLength =  NINJA_BIGHIT_01_WAV_LENGTH;
			Sound = NINJA_BIGHIT_01_WAV;
		break;	
		
		case 6:
			SoundLength =  NINJA_BIGHIT_02_WAV_LENGTH;
			Sound = NINJA_BIGHIT_02_WAV;
		break;	
		
		
		case 7:
			SoundLength =  NINJA_BIGHIT_03_WAV_LENGTH;
			Sound = NINJA_BIGHIT_03_WAV;
		break;	
				
		case 8:
			SoundLength =  NINJA_BIGHIT_04_WAV_LENGTH;
			Sound = NINJA_BIGHIT_04_WAV;
		break;	
		
		
		case 9:
			SoundLength =  NINJA_BIGHIT_05_WAV_LENGTH;
			Sound = NINJA_BIGHIT_05_WAV;
			
		break;		
		
		default:
		break;
		
	}


	AudioNodeEnable(button,HUGE_HIT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, SoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable((button+1)&0x07,HUGE_HIT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, SoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable((button-1)&0x07,HUGE_HIT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, SoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable((button+2)&0x07,HUGE_HIT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, SoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable((button-2)&0x07,HUGE_HIT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, SoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	
	
	SendNodeNOP();	
	EAudioPlaySound(HUGE_HIT_STREAM, Sound);
		
	
}





BOOL IsPartofPatternSubset(BYTE button,NinjaPattern *PatternToCheck, BYTE SubSetLength)
{
	BYTE i;
	
	if(SubSetLength == 0 || SubSetLength>PatternToCheck->PatternLength)
	{
		return FALSE;	
	}
	
	for(i=0; i< SubSetLength; i++)
	{
		if(button == PatternToCheck->Pattern[i])
		{
			return TRUE;	
		}
	}
	
	return FALSE;
}

	

BOOL PatternsEqual(NinjaPattern *Pattern1,NinjaPattern *Pattern2)
{
	BYTE i;
	
	if(Pattern1->PatternLength != Pattern2->PatternLength)
	{
		return FALSE;
	}
	
	for(i=0;i<Pattern1->PatternLength;i++)
	{
		if(Pattern1->Pattern[i] != Pattern2->Pattern[i])
		{
			return FALSE;	
		}
	}

	return TRUE;
}

	


void GetRandomNinjaPattern(NinjaPattern *NewNinjaPattern)
{
	
	BYTE i;
	
	if(rand()&0x01)
	{
		NewNinjaPattern->PatternLength = 2;	
		
	}
	else
	{
		if(rand()&0x01)
			{
			  NewNinjaPattern->PatternLength = 3;		
			}
			else
			{
		   	 NewNinjaPattern->PatternLength = 4;	
			}
	}
	
	for(i=0;i<NewNinjaPattern->PatternLength;i++)
	{
			
		NewNinjaPattern->Pattern[i] = RandomButton(NO_EXCLUDE, NO_EXCLUDE);
	}
	
}	

BOOL GetNextNinjaPattern(BYTE *PatternBuffer, NinjaPattern * PatternToPopulate)
{
	BYTE i;

	
	if(PatternBuffer[CurrentPatternBufferPosition] == 0xFF)
	{
		return FALSE;	
	}
	
	PatternToPopulate->PatternLength = PatternBuffer[CurrentPatternBufferPosition];
	CurrentPatternBufferPosition++;
	
	for(i=0;i<PatternToPopulate->PatternLength;i++)
	{			
		PatternToPopulate->Pattern[i] = (PatternBuffer[CurrentPatternBufferPosition]) ;
		CurrentPatternBufferPosition++;
	}
	
	return TRUE;
}



void Generate2PPattern(NinjaPattern * P1Pattern,NinjaPattern * P2Pattern)
{
	BYTE i;

	P2Pattern->PatternLength = P1Pattern->PatternLength;
		
	for(i=0;i<P1Pattern->PatternLength;i++)
	{			
		P2Pattern->Pattern[i] = (P1Pattern->Pattern[i] + 4) & 0x7;
		
	}

}


BOOL IsPartofPattern(BYTE button,NinjaPattern *PatternToCheck)
{
	BYTE i;
	
	for(i=0; i<PatternToCheck->PatternLength;i++)
	{
		if(button == PatternToCheck->Pattern[i])
		{
			return TRUE;	
		}
	}
	
	return FALSE;
}

