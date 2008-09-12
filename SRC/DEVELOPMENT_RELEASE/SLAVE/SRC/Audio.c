#include <p30fxxxx.h>
#include <dci.h>
#include "Audio.h"
#include "SystemVariables.h"
#include "CANRoutines.h"
#include "TimerRoutines.h"
#include "DataTypes.h"
#include "InternalSounds.h"

BYTE AudioStreamTransmitFlag[4];  //Set to 1 after every 128 words are sent to codec 
BYTE AudioStreamCounter[4]={0,0,0,0,};		// Counts to 3F in interrupt
//Audio buffer contains 128 upper & 128 lower words
//While one half is being played the other half is being written by CAN data
SIGNED_BYTE AudioBuffer[256];	/* 256 samples at 16000 hz = 16ms of audio */
WORD AudioWriteIndex = 0;		/* Addr to write incoming data */
WORD AudioReadIndex = 0x80;		/* Addr to read stuff out to codec */

//BYTE AudioReadIndexResetStream=0;	// Stream # to look for to reset index
BYTE AudioReadIndexReset=0;		// Indicates a stream reset is needed

//Start read index 1/2 buffer out of phase with the stream counter
BYTE AudioOn = 0;	/* 0 for no sound, 1 for sound */
BYTE MyAudioStream=0;	//Stream 0,1,2 or 3 that THIS NODE is playing
BYTE MyAudioVolume=0;

BYTE AudioTimeoutVolume;	//Volume AFTER timeout
BYTE AudioTimeoutStatus;	//on/off status after timeout
BYTE AudioTimeoutStream;	//Stream AFTER timeout

WORD AudioTimer = 0;
WORD AudioTimeout = 0xFFFF;

DWORD AudioStreamAddress[4];	//moving addresses for the 4 current sounds
DWORD AudioStreamStartAddress[4];	//Starting addresses for the 4 current sounds
DWORD AudioStreamLength[4];		//and their lengths

const BYTE *InternalSoundPtr;
WORD InternalSoundLength;
WORD InternalSoundPosition;
BYTE AudioMode = STREAMING_AUDIO;
BYTE InternalSoundVolume = 0xFF;
BYTE InteralSoundRepeats = 0;

void ResetAudio()
{
	AudioOn=0;			// Sound off
	AudioTimer=0;
	AudioTimeout=0xFFFF;
	MyAudioVolume=0xFF;
	MyAudioStream=0;
	AudioMode = STREAMING_AUDIO;
}	

void AudioTimeoutCheck (void)
{
	if ((AudioTimeout != 0xFFFF) && (AudioTimer >= AudioTimeout) )
	{
		AudioOn=AudioTimeoutStatus;
		MyAudioVolume=AudioTimeoutVolume;
		MyAudioStream=AudioTimeoutStream;
		AudioReadIndexReset=1;		// RE-SYNCHRONIZE
		AudioTimeout=0xFFFF;		//Disable timeout
	}
}





void DCIInit(void)
{
	_DCIIE = 1;		//Clear the DCI Interrupt Flag    
	ConfigIntDCI(DCI_INT_PRI_7 & DCI_INT_ON);
	OpenDCI(
		DCI_EN & DCI_IDLE_STOP & DCI_DIGI_LPBACK_DIS & DCI_SCKD_OUP &
		DCI_SAMP_CLK_FAL & DCI_FSD_OUP & DCI_TX_ZERO_UNF & DCI_SDO_ZERO &
		DCI_DJST_OFF &DCI_FSM_I2S,
		DCI_BUFF_LEN_4 & DCI_FRAME_LEN_2 & 	DCI_DATA_WORD_16,
		7,
		DCI_EN_SLOT_ALL & DCI_DIS_SLOT_15 & DCI_DIS_SLOT_14 & DCI_DIS_SLOT_13 & DCI_DIS_SLOT_12 &
		DCI_DIS_SLOT_11 & DCI_DIS_SLOT_10 & DCI_DIS_SLOT_9 & DCI_DIS_SLOT_8 &
		DCI_DIS_SLOT_7 & DCI_DIS_SLOT_6 & DCI_DIS_SLOT_5 & DCI_DIS_SLOT_4 &
		DCI_DIS_SLOT_3 & DCI_DIS_SLOT_2 & DCI_DIS_SLOT_1,
		DCI_DIS_SLOT_ALL);
}



