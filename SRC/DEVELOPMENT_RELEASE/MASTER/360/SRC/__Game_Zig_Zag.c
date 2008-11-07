#include "__Game_Zig_Zag.h"
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
#define ESCAPE_FACTOR 	0x200
#define SERVE_FLASH_TIME	25
#define SERVE_AUDIO_TIME	200
#define ZIG_ZAG_GAME_LENGTH 		ZIGZAGBACKGROUNDMUSIC_A_WAV_LENGTH
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

#define ZIG_ZAG_1P_START	0x01
#define ZIG_ZAG_SERVE		0x02
#define ZIG_ZAG_1P			0x03
#define ZIG_ZAG_2P			0x04
#define ZIG_ZAG_END			0x05

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



//*************************************************
//*******Game Helper Function Declarations*********
//*************************************************

void PlayButtonFeebackSound(BYTE button);
void ZigZagPlayBounceSound(BYTE button);
void ZigZagPlayHitSound(BYTE button);
void FlashServe(BYTE P1Location);
void  PlayServeSound(BYTE P1Button);
void UpdateP1BallLight(BYTE OldPosition, BYTE NewPosition);
void UpdateP1P2BallLight(BYTE OldPosition, BYTE NewPosition);
void ZigZagPlayP1HitSound(BYTE button);
void ZigZagPlayP2HitSound(BYTE button);
void ScoreDecrement2P();

void InitZigZagStereoStreams();
BYTE GetZigZagBackgroundStream(BYTE button);

//*************************************************
//*******Game Functions****************************
//*************************************************


