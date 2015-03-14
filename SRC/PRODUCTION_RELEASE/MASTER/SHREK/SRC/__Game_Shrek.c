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

#define MASTER_VOLUME	0xBF

uint8_t  MyStationID = 4;

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define BACKGROUND_MUSIC_STREAM		        	0x00
#define P1_NODE_SOUND_EFFECT_STREAM	        	0x01
#define P2_NODE_SOUND_EFFECT_STREAM	        	0x02
#define BONUS_STREAM				         	0x03

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define SHREK_INIT									0x01
#define SHREK_STATION_ID							0x20
#define SHREK_START									0x21
#define SHREK_BUTTON_SOUND_BEFORE_TRUMPET_SUCCESS	0x22
#define SHREK_GAME_WAIT_FOR_BUTTON_INCORRECT_SOUND	0x23
#define SHREK_GAME_WAIT_FOR_MORE_CHANCES			0x24

#define SHREK_ATTRACT_DISPLAY				0x02
#define SHREK_ATTRACT_WAIT					0x03
#define SHREK_GENERATE_NEXT_LEVEL			0x04
#define SHREK_WAIT_GET_READY				0x05
#define SHREK_SHOW_PATTERN					0x06
#define SHREK_SHOW_WIN						0x30

#define SHREK_PLAY_GAME						0x07
#define SHREK_MATCH_FAIL					0x08
#define SHREK_MATCH_SUCCESS					0x09
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
#define MAIN_LOOP_RESTART_TIMER			GPTimer[2]	

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
void SendShrekLight(uint8_t Node,uint8_t ShrekColorIndex,int16_t FadeTime);

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
		case 1:
			Idx = STATION1_IDLE_WAV;
			Length = STATION1_IDLE_WAV_LENGTH;
		break;

		case 2:
			Idx = STATION2_IDLE_WAV;
			Length = STATION2_IDLE_WAV_LENGTH;
		break;

		case 3:
			Idx = STATION3_IDLE_WAV;
			Length = STATION3_IDLE_WAV_LENGTH;
		break;

		case 4:
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
		case 1:
			Idx = STATIONID_1_WAV ;
			Length = STATIONID_1_WAV_LENGTH;
		break;

		case 2:
			Idx = STATIONID_2_WAV ;
			Length = STATIONID_2_WAV_LENGTH;
		break;

		case 3:
			Idx = STATIONID_3_WAV ;
			Length = STATIONID_3_WAV_LENGTH;
		break;

		case 4:
			Idx = STATIONID_4_WAV ;
			Length = STATIONID_3_WAV_LENGTH;
		break;

	}

	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	, Idx);

	return Length;
}


uint16_t PlayButtonSound(uint8_t StationId,uint8_t Button)
{ 
     uint16_t Length = 0;
	 uint16_t Idx = 0;

	switch(StationId)
	{
		default:
		case 1:
			switch(Button)
			{
				default:
				case 0:
					Idx = BUTTON1_1_WAV  ;
					Length = BUTTON1_1_WAV_LENGTH;
				break;

				case 1:
					Idx = BUTTON1_2_WAV  ;
					Length = BUTTON1_2_WAV_LENGTH;
				break;

				case 2:
					Idx = BUTTON1_3_WAV  ;
					Length = BUTTON1_3_WAV_LENGTH;
				break;

				case 3:
					Idx = BUTTON1_4_WAV  ;
					Length = BUTTON1_4_WAV_LENGTH;
				break;
			}			
	
		break;

		case 2:
			switch(Button)
			{
				default:
				case 0:
					Idx = BUTTON2_1_WAV  ;
					Length = BUTTON2_1_WAV_LENGTH;
				break;

				case 1:
					Idx = BUTTON2_2_WAV  ;
					Length = BUTTON2_2_WAV_LENGTH;
				break;

				case 2:
					Idx = BUTTON2_3_WAV  ;
					Length = BUTTON2_3_WAV_LENGTH;
				break;

				case 3:
					Idx = BUTTON2_4_WAV  ;
					Length = BUTTON2_4_WAV_LENGTH;
				break;
			}			
		break;

		case 3:
			switch(Button)
			{
				default:
				case 0:
					Idx = BUTTON3_1_WAV  ;
					Length = BUTTON3_1_WAV_LENGTH;
				break;

				case 1:
					Idx = BUTTON3_2_WAV  ;
					Length = BUTTON3_2_WAV_LENGTH;
				break;

				case 2:
					Idx = BUTTON3_3_WAV  ;
					Length = BUTTON3_3_WAV_LENGTH;
				break;

				case 3:
					Idx = BUTTON3_4_WAV  ;
					Length = BUTTON3_4_WAV_LENGTH;
				break;
			}	
		break;

		case 4:
			switch(Button)
			{
				default:
				case 0:
					Idx = BUTTON4_1_WAV  ;
					Length = BUTTON4_1_WAV_LENGTH;
				break;

				case 1:
					Idx = BUTTON4_2_WAV  ;
					Length = BUTTON4_2_WAV_LENGTH;
				break;

				case 2:
					Idx = BUTTON4_3_WAV  ;
					Length = BUTTON4_3_WAV_LENGTH;
				break;

				case 3:
					Idx = BUTTON4_4_WAV  ;
					Length = BUTTON4_4_WAV_LENGTH;
				break;
			}	
		break;

	}

	AudioNodeEnable(Button,Button,Button,AUDIO_ON_BEFORE_TIMEOUT,Length,0x80,0);
	EAudioPlaySound(Button	, Idx);

	return Length;
}

