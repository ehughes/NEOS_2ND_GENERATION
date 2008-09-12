#include "DataTypes.h"

#ifndef _AUDIO_H
#define _AUDIO_H

#define NO_CHANGE	0xFF
#define AUDIO_ON_BEFORE_TIMEOUT 1,0
#define AUDIO_ON_AFTER_TIMEOUT 	0,1
#define AUDIO_ON_BEFORE_AFTER_TIMEOUT 	1,1



#define NO_TIMEOUT				0xFFFF

void AudioNodeEnableAll(unsigned char strm, unsigned char vol);
void AudioTimeoutCheck(void);
void AudioStreamCheck(void);
void AudioPlaySound(unsigned int sound, unsigned char stream);
void AudioEnable(unsigned char node,unsigned char status,
unsigned char volume, unsigned char stream);
void AudioOffAllNodes(void);
void AudioReSync(unsigned int node);
void AudioSetNodesToStream(unsigned char stream, unsigned int w1, 
unsigned int w2, unsigned int w3, unsigned int w4);
void AudioNodeEnable(unsigned char node, unsigned char stream1, unsigned char stream2,
	unsigned char status1, unsigned char status2, unsigned int timeout,
	unsigned char volume1, unsigned char volume2);
void AudioSetNodesOnOff(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4);
void DCIInit (void);

void EAudioPlaySound(unsigned char stream,unsigned int sound);

void __attribute__((__interrupt__)) _DCIInterrupt(void);

extern unsigned char AudioStreamTransmitFlag[4];  //Set to 1 after every 128 words are sent to codec 
extern unsigned char AudioStreamCounter[4];		// Counts to 3F in interrupt
extern unsigned long AudioStreamAddress[4];	//moving addresses for the 4 current sounds
extern unsigned long AudioStreamStartAddress[4];	//Starting addresses for the 4 current sounds
extern unsigned long AudioStreamLength[4];		//and their lengths


void PlayInternalNodeSound(BYTE NodeNumber,BYTE InternalSoundNumber,BYTE Volume,BYTE Repeats,BYTE StreamWhenComplete,BYTE StreamVolumeWhenComplete,BYTE AudioStatusWhenComplete);


#endif

