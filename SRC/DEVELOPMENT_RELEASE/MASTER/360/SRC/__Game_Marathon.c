#include "__Game_Marathon.h"
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

#define IDLE_TIME			 1000
#define MARATHON_TIME_OUT 	 9998
#define MARATHON_DISTANCE	 40
#define NUM_HICCUPS 3


//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define BACKGROUND_MUSIC_STREAM			0x00
#define P1_HIT_EFFECT_STREAM			0x01
#define P2_HIT_EFFECT_STREAM			0x02
#define SPARE_STREAM					0x03

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define MARATHON				0x01
#define MARATHON_INIT			0x02
#define MARATHON_END			0x03
#define STANDARD_FINALE			0x04

//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER		 GPTimer[0]
#define P1_TIMER			 GPTimer[1]
#define SCORE_TIMER			 GPTimer[2]
#define IDLE_TIMER			 GPTimer[3]

//*************************************************
//*******Game Variables****************************
//*************************************************

static BYTE P1HitCount;
static BYTE P2HitCount;
static BYTE Hiccups[ NUM_HICCUPS ] = { 12 , 22 , 30};
static BYTE P1Position;
static BYTE P2Position;
static BYTE P1Time_Seconds = 0;
static BYTE P1Time_HundredMSeconds = 0;
static BYTE P2Time_Seconds = 0;
static BYTE P2Time_HundredMSeconds = 0;
static BOOL P1Finished;
static BOOL P2Finished;
static BYTE MarathonDirection;
static BYTE MarathonSection = 0;
static BOOL WinningFanfare;
static WORD FinalSoundLength;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void MarathonDisplayPlayerPosition();
void MarathonPlayPositionHit(BYTE Player, BYTE button);
void MarathonPlayP1BadHit(BYTE Player, BYTE button);
void MoveToMarathonEnd();
void GetNextToSwitchPosition();
void MarathonCheckLevelUp(BYTE Player,BYTE button);
void PlayMarathonSection(BYTE SectionNumber);
void MarathonPlayLevelUp(BYTE Player , BYTE button);
void MarathonPlayDirectionSwitch(BYTE Player,BYTE button);
void Marathon1PButtonProcess(BYTE button);
void Marathon2PButtonProcess(BYTE button);


//*************************************************
//*******Game Functions****************************
//*************************************************

