#include "DataTypes.h"

#ifndef _AUDIO_H
#define _AUDIO_H

#define NO_CHANGE						0xFF
#define AUDIO_ON_BEFORE_TIMEOUT 		1,0
#define AUDIO_ON_AFTER_TIMEOUT 			0,1
#define AUDIO_ON_BEFORE_AFTER_TIMEOUT 	1,1
#define NO_TIMEOUT						0xFFFF
#define NUM_AUDIO_STREAMS				4


extern BYTE AudioStreamTransmitFlag[NUM_AUDIO_STREAMS];  	//Set to 1 after every 128 words are sent to codec 
extern BYTE AudioStreamCounter[NUM_AUDIO_STREAMS];			// Counts to 3F in interrupt
extern DWORD AudioStreamAddress[NUM_AUDIO_STREAMS];			//moving addresses for the 4 current sounds
extern DWORD AudioStreamStartAddress[NUM_AUDIO_STREAMS];	//Starting addresses for the 4 current sounds
extern DWORD AudioStreamLength[NUM_AUDIO_STREAMS];			//and their lengths

void AudioNodeEnableAll(BYTE strm, BYTE vol);
void AudioTimeoutCheck(void);
void AudioStreamCheck(void);
//void AudioPlaySound(WORD sound, BYTE stream);
void AudioEnable(BYTE node,BYTE status,BYTE volume, BYTE stream);
void AudioOffAllNodes(void);
void AudioReSync(WORD node);
void AudioSetNodesToStream(BYTE stream, WORD w1,WORD w2, WORD w3, WORD w4);
void AudioNodeEnable(BYTE node, BYTE stream1, BYTE stream2,
					 WORD status1, BYTE status2, WORD timeout,
				 	 WORD volume1, BYTE volume2);
void AudioSetNodesOnOff(WORD w1, WORD w2, WORD w3, WORD w4);
void DCIInit (void);
void EAudioPlaySound(BYTE stream,WORD sound);
void PlayInternalNodeSound(BYTE NodeNumber,BYTE InternalSoundNumber,BYTE Volume,BYTE Repeats,BYTE StreamWhenComplete,BYTE StreamVolumeWhenComplete,BYTE AudioStatusWhenComplete);
void __attribute__((__interrupt__)) _DCIInterrupt(void);


#endif

