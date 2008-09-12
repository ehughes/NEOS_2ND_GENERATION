#include "__Game_RootGame0.h"
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
#include "USB.h"
#include "InternalButtonSoundMapping.h"
#include "TimerRoutines.h"
#include "TimerRoutines.h"
//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************
#define WAIT1 0x01
#define MAIN_SELECTOR 0x02
#define SELECT_TO_START_DELAY 0x03
#define COUNT_DOWN_TO_GAME 0x04
#define RESYNC_DELAY	0x10


#define SELECTOR_STATE_SPIN					1
#define SELECTOR_STATE_PLAYER_SELECT_FLASH 	2

#define MUSIC_IDLE					0
#define MUSIC_MAIN_THEME			1
#define MUSIC_HEARTBEAT				2


#define ONE_PLAYER_START_BUTTON 	9
#define TWO_PLAYER_START_BUTTON 	14

#define LIGHT_GRABBER_BUTTON		5
#define DOUBLE_DOTS_BUTTON			11
#define SURROUND_SOUND_BUTTON		12
#define RODEO_BUTTON				6
#define MARATHON_BUTTON 			13
#define ZIGZAG_BUTTON 				8
#define NINJA_BUTTON 				7
#define FIRE_FIGHTER_BUTTON 		10

//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER				GPTimer[0]
#define COUNT_TIMER					GPTimer[1]
#define SELECT_TO_START_TIMER		GPTimer[2]
#define GAME_BUTTON_ANIMATION_TIMER	GPTimer[3]
#define SCORE_DISPLAY_ANIMATION_TIMER GPTimer[4]
#define START_BUTTON_ANIMATION_TIMER GPTimer[5]
#define SELECTOR_SPIN_ANIMATION_TIMER GPTimer[6]
#define MUSIC_TIMER					 GPTimer[7]


//*************************************************
//*******Game Parameters***************************
//*************************************************

#define GAME_BUTTON_ANIMATION_TIME	 15
#define SCORE_DISPLAY_ANIMATION_TIME 8
#define SELECTOR_SPIN_ANIMATION_TIME 8
#define START_BUTTON_ANIMATION_TIME	 8

#define HEARTBEAT_TIME		HEARTBEAT_WAV_LENGTH * 30

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************
#define BACKGROUND_MUSIC_STREAM 0
#define MAINTHEME_L_STREAM		0
#define MAINTHEME_LS_STREAM		1
#define MAINTHEME_R_STREAM		2
#define MAINTHEME_RS_STREAM		3	

//*************************************************
//*******Game Variables****************************
//*************************************************
  
 BYTE SelectorCircle[8] = {LIGHT_GRABBER_BUTTON,
						  FIRE_FIGHTER_BUTTON,
						  DOUBLE_DOTS_BUTTON,
						  SURROUND_SOUND_BUTTON,
						  MARATHON_BUTTON,
						  ZIGZAG_BUTTON,
						  NINJA_BUTTON,
						  RODEO_BUTTON};
						  
BYTE OnePlayerStartButton = ONE_PLAYER_START_BUTTON;
BYTE TwoPlayerStartButton = TWO_PLAYER_START_BUTTON;

BYTE GameButtonSpin = 0;
BYTE SpinSeg1 = 0;
BYTE SpinSeg2 = 2;
BYTE Color1 =0;
BYTE Color2 =0;
BYTE PlayerSelectFlashTick=0;
BYTE PlayerSelectFlash=0;
BYTE PendingGameSelection = 1;
BYTE PendingPlayerSelection = 1;
WORD CountDownLength;
BYTE Count;
BYTE SelectorSpin = 0;
BYTE SelectState;


BYTE MusicState;

#define TOP			0x01
#define UPPER_RIGHT 0x02
#define LOWER_RIGHT	0x04
#define BOTTOM		0x08
#define LOWER_LEFT	0x10	
#define UPPER_LEFT	0x20
#define CENTER		0x40


const BYTE SpinSegments[6] = {TOP,UPPER_RIGHT,LOWER_RIGHT,BOTTOM,LOWER_LEFT,UPPER_LEFT};
const BYTE SpinBig1[10]={TOP,0	,0	,0			,0			,0		,0		,BOTTOM	,LOWER_LEFT	,UPPER_LEFT};
const BYTE SpinBig2[10]={0	,TOP,0	,0			,0			,0		,BOTTOM ,0		,0			,0		   };
const BYTE SpinBig3[10]={0  ,0	,TOP,UPPER_RIGHT,LOWER_RIGHT,BOTTOM ,0		,0		,0			,0		   };	