void Marathon(void)
{
	switch (GameState)
	{
		case INIT:
			ResetAudioAndLEDS();
			MAIN_GAME_TIMER = 0;
			GameState = MARATHON_INIT;
			ScoreManagerEnabled = TRUE;
		break;

		case MARATHON_INIT:
		
		if(MAIN_GAME_TIMER>GLOBAL_INIT_TIME)
		{
			if(GamePlayers == 1)
			{				
			   	MarathonSection = 0;
			   	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,0);
			    AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,0);
			   	PlayMarathonSection(MarathonSection);
				P1Position = rand() &0x07;
				MarathonDisplayPlayerPosition();
				
				P1HitCount = 0;
				Player1Score = 0;
				P1Time_Seconds = 0;
				P1Time_HundredMSeconds = 0;
				GameState = MARATHON;
				P1_TIMER = 0;
				P1Finished = FALSE;
				P2ScoreDisplayState = SCORE_NORMAL;
				P1ScoreDisplayState = SCORE_NORMAL;
				Player1Score = 0;
				Player2Score = 0;
			}
			else
			{
				MarathonSection = 0;
				AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,0);
				AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,0);
				PlayMarathonSection(MarathonSection);	
				P1Position = rand() &0x07;
				P2Position = (P1Position + 4)&0x07;
				MarathonDisplayPlayerPosition();
				P1HitCount = 0;
				P2HitCount = 0;
				Player1Score = 0;
				Player2Score = 0;
				P1Time_Seconds = 0;
				P1Time_HundredMSeconds = 0;
				P2Time_Seconds = 0;
				P2Time_HundredMSeconds = 0;
				P1_TIMER = 0;
				P1Finished = FALSE;
				P2Finished = FALSE;
				P2ScoreDisplayState = SCORE_NORMAL;
				P1ScoreDisplayState = SCORE_NORMAL;
				Player1Score = 0;
				Player2Score = 0;
				GameState = MARATHON;
			}
			
			MarathonDirection = rand() & 0x01;
			IDLE_TIMER = 0;
		}
		
		break;
		
		case MARATHON:
						
			if(MAIN_GAME_TIMER>MARATHON_TIME_OUT)
			{
				 MoveToMarathonEnd();
			}
			if((P1Finished == TRUE) && (P2Finished == TRUE))
			{
				if(Player1Score < Player2Score)
				{
					P1ScoreDisplayState = SCORE_FLASHING;
					P2ScoreDisplayState = SCORE_NORMAL;
				}
				else if(Player2Score < Player1Score)
				{
					P2ScoreDisplayState = SCORE_FLASHING;
					P1ScoreDisplayState = SCORE_NORMAL;
				}
				else 
				{
					P2ScoreDisplayState = SCORE_FLASHING;
					P1ScoreDisplayState = SCORE_FLASHING;
				}
				
				WinningFanfare = TRUE;
				SyncDisplayFlashing();
				MoveToMarathonEnd();
			}
			if(P1_TIMER == 10)
			{
					P1_TIMER = 0;
					MarathonDisplayPlayerPosition();
					
					if(GamePlayers == 1)
					{
						 P1Time_HundredMSeconds++;
						
						 if(P1Time_HundredMSeconds == 10)
						 {
							P1Time_HundredMSeconds = 0;
							P1Time_Seconds++;	
							MarathonDisplayPlayerPosition(); 
						 }
						  Player2Score = (P1Time_Seconds*10) + (WORD)P1Time_HundredMSeconds;
						  Player1Score = P1HitCount;
					}
					else
					{
						
					   if(P1Finished == FALSE)
					   {					
							 P1Time_HundredMSeconds++;
							 if(P1Time_HundredMSeconds == 10)
							 {
								P1Time_HundredMSeconds = 0;
								P1Time_Seconds++;
							 }
							Player1Score =  (P1Time_Seconds*10) + (WORD)P1Time_HundredMSeconds;
							P1ScoreDisplayState = SCORE_NORMAL; 
						}
						if(P2Finished == FALSE)
						{
							 P2Time_HundredMSeconds++;
							 if(P2Time_HundredMSeconds == 10)
							 {
								P2Time_HundredMSeconds = 0;
								P2Time_Seconds++;	 
							 }
							 Player2Score =  (P2Time_Seconds*10)+P2Time_HundredMSeconds;
							 P2ScoreDisplayState = SCORE_NORMAL;
						 }
					}
			}	
			if(IDLE_TIMER > IDLE_TIME)
			{
				WinningFanfare = FALSE;
				
				if(GamePlayers == 1)
				{
					 MoveToMarathonEnd();
					 				
					 Player2Score = 0;
					 P1ScoreDisplayState = SCORE_NORMAL;
					 P2ScoreDisplayState = SCORE_FLASHING;
					 
					 
				}
				else
				{
					if((P1Finished == TRUE) && (P2Finished == FALSE))
					{
						SyncDisplayFlashing();
						P1ScoreDisplayState = SCORE_FLASHING;
						Player2Score = 0;
						P2ScoreDisplayState = SCORE_FLASHING;
					}
					else if((P1Finished == FALSE) && (P2Finished == TRUE))
					{
						SyncDisplayFlashing();
						Player1Score = 0;
						P2ScoreDisplayState = SCORE_FLASHING;
						P1ScoreDisplayState = SCORE_FLASHING;
					}
					else
					{
						SyncDisplayFlashing();
						Player1Score = 0;
						Player2Score = 0;
						P1ScoreDisplayState = SCORE_FLASHING;
						P2ScoreDisplayState = SCORE_FLASHING;
					}
					MoveToMarathonEnd();
				}
			}
		break;

		case MARATHON_END:
			
			if(MAIN_GAME_TIMER>FinalSoundLength)
				{
					GameState = STANDARD_FINALE;
					AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
					SendNodeNOP();	
					EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);
					MAIN_GAME_TIMER = 0;
				}
				
		break;
		
		case STANDARD_FINALE:
			
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




void OnButtonPressMarathon(BYTE button)
{

switch(GameState)
	{
		case MARATHON:
	
		if(GamePlayers == 1)
		{
			Marathon1PButtonProcess(button);
		}
		else 
		{
			
			Marathon2PButtonProcess(button);
		}
		break;
		
		default:
		break;	
	
	}	
	
}


