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

#define SURROUND_SOUND			0x01
#define SURROUND_SOUND_SELECT	0x02
#define SURROUND_SOUND_END		0x03



//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER			GPTimer[0]
#define SOUND_CHANNEL0_TIMER	GPTimer[1]
#define SOUND_CHANNEL1_TIMER	GPTimer[2]
#define SOUND_CHANNEL2_TIMER	GPTimer[3]


//*************************************************
//*******Game Variables****************************
//*************************************************
WORD SurroundSoundsBaseLocation = 150;

BYTE NextAvailableSoundChannel = 0;

struct {
	
		BOOL Active;
		DWORD CurrentSoundLength;
		WORD ButtonAssignment;
	
}	SoundChannelInfo[3];	

struct{
	
	WORD Index[NUM_BUTTONS][NUM_SOUNDS_PER_BUTTON];
	DWORD Length[NUM_BUTTONS][NUM_SOUNDS_PER_BUTTON];
	BYTE Offset[0];
} ButtonSound ;
	

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void InitSurroundSoundChannels();
void InitButtonSounds(BYTE SurroundSoundProfile);




void JamCircle(void)
{
			
	switch (GameState)
	{
		
		case INIT:
				
			GameState = SURROUND_SOUND_SELECT;					
		
		break;
		
		
		
		case SURROUND_SOUND_SELECT:
		
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ROCKBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ROCKBACKGROUND_WAV);
			
			InitButtonSounds(ROCK);
			
			MAIN_GAME_TIMER = 0;
			GameState = SURROUND_SOUND;
		
		break;
		
		
		case SURROUND_SOUND:
		
		
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
			
			
		
		
			if(MAIN_GAME_TIMER>ROCKBACKGROUND_WAV_LENGTH)
			{
				GameState = SURROUND_SOUND_END;
				AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
				SendNodeNOP();	
				EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);
				MAIN_GAME_TIMER = 0;
					
			}
		
		break;
		
		
		
		case SURROUND_SOUND_END:
	
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

	if(SoundChannelInfo[NextAvailableSoundChannel].Active == TRUE)
	{
		
		
		if(SoundChannelInfo[NextAvailableSoundChannel].ButtonAssignment == button)
		{
			
			
		}
		else
		{
				ButtonSound.Offset[button]++;
				if(ButtonSound.Offset[button] >  (NUM_SOUNDS_PER_BUTTON-1))
				{
					ButtonSound.Offset[button] = 0;
				}
				
			
			
		}
		
		
		AudioNodeEnable(SoundChannelInfo[NextAvailableSoundChannel].ButtonAssignment,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_AFTER_TIMEOUT,NO_TIMEOUT,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	
		
	}
	else
	{
		ButtonSound.Offset[button]++;
		if(ButtonSound.Offset[button] >  (NUM_SOUNDS_PER_BUTTON-1))
		{
			ButtonSound.Offset[button] = 0;
		}
		
		
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
		
		SoundChannelInfo[NextAvailableSoundChannel].CurrentSoundLength = ButtonSound.Length[button][ButtonSound.Offset[button]];
		
		AudioNodeEnable(button,NextAvailableSoundChannel,BACKGROUND_MUSIC_STREAM,1,1,SoundChannelInfo[NextAvailableSoundChannel].CurrentSoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
		SendNodeNOP();	
		EAudioPlaySound(NextAvailableSoundChannel,ButtonSound.Index[button][ButtonSound.Offset[button]]);
		
		
	
		
		NextAvailableSoundChannel++;
		
		if(NextAvailableSoundChannel > 2)
		{
			NextAvailableSoundChannel = 0;	
		}
		


	
}



void InitSurroundSoundChannels()
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

void InitButtonSounds(BYTE SurroundSoundProfile)
{

	BYTE i;
	
	for(i=0;i<NUM_SOUNDS_PER_BUTTON;i++)
	{
			
		ButtonSound.Offset[i] = 0;
	}

	switch(SurroundSoundProfile)
	{
		
		case ROCK:
			ButtonSound.Index[0][0] = BASS1A_WAV;
			ButtonSound.Index[0][1] = BASS1B_WAV;
			ButtonSound.Index[0][2] = BASS1C_WAV;
			ButtonSound.Index[1][0] = BASS2A_WAV;
			ButtonSound.Index[1][1] = BASS2B_WAV;
			ButtonSound.Index[1][2] = BASS2C_WAV;
			ButtonSound.Index[2][0] = DRUMS1A_WAV;
			ButtonSound.Index[2][1] = DRUMS1B_WAV;
			ButtonSound.Index[2][2] = DRUMS1C_WAV;
			ButtonSound.Index[3][0] = DRUMS2A_WAV;
			ButtonSound.Index[3][1] = DRUMS2B_WAV;
			ButtonSound.Index[3][2] = DRUMS2C_WAV;
			ButtonSound.Index[4][0] = GTR1A_WAV;
			ButtonSound.Index[4][1] = GTR1B_WAV;
		    ButtonSound.Index[4][2] = GTR1C_WAV;
			ButtonSound.Index[5][0] = GTR2A_WAV;
			ButtonSound.Index[5][1] = GTR2B_WAV;
			ButtonSound.Index[5][2] = GTR2C_WAV;
			ButtonSound.Index[6][0] = GTR3A_WAV;
			ButtonSound.Index[6][1] = GTR3B_WAV;
			ButtonSound.Index[6][2] = GTR3C_WAV;
			ButtonSound.Index[7][0] = GTR4A_WAV;
			ButtonSound.Index[7][1] = GTR4B_WAV;
			ButtonSound.Index[7][2] = GTR4C_WAV;
			
			ButtonSound.Length[0][0] = BASS1A_WAV_LENGTH;
			ButtonSound.Length[0][1] = BASS1B_WAV_LENGTH;
			ButtonSound.Length[0][2] = BASS1C_WAV_LENGTH;
			ButtonSound.Length[1][0] = BASS2A_WAV_LENGTH;
			ButtonSound.Length[1][1] = BASS2B_WAV_LENGTH;
			ButtonSound.Length[1][2] = BASS2C_WAV_LENGTH;
			ButtonSound.Length[2][0] = DRUMS1A_WAV_LENGTH;
			ButtonSound.Length[2][1] = DRUMS1B_WAV_LENGTH;
			ButtonSound.Length[2][2] = DRUMS1C_WAV_LENGTH;
			ButtonSound.Length[3][0] = DRUMS2A_WAV_LENGTH;
			ButtonSound.Length[3][1] = DRUMS2B_WAV_LENGTH;
			ButtonSound.Length[3][2] = DRUMS2C_WAV_LENGTH;
			ButtonSound.Length[4][0] = GTR1A_WAV_LENGTH;
			ButtonSound.Length[4][1] = GTR1B_WAV_LENGTH;
		    ButtonSound.Length[4][2] = GTR1C_WAV_LENGTH;
			ButtonSound.Length[5][0] = GTR2A_WAV_LENGTH;
			ButtonSound.Length[5][1] = GTR2B_WAV_LENGTH;
			ButtonSound.Length[5][2] = GTR2C_WAV_LENGTH;
			ButtonSound.Length[6][0] = GTR3A_WAV_LENGTH;
			ButtonSound.Length[6][1] = GTR3B_WAV_LENGTH;
			ButtonSound.Length[6][2] = GTR3C_WAV_LENGTH;
			ButtonSound.Length[7][0] = GTR4A_WAV_LENGTH;
			ButtonSound.Length[7][1] = GTR4B_WAV_LENGTH;
			ButtonSound.Length[7][2] = GTR4C_WAV_LENGTH;
			
			
		break;
		
		
		default:
		break;
		
	}
	
	
}	

