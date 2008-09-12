#include <p30fxxxx.h>
#include <dci.h>
#include "Audio.h"
#include "SystemVariables.h"
#include "CANRoutines.h"
#include "SPIRoutines.h"


unsigned char AudioStreamTransmitFlag[4];  //Set to 1 after every 128 words are sent to codec 
unsigned char AudioStreamCounter[4]={0,0,0,0,};		// Counts to 3F in interrupt
//Audio buffer contains 128 upper & 128 lower words
//While one half is being played the other half is being written by CAN data
char AudioBuffer[256];	/* 256 samples at 16000 hz = 16ms of audio */
unsigned int AudioWriteIndex = 0;		/* Addr to write incoming data */
unsigned int AudioReadIndex = 0x80;		/* Addr to read stuff out to codec */

//unsigned char AudioReadIndexResetStream=0;	// Stream # to look for to reset index
unsigned char AudioReadIndexReset=0;		// Indicates a stream reset is needed

//Start read index 1/2 buffer out of phase with the stream counter
unsigned char AudioOn = 0;	/* 0 for no sound, 1 for sound */
unsigned char MyAudioStream=0;	//Stream 0,1,2 or 3 that THIS NODE is playing
unsigned char MyAudioVolume=0;

unsigned char AudioTimeoutVolume;	//Volume AFTER timeout
unsigned char AudioTimeoutStatus;	//on/off status after timeout
unsigned char AudioTimeoutStream;	//Stream AFTER timeout
unsigned int AudioTimer;			//Count up at 1/100 sec rate to time audio switchover
unsigned int AudioTimeout;			//Value to look for for audio switchover

unsigned long AudioStreamAddress[4];	//moving addresses for the 4 current sounds
unsigned long AudioStreamStartAddress[4];	//Starting addresses for the 4 current sounds
unsigned long AudioStreamLength[4];		//and their lengths

//unsigned char AudioStreamVolume[4];
//long DCILast=0;				// Value for interpolating 16KHZ audio from 8KHz



/* AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO */
/* AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO */
/* AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO */
/* AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO */
/* AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO */
/* AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO AUDIO */