void Marathon1PButtonProcess(BYTE button)
{
	BYTE k,HiccupMatch;
	
		if(button == P1Position)
				{
					IDLE_TIMER = 0;
					LEDSendMessage(P1Position,LEDOFF,LEDOFF,0,0);
						LEDSendMessage(P2Position,LEDOFF,LEDOFF,0,0);
					P1HitCount++;
					Player1Score = P1HitCount;
					MarathonCheckLevelUp(PLAYER_1,button);
					MarathonPlayPositionHit(PLAYER_1,button);
					HiccupMatch = FALSE;
					
				
					for(k=0;k<NUM_HICCUPS;k++)
					{
						if(P1HitCount == Hiccups[k])
						{
							HiccupMatch = TRUE;
							break;
						}
					}
					
					
					if(HiccupMatch == TRUE)
					{
						MarathonPlayDirectionSwitch(PLAYER_1,button);
						
						if(MarathonDirection)
						{
							P1Position--;
						}
						else
						{
							P1Position++;
						}
					}
					else
					{
						if(MarathonDirection)
							{
								P1Position++;
							}
							else
							{
								P1Position--;
							}	
					}
					
					
					P1Position &= 0x07;
					
					MarathonDisplayPlayerPosition();
					
										
					if(P1HitCount == MARATHON_DISTANCE)
					{
						WinningFanfare = TRUE;
						MoveToMarathonEnd();
					}
					
					
				}
			
}	
void Marathon2PButtonProcess(BYTE button)
{
		if((button == P1Position) )
				{
					IDLE_TIMER = 0;
					LEDSendMessage(P1Position,LEDOFF,LEDOFF,0,0);
					LEDSendMessage(P1Position,LEDOFF,LEDOFF,0,0);
								
										
					P1HitCount++;
					MarathonCheckLevelUp(PLAYER_1,button);
							
					if(P1HitCount == MARATHON_DISTANCE)
					{
						P1ScoreDisplayState = SCORE_FLASHING;
						P1Position = 0xAA;  //To make sure it Doesn't interfere with P2 alone
						P1Finished = TRUE;
						
						if(P2Finished == FALSE)
						{
							MarathonPlayLevelUp(PLAYER_1 ,button);
						}
					}
					else
					{
					
						if(MarathonDirection)
						{
							
							P1Position = (P1Position + 1) & 0x07;
							
							if(P1Position == P2Position)
							{
								P2Position = (P2Position - 1)&0x07;
								MarathonPlayDirectionSwitch(PLAYER_1,button);	
							}
							else
							{	
								MarathonPlayPositionHit(PLAYER_1,button);
							}
						}
						else
						{
							P1Position = (P1Position - 1) & 0x07;
							
							if(P1Position == P2Position)
							{
								P2Position = (P2Position + 1)&0x07;
								MarathonPlayDirectionSwitch(PLAYER_1,button);	
							}
							else
							{	
								MarathonPlayPositionHit(PLAYER_1,button);
							}
						}
					}
		
					MarathonDisplayPlayerPosition();
					
					
				}
				
				else if((button == P2Position) )
				{
					
					IDLE_TIMER = 0;
					LEDSendMessage(P2Position,LEDOFF,LEDOFF,0,0);
						LEDSendMessage(P2Position,LEDOFF,LEDOFF,0,0);
					P2HitCount++;
					MarathonCheckLevelUp(PLAYER_2,button);
	
											
					if(P2HitCount == MARATHON_DISTANCE)
					{
						P2Position = 0xAA;  //To make sure it Doesn't interfere with P1 alon
						P2Finished = TRUE;
						P2ScoreDisplayState = SCORE_FLASHING;
							if(P1Finished == FALSE)
						{
							MarathonPlayLevelUp(PLAYER_2 ,button);
						}
					}
					else
					{
						if(MarathonDirection)
						{
							
							P2Position = (P2Position + 1) & 0x07;
							
							if(P2Position == P1Position)
							{
								P1Position = (P1Position - 1)&0x07;
								MarathonPlayDirectionSwitch(PLAYER_2,button);	
							}
							else
							{	
								MarathonPlayPositionHit(PLAYER_2,button);
							}
						}
						else
						{
							P2Position = (P2Position - 1) & 0x07;
							
							if(P2Position == P1Position)
							{
								P1Position = (P1Position + 1)&0x07;
								MarathonPlayDirectionSwitch(PLAYER_2,button);	
							}
							else
							{	
								MarathonPlayPositionHit(PLAYER_2,button);
							}
						}
					}
					MarathonDisplayPlayerPosition();
					
					
				}
	
	
			
}	

void MarathonCheckLevelUp(BYTE Player,BYTE button)
{
	if(Player == PLAYER_1)
	{
		if( ((P1HitCount == 10)&&(MarathonSection == 0)) || ((P1HitCount == 20)&&(MarathonSection == 1)) || ((P1HitCount == 30)&&(MarathonSection == 2)))
			{
				MarathonSection++;
				PlayMarathonSection(MarathonSection);
			}
	}
	else
	{
	if( ((P2HitCount == 10)&&(MarathonSection == 0)) || ((P2HitCount == 20)&&(MarathonSection == 1)) || ((P2HitCount == 30)&&(MarathonSection == 2)))
		
			{
				MarathonSection++;
				PlayMarathonSection(MarathonSection);
			}
	}
	
}	