BOOL IntroMusicOn = FALSE;


//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void StartCountDown();
void UpdateGameSettings();
void PlayCountDown();
void StartMainThemeMusic();
void AnimateGameButtons();
void AnimateScoreDisplay();
void SelectorButtonSpin();
void AnimateStartButtons();
void PlaySelectorSound();
void MusicWake();
void StartHeartBeat();

void SelectorPlayButtonFeebackSound();


//*************************************************
//*******Game Functions****************************
//*************************************************

void Root_Game0 (void)
{
			
	switch (GameState)
	{
		
		
		case INIT:
			RED_LED_OFF;
			GREEN_LED_ON;
		
			GameState = WAIT1;
			ScoreManagerEnabled = FALSE;
			SpinSeg1 = 0;
			SpinSeg2 = 2;
			Color1 =0;
			Color2 =0;
			PlayerSelectFlashTick=0;
			PlayerSelectFlash=0;
			PendingGameSelection = 1;
			PendingPlayerSelection = 1;
			GAME_BUTTON_ANIMATION_TIMER = 0;
			MAIN_GAME_TIMER = 0;
			SCORE_DISPLAY_ANIMATION_TIMER=0;
			SELECTOR_SPIN_ANIMATION_TIMER=0;
			START_BUTTON_ANIMATION_TIMER=0;
			SelectState = SELECTOR_STATE_SPIN;
			ResetAudioAndLEDS();
			StartHeartBeat();
			
			
		break;
		
		case WAIT1:
			if(MAIN_GAME_TIMER>100)
			{
			   MAIN_GAME_TIMER = 0;
			   GameState = MAIN_SELECTOR;	
			}
		break;
		
		case MAIN_SELECTOR:
		
		
		switch(MusicState)
		{
			case MUSIC_IDLE:
			break;
			
			case MUSIC_MAIN_THEME:
				if(MUSIC_TIMER > MAINTHEME_L_WAV_LENGTH-10 )
				{
					StartHeartBeat();	
				}
			break;
			
			case MUSIC_HEARTBEAT:
				if(MUSIC_TIMER > HEARTBEAT_TIME )
				{
					AudioOffAllNodes();
					MusicState = MUSIC_IDLE;	
				}
			break;
			
			default:
			break;
				
			
		}
		
			switch (SelectState)
			{
				case SELECTOR_STATE_SPIN:	
					AnimateGameButtons();
					AnimateScoreDisplay();
					SelectorButtonSpin();
				break;
				
				case SELECTOR_STATE_PLAYER_SELECT_FLASH:
					AnimateGameButtons();
					AnimateScoreDisplay();
					AnimateStartButtons();
				
				break;
				
			} 
			
			
		break;
		
			
		case SELECT_TO_START_DELAY:
		//This state was added to ensure that the coundown dsound happens.   If the 1 or 2p start happens right after a selector
		//button press, the count down wont happen.  THis is because the selector sound is played with internal sounds now.  
		//The coundown sound sound is stream.  When playing internal sounds, the buttons ignore stream commands.  So there has to be
		//at least a delay of the Selector sound length before attemping count down.
		
		if(SELECT_TO_START_TIMER>SELECTIONBUTTON_WAV_LENGTH+1)
		{
			StartCountDown();
			PlayCountDown();
		}
		
		break;
			
		case COUNT_DOWN_TO_GAME:
		
		
		if(COUNT_TIMER > CountDownLength/3)
		{
			Count--;
			COUNT_TIMER = 0;
			switch(Count)
			{
				case 3:
					if(GamePlayers == 2)
					{
						LEDSendDigits(DISPLAY_ADDRESS, ScoreBrightness,3,BLANK_DIGIT,BLANK_DIGIT, 3,BLANK_DIGIT, BLANK_DIGIT);
					}
					else
					{
						LEDSendDigits(DISPLAY_ADDRESS, ScoreBrightness,3,BLANK_DIGIT,BLANK_DIGIT, BLANK_DIGIT,BLANK_DIGIT, BLANK_DIGIT);
					}
				break;	
				
				case 2:
					if(GamePlayers == 2)
					{
						LEDSendDigits(DISPLAY_ADDRESS, ScoreBrightness,BLANK_DIGIT,2,BLANK_DIGIT,BLANK_DIGIT,2,BLANK_DIGIT);
					}
					else
					{
						LEDSendDigits(DISPLAY_ADDRESS, ScoreBrightness,BLANK_DIGIT,2,BLANK_DIGIT,BLANK_DIGIT,BLANK_DIGIT,BLANK_DIGIT);	
					}
				break;
				
				case 1:
					if(GamePlayers == 2)
					{
						LEDSendDigits(DISPLAY_ADDRESS, ScoreBrightness,BLANK_DIGIT,BLANK_DIGIT,1,BLANK_DIGIT,BLANK_DIGIT,1);
					}
					else
					{
						LEDSendDigits(DISPLAY_ADDRESS, ScoreBrightness,BLANK_DIGIT,BLANK_DIGIT,1,BLANK_DIGIT,BLANK_DIGIT,BLANK_DIGIT);
					}
				break;
				
				
				case 0:
				default:
					if(GamePlayers == 2)
					{
						LEDSendDigits(DISPLAY_ADDRESS, ScoreBrightness,BLANK_DIGIT,BLANK_DIGIT,0,BLANK_DIGIT,BLANK_DIGIT,0);
					}
					else
					{
						LEDSendDigits(DISPLAY_ADDRESS, ScoreBrightness,BLANK_DIGIT,BLANK_DIGIT,0,BLANK_DIGIT,BLANK_DIGIT,BLANK_DIGIT);
					}
				break;
					
			}
			
		}
		
		if(MAIN_GAME_TIMER>CountDownLength)
			{
				GameState = INIT;
		
				LEDSendScores(DISPLAY_ADDRESS,ScoreBrightness, BLANK_SCORE,BLANK_SCORE);		
				Player1Score = 0;
				Player2Score = 0;		
				InitRandomButton();	
				switch(PendingGameSelection)
				{
								
					case LIGHT_GRABBER_BUTTON:
						GameSelected = GAME_LIGHT_GRABBER;
					break;
							
					case DOUBLE_DOTS_BUTTON:
						GameSelected = GAME_DOUBLE_DOTS;		
					break;
					
					case SURROUND_SOUND_BUTTON:	
						GameSelected = GAME_SURROUND_SOUND;	
					break;
									
					case RODEO_BUTTON:
						GameSelected = GAME_RODEO;				
					break;
					
					case MARATHON_BUTTON: 
						GameSelected = GAME_MARATHON;			
					break;
					
					case ZIGZAG_BUTTON: 
						GameSelected = GAME_ZIGZAG;			
					break;
					
					case NINJA_BUTTON: 	
						GameSelected = GAME_NINJA;			
					break;
					
					case FIRE_FIGHTER_BUTTON: 
						GameSelected = GAME_FIRE_FIGHTER;		
					break;
					
					default:
						GameSelected = GAME_ROOT_GAME0;
					break;
							
				}
				
			}
			UpdateGameSettings();
			
			
			
		break;
	
		
		
	
		
		default:
		GameState = INIT;
		break;
		
	}
}



