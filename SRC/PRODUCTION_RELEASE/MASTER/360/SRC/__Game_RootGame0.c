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
#include "BoardSupport.h"
#include "config.h"

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************
#define WAIT1 				  0x01
#define MAIN_SELECTOR 		  0x02
#define SELECT_TO_START_DELAY 0x03
#define COUNT_DOWN_TO_GAME    0x04
#define RESYNC_DELAY		  0x05
#define VOLUME_CHANGE_INIT	  0x06
#define VOLUME_CHANGE		  0x07
#define DISPLAY_GAME_COUNT	  0x08


#define SELECTOR_STATE_SPIN					1
#define SELECTOR_STATE_PLAYER_SELECT_FLASH 	2
#define EASTER_EGG_DISPLAY_1			 	3
#define EASTER_EGG_DISPLAY_2			 	4
#define EASTER_EGG_DISPLAY_3			 	5
#define DISPLAY_TRAIL					 	6
#define	SELECTOR_STATE_LIGHTS_OUT			7

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

#define MAIN_GAME_TIMER					 GPTimer[0]
#define COUNT_TIMER					 	 GPTimer[1]
#define SELECT_TO_START_TIMER		  	 GPTimer[2]
#define GAME_BUTTON_ANIMATION_TIMER		 GPTimer[3]
#define SCORE_DISPLAY_ANIMATION_TIMER	 GPTimer[4]
#define START_BUTTON_ANIMATION_TIMER	 GPTimer[5]
#define SELECTOR_SPIN_ANIMATION_TIMER	 GPTimer[6]
#define MUSIC_TIMER						 GPTimer[7]
#define VC_ENTRY_TIMER					 GPTimer[8]
#define GC_ENTRY_TIMER					 GPTimer[9]
#define EASTER_EGG_TIMER				 GPTimer[10]
#define VOLUME_CHANGE_TIMER				 GPTimer[11]

//*************************************************
//*******Game Parameters***************************
//*************************************************

#define GAME_BUTTON_ANIMATION_TIME	 15
#define SCORE_DISPLAY_ANIMATION_TIME 8
#define SELECTOR_SPIN_ANIMATION_TIME 8
#define START_BUTTON_ANIMATION_TIME	 8

#define HEARTBEAT_TIME		HEARTBEAT_WAV_LENGTH * 15

#define VC_ENTRY_TIME	150
#define GC_ENTRY_TIME	100


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



BYTE VolumeChangeEntrySequence [5] = {LIGHT_GRABBER_BUTTON,MARATHON_BUTTON, NINJA_BUTTON,DOUBLE_DOTS_BUTTON,LIGHT_GRABBER_BUTTON};
BYTE VolumeChangeEntrySequencePosition = 0;


BYTE GameCountEntrySequence [5] = { MARATHON_BUTTON, MARATHON_BUTTON, MARATHON_BUTTON, MARATHON_BUTTON, MARATHON_BUTTON};
BYTE GameCountEntrySequencePosition = 0;


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
BYTE QueuedSelectState;
BYTE QueuedEasterEggDisplay;
BYTE MusicState;

static BYTE ButtonHistory[4];
static BYTE BHPointer;
static BYTE EERepeats;

static BYTE ButtonTrail[4];

static BYTE HitsSinceLastClear;


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


//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void StartCountDown();
void UpdateGameSettings();
void PlayCountDown();
void StartMainthemeMusic();
void AnimateGameButtons();
void AnimateScoreDisplay();
void SelectorButtonSpin();
void AnimateStartButtons();
void PlaySelectorSound();
void MusicWake();
void StartHeartBeat();

void SelectorPlayButtonFeebackSound();
BOOL CheckVolumeChangeEntrySequence(BYTE Button);
void MoveToVolumeChange();
void Game0PlayButtonFeebackSound(BYTE Volume, BYTE Repeats);
void ClearButtonHistory();
void EasterEggHunt();

WORD EasterEggDisplayTimeout;
BYTE GetStream(BYTE button);
BOOL CheckGameCountEntrySequence(BYTE button);
void MoveToGameCountDisplay();

//*************************************************
//*******Game Functions****************************
//*************************************************

