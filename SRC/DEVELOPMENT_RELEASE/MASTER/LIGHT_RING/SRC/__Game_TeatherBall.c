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

#define HIT_SCORE_ONE_PLAYER 25
#define ONE_SPACE_SCORE		25
#define TWO_SPACE_SCORE		20
#define THREE_SPACE_SCORE	15
#define FOUR_SPACE_SCORE	10
#define DEFAULT_SCORE		5
#define SERVE_SCORE_TWO_PLAYER 25
#define TWO_REVOLUTION_SCORE_DECREMENT 5
#define THREE_REVOLUTION_SCORE_DECREMENT 10
#define BALL_SPEED_CIELING	((DWORD)(0x1C00))	
#define BALL_SPEED_FLOOR	((DWORD)(0x0800))
#define BALL_SPEED_DECELERATION_FACTOR ((DWORD)(0xFB00)) //0.16 Fraction
#define ESCAPE_FACTOR 	0x400
#define SERVE_FLASH_TIME	25
#define SERVE_AUDIO_TIME	200
#define TEATHER_BALL_GAME_LENGTH 		ZIGZAGBACKGROUNDMUSIC_A_WAV_LENGTH
#define LED_FADE_IN_TIME	 10
#define LED_FADE_OUT_TIME 	 10

//*************************************************
//******** AUDIO STREAM MAPPING *******************
//*************************************************

#define BACKGROUND_MUSIC_STREAM_A			0x00
#define BACKGROUND_MUSIC_STREAM_B			0x01

#define BOUNCE_SOUND_EFFECT_STREAM			0x02
#define HIT_SOUND_EFFECT_STREAM				0x03

//*************************************************
//******** System Timer MACROS*********************
//*************************************************

#define MAIN_GAME_TIMER		 GPTimer[0]
#define PHYSICS_UPDATE_TIMER GPTimer[1]
#define SERVE_FLASH_TIMER	 GPTimer[2]
#define SERVE_AUDIO_TIMER	 GPTimer[3]

//*************************************************
//******** GAME STATE MACROS*** *******************
//*************************************************

#define TEATHER_BALL_1P_START	0x01
#define TEATHER_BALL_SERVE		0x02
#define TEATHER_BALL			0x03

#define TEATHER_BALL_END			0x05

//*************************************************
//*******Game Variables****************************
//*************************************************

static BYTE BallDirection;
static DWORD BallPosition;  //8.16 Fractional Position
static DWORD BallSpeed;	//0.16 Fractional Speed
static BYTE LightPosition;
static BYTE OldLightPosition;
static BYTE LastButtonHit;
static BYTE CurrentBounceSound = 0;
static BYTE BallOwner = PLAYER_1;
//Used for ball "escape"  when it is being hit really quick
static BYTE BallCaptures = 0;
//used for counting how many times the ball has bounced in between hits
static WORD RevolutionTicks=0;

BYTE Player1ServeLocation;
			
	
BYTE Player2ServeLocation;


//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void PlayButtonFeebackSound(BYTE button);
void TeatherBallPlayBounceSound(BYTE button);
void TeatherBallPlayHitSound(BYTE button);
void FlashServe();
void  PlayServeSound(BYTE P1Button);
void UpdateBallLight(BYTE OldPosition, BYTE NewPosition);
void TeatherBallPlayP2HitSound(BYTE button);
void TeatherBallPlayP1HitSound(BYTE button);
void InitTeatherBallStereoStreams();
BYTE GetTeatherBallBackgroundStream(BYTE button);

//*************************************************
//*******Game Functions****************************
//*************************************************


