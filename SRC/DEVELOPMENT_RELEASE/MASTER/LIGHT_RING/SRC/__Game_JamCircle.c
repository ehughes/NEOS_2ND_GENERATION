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

#define NUM_SOUND_CHANNELS	3


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

#define NO_MATCH	0xFF

struct {
	
		BOOL Active;
		DWORD CurrentSoundLength;
		WORD ButtonAssignment;
	
}	SoundChannelInfo[3];	

struct{
	
	WORD Index[NUM_SOUNDS_PER_BUTTON][NUM_BUTTONS];
	DWORD Length[NUM_SOUNDS_PER_BUTTON][NUM_BUTTONS];
	BYTE CycleCnt[NUM_BUTTONS];
	
} ButtonSound ;
	

//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void InitJamCircleChannels();
void JamCircleInitButtonSounds();


void ResetSoundChannelTimer(BYTE  Channel);
BYTE MatchButtonToActiveSoundChannel(BYTE button);
BYTE GetNextSoundChannel(BYTE button);


void JamCircle(void)
{
			
	switch (GameState)
	{
		
		case INIT:
				
			GameState = JAM_CIRCLE_SELECT;					
		
		break;
		
		
		
		case JAM_CIRCLE_SELECT:
		
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,JAMCIRCLEBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM,JAMCIRCLEBACKGROUND_WAV );
			JamCircleInitButtonSounds();
			MAIN_GAME_TIMER = 0;
			GameState = JAM_CIRCLE;
			SHOW_TIMER = 0;
			
		break;
		
		
		case JAM_CIRCLE:
		
		
			if((SOUND_CHANNEL0_TIMER > SoundChannelInfo[0].CurrentSoundLength) && (SoundChannelInfo[0].Active == TRUE))
			{
				SoundChannelInfo[0].Active = FALSE;
				
			//	AudioNodeEnable(SoundChannelInfo[0].ButtonAssignment,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ROCKBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
				AudioReSync(SoundChannelInfo[0].ButtonAssignment);
			}
			
			if((SOUND_CHANNEL1_TIMER > SoundChannelInfo[0].CurrentSoundLength) && (SoundChannelInfo[10].Active == TRUE) )
			{
				SoundChannelInfo[1].Active = FALSE;
			//	AudioNodeEnable(SoundChannelInfo[1].ButtonAssignment,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ROCKBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
				AudioReSync(SoundChannelInfo[1].ButtonAssignment);
			}
			
			if((SOUND_CHANNEL2_TIMER > SoundChannelInfo[0].CurrentSoundLength) && (SoundChannelInfo[2].Active == TRUE))
			{
				SoundChannelInfo[2].Active = FALSE;
			//	AudioNodeEnable(SoundChannelInfo[2].ButtonAssignment,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ROCKBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
				AudioReSync(SoundChannelInfo[2].ButtonAssignment);
			}
			
			if(SHOW_TIMER> 50)
			{
				BYTE R,G;
				R = (rand()&0x1f)+31;
				G = (rand()&0x1f)+31;
				
				SHOW_TIMER = 0;
				LEDSendMessage(ENABLE_ALL, R,G,0, LEDOFF, 75, 75);
			}
			
		
		
			if(MAIN_GAME_TIMER>JAMCIRCLEBACKGROUND_WAV_LENGTH-25)
			{
				/*
				GameState = JAM_CIRCLE_END;
				AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
				SendNodeNOP();	
				EAudioPlaySound(BACKGROUND_MUSIC_STREAM,ENDING_WAV);
				MAIN_GAME_TIMER = 0;*/
				
				ResetToGameSelector();
					
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
	BYTE SoundChannelTemp;
	
	
	if(GameState == JAM_CIRCLE)
	{
    	LEDSendMessage(button, YELLOW, LEDOFF, 50, 50);

		//See if this button alread has a sound
		
			if(ButtonSound.CycleCnt[button] ==(NUM_SOUNDS_PER_BUTTON-1) )
			{
				ButtonSound.CycleCnt[button] = 0;
			}
			else
			{
				ButtonSound.CycleCnt[button]++;
			}
		
		SoundChannelTemp = MatchButtonToActiveSoundChannel(button);
		
		if(SoundChannelTemp!=NO_MATCH)
		{
			//if the button already maps to a sound, then just start a new one at the same button
			
			SoundChannelInfo[SoundChannelTemp].Active = TRUE;
			SoundChannelInfo[SoundChannelTemp].ButtonAssignment = button;
			
			SoundChannelInfo[SoundChannelTemp].CurrentSoundLength = ButtonSound.Length[ButtonSound.CycleCnt[button]][button];
			
			ResetSoundChannelTimer(SoundChannelTemp);
			
			AudioNodeEnable(button,SoundChannelTemp,BACKGROUND_MUSIC_STREAM,1,1,SoundChannelInfo[SoundChannelTemp].CurrentSoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(SoundChannelTemp,ButtonSound.Index	[ButtonSound.CycleCnt[button]][button]);
			
		}
		else
		{
		
			if(ButtonSound.CycleCnt[button] ==(NUM_SOUNDS_PER_BUTTON-1) )
			{
				ButtonSound.CycleCnt[button] = 0;
			}
			else
			{
				ButtonSound.CycleCnt[button]++;
			}
			SoundChannelTemp = GetNextSoundChannel(button);
		
			//First Turn off the old button (if there is one)
			
			if(SoundChannelInfo[SoundChannelTemp].Active == TRUE)
			{
			//	AudioNodeEnable(SoundChannelInfo[SoundChannelTemp].ButtonAssignment,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,ROCKBACKGROUND_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
			
			}
			
			SoundChannelInfo[SoundChannelTemp].Active = TRUE;
			SoundChannelInfo[SoundChannelTemp].ButtonAssignment = button;
			
			SoundChannelInfo[SoundChannelTemp].CurrentSoundLength = ButtonSound.Length[ButtonSound.CycleCnt[button]][button];
			
			ResetSoundChannelTimer(SoundChannelTemp);
			
			AudioNodeEnable(button,SoundChannelTemp,BACKGROUND_MUSIC_STREAM,1,1,SoundChannelInfo[SoundChannelTemp].CurrentSoundLength,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(SoundChannelTemp,ButtonSound.Index[ButtonSound.CycleCnt[button]][button]);
			
			
			
		}
		
	
	}



}


void ResetSoundChannelTimer(BYTE  Channel)
{
		switch(Channel)
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
}	

BYTE MatchButtonToActiveSoundChannel(BYTE button)
{
	BYTE i;
	
	for(i=0;i<NUM_SOUND_CHANNELS;i++)
	{
		if(SoundChannelInfo[i].Active == TRUE)
		{
			if(SoundChannelInfo[i].ButtonAssignment == button)
			{
				return i;	
			}
			
		}
	}
	
	return NO_MATCH;
}	

BYTE GetNextSoundChannel(BYTE button)
{
	BYTE Temp;
	BYTE i;
	
	Temp = MatchButtonToActiveSoundChannel(button);
	
	
	//if the button already has a sound channel mapped , just reuse it
	if(Temp != NO_MATCH)
	{
		return i;
	}
	
	Temp = NO_MATCH;
	
	for(i=0;i<NUM_SOUND_CHANNELS;i++)
	{
		if(SoundChannelInfo[i].Active == FALSE)
		{
			return i;
		}	
	}
	
	//if we are here then all channels are used up.  Just pick a Random one
	
	return (rand()%3);

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

void JamCircleInitButtonSounds()
{
	BYTE i;
	
	for(i=0;i<NUM_BUTTONS;i++)
	{
		ButtonSound.CycleCnt[i] = 0;	
	}

			ButtonSound.Index[0][0] = BRASS1_WAV;
			ButtonSound.Index[1][0] = BRASS2_WAV;
			ButtonSound.Index[2][0] = BRASS3_WAV;
			ButtonSound.Index[0][1] = FLT1_WAV;
			ButtonSound.Index[1][1] = FLT2_WAV;
			ButtonSound.Index[2][1] = FLT3_WAV;
		    ButtonSound.Index[0][2] = GLOCK1_WAV;
		    ButtonSound.Index[1][2] = GLOCK2_WAV;
		    ButtonSound.Index[2][2] = GLOCK3_WAV;
			ButtonSound.Index[0][3] = HARP1_WAV;
			ButtonSound.Index[1][3] = HARP2_WAV;
			ButtonSound.Index[2][3] = HARP3_WAV;
			ButtonSound.Index[0][4] = PNO1_WAV;
			ButtonSound.Index[1][4] = PNO2_WAV;
			ButtonSound.Index[2][4] = PNO3_WAV;
			ButtonSound.Index[0][5] = VLN1_WAV;
			ButtonSound.Index[1][5] = VLN2_WAV;
			ButtonSound.Index[2][5] = VLN3_WAV;
			
			ButtonSound.Length[0][0] = BRASS1_WAV_LENGTH;
			ButtonSound.Length[1][0] = BRASS2_WAV_LENGTH;
			ButtonSound.Length[2][0] = BRASS3_WAV_LENGTH;
			ButtonSound.Length[0][1] = FLT1_WAV_LENGTH;
			ButtonSound.Length[1][1] = FLT2_WAV_LENGTH;
			ButtonSound.Length[2][1] = FLT3_WAV_LENGTH;
		    ButtonSound.Length[0][2] = GLOCK1_WAV_LENGTH;
		    ButtonSound.Length[1][2] = GLOCK2_WAV_LENGTH;
		    ButtonSound.Length[2][2] = GLOCK3_WAV_LENGTH;
			ButtonSound.Length[0][3] = HARP1_WAV_LENGTH;
			ButtonSound.Length[1][3] = HARP2_WAV_LENGTH;
			ButtonSound.Length[2][3] = HARP3_WAV_LENGTH;
			ButtonSound.Length[0][4] = PNO1_WAV_LENGTH;
			ButtonSound.Length[1][4] = PNO2_WAV_LENGTH;
			ButtonSound.Length[2][4] = PNO3_WAV_LENGTH;
			ButtonSound.Length[0][5] = VLN1_WAV_LENGTH;
			ButtonSound.Length[1][5] = VLN2_WAV_LENGTH;
			ButtonSound.Length[2][5] = VLN3_WAV_LENGTH;
			


}	