void Root_Game0 (void)
{
			
	switch (GameState)
	{
		
		
		case INIT:
			ResetAudioAndLEDS();
			RED_LED_OFF;
			GREEN_LED_ON;
			GameState = WAIT1;
			MAIN_GAME_TIMER = 0;
		break;
		
		case WAIT1:
			if(MAIN_GAME_TIMER>20)
			{
				VolumeChangeEntrySequencePosition = 0;
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
				UpdateGameSettings();
				StartHeartBeat();	
				ClearButtonHistory();
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
				if(MUSIC_TIMER > MAINTHEME_L_WAV_LENGTH-40 )
				{
					StartHeartBeat();	
				}
			break;
			
			case MUSIC_HEARTBEAT:
				if(MUSIC_TIMER > HEARTBEAT_TIME )
				{
					AudioOffAllNodes();
					MusicState = MUSIC_IDLE;	

					#ifdef LIGHTS_OFF_AFTER_HEARTBEAT
			
					SelectState = SELECTOR_STATE_LIGHTS_OUT;
					ResetLeds();
					ResetLeds();
					
					#endif
					
				}
			break;
			
			default:
			break;
				
			
		}
		
			switch (SelectState)
			{
				
				case SELECTOR_STATE_LIGHTS_OUT:

				break;

				case SELECTOR_STATE_SPIN:	
					EasterEggHunt();
					AnimateGameButtons();
					AnimateScoreDisplay();
					SelectorButtonSpin();
				break;
				
				case SELECTOR_STATE_PLAYER_SELECT_FLASH:
					EasterEggHunt();
					AnimateGameButtons();
					AnimateScoreDisplay();
					AnimateStartButtons();
				break;
				
				case EASTER_EGG_DISPLAY_1:
				
					AnimateScoreDisplay();
					AnimateStartButtons();
				
					if(EASTER_EGG_TIMER > (EasterEggDisplayTimeout))
					{
						EASTER_EGG_TIMER = 0;
						if(EasterEggDisplayTimeout >=10)
						{
						EasterEggDisplayTimeout *= 0xC7;
						EasterEggDisplayTimeout=EasterEggDisplayTimeout>>8;
						}
						if((EasterEggDisplayTimeout < 10) && (EERepeats >20))
						{
							SelectState = QueuedSelectState;	
							EASTER_EGG_TIMER = 0;
							PlayInternalNodeSound(ButtonTrail[0],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(0),CurrentGameSettings.GameBackgroundMusicVolume,1);
							PlayInternalNodeSound(ButtonTrail[1],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(2),CurrentGameSettings.GameBackgroundMusicVolume,1);
							PlayInternalNodeSound(ButtonTrail[2],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(4),CurrentGameSettings.GameBackgroundMusicVolume,1);
							PlayInternalNodeSound(ButtonTrail[3],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(6),CurrentGameSettings.GameBackgroundMusicVolume,1);
							LEDSendMessage(ENABLE_ALL,YELLOW,LEDOFF,50,50);
							ClearButtonHistory();
							
						}
						else
						{
							EERepeats++;
							BYTE button = RandomButton(NO_EXCLUDE, NO_EXCLUDE, NO_EXCLUDE);
							PlayInternalNodeSound(button,INTERNAL_SOUND_FF_WATER_HIT,AudioGlobalVolume,1,GetStream(button),AudioGlobalVolume,1);
							LEDSendMessage(button,rand()&0x3f,rand()&0x3f,0,LEDOFF,(EasterEggDisplayTimeout),(EasterEggDisplayTimeout));
						}		
						
					}	
					
				
				break;
				
				case EASTER_EGG_DISPLAY_2:
				
					AnimateScoreDisplay();
					AnimateStartButtons();
					
					if(EASTER_EGG_TIMER > EasterEggDisplayTimeout)
					{
						EASTER_EGG_TIMER = 0;
						
						
							if(EasterEggDisplayTimeout<= 25)
							{
								BYTE button = RandomButton(NO_EXCLUDE, NO_EXCLUDE, NO_EXCLUDE);
								EasterEggDisplayTimeout++;
								PlayInternalNodeSound(button,INTERNAL_SOUND_FF_WATER_HIT,AudioGlobalVolume,1,GetStream(button),AudioGlobalVolume,1);
								LEDSendMessage(button,rand()&0x3f,rand()&0x3f,0,LEDOFF,(EasterEggDisplayTimeout),(EasterEggDisplayTimeout));
							}
							else
							{
								SelectState = QueuedSelectState;	
								EASTER_EGG_TIMER = 0;
								PlayInternalNodeSound(ButtonTrail[0],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(0),CurrentGameSettings.GameBackgroundMusicVolume,1);
								PlayInternalNodeSound(ButtonTrail[1],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(2),CurrentGameSettings.GameBackgroundMusicVolume,1);
								PlayInternalNodeSound(ButtonTrail[2],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(4),CurrentGameSettings.GameBackgroundMusicVolume,1);
								PlayInternalNodeSound(ButtonTrail[3],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(6),CurrentGameSettings.GameBackgroundMusicVolume,1);
								LEDSendMessage(ENABLE_ALL,YELLOW,LEDOFF,50,50);
								ClearButtonHistory();
							}	
					}
				
				break;
				
				case EASTER_EGG_DISPLAY_3:
				
					AnimateScoreDisplay();
					AnimateStartButtons();
					
					if(EASTER_EGG_TIMER > EasterEggDisplayTimeout)
					{
						EASTER_EGG_TIMER = 0;
						
						PlayInternalNodeSound(BHPointer,INTERNAL_SOUND_FF_WATER_MISS,AudioGlobalVolume,1,GetStream(BHPointer),AudioGlobalVolume,1);
						LEDSendMessage(BHPointer,rand()&0x3f,rand()&0x3f,0,LEDOFF,(EasterEggDisplayTimeout*4),(EasterEggDisplayTimeout*4));
						BHPointer++;
						BHPointer&=0x7;
						EERepeats++;
						
						if(EERepeats == 17)
						{
							SelectState = QueuedSelectState;	
							EASTER_EGG_TIMER = 0;
							PlayInternalNodeSound(ButtonTrail[0],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(0),CurrentGameSettings.GameBackgroundMusicVolume,1);
							PlayInternalNodeSound(ButtonTrail[1],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(2),CurrentGameSettings.GameBackgroundMusicVolume,1);
							PlayInternalNodeSound(ButtonTrail[2],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(4),CurrentGameSettings.GameBackgroundMusicVolume,1);
							PlayInternalNodeSound(ButtonTrail[3],INTERNAL_SOUND_FF_FIRE_OUT,CurrentGameSettings.GameSoundEffectVolume,1,GetStream(6),CurrentGameSettings.GameBackgroundMusicVolume,1);
							LEDSendMessage(ENABLE_ALL,YELLOW,LEDOFF,50,50);
							ClearButtonHistory();
						}	
					}
				
				break;
				
				
				case DISPLAY_TRAIL:
					if(EASTER_EGG_TIMER > 20)
					{
						EASTER_EGG_TIMER = 0;
						LEDSendMessage(ButtonTrail[0],RED,LEDOFF,12,12);
						LEDSendMessage(ButtonTrail[1],RED,LEDOFF,12,12);
						LEDSendMessage(ButtonTrail[2],RED,LEDOFF,12,12);
						LEDSendMessage(ButtonTrail[3],RED,LEDOFF,12,12);
						EERepeats++;
						if(EERepeats > 5)
						{
							EERepeats = 0;
							EASTER_EGG_TIMER = 0xFFFF;
							SelectState = QueuedEasterEggDisplay;	
						}	
					}
				
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
						LEDSendDigits(DISPLAY_ADDRESS, 0xFF,3,BLANK_DIGIT,BLANK_DIGIT, 3,BLANK_DIGIT, BLANK_DIGIT);
					}
					else
					{
						LEDSendDigits(DISPLAY_ADDRESS, 0xFF,3,BLANK_DIGIT,BLANK_DIGIT, BLANK_DIGIT,BLANK_DIGIT, BLANK_DIGIT);
					}
				break;	
				
				case 2:
					if(GamePlayers == 2)
					{
						LEDSendDigits(DISPLAY_ADDRESS, 0xFF,BLANK_DIGIT,2,BLANK_DIGIT,BLANK_DIGIT,2,BLANK_DIGIT);
					}
					else
					{
						LEDSendDigits(DISPLAY_ADDRESS, 0xFF,BLANK_DIGIT,2,BLANK_DIGIT,BLANK_DIGIT,BLANK_DIGIT,BLANK_DIGIT);	
					}
				break;
				
				case 1:
					if(GamePlayers == 2)
					{
						LEDSendDigits(DISPLAY_ADDRESS, 0xFF,BLANK_DIGIT,BLANK_DIGIT,1,BLANK_DIGIT,BLANK_DIGIT,1);
					}
					else
					{
						LEDSendDigits(DISPLAY_ADDRESS, 0xFF,BLANK_DIGIT,BLANK_DIGIT,1,BLANK_DIGIT,BLANK_DIGIT,BLANK_DIGIT);
					}
				break;
				
				
				case 0:
				default:
					if(GamePlayers == 2)
					{
						LEDSendDigits(DISPLAY_ADDRESS, 0xFF,BLANK_DIGIT,BLANK_DIGIT,0,BLANK_DIGIT,BLANK_DIGIT,0);
					}
					else
					{
						LEDSendDigits(DISPLAY_ADDRESS, 0xFF,BLANK_DIGIT,BLANK_DIGIT,0,BLANK_DIGIT,BLANK_DIGIT,BLANK_DIGIT);
					}
				break;
					
			}
			
		}
		
		if(MAIN_GAME_TIMER>CountDownLength)
			{
				GameState = INIT;
		
				LEDSendScores(DISPLAY_ADDRESS,0xFF, BLANK_SCORE,BLANK_SCORE);		
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
				
					UpdateGameSettings();
		
					if(GameCount >= 999999)
					{
						GameCount = 999999;	
					}		
					else
					{
						GameCount++;
						StoreGameCount();	
					}	
				
				
			}
		
				
		break;
	
		
		case VOLUME_CHANGE_INIT:
		
		if(MAIN_GAME_TIMER > 25)
		{
			Game0PlayButtonFeebackSound(0xFF,1);
			LEDSendVariable(DISPLAY_VOL, AudioGlobalVolumeIndex);
			GameState = VOLUME_CHANGE;
			MAIN_GAME_TIMER = 0xFFFF;
			VOLUME_CHANGE_TIMER = 0;
			SelectState = 0;
		}
		
		break;	
		
		
		case VOLUME_CHANGE:
		
			if(VOLUME_CHANGE_TIMER>1000)
			{
				GameState = INIT;		
				
			}	
			if(MAIN_GAME_TIMER > 50)
			{
				MAIN_GAME_TIMER= 0;
				if(SelectState)
				{
					ScoreSendLights(248, (1<<LIGHT_GRABBER_BUTTON),0x0);
					SelectState = 0;
					
				}
				else
				{
					ScoreSendLights(248, (1<<MARATHON_BUTTON),0x0);
					SelectState = 1;
					
				}
			}
		break;
		
		
		case DISPLAY_GAME_COUNT:
		
			if(MAIN_GAME_TIMER > 1000)
			{
				GameState = INIT;	
			}	
			
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
	  		CANQueueTxMessage(0x70,248+(0xFF<<8),SpinBig1[SpinSeg1]+(SpinBig2[SpinSeg1]<<8),SpinBig3[SpinSeg1]+(SpinBig1[SpinSeg2]<<8),SpinBig2[SpinSeg2]+(SpinBig3[SpinSeg2]<<8));
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