uint16_t PlayMagicMirrorWelcomeTrack()
{
   	 uint16_t Length;
   	 uint16_t Idx;

	switch(rand()%14)
	{
		default:
		case 0:
		Idx =	   		MM_FL_WELCOME_01A_WAV            ;
		Length = 		MM_FL_WELCOME_01A_WAV_LENGTH     ;
		break;
		case 1:
		Idx =	    	MM_FL_WELCOME_01B_WAV            ;
		Length = 		MM_FL_WELCOME_01B_WAV_LENGTH     ;
		break;
		case 2:
		Idx =	    	MM_FL_WELCOME_01C_WAV            ;
		Length = 		MM_FL_WELCOME_01C_WAV_LENGTH     ;
		break;
		case 3:
		Idx =	    	MM_FL_WELCOME_01D_WAV            ;
		Length = 		MM_FL_WELCOME_01D_WAV_LENGTH     ;
		break;
		case 4:
		Idx =	    	MM_FL_WELCOME_02B_WAV            ;
		Length = 		MM_FL_WELCOME_02B_WAV_LENGTH     ;
		break;
		case 5:
		Idx =	    	MM_FL_WELCOME_03A_WAV            ;
		Length = 		MM_FL_WELCOME_03A_WAV_LENGTH     ;
		break;
		case 6:
		Idx =	    	MM_FL_WELCOME_04B_WAV            ;
		Length = 		MM_FL_WELCOME_04B_WAV_LENGTH     ;
		break;
		case 7:
		Idx =	    	MM_FL_WELCOME_04C_WAV            ;
		Length = 		MM_FL_WELCOME_04C_WAV_LENGTH     ;
		break;
		case 8:
		Idx =	    	MM_FL_WELCOME_05A_WAV            ;
		Length = 		MM_FL_WELCOME_05A_WAV_LENGTH     ;
		break;
		case 9:
		Idx =	    	MM_FL_WELCOME_06A_WAV            ;
		Length = 		MM_FL_WELCOME_06A_WAV_LENGTH     ;
		break;
		case 10:
		Idx =	    	MM_FL_WELCOME_07A_WAV            ;
		Length = 		MM_FL_WELCOME_07A_WAV_LENGTH     ;
		break;
		case 11:
		Idx =	    	MM_FL_WELCOME_08B_WAV            ;
		Length = 		MM_FL_WELCOME_08B_WAV_LENGTH     ;
		break;
		case 12:
		Idx =	    	MM_FL_WELCOME_09A_WAV            ;
		Length = 		MM_FL_WELCOME_09A_WAV_LENGTH     ;
		break;
		case 13:
		Idx =	    	MM_FL_WELCOME_09B_WAV            ;
		Length =	    	MM_FL_WELCOME_09B_WAV_LENGTH           ;
		break;
	}
	
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);
	
	return Length;
}