void AnimateStartButtons()
{
	if(START_BUTTON_ANIMATION_TIMER > START_BUTTON_ANIMATION_TIME)
	{
			START_BUTTON_ANIMATION_TIMER = 0;
		
			//Animate the Player select buttons
	   	 	PlayerSelectFlashTick++;
	   	 	PlayerSelectFlashTick&=0x3;
		 	if(PlayerSelectFlashTick==0)
		 	{
				 if(PlayerSelectFlash == 1)
				 {
		   			PlayerSelectFlash =2;
		   		 	ScoreSendLights(248,  (1<<PendingGameSelection)|(1<<TwoPlayerStartButton),0x0);
				 }
		   		 else
		   		 {
			   		PlayerSelectFlash =1;	 
			   		ScoreSendLights(248,   (1<<PendingGameSelection)|(1<<OnePlayerStartButton),0x0);
			   	 }
		   	}
	 }
}	

void SelectorButtonSpin()
{
	if(SELECTOR_SPIN_ANIMATION_TIMER > SELECTOR_SPIN_ANIMATION_TIME)
		{
			SELECTOR_SPIN_ANIMATION_TIMER = 0;
			ScoreSendLights(248, (1<<SelectorCircle[SelectorSpin]),0x0);
			SelectorSpin++;
			SelectorSpin &= 0x07;
	   }		
	
	
}	

void AnimateScoreDisplay()
{
		if(SCORE_DISPLAY_ANIMATION_TIMER > SCORE_DISPLAY_ANIMATION_TIME)
		{
			SCORE_DISPLAY_ANIMATION_TIMER = 0;
	  		CANQueueTxMessage(0x70,248+(ScoreBrightness<<8),SpinBig1[SpinSeg1]+(SpinBig2[SpinSeg1]<<8),SpinBig3[SpinSeg1]+(SpinBig1[SpinSeg2]<<8),SpinBig2[SpinSeg2]+(SpinBig3[SpinSeg2]<<8));
		   	SpinSeg1++;if(SpinSeg1>9)SpinSeg1=0;
		   	SpinSeg2--;if(SpinSeg2>9)SpinSeg2=9;
	   }			 
	
	
}	