void OnButtonPressRootGame0(BYTE button)
{
	ButtonHistory[BHPointer] = button;
	BHPointer++;
	BHPointer &= 0x3;
	
	if(HitsSinceLastClear < 255)
	{
		HitsSinceLastClear++;
	}
	
	PlayInternalNodeSound(button,INTERNAL_SOUND_SELECTION,CurrentGameSettings.GameSoundEffectVolume,1,0,CurrentGameSettings.GameBackgroundMusicVolume,1);
	
	if( SelectState == SELECTOR_STATE_LIGHTS_OUT)
	{
		SelectState = SELECTOR_STATE_SPIN;
	}

	MusicWake();
		
}

void OnMasterSwitchPressRootGame0(void)
{
}	


void OnSelectPressRootGame0(BYTE button)
{
	switch(GameState)
	{
		case VOLUME_CHANGE:
		
			VOLUME_CHANGE_TIMER = 0;
			
			switch(button)
			{
				case LIGHT_GRABBER_BUTTON:
				
					if(AudioGlobalVolumeIndex < GLOBAL_VOLUME_INDEX_MAX)
					{
						AudioGlobalVolumeIndex++;
					}
					
					LEDSendVariable(DISPLAY_VOL, AudioGlobalVolumeIndex);
					AudioGlobalVolume = VolumeIndexTable[AudioGlobalVolumeIndex];
					Game0PlayButtonFeebackSound(AudioGlobalVolume, 1);
				
				break;
				
				case MARATHON_BUTTON:
				
					if(AudioGlobalVolumeIndex > GLOBAL_VOLUME_INDEX_MIN)
					{
						AudioGlobalVolumeIndex--;
					}
					
					LEDSendVariable(DISPLAY_VOL, AudioGlobalVolumeIndex);
					AudioGlobalVolume = VolumeIndexTable[AudioGlobalVolumeIndex];
					Game0PlayButtonFeebackSound(AudioGlobalVolume, 1);
				
				break;
				
				default:
				EEStoreVariable(AUDIO_VOLUME_INDEX_LOCATION,  AudioGlobalVolumeIndex);
				ResetToGameSelector();
				break;				
				
				
			}

		break;
		
		case DISPLAY_GAME_COUNT:
			
		ResetToGameSelector();
			
		break;
		
		case MAIN_SELECTOR:
		
		if(CheckVolumeChangeEntrySequence(button) == TRUE) { MoveToVolumeChange(); return; }
		if(CheckGameCountEntrySequence(button) == TRUE) { MoveToGameCountDisplay(); return; }
				
		switch(SelectState)
		{
				case SELECTOR_STATE_LIGHTS_OUT:

				SelectState = SELECTOR_STATE_SPIN;

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
		
		
		
		break;
		
		
		
	}
	
	


}	


void MusicWake()
{
switch(MusicState)
	{
		case MUSIC_IDLE:
		case MUSIC_HEARTBEAT:
			StartMainthemeMusic();
		break;
		
		default:
		break;
	}	
	
}	

void PlaySelectorSound()
{
	PlayInternalNodeSound(0,INTERNAL_SOUND_SELECTION,CurrentGameSettings.GameSoundEffectVolume,1,0,CurrentGameSettings.GameBackgroundMusicVolume,1);
	PlayInternalNodeSound(2,INTERNAL_SOUND_SELECTION,CurrentGameSettings.GameSoundEffectVolume,1,1,CurrentGameSettings.GameBackgroundMusicVolume,1);
	PlayInternalNodeSound(4,INTERNAL_SOUND_SELECTION,CurrentGameSettings.GameSoundEffectVolume,1,2,CurrentGameSettings.GameBackgroundMusicVolume,1);
	PlayInternalNodeSound(6,INTERNAL_SOUND_SELECTION,CurrentGameSettings.GameSoundEffectVolume,1,3,CurrentGameSettings.GameBackgroundMusicVolume,1);
	
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
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,1,0,CountDownLength,CurrentGameSettings.GameSoundEffectVolume,0x00);
	SendNodeNOP();				
	EAudioPlaySound( BACKGROUND_MUSIC_STREAM,COUNTDOWN_WAV);	
}	