void ZigZag(void)
{
			
	switch (GameState)
	{
		
		case INIT:
			ResetAudioAndLEDS();
			
			MAIN_GAME_TIMER = 0;
			
			if(GamePlayers == 1)
			{
				GameState = ZIG_ZAG_SERVE;
				BallDirection = SelectRandomDirection();
				LightPosition = RandomButton(NO_EXCLUDE,NO_EXCLUDE, NO_EXCLUDE);
				
					//Make sure light is on the bottom
				if(LightPosition &0x01)
				{
					LightPosition++;
					LightPosition &= 0x7;
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
				
				InitZigZagStereoStreams();
				
								
				SERVE_FLASH_TIMER = 0;
				SERVE_AUDIO_TIMER =SERVE_AUDIO_TIME+1;
				
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_BLANK;
				ScoreManagerEnabled = TRUE;
			}
			else
			{
				GameState = ZIG_ZAG_SERVE;
				BallDirection = SelectRandomDirection();
				LightPosition = RandomButton(NO_EXCLUDE,NO_EXCLUDE, NO_EXCLUDE);
				
				//Make sure light is on the bottom
				if(LightPosition &0x01)
				{
					LightPosition++;
					LightPosition &= 0x7;
				}
				
				BallPosition = (DWORD)(LightPosition)<<16;
				BallSpeed = BALL_SPEED_CIELING;
				PHYSICS_UPDATE_TIMER = 0;
			 	MAIN_GAME_TIMER = 0;
				SERVE_FLASH_TIMER = 0;
				SERVE_AUDIO_TIMER =SERVE_AUDIO_TIME+1;
				Player2Score = 0;
				Player1Score = 0;
				BallCaptures = 0;
				LastButtonHit = 0xFF;
				RevolutionTicks=0;
  			     
				InitZigZagStereoStreams();
				
				
				P1ScoreDisplayState = SCORE_NORMAL;
				P2ScoreDisplayState = SCORE_NORMAL;
				ScoreManagerEnabled = TRUE;
			}
			
			
		break;
		
		case ZIG_ZAG_SERVE:
			
		
			if(MAIN_GAME_TIMER>ZIG_ZAG_GAME_LENGTH)
			{
				if(GamePlayers == 1)
				{
					GameState = ZIG_ZAG_1P;
				}
				else
				{
					GameState = ZIG_ZAG_2P;
				}
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
		
		
		case ZIG_ZAG_1P:
			
		if(MAIN_GAME_TIMER>ZIG_ZAG_GAME_LENGTH)
		{
			GameState = ZIG_ZAG_END;
			MAIN_GAME_TIMER = 0;
			AudioNodeEnable(ENABLE_ALL,BACKGROUND_MUSIC_STREAM_A,BACKGROUND_MUSIC_STREAM_A,AUDIO_ON_BEFORE_TIMEOUT,ENDING_WAV_LENGTH,CurrentGameSettings.FinaleMusicVolume,0);
			SendNodeNOP();	
			EAudioPlaySound(BACKGROUND_MUSIC_STREAM_A,ENDING_WAV);		
			LEDSendMessage(ENABLE_ALL,LEDOFF,LEDOFF,0,50);
			
				P1ScoreDisplayState = SCORE_FLASHING;
				P2ScoreDisplayState = SCORE_BLANK;
			
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
			}
			else
			{
				BallPosition -= BallSpeed;	
			}
			
			BallPosition &= 0x0007FFFF;
		
			LightPosition = BallPosition>>16;
			
			if(LightPosition != OldLightPosition)
			{
				UpdateP1BallLight(OldLightPosition,LightPosition);
		
				CurrentBounceSound = LightPosition;		
				ZigZagPlayBounceSound(LightPosition);
			
			
				if(BallDirection)
				{
					CurrentBounceSound++;
					CurrentBounceSound&= 0x7;	
				}
				else
				{
					CurrentBounceSound--;
					CurrentBounceSound&= 0x7;
			    }
			
		//		CurrentBounceSound++;
			
		//		if(CurrentBounceSound > 7)
		//		{
		//			CurrentBounceSound =0;	
		//		}
				//Only update if the position has changed one whole tick so we don't flood the CAN bus)	
				RevolutionTicks++;
			}
			
		}
		
		break;	
		
		case ZIG_ZAG_2P:
			
		if(MAIN_GAME_TIMER>ZIG_ZAG_GAME_LENGTH)
		{
			GameState = ZIG_ZAG_END;
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
			}
			else
			{
				BallPosition -= BallSpeed;	
			}
			
			BallPosition &= 0x0007FFFF;
		
			LightPosition = BallPosition>>16;
			
			//Only update if the position has changed one whole tick so we don't flood the CAN bus)
			if(LightPosition != OldLightPosition)
			{
				UpdateP1P2BallLight(OldLightPosition,LightPosition);
				ZigZagPlayBounceSound(LightPosition);
				
				if(BallOwner == PLAYER_1)
				{
					CurrentBounceSound++;
					CurrentBounceSound&= 0x7;	
				}
				else
				{
					CurrentBounceSound--;
					CurrentBounceSound&= 0x7;
			    }
				
				ScoreDecrement2P();
			 }
			
		}
		
		break;	
	
	
		
		case ZIG_ZAG_END:
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


void ScoreDecrement2P()
{
			
				RevolutionTicks++;
				
				
				if(RevolutionTicks > 15)
				{
					if(RevolutionTicks< 22)
					{
						if(BallOwner == PLAYER_1) 
						{
						 	if((Player1Score>0) && (Player1Score>TWO_REVOLUTION_SCORE_DECREMENT))
							{ 
								Player1Score-=TWO_REVOLUTION_SCORE_DECREMENT;
							}
							else
							{
								Player1Score = 0;
							}
						 				 
						if	(BallOwner == PLAYER_2) 
						{ 
							if((Player2Score>0) && (Player2Score>TWO_REVOLUTION_SCORE_DECREMENT))
							{ 
								Player2Score-=TWO_REVOLUTION_SCORE_DECREMENT;
							}
							else
							{
								Player2Score = 0;
							}
						}
					}
					else
					{
						if(BallOwner == PLAYER_1)
						 {
						 	 if((Player1Score>0)  && (Player1Score>THREE_REVOLUTION_SCORE_DECREMENT))
						 	 {
						    	Player1Score-=THREE_REVOLUTION_SCORE_DECREMENT;
						   	 }
						     else
						  	 {
						    	Player1Score = 0;	
						  	 }
						 }
						 if(BallOwner == PLAYER_2)
						  {
						 	  if((Player2Score>0)  && (Player2Score>THREE_REVOLUTION_SCORE_DECREMENT))
						   		{
						    		Player2Score-=THREE_REVOLUTION_SCORE_DECREMENT;
						  		}
						  	   else
						  		{
								    Player2Score = 0;	
							 	}
						  }
					}
				
				}
			}

	
}	

void OnButtonPressZigZag(unsigned char button)
{

switch(GameState)
{
	
	case ZIG_ZAG_SERVE:
	
	if(GamePlayers == 2)
	{
	    if(button == (BallPosition>>16))
		{
			Player1Score += SERVE_SCORE_TWO_PLAYER;
			BallOwner = PLAYER_2;
			GameState = ZIG_ZAG_2P;
	    	ZigZagPlayP1HitSound(button);	
		}
		else if(button == (((BallPosition>>16)+4)&0x7))
		{
			BallPosition = ((DWORD)(button)<<16);
			Player2Score += SERVE_SCORE_TWO_PLAYER;
			BallOwner = PLAYER_1;
			GameState = ZIG_ZAG_2P;
		       ZigZagPlayP2HitSound(button);	
	
		}
		}
	else
		{
			GameState = ZIG_ZAG_1P;
			ZigZagPlayP1HitSound(button);
		}
	
	break;

	case ZIG_ZAG_1P:
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
			
			
			
			LastButtonHit = button;
			
			RevolutionTicks = 0;
			
			ZigZagPlayP1HitSound(button);
			BallDirection++;
			BallDirection &= 0x01;
	
			Player1Score += HIT_SCORE_ONE_PLAYER;
		
		}
	break;	
	
	   case ZIG_ZAG_2P:
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
				ZigZagPlayP2HitSound(button);
				
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
			    ZigZagPlayP1HitSound(button);
				
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

void FlashServe(BYTE P1Location)
{
	 LEDSendMessage(P1Location,RED,RED,SERVE_FLASH_TIME>>1,0);
	 if(GamePlayers == 2)
	 {
	 	LEDSendMessage((P1Location+4)&0x7,GREEN,GREEN,SERVE_FLASH_TIME>>1,0);
	}
}	 


void UpdateP1BallLight(BYTE OldPosition, BYTE NewPosition)
{
	LEDSendMessage(OldPosition,RED,LEDOFF,0,LED_FADE_OUT_TIME);
		LEDSendMessage(NewPosition,LEDOFF,RED,0,LED_FADE_IN_TIME);
}


void UpdateP1P2BallLight(BYTE OldPosition, BYTE NewPosition)
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


void ZigZagPlayP1HitSound(BYTE button)
{
	
	BYTE Stream;
	Stream = GetZigZagBackgroundStream(button);
	
	AudioNodeEnable(button,HIT_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX1_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(HIT_SOUND_EFFECT_STREAM,ZIGZAGSFX1_WAV);
}

void ZigZagPlayP2HitSound(BYTE button)
{
	BYTE Stream;
	Stream = GetZigZagBackgroundStream(button);
	
	AudioNodeEnable(button,HIT_SOUND_EFFECT_STREAM,Stream,1,1, ZIGZAGSFX10_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	SendNodeNOP();	
	EAudioPlaySound(HIT_SOUND_EFFECT_STREAM,ZIGZAGSFX10_WAV);
}


void  PlayServeSound(BYTE button)
{	
	
	BYTE Stream;
	Stream = GetZigZagBackgroundStream(button);
	
	
	AudioNodeEnable(button,HIT_SOUND_EFFECT_STREAM,Stream,1,1, BALL_SERVE_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	AudioNodeEnable((button+4)&0x7,HIT_SOUND_EFFECT_STREAM,Stream,1,1, BALL_SERVE_WAV_LENGTH,CurrentGameSettings.GameSoundEffectVolume,CurrentGameSettings.GameBackgroundMusicVolume);
	EAudioPlaySound(HIT_SOUND_EFFECT_STREAM,BALL_SERVE_WAV);
	
}	



void ZigZagPlayBounceSound(BYTE button)
{
	BYTE Stream;
	Stream = GetZigZagBackgroundStream(button);
	
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


BYTE GetZigZagBackgroundStream(BYTE button)
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


void InitZigZagStereoStreams()
{
 BYTE k,Stream;
 
 for(k=0;k<8;k++)
 {
	Stream = GetZigZagBackgroundStream(k);
	AudioNodeEnable(k,Stream,Stream,AUDIO_ON_BEFORE_TIMEOUT,ZIGZAGBACKGROUNDMUSIC_A_WAV_LENGTH,CurrentGameSettings.GameBackgroundMusicVolume,0);
 	AudioReSync(k);
 }
 	
 	SendNodeNOP();	
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM_A,ZIGZAGBACKGROUNDMUSIC_A_WAV );
	EAudioPlaySound(BACKGROUND_MUSIC_STREAM_B,ZIGZAGBACKGROUNDMUSIC_B_WAV);	
	
}


