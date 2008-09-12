
#include "DataTypes.h"

#ifndef _AUDIO_H
#define _AUDIO_H

void AudioNodeEnableAll(BYTE strm, BYTE vol);
void AudioTimeoutCheck(void);
void AudioStreamCheck(void);
void AudioPlaySound(WORD sound, BYTE stream);

void AudioEnable(BYTE node,BYTE status,
BYTE volume, BYTE stream);

void AudioOffAllNodes(void);
void AudioReSync(WORD node);

void AudioSetNodesToStream(BYTE stream, WORD w1, 
WORD w2, WORD w3, WORD w4);

void AudioNodeEnable(BYTE node, BYTE stream1, BYTE stream2,
	BYTE status1, BYTE status2, WORD timeout,
	BYTE volume1, BYTE volume2);

void AudioSetNodesOnOff(WORD w1, WORD w2, WORD w3, WORD w4);

void DCIInit (void);


void __attribute__((__interrupt__)) _DCIInterrupt(void);
void ResetAudio();


void PlayInternalSound(BYTE SoundNumber,
					   BYTE Volume, 
					   BYTE Repeats, 
					   BYTE StreamAfterTimeout, 
					   BYTE VolumeAfterTimeout,
					   BYTE AudioStatutsAfterTimeout);



extern BYTE AudioStreamTransmitFlag[4];  //Set to 1 after every 128 words are sent to codec 
extern BYTE AudioStreamCounter[4];		// Counts to 3F in interrupt
//Audio buffer contains 128 upper & 128 lower words
//While one half is being played the other half is being written by CAN data
extern SIGNED_BYTE AudioBuffer[256];	/* 256 samples at 16000 hz = 16ms of audio */
extern WORD AudioWriteIndex ;		/* Addr to write incoming data */
extern WORD AudioReadIndex;		/* Addr to read stuff out to codec */

//BYTE AudioReadIndexResetStream=0;	// Stream # to look for to reset index
extern BYTE AudioReadIndexReset;		// Indicates a stream reset is needed

//Start read index 1/2 buffer out of phase with the stream counter
extern BYTE AudioOn;	/* 0 for no sound, 1 for sound */
extern BYTE MyAudioStream;	//Stream 0,1,2 or 3 that THIS NODE is playing
extern BYTE MyAudioVolume;

extern BYTE AudioTimeoutVolume;	//Volume AFTER timeout
extern BYTE AudioTimeoutStatus;	//on/off status after timeout
extern BYTE AudioTimeoutStream;	//Stream AFTER timeout
extern WORD AudioTimer;			//Count up at 1/100 sec rate to time audio switchover
extern WORD AudioTimeout;			//Value to look for for audio switchover

extern DWORD AudioStreamAddress[4];	//moving addresses for the 4 current sounds
extern DWORD AudioStreamStartAddress[4];	//Starting addresses for the 4 current sounds
extern DWORD AudioStreamLength[4];		//and their lengths






#define STREAMING_AUDIO 0
#define INTERNAL_AUDIO 1


extern const BYTE *InternalSoundPtr;
extern WORD InternalSoundLength;
extern WORD InternalSoundPosition;
extern BYTE AudioMode;
extern BYTE InternalSoundVolume;
extern BYTE InteralSoundRepeats;




#endif