void TeatherBall(void)
{
			
	switch (GameState)
	{
		
		case INIT:
		
			MAIN_GAME_TIMER = 0;
			
			GameState = TEATHER_BALL_SERVE;
				BallDirection = SelectRandomDirection();
				LightPosition = RandomButton(NO_EXCLUDE,NO_EXCLUDE);
				
				
				Player1ServeLocation = LightPosition;
				Player2ServeLocation = Player1ServeLocation + (NUM_BUTTONS/2);
	 			 if(Player2ServeLocation > (NUM_BUTTONS-1))
				 {
					Player2ServeLocation -= NUM_BUTTONS;	 
				 }
										
				BallPosition = ((DWORD)(LightPosition))<<16;
				BallOwner = PLAYER_1;
				BallSpeed = BALL_SPEED_CIELING;
				PHYSICS_UPDATE_TIMER = 0;
			 	MAIN_GAME_TIMER = 0;
				SERVE_FLASH_TIMER = 0;
				Player1Score = 0;
				BallCaptures = 0;
				LastButtonHit = 0xFF;
				RevolutionTicks=0;
  			    BallCaptures = 0;
				CurrentBounceSound = 0;
				
				InitTeatherBallStereoStreams();
				
								
				SERVE_FLASH_TIMER = 0;
				SERVE_AUDIO_TIMER = SERVE_AUDIO_TIME+1;
				
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_NORMAL;
				ScoreManagerEnabled = TRUE;
	
			
		break;
		
		case TEATHER_BALL_SERVE:
			
		
			if(MAIN_GAME_TIMER>TEATHER_BALL_GAME_LENGTH)
			{
				GameState = TEATHER_BALL;
			}
		
		
			if(SERVE_FLASH_TIMER > SERVE_FLASH_TIME)
			{
				SERVE_FLASH_TIMER = 0;
				FlashServe(BallPosition>>16);
			}
		
			if(SERVE_AUDIO_TIMER > SERVE_AUDIO_TIME)
			{
				SERVE_AUDIO_TIMER = 0;
				PlayServeSound(BallPosition>>16);
			}
		
		
		
		
		break;
		
		
		case TEATHER_BALL:
			
		if(MAIN_GAME_TIMER>TEATHER_BALL_GAME_LENGTH)
		{
			GameState = TEATHER_BALL_END;
			MAIN_GAME_TIMER = 0;
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM_A,BACKGROUND_MUSIC_STREAM_A,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM_A,ENDING_WAV);		
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
		
		if(PHYSICS_UPDATE_TIMER > 1)
		{
			PHYSICS_UPDATE_TIMER = 0;
		
			if(BallSpeed > BALL_SPEED_FLOOR)
			{
				BallSpeed = (BallSpeed * BALL_SPEED_DECELERATION_FACTOR)>>16;
			}
			else
			{
				BallSpeed = BALL_SPEED_FLOOR;		
			}
						
			OldLightPosition = LightPosition;
			
			if(BallDirection == RIGHT)
			{	
				BallPosition += BallSpeed;
				
		
				if(BallPosition > 0x0005FFFF)
				{
					BallPosition =   BallPosition - 0x60000 ;	
				}
			}
			else
			{
				BallPosition -= BallSpeed;	
				
				BallPosition &=  0x0007FFFF;
				if(BallPosition > 0x0005FFFF)
				{
					BallPosition =  BallPosition - 0x20000;	
				}
				
				
			}
			
		
		
			LightPosition = BallPosition>>16;
			
			if(LightPosition != OldLightPosition)
			{
				UpdateBallLight(OldLightPosition,LightPosition);
		
				CurrentBounceSound = LightPosition;		
				TeatherBallPlayBounceSound(LightPosition);
			
			
				if(BallDirection)
				{
					
					if(CurrentBounceSound == (NUM_BUTTONS-1))
					{
						CurrentBounceSound = 0;	
					}
					else
					{
						CurrentBounceSound++;	
					}
				}
				else
				{
					if(CurrentBounceSound == 0)
					{
						CurrentBounceSound = (NUM_BUTTONS-1);	
					}
					else
					{
						CurrentBounceSound--;
					}
			    }
			
	
				//Only update if the position has changed one whole tick so we don't flood the CAN bus)	
				RevolutionTicks++;
			}
			
		}
		
		break;	
		
	
	
		
		case TEATHER_BALL_END:
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


void OnButtonPressTeatherBall(unsigned char button)
{

switch(GameState)
{
	
	case TEATHER_BALL_SERVE:
	
		if(button == Player1ServeLocation)
		{
			LightPosition = Player1ServeLocation;
			BallPosition = ((DWORD)(LightPosition))<<16;
			
			BallOwner = PLAYER_2;
			GameState = TEATHER_BALL;
			TeatherBallPlayP1HitSound(button);
		}
		else if (button == Player2ServeLocation)
		{
			LightPosition = Player2ServeLocation;
			BallPosition = ((DWORD)(LightPosition))<<16;
			
			BallOwner = PLAYER_1;
			GameState = TEATHER_BALL;
			TeatherBallPlayP2HitSound(button);
		}
	
	break;

	case TEATHER_BALL:
		if(button == LightPosition)
		{
				
			if((LastButtonHit != 0xFF) && (RevolutionTicks < 4))
			{
				switch(RevolutionTicks)
				{
					
					case 1:
						BallSpeed = BALL_SPEED_CIELING + (BallCaptures*ESCAPE_FACTOR);
						BallCaptures++;
					break;
					
					case 2:
						BallSpeed = BALL_SPEED_CIELING + (BallCaptures*(ESCAPE_FACTOR>>1));
						BallCaptures++;
					break;
					
					case 3:
						BallSpeed = BALL_SPEED_CIELING + (BallCaptures*(ESCAPE_FACTOR>>2));
						BallCaptures++;
					break;
					
					default:
						BallSpeed = BALL_SPEED_CIELING;	
						BallCaptures=0;	
					break;
					
					}
					
			}
			else
			
			{
				BallCaptures=0;	
				BallSpeed = BALL_SPEED_CIELING;
			}
			
			BallDirection++;
			BallDirection &= 0x01;
			CurrentBounceSound = 0;
			
			if(BallOwner == PLAYER_1)
			{
				TeatherBallPlayP2HitSound(button);
				
				BallOwner = PLAYER_2;
				
				switch(RevolutionTicks)
				{
					case 1:
						Player1Score += ONE_SPACE_SCORE;
					break;
						
					case 2:
						Player1Score += TWO_SPACE_SCORE;
					break;
					
					case 3:
						Player1Score += THREE_SPACE_SCORE;
					break;
					
					case 4:
						Player1Score += FOUR_SPACE_SCORE;
					break;
					
					default:
						Player1Score += DEFAULT_SCORE;
					break;
				}
			}
			else
			{
				BallOwner = PLAYER_1;
			    TeatherBallPlayP1HitSound(button);
				
				switch(RevolutionTicks)
				{
					case 1:
						Player2Score += ONE_SPACE_SCORE;
					break;
						
					case 2:
						Player2Score += TWO_SPACE_SCORE;
					break;
					
					case 3:
						Player2Score += THREE_SPACE_SCORE;
					break;
					
					case 4:
						Player2Score += FOUR_SPACE_SCORE;
					break;
					
					default:
						Player2Score += DEFAULT_SCORE;
					break;
				}
			}
			
			LastButtonHit = button;
			RevolutionTicks = 0;	
	}
	break;	
	
	

	default:
	break;	
}	

	
}

void FlashServe()
{
	LEDSendMessage(Player1ServeLocation,RED,RED,SERVE_FLASH_TIME>>1,0);
 	LEDSendMessage(Player2ServeLocation,GREEN,GREEN,SERVE_FLASH_TIME>>1,0);
}	 


void UpdateBallLight(BYTE OldPosition, BYTE NewPosition)
{
	if(BallOwner == PLAYER_1)
	{
		
		LEDSendMessage(OldPosition,RED,LEDOFF,0,LED_FADE_OUT_TIME);
		LEDSendMessage(NewPosition,LEDOFF,RED,0,LED_FADE_IN_TIME);
		
	}
	else
	{
		LEDSendMessage(OldPosition,GREEN,LEDOFF,0,LED_FADE_OUT_TIME);
		LEDSendMessage(NewPosition,LEDOFF,GREEN,0,LED_FADE_IN_TIME);
		
	}
}




void TeatherBallPlayP1HitSound(BYTE button)
{
	
	BYTE Stream;
	Stream = GetTeatherBallBackgroundStream(button);
	
	AudioNodeEnable(button,HIT_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX1_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(HIT_SOUND_EFFECT_STREAM,ZIGZAGSFX1_WAV);
}

void TeatherBallPlayP2HitSound(BYTE button)
{
	
	BYTE Stream;
	Stream = GetTeatherBallBackgroundStream(button);
	
	AudioNodeEnable(button,HIT_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX10_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(HIT_SOUND_EFFECT_STREAM,ZIGZAGSFX10_WAV);
}


void  PlayServeSound(BYTE button)
{	
	
	BYTE Stream;
	Stream = GetTeatherBallBackgroundStream(button);
	
	
	AudioNodeEnable(button,HIT_SOUND_EFFECT_STREAM,Stream,1,1, BALL_SERVE_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable((button+4)&0x7,HIT_SOUND_EFFECT_STREAM,Stream,1,1, BALL_SERVE_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	EAudioPlaySound(HIT_SOUND_EFFECT_STREAM,BALL_SERVE_WAV);
	
}	



void TeatherBallPlayBounceSound(BYTE button)
{
	BYTE Stream;
	Stream = GetTeatherBallBackgroundStream(button);
	
	switch(CurrentBounceSound)
	{
		case 0:
			AudioNodeEnable(button,BOUNCE_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX2_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BOUNCE_SOUND_EFFECT_STREAM,ZIGZAGSFX2_WAV);
		break;
		case 1:
			AudioNodeEnable(button,BOUNCE_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX3_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BOUNCE_SOUND_EFFECT_STREAM,ZIGZAGSFX3_WAV);
		break;
		case 2:
			AudioNodeEnable(button,BOUNCE_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX4_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BOUNCE_SOUND_EFFECT_STREAM,ZIGZAGSFX4_WAV);
		break;
		case 3:
			AudioNodeEnable(button,BOUNCE_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX5_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BOUNCE_SOUND_EFFECT_STREAM,ZIGZAGSFX5_WAV);
		break;
		case 4:
			AudioNodeEnable(button,BOUNCE_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX6_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BOUNCE_SOUND_EFFECT_STREAM,ZIGZAGSFX6_WAV);
		break;
		case 5:
			AudioNodeEnable(button,BOUNCE_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX7_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BOUNCE_SOUND_EFFECT_STREAM,ZIGZAGSFX7_WAV);
		break;
		case 6:
			AudioNodeEnable(button,BOUNCE_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX8_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BOUNCE_SOUND_EFFECT_STREAM,ZIGZAGSFX8_WAV);
		break;
		case 7:
			AudioNodeEnable(button,BOUNCE_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX9_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
			SendNodeNOP();	
			EAudioPlaySound(BOUNCE_SOUND_EFFECT_STREAM,ZIGZAGSFX9_WAV);
		break;
		
		default:
		break;
		
	}

}


BYTE GetTeatherBallBackgroundStream(BYTE button)
{
	BYTE Stream;
	
	switch(button)
		{
			case 1:
			case 3:
			case 5:
			case 7:
			default:
				Stream = BACKGROUND_MUSIC_STREAM_A;
		 			
			break;
					
			case 2:
			case 4:
			case 6:		
			case 8:
			 Stream = 	BACKGROUND_MUSIC_STREAM_B;
		
			
			break;
		}
	
	return Stream;
	
}	


void InitTeatherBallStereoStreams()
{
 BYTE k,Stream;
 
 for(k=0;k<NUM_BUTTONS;k++)
 {
	Stream = GetTeatherBallBackgroundStream(k);
	AudioNodeEnable(k,Stream,Stream,AUDIO_ON_BEFORE_TIMEOUT,ZIGZAGBACKGROUNDMUSIC_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
 	AudioReSync(k);
 }
 	
 	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM_A,ZIGZAGBACKGROUNDMUSIC_A_WAV );
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM_B,ZIGZAGBACKGROUNDMUSIC_B_WAV);	
	
}	