void AnimateGameButtons()
{
	if(GAME_BUTTON_ANIMATION_TIMER > GAME_BUTTON_ANIMATION_TIME)
	{
		GAME_BUTTON_ANIMATION_TIMER = 0;
		
		GameButtonSpin += 1;
	   	GameButtonSpin &= 0x07;
	
		Color1++;
	   	Color1 &= 0x3f;
	   	if(Color1==0)
	   	{
		 	Color2++;
		 	Color2&=0x3F;
		}
	   	LEDSendMessage(GameButtonSpin, Color1,Color2,0, LEDOFF, 60, 50);	
	 }			 	
	
}	

void OnButtonPressRootGame0(unsigned char button)
{
	PlaySelectorSound();
	
	MusicWake();
		
}

void OnMasterSwitchPressRootGame0(void)
{
	AudioNodeEnable(ENABLE_ALL,3,1,1,0,BUTTONFEEDBACK_WAV_LENGTH,0xFF,0xFF);
	SendNodeNOP();
	EAudioPlaySound(3,BUTTONFEEDBACK_WAV);
}




void OnSelectPressRootGame0(unsigned char button)
{
	
	
	switch(SelectState)
		{
		
				case SELECTOR_STATE_SPIN:
				
					MusicWake();
				
					switch(button)
					{
						case 	ONE_PLAYER_START_BUTTON:
						break;
						
						case 	TWO_PLAYER_START_BUTTON:
						break;
						
						case 0xf:
							//some displays send out this code...
							//Short on the PCB perhaps.
						break;
						
						default:
							SelectState = SELECTOR_STATE_PLAYER_SELECT_FLASH;
							PendingGameSelection = button;
							PlaySelectorSound();
						break;
					}
					
				break;
				
				case SELECTOR_STATE_PLAYER_SELECT_FLASH:
				
					MusicWake();
									
					switch(button)
					{
						case 	ONE_PLAYER_START_BUTTON:
						case 	TWO_PLAYER_START_BUTTON:
						
						if(button == OnePlayerStartButton)
						{
								GamePlayers = 1;
							ScoreSendLights(248,  1<<(PendingGameSelection) | (1<<OnePlayerStartButton),0x0);
						}
						else
						{
								GamePlayers = 2;
							ScoreSendLights(248,  1<<(PendingGameSelection) | (1<<TwoPlayerStartButton),0x0);
						}
								
					
						GameState = SELECT_TO_START_DELAY; // added to make sure countdown sound happens
						
						break;
						
						default:
							GameState = SELECTOR_STATE_PLAYER_SELECT_FLASH;
							PendingGameSelection = button;
							PlaySelectorSound();
						break;
					}
					

				break;
				
				default:
				
				break;
	}


}	


void MusicWake()
{
switch(MusicState)
	{
		case MUSIC_IDLE:
		case MUSIC_HEARTBEAT:
			StartMainThemeMusic();
		break;
		
		default:
		break;
	}	
	
}	

void PlaySelectorSound()
{
	PlayInternalNodeSound(0,INTERNAL_SOUND_SELECTION,0xFF,1,0,0xFF,1);
	PlayInternalNodeSound(2,INTERNAL_SOUND_SELECTION,0xFF,1,1,0xFF,1);
	PlayInternalNodeSound(4,INTERNAL_SOUND_SELECTION,0xFF,1,2,0xFF,1);
	PlayInternalNodeSound(6,INTERNAL_SOUND_SELECTION,0xFF,1,3,0xFF,1);
	
	SELECT_TO_START_TIMER = 0;
}



void StartCountDown()
{
		GameState = COUNT_DOWN_TO_GAME;
		CountDownLength = 190;
		MAIN_GAME_TIMER = 0;
		COUNT_TIMER = 0xFFFF;
		Count = 4;
}	

void PlayCountDown()
{
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,1,0,CountDownLength,0xFF,0x00);
	SendNodeNOP();				
	EAudioPlaySound( BACKGROUND_MUSIC_STREAM,COUNTDOWN_WAV);	
}	