uint16_t PlayGetReady()
{
	uint16_t Length;
	uint16_t Idx;
	
	switch(rand()%38)
	{
		default:
		case 0:
		Idx =		MM_FL_GETREADY_01A_WAV        ;                               
		Length=		MM_FL_GETREADY_01A_WAV_LENGTH ;                               
		break;
		case 1:	
		Idx =		MM_FL_GETREADY_01B_WAV        ;                               
		Length=		MM_FL_GETREADY_01B_WAV_LENGTH ;                               
		break;									  
		case 2:
		Idx =		MM_FL_GETREADY_01C_WAV         ;                              
		Length=		MM_FL_GETREADY_01C_WAV_LENGTH  ;                              
		break;
		case 3:
		Idx =		MM_FL_GETREADY_02A_WAV         ;                              
		Length=		MM_FL_GETREADY_02A_WAV_LENGTH  ;                              
		break;
		case 4:
		Idx =		MM_FL_GETREADY_02B_WAV        ;                               
		Length=		MM_FL_GETREADY_02B_WAV_LENGTH ;                               
		break;
		case 5:
		Idx =		MM_FL_GETREADY_02C_WAV       ;                                
		Length=		MM_FL_GETREADY_02C_WAV_LENGTH;                                
		break;
		case 6:
		Idx =		MM_FL_GETREADY_03A_WAV       ;                               
		Length=		MM_FL_GETREADY_03A_WAV_LENGTH;                                
		break;
		case 7:
		Idx =		MM_FL_GETREADY_03B_WAV       ;                                
		Length=		MM_FL_GETREADY_03B_WAV_LENGTH;                                
		break;
		case 8:
		Idx =		MM_FL_GETREADY_03C_WAV        ;                               
		Length=		MM_FL_GETREADY_03C_WAV_LENGTH ;                               
		break;
		case 9:
		Idx =		MM_FL_GETREADY_04A_WAV        ;                               
		Length=		MM_FL_GETREADY_04A_WAV_LENGTH ;                               
		break;
		case 10:
		Idx =		MM_FL_GETREADY_04B_WAV        ;                               
		Length=		MM_FL_GETREADY_04B_WAV_LENGTH ;                               
		break;
		case 12:
		Idx =		MM_FL_GETREADY_04C_WAV        ;                               
		Length=		MM_FL_GETREADY_04C_WAV_LENGTH ;                               
		break;
		case 13:
		Idx =		MM_FL_GETREADY_05A_WAV        ;                               
		Length=		MM_FL_GETREADY_05A_WAV_LENGTH ;                               
		break;
		case 14:
		Idx =		MM_FL_GETREADY_05B_WAV       ;                                
		Length=		MM_FL_GETREADY_05B_WAV_LENGTH;                                
		break;
		case 15:
		Idx =		MM_FL_GETREADY_05C_WAV        ;                               
		Length=		MM_FL_GETREADY_05C_WAV_LENGTH ;                               
		
		case 16:
		Idx =		MM_FL_GETREADY_06A_WAV        ;                               
		Length=		MM_FL_GETREADY_06A_WAV_LENGTH ;                               
		break;
		case 17:
		Idx =		MM_FL_GETREADY_06B_WAV        ;                               
		Length=		MM_FL_GETREADY_06B_WAV_LENGTH ;                               
		break;
		case 18:
		Idx =		MM_FL_GETREADY_06C_WAV       ;                                
		Length=		MM_FL_GETREADY_06C_WAV_LENGTH;                                
		break;
		case 19:
		Idx =		MM_FL_GETREADY_07A_WAV       ;                                
		Length=		MM_FL_GETREADY_07A_WAV_LENGTH;                                
		break;
		case 20:
		Idx =		MM_FL_GETREADY_07B_WAV       ;                                
		Length=		MM_FL_GETREADY_07B_WAV_LENGTH;                                
		break;
		case 21:
		Idx =		MM_FL_GETREADY_07C_WAV       ;                                
		Length=		MM_FL_GETREADY_07C_WAV_LENGTH;                                
		break;
		case 22:
		Idx =		MM_FL_GETREADY_08A_WAV       ;                                
		Length=		MM_FL_GETREADY_08A_WAV_LENGTH;                                
		break;
		case 23:
		Idx =		MM_FL_GETREADY_08B_WAV       ;                                
		Length=		MM_FL_GETREADY_08B_WAV_LENGTH;                                
		break;
		case 24:
		Idx =		MM_FL_GETREADY_09A_WAV       ;                                
		Length=		MM_FL_GETREADY_09A_WAV_LENGTH;                                
		break;
		case 25:
		Idx =		MM_FL_GETREADY_09B_WAV        ;                               
		Length=		MM_FL_GETREADY_09B_WAV_LENGTH ;                               
		break;
		case 26:
		Idx =		MM_FL_GETREADY_09C_WAV        ;                               
		Length=		MM_FL_GETREADY_09C_WAV_LENGTH ;                               
		break;
		case 27:
		Idx =		MM_FL_GETREADY_10A_WAV       ;                                
		Length=		MM_FL_GETREADY_10A_WAV_LENGTH;                                
		break;
		case 28:
		Idx =		MM_FL_GETREADY_10B_WAV        ;                               
		Length=		MM_FL_GETREADY_10B_WAV_LENGTH ;                               
		break;
		case 29:
		Idx =		MM_FL_GETREADY_10C_WAV       ;                                
		Length=		MM_FL_GETREADY_10C_WAV_LENGTH;                                
		break;
		case 30:
		Idx =		MM_FL_GETREADY_11A_WAV       ;                                
		Length=		MM_FL_GETREADY_11A_WAV_LENGTH;                                
		break;
		case 31:
		Idx =		MM_FL_GETREADY_11B_WAV        ;                               
		Length=		MM_FL_GETREADY_11B_WAV_LENGTH ;                               
		break;
		case 32:
		Idx =		MM_FL_GETREADY_11C_WAV         ;                              
		Length=		MM_FL_GETREADY_11C_WAV_LENGTH  ;                              
		break;
		case 33:
		Idx =		MM_FL_GETREADY_12A_WAV        ;                               
		Length=		MM_FL_GETREADY_12A_WAV_LENGTH ;                               
		break;
		case 34:
		Idx =		MM_FL_GETREADY_12B_WAV        ;                               
		Length=		MM_FL_GETREADY_12B_WAV_LENGTH ;                               
		break;
		case 35:
		Idx =		MM_FL_GETREADY_12C_WAV        ;                               
		Length=		MM_FL_GETREADY_12C_WAV_LENGTH ;                               
		break;
		case 36:
		Idx =		MM_FL_GETREADY_13A_WAV        ;                               
		Length=		MM_FL_GETREADY_13A_WAV_LENGTH ;                               
		break;
		case 37:
		Idx =		MM_FL_GETREADY_13B_WAV        ;                               
		Length=		MM_FL_GETREADY_13B_WAV_LENGTH ;                               
		break;
	}
	
	
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

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

uint16_t PlayTrumpetSuccess()
{
	uint16_t Length;
	uint16_t Idx;
	
	switch(rand()%67)
	{
			default:
		    case 0:                                                     
			Idx =		MM_FL_POSITIVE_01A_WAV                       ;
			Length =		MM_FL_POSITIVE_01A_WAV_LENGTH            ;
			break;
			 case 1:
			Idx =		MM_FL_POSITIVE_01B_WAV                       ;
			Length =		MM_FL_POSITIVE_01B_WAV_LENGTH            ;
			break;
			case 2:                                                        
			Idx =		MM_FL_POSITIVE_01C_WAV                       ;
			Length =		MM_FL_POSITIVE_01C_WAV_LENGTH            ;
			break;
			case 3:                                                       
			Idx =		MM_FL_POSITIVE_02A_WAV                       ;
			Length =		MM_FL_POSITIVE_02A_WAV_LENGTH            ;
			break;
			 case 4:                                                       
			Idx =		MM_FL_POSITIVE_02B_WAV                       ;
			Length =		MM_FL_POSITIVE_02B_WAV_LENGTH            ;
			break;
			 case 5:                                                       
			Idx =		MM_FL_POSITIVE_02C_WAV                       ;
			Length =		MM_FL_POSITIVE_02C_WAV_LENGTH            ;
			break;
			 case 6:                                                        
			Idx =		MM_FL_POSITIVE_03A_WAV                       ;
			Length =		MM_FL_POSITIVE_03A_WAV_LENGTH            ;
			break;
			 case 7:                                                       
			Idx =		MM_FL_POSITIVE_03B_WAV                       ;
			Length =		MM_FL_POSITIVE_03B_WAV_LENGTH            ;
			break;
			   case 8:                                                      
			Idx =		MM_FL_POSITIVE_03C_WAV                       ;
			Length =		MM_FL_POSITIVE_03C_WAV_LENGTH            ;
			break;
			 case 9:                                                        
			Idx =		MM_FL_POSITIVE_04A_WAV                       ;
			Length =		MM_FL_POSITIVE_04A_WAV_LENGTH            ;
			break;
			 case 10:                                                       
			Idx =		MM_FL_POSITIVE_04B_WAV                       ;
			Length =		MM_FL_POSITIVE_04B_WAV_LENGTH            ;
			break;
			 case 11:                                                       
			Idx =		MM_FL_POSITIVE_04C_WAV                       ;
			Length =		MM_FL_POSITIVE_04C_WAV_LENGTH            ;
			break;
			case 12:                                                         
			Idx =		MM_FL_POSITIVE_05A_WAV                       ;
			Length =		MM_FL_POSITIVE_05A_WAV_LENGTH            ;
			break;
			case 13:                                                         
			Idx =		MM_FL_POSITIVE_05B_WAV                       ;
			Length =		MM_FL_POSITIVE_05B_WAV_LENGTH            ;
			break;
			 case 15:                                                        
			Idx =		MM_FL_POSITIVE_05C_WAV                       ;
			Length =		MM_FL_POSITIVE_05C_WAV_LENGTH            ;
			break;
			case 16:                                                        
			Idx =		MM_FL_POSITIVE_05D_WAV                       ;
			Length =		MM_FL_POSITIVE_05D_WAV_LENGTH            ;
			break;
			 case 17:                                                       
			Idx =		MM_FL_POSITIVE_06A_WAV                       ;
			Length =		MM_FL_POSITIVE_06A_WAV_LENGTH            ;
			break;
			 case 18:                                                       
			Idx =		MM_FL_POSITIVE_06B_WAV                       ;
			Length =		MM_FL_POSITIVE_06B_WAV_LENGTH            ;
			break;
			case 19:                                                         
			Idx =		MM_FL_POSITIVE_06C_WAV                       ;
			Length =		MM_FL_POSITIVE_06C_WAV_LENGTH            ;
			break;
			 case 20:                                                       
			Idx =		MM_FL_POSITIVE_07A_WAV                       ;
			Length =		MM_FL_POSITIVE_07A_WAV_LENGTH            ;
			break;
			case 21:                                                        
			Idx =		MM_FL_POSITIVE_07B_WAV                       ;
			Length =		MM_FL_POSITIVE_07B_WAV_LENGTH            ;
			break;
			 case 22:                                                       
			Idx =		MM_FL_POSITIVE_07C_WAV                       ;
			Length =		MM_FL_POSITIVE_07C_WAV_LENGTH            ;
			break;
			case 23:                                                        
			Idx =		MM_FL_POSITIVE_08A_WAV                       ;
			Length =		MM_FL_POSITIVE_08A_WAV_LENGTH            ;
			break;
			case 24:                                                        
			Idx =		MM_FL_POSITIVE_08B_WAV                       ;
			Length =		MM_FL_POSITIVE_08B_WAV_LENGTH            ;
			break;
			 case 25:                                                        
			Idx =		MM_FL_POSITIVE_08C_WAV                       ;
			Length =		MM_FL_POSITIVE_08C_WAV_LENGTH            ;
			break;
			 case 26:                                                       
			Idx =		MM_FL_POSITIVE_09A_WAV                       ;
			Length =		MM_FL_POSITIVE_09A_WAV_LENGTH            ;
			break;
			case 27:                                                     
			Idx =		MM_FL_POSITIVE_09B_WAV                       ;
			Length =		MM_FL_POSITIVE_09B_WAV_LENGTH            ;
			break;
			 case 28:                                                       
			Idx =		MM_FL_POSITIVE_10A_WAV                       ;
			Length =		MM_FL_POSITIVE_10A_WAV_LENGTH            ;
			break;
			case 29:                                                        
			Idx =		MM_FL_POSITIVE_10B_WAV                       ;
			Length =		MM_FL_POSITIVE_10B_WAV_LENGTH            ;
			break;
			case 30:                                                         
			Idx =		MM_FL_POSITIVE_10C_WAV                       ;
			Length =		MM_FL_POSITIVE_10C_WAV_LENGTH            ;
			break;
			case 31:                                                        
			Idx =		MM_FL_POSITIVE_11A_WAV                       ;
			Length =		MM_FL_POSITIVE_11A_WAV_LENGTH            ;
			break;
			 case 32:                                                       
			Idx =		MM_FL_POSITIVE_11B_WAV                       ;
			Length =		MM_FL_POSITIVE_11B_WAV_LENGTH            ;
			break;
			 case 33:                                                       
			Idx =		MM_FL_POSITIVE_11C_WAV                       ;
			Length =		MM_FL_POSITIVE_11C_WAV_LENGTH            ;
			break;
			  case 34:                                                       
			Idx =		MM_FL_POSITIVE_12A_WAV                       ;
			Length =		MM_FL_POSITIVE_12A_WAV_LENGTH            ;
			break;
			case 35:                                                        
			Idx =		MM_FL_POSITIVE_12B_WAV                       ;
			Length =		MM_FL_POSITIVE_12B_WAV_LENGTH            ;
			break;
			case 36:                                                        
			Idx =		MM_FL_POSITIVE_12C_WAV                       ;
			Length =		MM_FL_POSITIVE_12C_WAV_LENGTH            ;
			break;
			case 37:                                                        
			Idx =		MM_FL_POSITIVE_13A_WAV                       ;
			Length =		MM_FL_POSITIVE_13A_WAV_LENGTH            ;
			break;
			 case 38:                                                       
			Idx =		MM_FL_POSITIVE_13B_WAV                       ;
			Length =		MM_FL_POSITIVE_13B_WAV_LENGTH            ;
			break;
			case 39:                                                         
			Idx =		MM_FL_POSITIVE_13C_WAV                       ;
			Length =		MM_FL_POSITIVE_13C_WAV_LENGTH            ;
			break;
			case 40:                                                        
			Idx =		MM_FL_POSITIVE_14A_WAV                       ;
			Length =		MM_FL_POSITIVE_14A_WAV_LENGTH            ;
			break;
			case 41:                                                        
			Idx =		MM_FL_POSITIVE_14B_WAV                       ;
			Length =		MM_FL_POSITIVE_14B_WAV_LENGTH            ;
			break;
			case 42:                                                        
			Idx =		MM_FL_POSITIVE_14C_WAV                       ;
			Length =		MM_FL_POSITIVE_14C_WAV_LENGTH            ;
			break;
			 case 43:                                                        
			Idx =		MM_FL_POSITIVE_15A_WAV                       ;
			Length =		MM_FL_POSITIVE_15A_WAV_LENGTH            ;
			break;
			case 44:                                                        
			Idx =		MM_FL_POSITIVE_15B_WAV                       ;
			Length =		MM_FL_POSITIVE_15B_WAV_LENGTH            ;
			break;
			case 45:                                                        
			Idx =		MM_FL_POSITIVE_15C_WAV                       ;
			Length =		MM_FL_POSITIVE_15C_WAV_LENGTH            ;
			break;
			 case 46:                                                       
			Idx =		MM_FL_POSITIVE_16A_WAV                       ;
			Length =		MM_FL_POSITIVE_16A_WAV_LENGTH            ;
			break;
			case 47:                                                         
			Idx =		MM_FL_POSITIVE_16B_WAV                       ;
			Length =		MM_FL_POSITIVE_16B_WAV_LENGTH            ;
			break;
			 case 48:                                                        
			Idx =		MM_FL_POSITIVE_16C_WAV                       ;
			Length =		MM_FL_POSITIVE_16C_WAV_LENGTH            ;
			break;
			case 49:                                                         
			Idx =		MM_FL_POSITIVE_17A_WAV                       ;
			Length =		MM_FL_POSITIVE_17A_WAV_LENGTH            ;
			break;
			case 50:                                                        
			Idx =		MM_FL_POSITIVE_17B_WAV                       ;
			Length =		MM_FL_POSITIVE_17B_WAV_LENGTH            ;
			break;
			case 51:                                                         
			Idx =		MM_FL_POSITIVE_17C_WAV                       ;
			Length =		MM_FL_POSITIVE_17C_WAV_LENGTH            ;
			break;
			case 52: 
			Idx = MM_FL_POSITIVE_18A_WAV            ;
			Length =		MM_FL_POSITIVE_18A_WAV_LENGTH            ;
			break;
			 case 53:                                                         
			Idx =		MM_FL_POSITIVE_18B_WAV                       ;
			Length =		MM_FL_POSITIVE_18B_WAV_LENGTH            ;
			break;
			case 54:                                                         
			Idx =		MM_FL_POSITIVE_18C_WAV                       ;
			Length =		MM_FL_POSITIVE_18C_WAV_LENGTH            ;
			break;
			case 55:                                                          
			Idx =		MM_FL_POSITIVE_19A_WAV                       ;
			Length =		MM_FL_POSITIVE_19A_WAV_LENGTH            ;
			break;
			 case 56:                                                        
			Idx =		MM_FL_POSITIVE_19B_WAV                       ;
			Length =		MM_FL_POSITIVE_19B_WAV_LENGTH            ;
			break;
			case 57:                                                          
			Idx =		MM_FL_POSITIVE_19C_WAV                       ;
			Length =		MM_FL_POSITIVE_19C_WAV_LENGTH            ;
			break;
			case 58:                                                          
			Idx =		MM_FL_POSITIVE_20A_WAV                       ;
			Length =		MM_FL_POSITIVE_20A_WAV_LENGTH            ;
			break;
			case 59:                                                          
			Idx =		MM_FL_POSITIVE_20B_WAV                       ;
			Length =		MM_FL_POSITIVE_20B_WAV_LENGTH            ;
			break;
			 case 60:                                                        
			Idx =		MM_FL_POSITIVE_20C_WAV                       ;
			Length =		MM_FL_POSITIVE_20C_WAV_LENGTH            ;
			break;
			 case 61:                                                        
			Idx =		MM_FL_POSITIVE_21A_WAV                       ;
			Length =		MM_FL_POSITIVE_21A_WAV_LENGTH            ;
			break;
			 case 62:                                                         
			Idx =		MM_FL_POSITIVE_21B_WAV                       ;
			Length =		MM_FL_POSITIVE_21B_WAV_LENGTH            ;
			break;
			 case 63:                                                         
			Idx =		MM_FL_POSITIVE_22A_WAV                       ;
			Length =		MM_FL_POSITIVE_22A_WAV_LENGTH            ;
			break;
             case 64:                                                         
			Idx =		MM_FL_POSITIVE_22B_WAV                       ;
			Length =		MM_FL_POSITIVE_22B_WAV_LENGTH            ;
			break;
             case 65:                                                         
			Idx =		MM_FL_POSITIVE_22C_WAV                       ;
			Length =		MM_FL_POSITIVE_22C_WAV_LENGTH            ;
			break;
			
              case 66:                                                        
			Idx =		MM_FL_POSITIVE_23A_WAV                       ;
			Length =		MM_FL_POSITIVE_23A_WAV_LENGTH            ;
			break;
	}
	
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

	return Length;
}

uint16_t PlayButtonIncorrect()
{
	uint16_t Length =BELCH_WAV_LENGTH;
	uint16_t Idx = BELCH_WAV;
	
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

	return Length;
}

uint16_t PlayTwoMoreChances()
{
	uint16_t Length;
	uint16_t Idx;
	
	switch(rand()%24)
	{
		default:
		case 0:
		Idx=			MM_FL_TWOMORE_01A_WAV                                         ;
		Length=			MM_FL_TWOMORE_01A_WAV_LENGTH                                  ;
		break;
		case 1:
		Idx=			MM_FL_TWOMORE_01B_WAV                                         ;
		Length=			MM_FL_TWOMORE_01B_WAV_LENGTH                                  ;
		break;
		case 2:
		Idx=			MM_FL_TWOMORE_01C_WAV                                         ;
		Length=			MM_FL_TWOMORE_01C_WAV_LENGTH                                  ;
		break;
		case 3:
		Idx=			MM_FL_TWOMORE_02A_WAV                                         ;
		Length=			MM_FL_TWOMORE_02A_WAV_LENGTH                                  ;
		break;
		case 4:
		Idx=			MM_FL_TWOMORE_02B_WAV                                         ;
		Length=			MM_FL_TWOMORE_02B_WAV_LENGTH                                  ;
		break;
		case 5:
		Idx=			MM_FL_TWOMORE_02C_WAV                                         ;
		Length=			MM_FL_TWOMORE_02C_WAV_LENGTH                                  ;
		break;
		case 6:
		Idx=			MM_FL_TWOMORE_03A_WAV                                         ;
		Length=			MM_FL_TWOMORE_03A_WAV_LENGTH                                  ;
		break;
		case 7:
		Idx=			MM_FL_TWOMORE_03B_WAV                                         ;
		Length=			MM_FL_TWOMORE_03B_WAV_LENGTH                                  ;
		break;
		case 8:
		Idx=			MM_FL_TWOMORE_03C_WAV                                         ;
		Length=			MM_FL_TWOMORE_03C_WAV_LENGTH                                  ;
		break;
		case 9:
		Idx=			MM_FL_TWOMORE_04A_WAV                                         ;
		Length=			MM_FL_TWOMORE_04A_WAV_LENGTH                                  ;
		break;
		case 10:
		Idx=			MM_FL_TWOMORE_04B_WAV                                         ;
		Length=			MM_FL_TWOMORE_04B_WAV_LENGTH                                  ;
		break;
		case 11:
		Idx=			MM_FL_TWOMORE_04C_WAV                                         ;
		Length=			MM_FL_TWOMORE_04C_WAV_LENGTH                                  ;
		break;
		case 12:
		Idx=			MM_FL_TWOMORE_05A_WAV                                         ;
		Length=			MM_FL_TWOMORE_05A_WAV_LENGTH                                  ;
		break;
		case 13:
		Idx=			MM_FL_TWOMORE_05B_WAV                                         ;
		Length=			MM_FL_TWOMORE_05B_WAV_LENGTH                                  ;
		break;
		case 14:
		Idx=			MM_FL_TWOMORE_05C_WAV                                         ;
		Length=			MM_FL_TWOMORE_05C_WAV_LENGTH                                  ;
		break;
		case 15:
		Idx=			MM_FL_TWOMORE_06A_WAV                                         ;
		Length=			MM_FL_TWOMORE_06A_WAV_LENGTH                                  ;
		break;
		case 16:
		Idx=			MM_FL_TWOMORE_06B_WAV                                         ;
		Length=			MM_FL_TWOMORE_06B_WAV_LENGTH                                  ;
		break;
		case 17:
		Idx=			MM_FL_TWOMORE_06C_WAV                                         ;
		Length=			MM_FL_TWOMORE_06C_WAV_LENGTH                                  ;
		break;
		case 18:
		Idx=			MM_FL_TWOMORE_07A_WAV                                         ;
		Length=			MM_FL_TWOMORE_07A_WAV_LENGTH                                  ;
		break;
		case 19:
		Idx=			MM_FL_TWOMORE_07B_WAV                                         ;
		Length=			MM_FL_TWOMORE_07B_WAV_LENGTH                                  ;
		break;
		case 20:
		Idx=			MM_FL_TWOMORE_07C_WAV                                         ;
		Length=			MM_FL_TWOMORE_07C_WAV_LENGTH                                  ;
		break;
		case 21:
		Idx=			MM_FL_TWOMORE_08A_WAV                                         ;
		Length=			MM_FL_TWOMORE_08A_WAV_LENGTH                                  ;
		break;
		case 22:
		Idx=			MM_FL_TWOMORE_08B_WAV                                         ;
		Length=			MM_FL_TWOMORE_08B_WAV_LENGTH                                  ;
		break;
		case 23:
		Idx=			MM_FL_TWOMORE_08C_WAV                                         ;
		Length=			MM_FL_TWOMORE_08C_WAV_LENGTH                                  ;
		break;
   }
	

	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

	return Length;
}


uint16_t PlayOneMoreChance()
{
	uint16_t Length;
	uint16_t Idx;
	
	
	switch(rand()%24)
{
		default:
		case 0:
		Idx=		MM_FL_ONEMORE_01A_WAV                                         ;
		Length=		MM_FL_ONEMORE_01A_WAV_LENGTH                                  ;
		break;
		case 1:
		Idx=			MM_FL_ONEMORE_01B_WAV                                         ;
		Length=			MM_FL_ONEMORE_01B_WAV_LENGTH                                  ;
		break;
		case 2:
		Idx=			MM_FL_ONEMORE_01C_WAV                                         ;
		Length=			MM_FL_ONEMORE_01C_WAV_LENGTH                                  ;
		break;
		case 3:
		Idx=			MM_FL_ONEMORE_02A_WAV                                         ;
		Length=			MM_FL_ONEMORE_02A_WAV_LENGTH                                  ;
		break;
		case 4:
		Idx=			MM_FL_ONEMORE_02B_WAV                                         ;
		Length=			MM_FL_ONEMORE_02B_WAV_LENGTH                                  ;
		break;
		case 5:
		Idx=			MM_FL_ONEMORE_02C_WAV                                         ;
		Length=			MM_FL_ONEMORE_02C_WAV_LENGTH                                  ;
		break;
		case 6:
		Idx=			MM_FL_ONEMORE_03A_WAV                                         ;
		Length=			MM_FL_ONEMORE_03A_WAV_LENGTH                                  ;
		break;
		case 7:
		Idx=			MM_FL_ONEMORE_03B_WAV                                         ;
		Length=			MM_FL_ONEMORE_03B_WAV_LENGTH                                  ;
		break;
		case 8:
		Idx=			MM_FL_ONEMORE_03C_WAV                                         ;
		Length=			MM_FL_ONEMORE_03C_WAV_LENGTH                                  ;
		break;
		case 9:
		Idx=			MM_FL_ONEMORE_04A_WAV                                         ;
		Length=			MM_FL_ONEMORE_04A_WAV_LENGTH                                  ;
		break;
		case 10:
		Idx=			MM_FL_ONEMORE_04B_WAV                                         ;
		Length=			MM_FL_ONEMORE_04B_WAV_LENGTH                                  ;
		break;
		case 11:
		Idx=			MM_FL_ONEMORE_04C_WAV                                         ;
		Length=			MM_FL_ONEMORE_04C_WAV_LENGTH                                  ;
		break;
		case 12:
		Idx=			MM_FL_ONEMORE_05A_WAV                                         ;
		Length=			MM_FL_ONEMORE_05A_WAV_LENGTH                                  ;
		break;
		case 13:
		Idx=			MM_FL_ONEMORE_05B_WAV                                         ;
		Length=			MM_FL_ONEMORE_05B_WAV_LENGTH                                  ;
		break;
		case 14:
		Idx=			MM_FL_ONEMORE_05C_WAV                                         ;
		Length=			MM_FL_ONEMORE_05C_WAV_LENGTH                                  ;
		break;
		case 15:
		Idx=			MM_FL_ONEMORE_06A_WAV                                         ;
		Length=			MM_FL_ONEMORE_06A_WAV_LENGTH                                  ;
		break;
		case 16:
		Idx=			MM_FL_ONEMORE_06B_WAV                                         ;
		Length=			MM_FL_ONEMORE_06B_WAV_LENGTH                                  ;
		break;
		case 17:
		Idx=			MM_FL_ONEMORE_06C_WAV                                         ;
		Length=			MM_FL_ONEMORE_06C_WAV_LENGTH                                  ;
		break;
		case 18:
		Idx=			MM_FL_ONEMORE_07A_WAV                                         ;
		Length=			MM_FL_ONEMORE_07A_WAV_LENGTH                                  ;
		break;
		case 19:
		Idx=			MM_FL_ONEMORE_07B_WAV                                         ;
		Length=			MM_FL_ONEMORE_07B_WAV_LENGTH                                  ;
		break;
		case 20:
		Idx=			MM_FL_ONEMORE_07C_WAV                                         ;
		Length=			MM_FL_ONEMORE_07C_WAV_LENGTH                                  ;
		break;
		case 21:
		Idx=			MM_FL_ONEMORE_08A_WAV                                         ;
		Length=			MM_FL_ONEMORE_08A_WAV_LENGTH                                  ;
		break;
		case 22:
		Idx=			MM_FL_ONEMORE_08B_WAV                                         ;
		Length=			MM_FL_ONEMORE_08B_WAV_LENGTH                                  ;
		break;
		case 23:
		Idx=			MM_FL_ONEMORE_08C_WAV                                         ;
		Length=			MM_FL_ONEMORE_08C_WAV_LENGTH                                  ;
		break;


}
	
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

	return Length;
}



uint16_t PlaySendoff()
{
	uint16_t Length ;
	uint16_t Idx;
	
	switch(rand()%23)
	{
		default:
		case 0:
		Idx=		MM_FL_SENDOFF_01A_WAV                                         ;
		Length=		MM_FL_SENDOFF_01A_WAV_LENGTH                                  ;
		break;
		case 1:
		Idx=		MM_FL_SENDOFF_01B_WAV                                         ;
		Length=		MM_FL_SENDOFF_01B_WAV_LENGTH                                  ;
		break;
		case 2:
		Idx=		MM_FL_SENDOFF_01C_WAV                                         ;
		Length=		MM_FL_SENDOFF_01C_WAV_LENGTH                                  ;
		break;
		case 3:
		Idx=		MM_FL_SENDOFF_02A_WAV                                         ;
		Length=		MM_FL_SENDOFF_02A_WAV_LENGTH                                  ;
		break;
		case 4:
		Idx=		MM_FL_SENDOFF_02B_WAV                                         ;
		Length=		MM_FL_SENDOFF_02B_WAV_LENGTH                                  ;
		break;
		case 5:
		Idx=		MM_FL_SENDOFF_02C_WAV                                         ;
		Length=		MM_FL_SENDOFF_02C_WAV_LENGTH                                  ;
		break;
		case 6:
		Idx=		MM_FL_SENDOFF_03A_WAV                                         ;
		Length=		MM_FL_SENDOFF_03A_WAV_LENGTH                                  ;
		break;
		case 7:
		Idx=		MM_FL_SENDOFF_03B_WAV                                         ;
		Length=		MM_FL_SENDOFF_03B_WAV_LENGTH                                  ;
		break;
		case 8:
		Idx=		MM_FL_SENDOFF_03C_WAV                                         ;
		Length=		MM_FL_SENDOFF_03C_WAV_LENGTH                                  ;
		break;
		case 9:
		Idx=		MM_FL_SENDOFF_04A_WAV                                         ;
		Length=		MM_FL_SENDOFF_04A_WAV_LENGTH                                  ;
		break;
		case 10:
		Idx=		MM_FL_SENDOFF_04B_WAV                                         ;
		Length=		MM_FL_SENDOFF_04B_WAV_LENGTH                                  ;
		break;
		case 11:
		Idx=		MM_FL_SENDOFF_04C_WAV                                         ;
		Length=		MM_FL_SENDOFF_04C_WAV_LENGTH                                  ;
		break;
		case 12:
		Idx=		MM_FL_SENDOFF_05A_WAV                                         ;
		Length=		MM_FL_SENDOFF_05A_WAV_LENGTH                                  ;
		break;
		case 13:
		Idx=		MM_FL_SENDOFF_05B_WAV                                         ;
		Length=		MM_FL_SENDOFF_05B_WAV_LENGTH                                  ;
		break;
		case 14:
		Idx=		MM_FL_SENDOFF_05C_WAV                                         ;
		Length=		MM_FL_SENDOFF_05C_WAV_LENGTH                                  ;
		break;
		case 15:
		Idx=		MM_FL_SENDOFF_05D_WAV                                         ;
		Length=		MM_FL_SENDOFF_05D_WAV_LENGTH                                  ;
		break;
		case 16:
		Idx=		MM_FL_SENDOFF_06A_WAV                                         ;
		Length=		MM_FL_SENDOFF_06A_WAV_LENGTH                                  ;
		break;
		case 17:
		Idx=		MM_FL_SENDOFF_06B_WAV                                         ;
		Length=		MM_FL_SENDOFF_06B_WAV_LENGTH                                  ;
		break;
		case 18:
		Idx=		MM_FL_SENDOFF_06C_WAV                                         ;
		Length=		MM_FL_SENDOFF_06C_WAV_LENGTH                                  ;
		break;
		case 19:
		Idx=		MM_FL_SENDOFF_07A_WAV                                         ;
		Length=		MM_FL_SENDOFF_07A_WAV_LENGTH                                  ;
		break;
		case 20:
		Idx=		MM_FL_SENDOFF_07B_WAV                                         ;
		Length=		MM_FL_SENDOFF_07B_WAV_LENGTH                                  ;
		break;
		case 21:
		Idx=		MM_FL_SENDOFF_07C_WAV                                         ;
		Length=		MM_FL_SENDOFF_07C_WAV_LENGTH                                  ;
		break;
		case 22:
		Idx=		MM_FL_SENDOFF_07D_WAV                                         ;
		Length=		MM_FL_SENDOFF_07D_WAV_LENGTH                                  ;
		break;
	
	}
	
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

	return Length;
}
	

uint16_t PlayWin()
{
	uint16_t Length = WIN_WAV_LENGTH;
	uint16_t Idx = WIN_WAV;
	
	AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM,BACKGROUND_MUSIC_STREAM,AUDIO_ON_BEFORE_TIMEOUT,Length ,MASTER_VOLUME,0);
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM	,Idx);

	return Length;
}