//DCI interrupt is hit 8000 times per second and queues up 2 16 bit words
//for the audio codec. It makes duplicates for the left channel */
//4.5uS measured duration 6/21/06  125us apart = 4% CPU usage
//void __attribute__((__interrupt__,__auto_psv__)) _DCIInterrupt(void)

 
void __attribute__((__interrupt__,__auto_psv__)) _DCIInterrupt(void)
{



 WORD DCIIntTemp=0x0000;

// Must read in the RX data even though there is none 
	DCIIntTemp = RXBUF0;
	DCIIntTemp = RXBUF1;
	DCIIntTemp = RXBUF2;
	DCIIntTemp = RXBUF3;


	switch(AudioMode)
	{
		
		case STREAMING_AUDIO:
			if ( (AudioOn) && (AudioReadIndexReset==0) )
			{
				DCIIntTemp=((WORD)AudioBuffer[AudioReadIndex]);
				DCIIntTemp *= (MyAudioVolume+1);
		
			}
			else
				DCIIntTemp=(int)0;
		 break;
		 
		case INTERNAL_AUDIO:
			DCIIntTemp = (WORD)(*InternalSoundPtr);
		
			DCIIntTemp *= (InternalSoundVolume+1);
	
			InternalSoundPtr++;
			InternalSoundPosition++;
			
			if(InternalSoundPosition == InternalSoundLength)
			{
				InteralSoundRepeats--;
				
					if(InteralSoundRepeats == 0)
					{
						AudioMode = STREAMING_AUDIO;
						AudioOn=AudioTimeoutStatus;
						MyAudioVolume=AudioTimeoutVolume;
						MyAudioStream=AudioTimeoutStream;		
					}
					else
					{
							InternalSoundPtr -= InternalSoundLength;
							InternalSoundPosition = 0;
					}
				
			}
		
		break;
		
		default:
			DCIIntTemp = (int)0;
		break;
	}
	
	TXBUF0 = DCIIntTemp;			//Right channel
	TXBUF1 = DCIIntTemp;			//Left channel							// Left Channel/
	TXBUF2 = DCIIntTemp;			//Right Channel
	TXBUF3 = DCIIntTemp;			// Left channel							// Left Channel


	AudioReadIndex+=1;
	AudioReadIndex &= 0xFF;	// Limit value to 255 

	AudioStreamCounter[0] += 1;		// Count 2 more words sent in each stream 
	AudioStreamCounter[1] += 1;
	AudioStreamCounter[2] += 1;
	AudioStreamCounter[3] += 1;

	if ((AudioStreamCounter[0] & 0x3F)==0) AudioStreamTransmitFlag[0]=1+(AudioStreamCounter[0]>>6);	//every 64 passes (128 words sent to codec)
	if ((AudioStreamCounter[1] & 0x3F)==0) AudioStreamTransmitFlag[1]=1+(AudioStreamCounter[1]>>6);	//every 64 passes (128 words sent to codec)
	if ((AudioStreamCounter[2] & 0x3F)==0) AudioStreamTransmitFlag[2]=1+(AudioStreamCounter[2]>>6);	//every 64 passes (128 words sent to codec)
	if ((AudioStreamCounter[3] & 0x3F)==0) AudioStreamTransmitFlag[3]=1+(AudioStreamCounter[3]>>6);	//every 64 passes (128 words sent to codec)

	_DCIIF = 0;		// Clear the flag so it ain't stuck! 
}


#define INTERNAL_SOUND_POSITIVE_FEEDBACK	0
#define INTERNAL_SOUND_SELECTION			1
#define INTERNAL_SOUND_FF_WATER_HIT			2
#define INTERNAL_SOUND_FF_WATER_MISS		3
#define INTERNAL_SOUND_FF_FIRE_ON			4


void PlayInternalSound(BYTE SoundNumber, BYTE Volume, BYTE Repeats, BYTE StreamAfterTimeout, BYTE VolumeAfterTimeout, BYTE AudioStatutsAfterTimeout)
{
		switch(SoundNumber)
			{
				case INTERNAL_SOUND_POSITIVE_FEEDBACK:
				default:
					InternalSoundPtr = &PositiveFeedback[0];
					InternalSoundLength = POSITIVE_FEEDBACK_LENGTH;
					InternalSoundVolume = Volume;
					if(Repeats>0)
						InteralSoundRepeats = Repeats;
					else
						InteralSoundRepeats = 1;
				break;
				
				case INTERNAL_SOUND_SELECTION:
					InternalSoundPtr = &Selection[0];
					InternalSoundLength = SELECTION_LENGTH;
					InternalSoundVolume = Volume;
					if(Repeats>0)
						InteralSoundRepeats = Repeats;
					else
						InteralSoundRepeats = 1;
				break;
				
				case INTERNAL_SOUND_FF_WATER_HIT:
					InternalSoundPtr = &FF_WaterHit[0];
					InternalSoundLength = FF_WATER_HIT_LENGTH;
					InternalSoundVolume = Volume;
					if(Repeats>0)
						InteralSoundRepeats = Repeats;
					else
						InteralSoundRepeats = 1;
				break;
				
				
				case INTERNAL_SOUND_FF_WATER_MISS:
					InternalSoundPtr = &FF_WaterMiss[0];
					InternalSoundLength = FF_WATER_MISS_LENGTH;
					InternalSoundVolume = Volume;
					if(Repeats>0)
						InteralSoundRepeats = Repeats;
					else
						InteralSoundRepeats = 1;
				break;
				
				case INTERNAL_SOUND_FF_FIRE_ON:
					InternalSoundPtr = &FF_FireOn[0];
					InternalSoundLength = FF_FIRE_ON_LENGTH;
					InternalSoundVolume = Volume;
					if(Repeats>0)
						InteralSoundRepeats = Repeats;
					else
						InteralSoundRepeats = 1;
				break;
				
				
			}
		
		
		if(AudioTimeoutStream != 0xFF)
		{			
			AudioTimeoutStream=StreamAfterTimeout&0x03;	
		}		
		
		if(AudioTimeoutVolume != 0xFF)
		{
		AudioTimeoutVolume=VolumeAfterTimeout;
		}
		
		if(AudioTimeoutStatus != 0xFF)
		{
		AudioTimeoutStatus=AudioStatutsAfterTimeout&0x01;
		}
			
		InternalSoundPosition=0;	
		AudioMode = INTERNAL_AUDIO;
}
				



	