void UpdateGameSettings()
{
	
	switch (GameSelected)
	{
		case GAME_LIGHT_GRABBER:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume =			((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			
		break;
		
		case GAME_DOUBLE_DOTS:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume = 		((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
			
		break;
		
		case GAME_SURROUND_SOUND:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0xA0)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume = 		((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
			
		break;
		
		case GAME_RODEO:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0xB0)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume = 		((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
						
		break;
		
		case GAME_MARATHON:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0x75)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume = 		((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
			
		break;
		
		case GAME_ZIGZAG:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0x80)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume = 		((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
					
		break;
		
		case GAME_NINJA:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0x75)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume = 		((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
			
		break;
		
		case GAME_FIRE_FIGHTER:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0x80)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume = 		((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
					
		break;
	
		case GAME_ROOT_GAME0:
		
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0xDF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
		
		break;

		default:
			CurrentGameSettings.GameBackgroundMusicVolume = ((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.GameSoundEffectVolume = 	((WORD)(0xFF)*(WORD)(AudioGlobalVolume))>>8;
			CurrentGameSettings.FinaleMusicVolume =			((WORD)(0xC0)*(WORD)(AudioGlobalVolume))>>8;
			
		break;
	}
	
}	

	
BYTE GetStream(BYTE button)
{
	BYTE RetVal;
	
	switch(button)
	{
		case 0:
		case 1:
			RetVal=	MAINTHEME_L_STREAM;
		break;
		
		case 2:
		case 3:
			RetVal=MAINTHEME_LS_STREAM;
		break;
				
		case 4:
		case 5:
			RetVal=	MAINTHEME_R_STREAM;
		break;
		
		case 6:
		case 7:
			RetVal=	MAINTHEME_RS_STREAM;
		break;
		
	}	
	
	return RetVal;
}		
	
void StartMainthemeMusic()
{
	MusicState = MUSIC_MAIN_THEME;
	MUSIC_TIMER = 0;
	
	AudioNodeEnable(0,MAINTHEME_L_STREAM,MAINTHEME_L_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	AudioNodeEnable(1,MAINTHEME_L_STREAM,MAINTHEME_L_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	
	AudioNodeEnable(2,MAINTHEME_LS_STREAM,MAINTHEME_LS_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	AudioNodeEnable(3,MAINTHEME_LS_STREAM,MAINTHEME_LS_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	
	AudioNodeEnable(4,MAINTHEME_R_STREAM,MAINTHEME_R_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	AudioNodeEnable(5,MAINTHEME_R_STREAM,MAINTHEME_R_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	
	AudioNodeEnable(6,MAINTHEME_RS_STREAM,MAINTHEME_RS_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	AudioNodeEnable(7,MAINTHEME_RS_STREAM,MAINTHEME_RS_STREAM,1,1,0xFFFF,CurrentGameSettings.GameBackgroundMusicVolume,CurrentGameSettings.GameBackgroundMusicVolume);
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
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,1,1,0xFFFF,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,HEARTBEAT_WAV);

	
}

BOOL CheckVolumeChangeEntrySequence(BYTE button)
{
	BOOL RetVal = FALSE;
	

	
	switch(VolumeChangeEntrySequencePosition)
	{
		
		case 0:
			if(button == VolumeChangeEntrySequence[VolumeChangeEntrySequencePosition])
			{
				VolumeChangeEntrySequencePosition++;
				VC_ENTRY_TIMER = 0;
				RetVal = FALSE;
			}
		break;
		
		case 1:
		case 2:
		case 3:
		case 4:
		
			if((button == VolumeChangeEntrySequence[VolumeChangeEntrySequencePosition])  && (VC_ENTRY_TIMER<VC_ENTRY_TIME))
			{
				VolumeChangeEntrySequencePosition++;
				
					if(VolumeChangeEntrySequencePosition == 5)
					{
						RetVal = TRUE;	
					}
					else
					{
						VC_ENTRY_TIMER = 0;
						RetVal = FALSE;
					}
			}
			else
			{
				VolumeChangeEntrySequencePosition = 0;
				VC_ENTRY_TIMER = 0;
				RetVal = FALSE;	
			}
		break;
		
		default:
			VolumeChangeEntrySequencePosition = 0;	
			VC_ENTRY_TIMER = 0;
			RetVal = FALSE;
		break;	
		
		
	}
	
	return RetVal;
	
}		
	
	
BOOL CheckGameCountEntrySequence(BYTE button)
{
	BOOL RetVal = FALSE;
	

	switch(GameCountEntrySequencePosition)
	{
		
		case 0:
			if(button == GameCountEntrySequence[GameCountEntrySequencePosition])
			{
				GameCountEntrySequencePosition++;
				GC_ENTRY_TIMER = 0;
				RetVal = FALSE;
			}
		break;
		
		case 1:
		case 2:
		case 3:
		case 4:
		
			if((button == GameCountEntrySequence[GameCountEntrySequencePosition])  && (GC_ENTRY_TIMER<GC_ENTRY_TIME))
			{
			GameCountEntrySequencePosition++;
				
					if(GameCountEntrySequencePosition == 5)
					{
						RetVal = TRUE;	
					}
					else
					{
						GC_ENTRY_TIMER = 0;
						RetVal = FALSE;
					}
			}
			else
			{
				GameCountEntrySequencePosition = 0;
				GC_ENTRY_TIMER = 0;
				RetVal = FALSE;	
			}
		break;
		
		default:
			GameCountEntrySequencePosition = 0;	
			GC_ENTRY_TIMER = 0;
			RetVal = FALSE;
		break;	
		
		
	}
	
	return RetVal;
	
}		
		
void MoveToVolumeChange()
{
	ResetAudioAndLEDS();
	GameState = VOLUME_CHANGE_INIT;
	MAIN_GAME_TIMER = 0;
}

void MoveToGameCountDisplay()
{

	GameState = DISPLAY_GAME_COUNT;
	DisplayGameCount();
	Game0PlayButtonFeebackSound(AudioGlobalVolume, 1);
	MAIN_GAME_TIMER = 0;
	
}
	
	
void Game0PlayButtonFeebackSound(BYTE Volume, BYTE Repeats)
{

  PlayInternalNodeSound(0xFF,INTERNAL_SOUND_POSITIVE_FEEDBACK,Volume,Repeats,0,0,0);

}	


void EasterEggHunt()
{
	
	if(EASTER_EGG_TIMER > 20)
	{	
			//Look for all the Bottom Row
		
		EASTER_EGG_TIMER = 0;
		
		
		if(HitsSinceLastClear >3)
		{	
			ButtonTrail[0] = ButtonHistory[BHPointer];
			ButtonTrail[1] = ButtonHistory[(BHPointer+1)&0x03];
			ButtonTrail[2] = ButtonHistory[(BHPointer+2)&0x03];
			ButtonTrail[3] = ButtonHistory[(BHPointer+3)&0x03];
				
			
			if( (!(ButtonTrail[3]&0x01)) && (((ButtonTrail[3]-ButtonTrail[2])&0x03)==2)
								 && (((ButtonTrail[2]-ButtonTrail[1])&0x03)==2)
							   	 && (((ButtonTrail[1]-ButtonTrail[0])&0x03)==2))
			{
			
				 
				 QueuedSelectState = SelectState;
				 QueuedEasterEggDisplay = EASTER_EGG_DISPLAY_1;
				 SelectState = DISPLAY_TRAIL;
				 
				 EASTER_EGG_TIMER = 0xFFFF;
				 EasterEggDisplayTimeout = 60;
				 EERepeats=0;
			}					   	 
			
			if( ((ButtonTrail[3]&0x01)) && (((ButtonTrail[3]-ButtonTrail[2])&0x03)==2)
								 && (((ButtonTrail[2]-ButtonTrail[1])&0x03)==2)
							   	 && (((ButtonTrail[1]-ButtonTrail[0])&0x03)==2))
			{
			 
				 QueuedSelectState = SelectState;
			 	 QueuedEasterEggDisplay = EASTER_EGG_DISPLAY_2;
				 SelectState = DISPLAY_TRAIL;
				 EASTER_EGG_TIMER = 0xFFFF;
				 EasterEggDisplayTimeout = 5;
				  EERepeats=0;
			}					   	 
				
			if( 	(((ButtonTrail[3]-ButtonTrail[2])&0x03)==1)
				 && (((ButtonTrail[2]-ButtonTrail[1])&0x03)==1)
				 && (((ButtonTrail[1]-ButtonTrail[0])&0x03)==1))
			{
				
				 QueuedSelectState = SelectState;
			 	 QueuedEasterEggDisplay = EASTER_EGG_DISPLAY_3;
				 SelectState = DISPLAY_TRAIL;
				 
				 EASTER_EGG_TIMER = 0xFFFF;
				 EasterEggDisplayTimeout = 5;
				 EERepeats=0;
				 BHPointer = ButtonTrail[3];
			}	
			
			if( 	(((ButtonTrail[3]-ButtonTrail[2])&0x03)==0x03)
				 && (((ButtonTrail[2]-ButtonTrail[1])&0x03)==0x03)
				 && (((ButtonTrail[1]-ButtonTrail[0])&0x03)==0x03))
			{
				 QueuedSelectState = SelectState;
			 	 QueuedEasterEggDisplay = EASTER_EGG_DISPLAY_3;
				 SelectState = DISPLAY_TRAIL;
				 
				 EASTER_EGG_TIMER = 0xFFFF;
				 EasterEggDisplayTimeout = 5;
				 EERepeats=0;
				 BHPointer = ButtonTrail[3];
				 
			}	
	 }
	
	}
	
}


void ClearButtonHistory()
{
	BYTE i;
	BHPointer = 0;
	HitsSinceLastClear = 0;
	for(i=0;i<4;i++)
	{
		ButtonHistory[i] = 0x81;
	}	

}	