uint8_t IdleLoopsLeft;;

void ShrekStart()
{
	CurrentTimeout = PlayIdleSound(MyStationID);
	RegenerateShrekLightSequence(rand()%MAX_SEQUENCE_LENGTH);
	GameState = SHREK_ATTRACT_DISPLAY;
	CurrentTimeout = PlayIdleSound(MyStationID);
	MAIN_LOOP_RESTART_TIMER	= 0;
IdleLoopsLeft = 4;
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
			if(MAIN_GAME_TIMER == 100)
			{
				GameState = SHREK_STATION_ID;
				CurrentTimeout = PlayStationID(MyStationID) + 200;
			}
		
		break;

		case SHREK_STATION_ID:

			if(MAIN_GAME_TIMER > CurrentTimeout)
			{
					ShrekStart();
			}
		
		break;

	
		case SHREK_ATTRACT_DISPLAY:
	
			if(MAIN_LOOP_RESTART_TIMER> CurrentTimeout)
			{
				if(IdleLoopsLeft>0)
				{
					IdleLoopsLeft--;
					CurrentTimeout = PlayIdleSound(MyStationID);
					MAIN_LOOP_RESTART_TIMER = 0;
				}
				else
				{
						MAIN_LOOP_RESTART_TIMER = 0;
				}

			}

			if(MAIN_GAME_TIMER>35)
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
					IncorrectButtonPress = 0;
					MAIN_GAME_TIMER = 0;
					GameState = SHREK_WAIT_GET_READY;
					LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
					CurrentTimeout = PlayGetReady();
					MAIN_GAME_TIMER = 0;
			}

		break;


		case  SHREK_WAIT_GET_READY:
			
			if(MAIN_GAME_TIMER>CurrentTimeout)
			{
				GameState = SHREK_GENERATE_NEXT_LEVEL;
				MAIN_GAME_TIMER = 0;
					
			}
	
		break;

		case	SHREK_GENERATE_NEXT_LEVEL:

			CurrentLightSequenceLength++;
			
			if(CurrentLightSequenceLength == 2)
				CurrentLightSequenceLength = 3;   //The want to skip #2

			if(CurrentLightSequenceLength>MAX_SEQUENCE_LENGTH)
			{
			   	 GameState = SHREK_GAME_WAIT_BEFORE_RESTART;
				 LEDSendMessage(ENABLE_ALL,GREEN,LEDOFF,255,255);
				 MAIN_GAME_TIMER = 0;
				 CurrentTimeout = PlayWin();
			}
			else
			{
				RegenerateShrekLightSequence(CurrentLightSequenceLength);
				 GameState = SHREK_SHOW_PATTERN;
				 MAIN_GAME_TIMER = 0;
				 CurrentLightSequenceIdx = 0;
				 CurrentTimeout = 100;
			}
		break;

		case SHREK_BUTTON_SOUND_BEFORE_TRUMPET_SUCCESS:

			if(MAIN_GAME_TIMER > CurrentTimeout)
			{
						GameState = SHREK_TRUMPET_SUCCESS;
						CurrentTimeout = PlayTrumpetSuccess();
						MAIN_GAME_TIMER = 0;
			}

			break;

    	case	SHREK_TRUMPET_SUCCESS:
			if(MAIN_GAME_TIMER > CurrentTimeout)
			{
				MAIN_GAME_TIMER = 0;
				GameState = SHREK_GENERATE_NEXT_LEVEL;
			}

			if(EFFECT_TIMER>10)
			{
				EFFECT_TIMER = 0;
				Effect++;
				Effect&=0x03;
				SendShrekLight(Effect,RANDOM_COLOR_INDEX,30);
			}
		break;

	


		case SHREK_SHOW_PATTERN:

			if(MAIN_GAME_TIMER>CurrentTimeout)
			{
				MAIN_GAME_TIMER = 0;
				if(CurrentLightSequenceIdx == CurrentLightSequenceLength)
				{
					
					GameState = SHREK_PLAY_GAME;
					CurrentLightSequenceIdx = 0;
					MAIN_GAME_TIMER = 0;
				}
				else
				{

				    PlayButtonSound(MyStationID,LightSequence[CurrentLightSequenceIdx]);

					CurrentTimeout = 100;

					SendShrekLight(LightSequence[CurrentLightSequenceIdx],
								   ColorSequence[CurrentLightSequenceIdx],
								   CurrentTimeout);
					CurrentLightSequenceIdx++;
				}
			}

		break;

		case SHREK_GAME_WAIT_FOR_BUTTON_INCORRECT_SOUND:
		
			if(MAIN_GAME_TIMER>CurrentTimeout)
				{
				
					GameState = SHREK_GAME_WAIT_FOR_MORE_CHANCES;
					if(	IncorrectButtonPress == 1)
					{
						CurrentTimeout = PlayTwoMoreChances();
					}
					else
					{
						CurrentTimeout = PlayOneMoreChance();
					}
	
					MAIN_GAME_TIMER = 0;
				}

		break;

		case SHREK_GAME_WAIT_FOR_MORE_CHANCES:
	
			if(MAIN_GAME_TIMER>CurrentTimeout)
				{
			 	  	LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
					 
					//scott requested that we generate a pattern at the current level.  We
					//need to subtract from the current length as the state will add one before regenerating

					CurrentLightSequenceLength--;
					GameState = SHREK_GENERATE_NEXT_LEVEL;
					MAIN_GAME_TIMER = 0;
					CurrentLightSequenceIdx = 0;
					CurrentTimeout = 25;
				}
		
		break;

		case SHREK_PLAY_GAME:

			if(MAIN_GAME_TIMER >1000)
			{
				PlayFailed();
				MAIN_GAME_TIMER = 0;
				LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
				ShrekStart();
			}

		break;

		case SHREK_GAME_WAIT_BEFORE_RESTART:
		
			if(MAIN_GAME_TIMER >1000)
			{
				ShrekStart();
				LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,0);
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
	case SHREK_ATTRACT_WAIT:
	case SHREK_ATTRACT_DISPLAY:
	
		LEDSendMessage(ENABLE_ALL,GREEN,GREEN,0,0);
		GameState = SHREK_WAIT_WELCOME;
		CurrentLightSequenceLength = 0; //Reset so we start of with 1 light
	    CurrentTimeout = PlayMagicMirrorWelcomeTrack() + 100;
		MAIN_GAME_TIMER = 0;

	break;


	case SHREK_PLAY_GAME:

		if(button<4)
		{
			if(button == LightSequence[CurrentLightSequenceIdx])
			{
			
				CurrentTimeout = PlayButtonSound(MyStationID,LightSequence[CurrentLightSequenceIdx]);

				SendShrekLight(LightSequence[CurrentLightSequenceIdx],
								   ColorSequence[CurrentLightSequenceIdx],
								   100);
				
				CurrentLightSequenceIdx++;
			
				if(CurrentLightSequenceIdx >= CurrentLightSequenceLength)
				{
					MAIN_GAME_TIMER = 0;
					GameState = SHREK_BUTTON_SOUND_BEFORE_TRUMPET_SUCCESS;
				}
			}
			else
			{

				CurrentLightSequenceIdx = 0;
			
				IncorrectButtonPress++;
	
				if(IncorrectButtonPress == 3)
				{
					CurrentTimeout = PlaySendoff();
					MAIN_GAME_TIMER = 0;
					LEDSendMessage(ENABLE_ALL,RED,LEDOFF,	250,250);
					GameState = SHREK_GAME_WAIT_BEFORE_RESTART;
				}
				else
				{
					MAIN_GAME_TIMER = 0;
					CurrentTimeout = PlayButtonIncorrect();
					GameState = SHREK_GAME_WAIT_FOR_BUTTON_INCORRECT_SOUND;
					LEDSendMessage(ENABLE_ALL,RED,RED,	0,0);
				}
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

void SendShrekLight(uint8_t Node,uint8_t ShrekColorIndex,int16_t FadeTime)
{

	if(FadeTime>254) FadeTime = 254;

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