void UpdateGameSettings()
{
	
	switch (GameSelected)
	{
		case GAME_LIGHT_GRABBER:
		
			CurrentGameSettings.GameBackgroundMusicVolume = 0xFF;
			CurrentGameSettings.GameSoundEffectVolume = 0xFF;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
			
		break;
		
		case GAME_DOUBLE_DOTS:
		
			CurrentGameSettings.GameBackgroundMusicVolume = 0xC0;
			CurrentGameSettings.GameSoundEffectVolume = 0xFF;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
			
		break;
		
		case GAME_SURROUND_SOUND:
		
			CurrentGameSettings.GameBackgroundMusicVolume = 0xA0;
			CurrentGameSettings.GameSoundEffectVolume = 0xFF;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
			
		break;
		
		case GAME_RODEO:
		
			CurrentGameSettings.GameBackgroundMusicVolume = 0xFF;
			CurrentGameSettings.GameSoundEffectVolume = 0xFF;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
						
		break;
		
		case GAME_MARATHON:
		
			CurrentGameSettings.GameBackgroundMusicVolume = 0x75;
			CurrentGameSettings.GameSoundEffectVolume = 0xFF;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
			
		break;
		
		case GAME_ZIGZAG:
		
			CurrentGameSettings.GameBackgroundMusicVolume = 0xFF;
			CurrentGameSettings.GameSoundEffectVolume = 0x80;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
					
		break;
		
		case GAME_NINJA:
		
			CurrentGameSettings.GameBackgroundMusicVolume = 0x75;
			CurrentGameSettings.GameSoundEffectVolume = 0xFF;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
			
		break;
		
		case GAME_FIRE_FIGHTER:
		
			CurrentGameSettings.GameBackgroundMusicVolume = 0x80;
			CurrentGameSettings.GameSoundEffectVolume = 0xFF;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
					
		break;
	
		default:
			CurrentGameSettings.GameBackgroundMusicVolume = 0xFF;
			CurrentGameSettings.GameSoundEffectVolume = 0xFF;
			CurrentGameSettings.FinaleMusicVolume = 0xC0;
			
		break;
	}
	
}	




	

	
	
	
void StartMainThemeMusic()
{
	MusicState = MUSIC_MAIN_THEME;
	MUSIC_TIMER = 0;
	
	AudioNodeEnable(0,MAINTHEME_L_STREAM,MAINTHEME_L_STREAM,1,1,0xFFFF,0xFF,0xFF);
	SendNodeNOP();	
	AudioNodeEnable(1,MAINTHEME_L_STREAM,MAINTHEME_L_STREAM,1,1,0xFFFF,0xFF,0xFF);
	SendNodeNOP();	
	
	AudioNodeEnable(2,MAINTHEME_LS_STREAM,MAINTHEME_LS_STREAM,1,1,0xFFFF,0xFF,0xFF);
	SendNodeNOP();	
	AudioNodeEnable(3,MAINTHEME_LS_STREAM,MAINTHEME_LS_STREAM,1,1,0xFFFF,0xFF,0xFF);
	
	AudioNodeEnable(4,MAINTHEME_R_STREAM,MAINTHEME_R_STREAM,1,1,0xFFFF,0xFF,0xFF);
	SendNodeNOP();	
	AudioNodeEnable(5,MAINTHEME_R_STREAM,MAINTHEME_R_STREAM,1,1,0xFFFF,0xFF,0xFF);
	SendNodeNOP();	
	
	AudioNodeEnable(6,MAINTHEME_RS_STREAM,MAINTHEME_RS_STREAM,1,1,0xFFFF,0xFF,0xFF);
	SendNodeNOP();	
	AudioNodeEnable(7,MAINTHEME_RS_STREAM,MAINTHEME_RS_STREAM,1,1,0xFFFF,0xFF,0xFF);
	SendNodeNOP();	
	

	
	EAudioPlaySound(MAINTHEME_L_STREAM	,MAINTHEME_L_WAV);
	EAudioPlaySound(MAINTHEME_LS_STREAM	,MAINTHEME_LS_WAV);
	EAudioPlaySound(MAINTHEME_R_STREAM	,MAINTHEME_R_WAV);
	EAudioPlaySound(MAINTHEME_RS_STREAM	,MAINTHEME_RS_WAV);
	
		

}	


void StartHeartBeat()
{
	MusicState = MUSIC_HEARTBEAT;
	MUSIC_TIMER = 0;
	AudioReSync(0);
	AudioReSync(1);
	AudioReSync(2);
	AudioReSync(3);
	AudioReSync(4);
	AudioReSync(5);
	AudioReSync(6);
	AudioReSync(7);
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,1,1,0xFFFF,0xFF,0xFF);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,HEARTBEAT_WAV);

	
}

