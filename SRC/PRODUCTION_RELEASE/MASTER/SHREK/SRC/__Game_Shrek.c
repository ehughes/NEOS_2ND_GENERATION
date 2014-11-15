#include "__Game_Shrek.h"
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
#include "stdint.h"

//*************************************************
//*******Game Parameters***************************
//*************************************************

#define SHREK_COLOR_INDEX		0
#define DONKEY_COLOR_INDEX		1
#define FIONA_COLOR_INDEX		2
#define	RANDOM_COLOR_INDEX		0xFF

#define FIONA_COLOR 63,0,0
#define SHREK_COLOR 0,63,0
#define DONKEY_COLOR 63,63,0

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

#define SHREK_INIT							0x01
#define SHREK_ATTRACT_DISPLAY				0x02
#define SHREK_ATTRACT_WAIT					0x03
#define SHREK_GENERATE_NEXT_LEVEL			0x04
#define SHREK_SHOW_PATTERN					0x05
#define SHREK_MATCH_FAIL					0x06
#define SHREK_MATCH_SUCCESS					0x07
#define SHREK_GAME_WIN						0x08
	

//*************************************************
//******** System Timer MACROS*********************
//*************************************************


#define MAIN_GAME_TIMER					GPTimer[0]	
#define NEXT_SEQUENCE_TIMER				GPTimer[1]	
	
//*************************************************
//*******Game Variables****************************
//*************************************************

#define MAX_SEQUENCE_LENGTH	16

uint8_t CurrentLightSequenceIdx = 0;
uint8_t CurrentLightSequenceLength = 0;
uint8_t LightSequence[MAX_SEQUENCE_LENGTH];

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************
void RegenerateShrekLightSequence(uint8_t Length);
void SendShrekLight(uint8_t Node,uint8_t ShrekColorIndex,int8_t FadeTime);

//*************************************************
//*******Game Functions****************************
//*************************************************

void Shrek(void)
{
			
	switch (GameState)
	{
		
		case INIT:
		
			ResetAudioAndLEDS();
			ScoreManagerEnabled = FALSE;
			GameState = SHREK_INIT;	
			MAIN_GAME_TIMER = 0;
			
		break;
		
		case SHREK_INIT:
			if(MAIN_GAME_TIMER > 100)
			{
					LEDSendMessage(ENABLE_ALL,LEDOFF,GREEN,100,200);
					EAudioPlaySound(BACKGROUND_MUSIC_STREAM,WAFFLES_WAV);
			}
			if(MAIN_GAME_TIMER >2000)
			{
				GameState = SHREK_ATTRACT_DISPLAY;
				MAIN_GAME_TIMER = 0;
			}
		
		break;

	
		case SHREK_ATTRACT_DISPLAY:
			
			if(CurrentLightSequenceIdx<CurrentLightSequenceLength)
			{
				if(MAIN_GAME_TIMER>25)
				{
					MAIN_GAME_TIMER = 0;

					SendShrekLight(LightSequence[CurrentLightSequenceIdx],
								   RANDOM_COLOR_INDEX,25);

					CurrentLightSequenceIdx++;
					if(CurrentLightSequenceIdx >= CurrentLightSequenceLength)
					{
						RegenerateShrekLightSequence(rand()%MAX_SEQUENCE_LENGTH);
						GameState = SHREK_ATTRACT_WAIT;
						MAIN_GAME_TIMER = 0;
					}
				}
			}
			
		break;		
		
		case SHREK_ATTRACT_WAIT:
			
			if(MAIN_GAME_TIMER > 200)
			{
				MAIN_GAME_TIMER = 0;
				GameState = SHREK_ATTRACT_DISPLAY;
			}

		break;

		default:
			GameState = INIT;
		break;

		}
}




void OnButtonPressShrek( BYTE  button)
{
	switch(GameState)
	{
		
	default:
		break;
		
	}
	
}



void RegenerateShrekLightSequence(uint8_t Length)
{
	uint8_t i;

	if(Length > MAX_SEQUENCE_LENGTH)
	{	
		Length = MAX_SEQUENCE_LENGTH;
	}

	CurrentLightSequenceLength = Length;
	CurrentLightSequenceIdx = 0;

	for(i=0;i<Length;i++)
	{
		LightSequence[i] = rand() % 4; //the modudol gives us a bitt different pattern than a bit mask to rand()
	}
}

/*
void LEDSendMessage(BYTE  nodenumber, BYTE  redbrt, BYTE  greenbrt, BYTE  bluebrt,
BYTE  redfade, BYTE  greenfade, BYTE  bluefade,
WORD ledtime, WORD fadetime)
*/


//This function will mirror nodes 0-3 to 4-7 for the fairy lights.   Blue is included
//in the color for legacy purposes
//THis will turn a light on and fade it out over 500mS

void SendShrekLight(uint8_t Node,uint8_t ShrekColorIndex,int8_t FadeTime)
{

	if(ShrekColorIndex == RANDOM_COLOR_INDEX)
	{
		ShrekColorIndex = rand() %3;
	}

	switch(ShrekColorIndex)
	{
		default:
		case SHREK_COLOR_INDEX:

				LEDSendMessage(Node,SHREK_COLOR,LEDOFF,FadeTime+1,FadeTime);
				LEDSendMessage(Node,SHREK_COLOR,LEDOFF,FadeTime+1,FadeTime);

		break;

		case DONKEY_COLOR_INDEX:

				LEDSendMessage(Node,DONKEY_COLOR,LEDOFF,FadeTime+1,FadeTime);
				LEDSendMessage(Node,DONKEY_COLOR,LEDOFF,FadeTime+1,FadeTime);

		break;

		case FIONA_COLOR_INDEX:
			
				LEDSendMessage(Node,DONKEY_COLOR,LEDOFF,FadeTime+1,FadeTime);
				LEDSendMessage(Node,DONKEY_COLOR,LEDOFF,FadeTime+1,FadeTime);

		break;
	}
}

/*

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

*/

