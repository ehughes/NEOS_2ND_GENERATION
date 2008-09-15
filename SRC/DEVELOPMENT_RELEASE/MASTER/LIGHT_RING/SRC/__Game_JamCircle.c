#include "__Game_JamCircle.h"
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

#define NUM_SOUNDS_PER_BUTTON	3
#define ROCK					1
//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define	BACKGROUND_MUSIC_STREAM 3


//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define JAM_CIRCLE			0x01
#define JAM_CIRCLE_SELECT	0x02
#define JAM_CIRCLE_END		0x03



//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER			GPTimer[0]
#define SOUND_CHANNEL0_TIMER	GPTimer[1]
#define SOUND_CHANNEL1_TIMER	GPTimer[2]
#define SOUND_CHANNEL2_TIMER	GPTimer[3]
#define SHOW_TIMER				GPTimer[4]


//*************************************************
//*******Game Variables****************************
//*************************************************
WORD JamCirclesBaseLocation = 150;

BYTE NextAvailableSoundChannel = 0;

struct {
	
		BOOL Active;
		DWORD CurrentSoundLength;
		WORD ButtonAssignment;
	
}	SoundChannelInfo[3];	

struct{
	
	WORD Index[NUM_BUTTONS];
	DWORD Length[NUM_BUTTONS];
	
} ButtonSound ;
	

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void InitJamCircleChannels();
void InitButtonSounds(BYTE JamCircleProfile);




void JamCircle(void)
{
			
	switch (GameState)
	{
		
		case INIT:
				
			GameState = JAM_CIRCLE_SELECT;					
		
		break;
		
		
		
		case JAM_CIRCLE_SELECT:
		
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ROCKBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ROCKBACKGROUND_WAV);
			
			InitButtonSounds(ROCK);
			
			MAIN_GAME_TIMER = 0;
			GameState = JAM_CIRCLE;
		SHOW_TIMER = 0;
		break;
		
		
		case JAM_CIRCLE:
		
		
			if(SOUND_CHANNEL0_TIMER > SoundChannelInfo[0].CurrentSoundLength)
			{
				SoundChannelInfo[0].Active = FALSE;
				SoundChannelInfo[0].CurrentSoundLength = 0xFFFF;
			}
			
			if(SOUND_CHANNEL1_TIMER > SoundChannelInfo[0].CurrentSoundLength)
			{
				SoundChannelInfo[1].Active = FALSE;
				SoundChannelInfo[1].CurrentSoundLength = 0xFFFF;
			}
			
			if(SOUND_CHANNEL2_TIMER > SoundChannelInfo[0].CurrentSoundLength)
			{
				SoundChannelInfo[2].Active = FALSE;
				SoundChannelInfo[2].CurrentSoundLength = 0xFFFF;
			}
			
			if(SHOW_TIMER> 50)
			{
				SHOW_TIMER = 0;
				LEDSendMessage(ENABLE_ALL, RED, LEDOFF, 50, 50);
				
			}
			
		
		
			if(MAIN_GAME_TIMER>ROCKBACKGROUND_WAV_LENGTH)
			{
				GameState = JAM_CIRCLE_END;
				AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
				SendNodeNOP();	
				EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);
				MAIN_GAME_TIMER = 0;
					
			}
		
		break;
		
		
		
		case JAM_CIRCLE_END:
	
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




void OnButtonPressJamCircle(unsigned char button)
{
	
	LEDSendMessage(button, YELLOW, LEDOFF, 50, 50);
	
	if(GameState == JAM_CIRCLE)
	{

	if(SoundChannelInfo[NextAvailableSoundChannel].Active == TRUE)
	{
		AudioNodeEnable(SoundChannelInfo[NextAvailableSoundChannel].ButtonAssignment,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT,NO_TIMEOUT,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	}
	
		switch(NextAvailableSoundChannel)
		{
			case 0:
				SOUND_CHANNEL0_TIMER =0; 
			break;
			
			case 1:
				SOUND_CHANNEL1_TIMER =0;
			break;
			
			case 2:
				SOUND_CHANNEL2_TIMER =0;
			break;
			
			default:
			break;	
			
		}
		
		SoundChannelInfo[NextAvailableSoundChannel].Active = TRUE;
		SoundChannelInfo[NextAvailableSoundChannel].ButtonAssignment = button;
		
		SoundChannelInfo[NextAvailableSoundChannel].CurrentSoundLength = ButtonSound.Length[button];
		
		AudioNodeEnable(button,NextAvailableSoundChannel,BACKGROUND_MUSIC_STREAM,1,1,SoundChannelInfo[NextAvailableSoundChannel].CurrentSoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		SendNodeNOP();	
		EAudioPlaySound(NextAvailableSoundChannel,ButtonSound.Index[button]);
		
		
		NextAvailableSoundChannel++;
		
		if(NextAvailableSoundChannel > 2)
		{
			NextAvailableSoundChannel = 0;	
		}
	}

}

void InitJamCircleChannels()
{
	BYTE i;

	for(i=0;i<3;i++)
	{
		SoundChannelInfo[i].Active = FALSE;
		SoundChannelInfo[i].CurrentSoundLength = 0;
		SoundChannelInfo[i].ButtonAssignment = 0;
	}
	
	SOUND_CHANNEL0_TIMER = 0xFFFF;
	SOUND_CHANNEL1_TIMER = 0xFFFF;
	SOUND_CHANNEL2_TIMER = 0xFFFF;

}	

void InitButtonSounds(BYTE JamCircleProfile)
{
	switch(JamCircleProfile)
	{
		
		case ROCK:
			ButtonSound.Index[0] = GTR1A_WAV;
			ButtonSound.Index[1] = GTR2A_WAV;
		    ButtonSound.Index[2] = GTR3A_WAV;
			ButtonSound.Index[3] = GTR4A_WAV;
			ButtonSound.Index[4] = BASS1A_WAV;
			ButtonSound.Index[5] = BASS1B_WAV;

			ButtonSound.Length[0] = GTR1A_WAV_LENGTH;
			ButtonSound.Length[1] = GTR2A_WAV_LENGTH;
		    ButtonSound.Length[2] = GTR3A_WAV_LENGTH;
			ButtonSound.Length[3] = GTR4A_WAV_LENGTH;
			ButtonSound.Length[4] = BASS1A_WAV_LENGTH;
			ButtonSound.Length[5] = BASS1B_WAV_LENGTH;
		
			
		break;
		
		
		default:
		break;
		
	}
	
	
}	