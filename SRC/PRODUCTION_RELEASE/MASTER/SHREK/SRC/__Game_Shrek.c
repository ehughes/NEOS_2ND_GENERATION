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
#define GINGY_COLOR_INDEX		2
#define	RANDOM_COLOR_INDEX		0xFF

#define GINGY_COLOR 63,0,0
#define SHREK_COLOR 0,63,0
#define DONKEY_COLOR 63,63,0

#define MASTER_VOLUME	0x3f

uint8_t  MyStationID = 0;

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
#define SHREK_STATION_ID					0x20

#define SHREK_ATTRACT_DISPLAY				0x02
#define SHREK_ATTRACT_WAIT					0x03
#define SHREK_GENERATE_NEXT_LEVEL			0x04
#define SHREK_WAIT_GET_READY				0x05
#define SHREK_SHOW_PATTERN					0x06
#define SHREK_PLAY_GAME						0x07

#define SHREK_MATCH_FAIL					0x08
#define SHREK_MATCH_SUCCESS					0x09
#define SHREK_WIN							0x0A
#define SHREK_GAME_END						0x0B
#define SHREK_GAME_WAIT_BEFORE_RESTART		0x0C
#define SHREK_NEXT_SEQUENCE_START			0x0D
#define SHREK_TRUMPET_SUCCESS				0x0E
#define SHREK_WAIT_WELCOME					0xF

//*************************************************
//******** System Timer MACROS*********************
//*************************************************


#define MAIN_GAME_TIMER					GPTimer[0]	
#define NEXT_SEQUENCE_TIMER				GPTimer[1]	
#define ATTRACTION_TIMER				GPTimer[2]	

#define EFFECT_TIMER					GPTimer[3]	
	
//*************************************************
//*******Game Variables****************************
//*************************************************

#define MAX_SEQUENCE_LENGTH	16

uint8_t Effect = 0;

uint8_t CurrentLightSequenceIdx = 0;
uint8_t CurrentLightSequenceLength = 0;
uint8_t LightSequence[MAX_SEQUENCE_LENGTH];
uint8_t ColorSequence[MAX_SEQUENCE_LENGTH];
uint8_t IncorrectButtonPress = 0;

uint16_t  CurrentTimeout = 0;

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************
void RegenerateShrekLightSequence(uint8_t Length);
void SendShrekLight(uint8_t Node,uint8_t ShrekColorIndex,int8_t FadeTime);

//*************************************************
//*******Game Functions****************************
//*************************************************

#define MAX_RAND_RETRY	0x80

uint8_t LastShrekRandom = 0xFF;

uint8_t ShrekRandomButton()
{
	uint8_t  i = 0;
	uint8_t R;

	do
	{
		R = rand() % 4;
		i++;
	}
	while(R==LastShrekRandom && i < MAX_RAND_RETRY);

	LastShrekRandom = R;

	return R;
}

uint16_t PlayIdleSound(uint8_t StationId)
{
	 uint16_t Length = 0;
	 uint16_t Idx = 0;

	switch(StationId)
	{
		default:
		case 0:
			Idx = STATION1_IDLE_WAV;
			Length = STATION1_IDLE_WAV_LENGTH;
		break;

		case 1:
			Idx = STATION2_IDLE_WAV;
			Length = STATION2_IDLE_WAV_LENGTH;
		break;

		case 2:
			Idx = STATION3_IDLE_WAV;
			Length = STATION3_IDLE_WAV_LENGTH;
		break;

		case 3:
			Idx = STATION4_IDLE_WAV;
			Length = STATION4_IDLE_WAV_LENGTH;
		break;

	}

	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	, Idx);

	return Length;
}

uint16_t PlayStationID(uint8_t StationId)
{ 
     uint16_t Length = 0;
	 uint16_t Idx = 0;

	switch(StationId)
	{
		default:
		case 0:
			Idx = STATIONID_1_WAV ;
			Length = STATIONID_1_WAV_LENGTH;
		break;

		case 1:
			Idx = STATIONID_2_WAV ;
			Length = STATIONID_1_WAV_LENGTH;
		break;

		case 2:
			Idx = STATIONID_3_WAV ;
			Length = STATIONID_1_WAV_LENGTH;
		break;

		case 3:
			Idx = STATIONID_4_WAV ;
			Length = STATIONID_1_WAV_LENGTH;
		break;

	}

	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,NO_TIMEOUT,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	, Idx);

	return Length;
}