void AudioNodeEnableAll(unsigned char strm, unsigned char vol)
{
	AudioNodeEnable(0,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(1,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(2,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(3,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(4,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(5,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(6,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(7,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(8,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(9,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(10,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(11,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(12,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(13,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(14,strm,strm,1,0,0xFFFF,vol,0xFF);	
	AudioNodeEnable(15,strm,strm,1,0,0xFFFF,vol,0xFF);	
}



void AudioTimeoutCheck (void)
{
	if ((AudioTimeout != 0xFFFF) && (AudioTimer >= AudioTimeout))
	{
		AudioOn=AudioTimeoutStatus;
		MyAudioVolume=AudioTimeoutVolume;
		MyAudioStream=AudioTimeoutStream;
		AudioReadIndexReset=1;		// RE-SYNCHRONIZE
		AudioTimeout=0xFFFF;		//Disable timeout
	}
}


// MASTER - check all audio streams for update needs
//Queue up another 128 words if it's needed. 
void AudioStreamCheck(void)
{
	int stream,temp,temp2;
//	unsigned int value;
//	int w1,w2;  //w3,w4,w5;		// SIGNED values for audio manipluations
	if (InactivityTimer != 0)
	{
	for (stream=0; stream<=3; stream++)		//TEMPORARY - only check 2 streams
	{
		if (AudioStreamTransmitFlag[stream])
		{
			//Get audio data from flash
			if (SPITxAudioHalt==0)
				SPISendInstruction(0x43,AudioStreamAddress[stream]);  // Get 128 bytes
			AudioStreamAddress[stream]+=0x40;		// Advance 128 bytes
			//Check for address beyond end
			if (AudioStreamAddress[stream] >= (AudioStreamStartAddress[stream]+AudioStreamLength[stream]) )
				AudioStreamAddress[stream] = AudioStreamStartAddress[stream];

			//11/6/06 - queue 128 BYTES, not 128 words now
			//Bytes are packed 8 to a message so there's 16 messages now
			//Queue up 16  sound messages to CAN
			for (temp=0; temp <8; temp++)   //16 iterations 
			{
				/* check if writing upper half or lower half of buffer */
//				CANTxBuff[CANTxBuffNextAvailable][0] = CANMakeTxSID( (temp) + 0x100 + (stream * 0x40) + ((AudioStreamTransmitFlag[stream]-1)*0x20) );
				if ((AudioStreamTransmitFlag[stream] & 1)==0)
					temp2 = 8;  // upper half
				else
					temp2=0;

				CANTxBuff[CANTxBuffNextAvailable][0] = CANMakeTxSID( (temp) + 0x100 
				+ (temp2)	+ (stream * 0x40) + (((AudioStreamTransmitFlag[stream]-1)/2)*0x20) );
				
				temp2 = temp*8;  //each message skips 8 bytes from SPI

				CANTxBuff[CANTxBuffNextAvailable][1] = (SPIRxBuffer[temp2]) + (SPIRxBuffer [temp2+1] << 8);
				CANTxBuff[CANTxBuffNextAvailable][2] = (SPIRxBuffer[temp2+2]) + (SPIRxBuffer [temp2+3] << 8);
				CANTxBuff[CANTxBuffNextAvailable][3] = (SPIRxBuffer[temp2+4]) + (SPIRxBuffer [temp2+5] << 8);
				CANTxBuff[CANTxBuffNextAvailable][4] = (SPIRxBuffer[temp2+6]) + (SPIRxBuffer [temp2+7] << 8);

				CANTxBuffNextAvailable+=1;
				if (CANTxBuffNextAvailable > CANTXBUFFMAX) CANTxBuffNextAvailable=0;
			}
			AudioStreamTransmitFlag[stream]=0;
		}
	}
	}
}



/* Play a sound routine - initiates the playing of a recorded sound
All that is needed is to give it the sound number, stream & volume
this routine starts from the beginning of the sound */
void AudioPlaySound(unsigned int sound, unsigned char stream)
{
//	unsigned int temp;
	unsigned long temp1,temp2,temp3;
	stream &= 0x3;	//Limit value to allowable range
	// Find the sound by reading appropriate 256 byte page from index
	//sound values are 0 through 8191
	//for sound value = 8191 (0x1FFF)  we want Flash address 0x0000FF00

//	DisableIntDCI;	
	if (SPITxAudioHalt==0)
		SPISendInstruction(0x33 , (sound << 3) );		//8 Byte read


	//That page is now in SPIRxBuffer
	//Look up the address & length of this sound
	temp1 = SPIRxBuffer[1];
	temp2 = SPIRxBuffer[2];
	temp3 = SPIRxBuffer[3];
	AudioStreamAddress[stream] = (temp1<<8) + (temp2<<16) + (temp3<<24);

	AudioStreamStartAddress[stream]=AudioStreamAddress[stream];
	temp1 = SPIRxBuffer[5];
	temp2 = SPIRxBuffer[6];
	temp3 = SPIRxBuffer[7];
	AudioStreamLength[stream] = (temp1<<8) + (temp2<<16) + (temp3<<24);

	//Note AudioStreamCounter, AudioStreamTransmitFlag are affected by interrupt
	//Be careful what order they are changed
	//flag an update is needed instantly on that stream

//Set the nodes playing this stream to start at the beginning of the upper half
//of the buffer

	//Must make sure any prior instruction to set node to stream is processed
	//BEFORE this sync request
	CANQueueTxMessage(0x8C,0,0,0,0);		//nop
	CANQueueTxMessage(0x8C,0,0,0,0);		//nop
	CANQueueTxMessage(0x8C,0,0,0,0);		//nop
	CANQueueTxMessage(0x8A,stream,0,0,0);		// Indicate sync needed on this stream
	AudioStreamCounter[stream]=0;			//Even if int happens between these instrs it is OK
	AudioStreamTransmitFlag[stream]=1;	//flag may have just been set anyway

	//CANQueueTxMessage(0x8D,stream,0x80,1,0);		// Start slave nodes at 0 too
	//CANQueueTxMessage(0x8C,0,0,0,0);		//nop
	//CANQueueTxMessage(0x8C,0,0,0,0);		//nop
//Do 3 times so buffer is sure to be reset as new data comes in


//Clear out any sound messages having to do with THIS stream only
//100-13F messages are SID 2000-2800 etc.
// Nothing will be stopped in the middle because transmit isn't an interrupt


//	temp1 = 0x2000 + (stream * 0x800);
//	temp2 = temp1+0x800;
//	for (temp=0; temp<CANTXBUFFMAX; temp++)	// Fixed 8/8/06 - limit to CANTXBUFFMAX
//	{
//		if ((CANTxBuff[temp][0] >=temp1) && (CANTxBuff[temp][0] < temp2) )
//		{
//			CANTxBuff[temp][0]=0;		//Zero out the audio transmit instruction */
//		}
//	}				

//Problem here is there could be 26 messages waiting to be processed somewhere in an RX
//queue, and until it gets around to processing all of those the 
//Audio counter & stream counters will not be 180 out of phase any more!
//OK we can clear out the unneeded stuff still in the master transmit buffer,
//but that which has been received will still take time to deal with.

//So let's just not do ANY of this until that 0x8D message comes up as received

//	DisableIntDCI;	
//	AudioStreamCounter[stream]=0;		//Begin transmitting to audio buffer address 0
////	AudioStreamVolume[stream] = volume;
//	AudioStreamTransmitFlag[stream]=1;	//Request a new audio transmit block ASAP
//	EnableIntDCI;
//	AudioStreamCheck();		//Force CAN message update (Not needed - audio check is next)
}



void AudioEnable(unsigned char node,unsigned char status,
unsigned char volume, unsigned char stream)
{
	CANQueueTxMessage(0x8F,node,status,volume,stream);
}

void AudioOffAllNodes(void)
{
	CANQueueTxMessage(0x8E,0,0,0,0);
}


void AudioReSync(unsigned int node)
{
	CANQueueTxMessage(0x8C,0,0,0,0);	// 3 NOP's to make sure buffer gets cleared
	CANQueueTxMessage(0x8C,0,0,0,0);
	CANQueueTxMessage(0x8C,0,0,0,0);
	CANQueueTxMessage(0x89,node,0,0,0);
}


void AudioSetNodesToStream(unsigned char stream, unsigned int w1, 
unsigned int w2, unsigned int w3, unsigned int w4)
{
	CANQueueTxMessage(0x80 + (stream & 0x3),w1,w2,w3,w4);

}

// Send audio enable to node with timeout
void AudioNodeEnable(unsigned char node, unsigned char stream1, unsigned char stream2,
	unsigned char status1, unsigned char status2, unsigned int timeout,
	unsigned char volume1, unsigned char volume2)
{

	//Scale volumes by global value
	unsigned int volint;
	volint = volume1 * (AudioGlobalVolume+1);
	volume1 = volint >> 8;

	volint = volume2 * (AudioGlobalVolume+1);
	volume2 = volint >> 8;


#ifdef ROWFAKE
	if (node==4)
		node=1;
	else if (node==8)
		node=2;
	else if (node==12)
		node=3;
	else if (node==1)
		node=4;
	else if (node==2)
		node=8;
	else if (node==3)
		node=12;
#endif

	unsigned int w1,w3;
	w1= node + (stream1<<8) + (stream2<<10) + (status1<<12) + (status2<<13);
	w3 = volume1 + (volume2<<8);
	CANQueueTxMessage(0x88,w1,timeout,w3,0);
}

void AudioSetNodesOnOff(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4)
{
	CANQueueTxMessage(0x084,w1,w2,w3,w4);
}

//void AudioSetNodesVolumeMax(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4)
//{
//	CANQueueTxMessage(0x085,w1,w2,w3,w4);
//}

//void AudioSetNodesVolumeHalf(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4)
//{
//	CANQueueTxMessage(0x086,w1,w2,w3,w4);
//}

//void AudioSetNodesVolumeBackground(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4)
//{
//	CANQueueTxMessage(0x087,w1,w2,w3,w4);
//}



/* DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI */
/* DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI */
/* DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI */
/* DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI */
/* DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI */
/* DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI DCI */
//2/13/08 V1.25 replaced old DCIINIT with new one using library calls
void DCIInit(void)
{
	_DCIIE = 1;		//Clear the DCI Interrupt Flag    
	ConfigIntDCI(DCI_INT_PRI_7 & DCI_INT_ON);
	OpenDCI(
		DCI_EN & DCI_IDLE_STOP & DCI_DIGI_LPBACK_DIS & DCI_SCKD_OUP &
		DCI_SAMP_CLK_FAL & DCI_FSD_OUP & DCI_TX_ZERO_UNF & DCI_SDO_ZERO &
		DCI_DJST_ON & DCI_FSM_I2S,
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
//	PORTAbits.RA12=1;    // Temporary to check interrupt time
int DCIIntTemp;
//long DCILongTemp, DCILong1, DCILong2, DCILong3; //DCIInterpolated;

// Must read in the RX data even though there is none 
	DCIIntTemp = RXBUF0;
	DCIIntTemp = RXBUF1;
	DCIIntTemp = RXBUF2;
	DCIIntTemp = RXBUF3;

// But this is Left AND right data, have to send the same data out to
// Both channels 
	if ( (AudioOn) && (AudioReadIndexReset==0) )
	{
		DCIIntTemp=AudioBuffer[AudioReadIndex];
		DCIIntTemp *= (MyAudioVolume+1);
//		DCIIntTemp /= 256;
//		DCIIntTemp *= (AudioGlobalVolume+1);
		// Trial interpolation routine - didn't really have the intended effect. 
		//Kind of dulled the sound
		//		DCILongTemp=AudioBuffer[AudioReadIndex];
		//		if (DCILongTemp & 0x80)
		//			DCILongTemp |= 0xFFFFFF00;		// If negative, PAD IT
		//		DCILongTemp = AudioBuffer[AudioReadIndex] * (MyAudioVolume+1) * (AudioGlobalVolume+1) / 256;
		//		DCIInterpolated = (DCILongTemp + DCILast)/2;
		//		DCILast = DCILongTemp;
	}
	else
		DCIIntTemp=0;
	
	TXBUF0 = DCIIntTemp;			//Right channel
	TXBUF1 = DCIIntTemp;			//Left channel							// Left Channel
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
//	PORTAbits.RA12=0;     // Temporary to check interrupt time
}


//void _ISR _DefaultInterrupt(void)
//{
//	while(1) ClrWdt()

//	ClrWdt();
//}