void MarathonDisplayPlayerPosition()
{
	if(GamePlayers == 1)
	{
		
		   LEDSendMessage(P1Position,RED,RED,0,0);
		   LEDSendMessage(P1Position,RED,RED,0,0);
	}
	else
	{
			
			if(P1Finished == TRUE)
			{
				
			}
			else
			{
			LEDSendMessage(P1Position,RED,RED,0,0);	
			}
			
			if(P2Finished == TRUE)
			{
			}
			else
			{
				LEDSendMessage(P2Position,GREEN,GREEN,0,0);
			
			}
			
			
			
	}
}	



void MarathonPlayPositionHit(BYTE Player, BYTE button)
{
	if(Player == PLAYER_1)
	{
		AudioNodeEnable(button,P1_HIT_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_HIT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		SendNodeNOP();	
		EAudioPlaySound(P1_HIT_EFFECT_STREAM,MARATHON_HIT_WAV);
	}
	else
	{
		AudioNodeEnable(button,P2_HIT_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_HIT_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		SendNodeNOP();	
		EAudioPlaySound(P2_HIT_EFFECT_STREAM,MARATHON_HIT_WAV);
	}
}	

void MarathonPlayDirectionSwitch(BYTE Player,BYTE button)
{
		if(Player == PLAYER_1)
		{
			AudioNodeEnable((button+1)&0x7,P1_HIT_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_JUMP_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			AudioNodeEnable((button-1)&0x7,P1_HIT_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_JUMP_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			AudioNodeEnable(button,P1_HIT_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_JUMP_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(P1_HIT_EFFECT_STREAM,MARATHON_JUMP_WAV);
		}
		else
		{
			AudioNodeEnable((button+1)&0x7,P2_HIT_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_JUMP_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			AudioNodeEnable((button-1)&0x7,P2_HIT_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_JUMP_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			AudioNodeEnable(button,P2_HIT_EFFECT_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_JUMP_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(P2_HIT_EFFECT_STREAM,MARATHON_JUMP_WAV);
		}
}	

void MoveToMarathonEnd()
{
		GameState = MARATHON_END;
		MAIN_GAME_TIMER = 0;
		
		if(WinningFanfare == TRUE)
		{
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,MARATHON_ENDING_FANFARE_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,MARATHON_ENDING_FANFARE_WAV);	
			FinalSoundLength = MARATHON_ENDING_FANFARE_WAV_LENGTH;	
		}
		else
		{
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,BUZZER_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,BUZZER_WAV);	
			FinalSoundLength = 	BUZZER_WAV_LENGTH;
		}
		
		LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
}	


void PlayMarathonSection(BYTE SectionNumber)
{

	switch(SectionNumber)
	{
		case 0:
		default:
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,MARATHON_SECTION_1_WAV);
		break;
		
		case 1:
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,MARATHON_SECTION_2_WAV);
		break;
				
		case 2:
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,MARATHON_SECTION_3_WAV);
		break;
		
		case 3:
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,MARATHON_SECTION_4_WAV);
		break;
		
	}
}	

void MarathonPlayLevelUp(BYTE Player , BYTE button)
{
	if(Player == PLAYER_1)
	{
		AudioNodeEnable((button+1)&0x07,SPARE_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_LEVEL_FANFARE_WAV_LENGTH,0xFF,CurrentGameSettings.GameBackgroundMusicVolume);
		AudioNodeEnable((button-1)&0x07,SPARE_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_LEVEL_FANFARE_WAV_LENGTH,0xFF,CurrentGameSettings.GameBackgroundMusicVolume);
		AudioNodeEnable(button,SPARE_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_LEVEL_FANFARE_WAV_LENGTH,0xFF,CurrentGameSettings.GameBackgroundMusicVolume);
		SendNodeNOP();	
		EAudioPlaySound(SPARE_STREAM,MARATHON_LEVEL_FANFARE_WAV);
	}
	else
	{
		AudioNodeEnable((button+1)&0x07,SPARE_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_LEVEL_FANFARE_WAV_LENGTH,0xFF,CurrentGameSettings.GameBackgroundMusicVolume);
		AudioNodeEnable((button-1)&0x07,SPARE_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_LEVEL_FANFARE_WAV_LENGTH,0xFF,CurrentGameSettings.GameBackgroundMusicVolume);
		AudioNodeEnable(button,SPARE_STREAM,BACKGROUND_MUSIC_STREAM,1,1, MARATHON_LEVEL_FANFARE_WAV_LENGTH,0xFF,CurrentGameSettings.GameBackgroundMusicVolume);
		SendNodeNOP();	
		EAudioPlaySound(SPARE_STREAM,MARATHON_LEVEL_FANFARE_WAV);
	}
}	