uint16_t PlayMagicMirrorWelcomeTrack()
{
   //	 uint16_t Length = GINGY_GET_READY_1_WAV_LENGTH;
    uint16_t Length = 0;
	// uint16_t Idx = GINGY_GET_READY_1_WAV;

//	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
//	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

	return Length;
}

uint16_t PlayGetReady()
{

   // uint16_t Length = GINGY_GET_READY_1_WAV_LENGTH;
    uint16_t Length = 0;
//	uint16_t Idx = GINGY_GET_READY_1_WAV;

//	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
//	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

	return Length;
}
void PlayFailed()
{
//	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,GINGY_FAILED_WAV_LENGTH ,MASTER_VOLUME,0);
//	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,GINGY_FAILED_WAV);
}

void PlayButtonCorrect()
{
//	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,CORRECT_BUTTON_WAV_LENGTH ,MASTER_VOLUME,0);
//	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,CORRECT_BUTTON_WAV);
}

void PlayTrumpetSuccess()
{
//	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT, TRUMPET_SUCCESS_WAV_LENGTH ,MASTER_VOLUME,0);
//	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	, TRUMPET_SUCCESS_WAV);
}

void PlayButtonIncorrect()
{
	switch(rand()%3)
	{
		default:
		case 0:
		//	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,WRONG_BUTTON_1_WAV_LENGTH ,MASTER_VOLUME,0);
	//		EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,WRONG_BUTTON_1_WAV);
		break;

		case 1:
	//		AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,WRONG_BUTTON_2_WAV_LENGTH ,MASTER_VOLUME,0);
	//		EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,WRONG_BUTTON_2_WAV);
		break;
		
		case 2:
	//		AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,WRONG_BUTTON_3_WAV_LENGTH ,MASTER_VOLUME,0);
	//		EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,WRONG_BUTTON_3_WAV);
		break;
	}

}

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
			if(MAIN_GAME_TIMER == 25)
			{
				GameState = SHREK_STATION_ID;
				CurrentTimeout = PlayStationID(MyStationID) + 100;
			}
		
		break;

		case SHREK_STATION_ID:

			if(MAIN_GAME_TIMER > CurrentTimeout)
				{
				ATTRACTION_TIMER = 0xFFFF;
				RegenerateShrekLightSequence(rand()%MAX_SEQUENCE_LENGTH);
				GameState = SHREK_ATTRACT_DISPLAY;
				PlayIdleSound(MyStationID);
			
				}
		break;

		case SHREK_ATTRACT_DISPLAY:
	
			if(MAIN_GAME_TIMER>25)
				{
					MAIN_GAME_TIMER = 0;

					if(CurrentLightSequenceIdx<CurrentLightSequenceLength)
						{		
							SendShrekLight(LightSequence[CurrentLightSequenceIdx],
										   ColorSequence[CurrentLightSequenceIdx],75);
		
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
		
			if(MAIN_GAME_TIMER>150)
			{
					MAIN_GAME_TIMER = 0;
					GameState = SHREK_ATTRACT_DISPLAY;
					RegenerateShrekLightSequence(rand()%MAX_SEQUENCE_LENGTH);
			}

		break;


		case SHREK_WAIT_WELCOME:
		
			if(MAIN_GAME_TIMER>CurrentTimeout)
			{
					MAIN_GAME_TIMER = 0;
					GameState = SHREK_GENERATE_NEXT_LEVEL;
			}

		break;



    	case	SHREK_TRUMPET_SUCCESS:
			if(MAIN_GAME_TIMER > 300)
			{
				GameState = SHREK_GENERATE_NEXT_LEVEL;
			}

			if(EFFECT_TIMER>10)
			{
				EFFECT_TIMER = 0;
				Effect++;
				Effect&=0x03;
				SendShrekLight(Effect,RANDOM_COLOR_INDEX,10);
			}
		break;

		case	SHREK_GENERATE_NEXT_LEVEL:

			CurrentLightSequenceLength++;
			
			RegenerateShrekLightSequence(CurrentLightSequenceLength);

			if(CurrentLightSequenceLength>MAX_SEQUENCE_LENGTH)
			{
				GameState = SHREK_WIN;
			}
			else
			{
			 	 CurrentTimeout = PlayGetReady();
				 GameState = SHREK_WAIT_GET_READY;
				 MAIN_GAME_TIMER = 0;
				 CurrentLightSequenceIdx = 0;
			}
		break;


		case  SHREK_WAIT_GET_READY:
			
			if(MAIN_GAME_TIMER>CurrentTimeout)
			{
				GameState = SHREK_SHOW_PATTERN;
				MAIN_GAME_TIMER = 0;
				LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);		
			}
	
		break;

		case SHREK_SHOW_PATTERN:

			if(MAIN_GAME_TIMER>100)
			{
				MAIN_GAME_TIMER = 0;
				if(CurrentLightSequenceIdx == CurrentLightSequenceLength)
				{
					IncorrectButtonPress = 0;
					GameState = SHREK_PLAY_GAME;
					CurrentLightSequenceIdx = 0;
					MAIN_GAME_TIMER = 0;
				}
				else
				{
					SendShrekLight(LightSequence[CurrentLightSequenceIdx],
								   ColorSequence[CurrentLightSequenceIdx],
									30);
					CurrentLightSequenceIdx++;
				}
			}

		break;

		case SHREK_PLAY_GAME:

			if(MAIN_GAME_TIMER >3000)
			{
				PlayFailed();
				MAIN_GAME_TIMER = 0;
				LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
				GameState = SHREK_GAME_WAIT_BEFORE_RESTART;
			}

		break;

		case SHREK_GAME_WAIT_BEFORE_RESTART:
		
			if(MAIN_GAME_TIMER >1000)
			{
					GameState = SHREK_INIT;
					MAIN_GAME_TIMER = 0;
			}

		break;

        case SHREK_WIN:
				GameState = SHREK_INIT;
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
	case SHREK_ATTRACT_WAIT:
	case SHREK_ATTRACT_DISPLAY:
	
		LEDSendMessage(ENABLE_ALL,GREEN,GREEN,0,0);
		GameState = SHREK_WAIT_WELCOME;
		CurrentLightSequenceLength = 0; //Reset so we start of with 1 light
	    CurrentTimeout = PlayMagicMirrorWelcomeTrack();
		MAIN_GAME_TIMER = 0;

	break;


	case SHREK_PLAY_GAME:

		if(button<4)
		{
			if(button == LightSequence[CurrentLightSequenceIdx])
			{
			
				SendShrekLight(button,
							   ColorSequence[CurrentLightSequenceIdx],
							   20);
				
				CurrentLightSequenceIdx++;
				if(CurrentLightSequenceIdx == CurrentLightSequenceLength)
				{
					PlayTrumpetSuccess();
					MAIN_GAME_TIMER = 0;
					GameState = SHREK_TRUMPET_SUCCESS;
				}
				else
				{
					PlayButtonCorrect();
				}
			}
			else
			{
				IncorrectButtonPress++;
	
				if(IncorrectButtonPress == 3)
				{
					PlayFailed();
					MAIN_GAME_TIMER = 0;
					LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
					GameState = SHREK_GAME_WAIT_BEFORE_RESTART;
				}
				else
					PlayButtonIncorrect();
			}
		}

	break;

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

	if(Length == 0)
		Length = 1;

	CurrentLightSequenceLength = Length;
	CurrentLightSequenceIdx = 0;

	for(i=0;i<Length;i++)
	{
		LightSequence[i] = ShrekRandomButton(); 
		ColorSequence[i] = rand() % 3; //Pick a random color
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
				LEDSendMessage(Node+4,SHREK_COLOR,LEDOFF,FadeTime+1,FadeTime);

		break;

		case DONKEY_COLOR_INDEX:

				LEDSendMessage(Node,DONKEY_COLOR,LEDOFF,FadeTime+1,FadeTime);
				LEDSendMessage(Node+4,DONKEY_COLOR,LEDOFF,FadeTime+1,FadeTime);

		break;

		case GINGY_COLOR_INDEX:
			
				LEDSendMessage(Node,GINGY_COLOR,LEDOFF,FadeTime+1,FadeTime);
				LEDSendMessage(Node+4,GINGY_COLOR,LEDOFF,FadeTime+1,FadeTime);

		break;
	}
}







