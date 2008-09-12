#include <p30fxxxx.h>
#include <dci.h>
#include "Audio.h"
#include "SystemVariables.h"
#include "CANRoutines.h"
#include "SPIRoutines.h"
#include "EEPROMRoutines.h"
#include "TimerRoutines.h"
#include "CANCommands.h"
#include "DataTypes.h"

unsigned char AudioStreamTransmitFlag[4];  //Set to 1 after every 128 words are sent to codec 
unsigned char AudioStreamCounter[4]={0,0,0,0,};		// Counts to 3F in interrupt
unsigned long AudioStreamAddress[4];	//moving addresses for the 4 current sounds
unsigned long AudioStreamStartAddress[4];	//Starting addresses for the 4 current sounds
unsigned long AudioStreamLength[4];		//and their lengths


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


// MASTER - check all audio streams for update needs
//Queue up another 128 words if it's needed. 

CANMsg AudioOutCANMsg;

void AudioStreamCheck(void)
{
	int stream,temp,temp2;

	if(InhibitAudio == FALSE)
	{
		
//	if (InactivityTimer != 0)
	{
	for (stream=0; stream<=3; stream++)		//TEMPORARY - only check 2 streams
	{
		if (AudioStreamTransmitFlag[stream])
		{
			//Get audio data from flash
		    
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


				AudioOutCANMsg.SID = (temp) + 0x100 + (temp2)	+ (stream * 0x40) + (((AudioStreamTransmitFlag[stream]-1)/2)*0x20);
							
				temp2 = temp*8;  //each message skips 8 bytes from SPI


				AudioOutCANMsg.Data[0] =SPIRxBuffer[temp2];
				AudioOutCANMsg.Data[1] =SPIRxBuffer [temp2+1]; 
				AudioOutCANMsg.Data[2] =SPIRxBuffer[temp2+2];
				AudioOutCANMsg.Data[3] =SPIRxBuffer [temp2+3]; 
				AudioOutCANMsg.Data[4] =SPIRxBuffer[temp2+4];
				AudioOutCANMsg.Data[5] =SPIRxBuffer [temp2+5];
				AudioOutCANMsg.Data[6] =SPIRxBuffer[temp2+6];
				AudioOutCANMsg.Data[7] =SPIRxBuffer [temp2+7]; 
				
				CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&AudioOutCANMsg);

				
			}
			AudioStreamTransmitFlag[stream]=0;
		}
	}
	}
	
	}
}



//Play a sound routine - initiates the playing of a recorded sound
//All that is needed is to give it the sound number, stream & volume
//this routine starts from the beginning of the sound 
void AudioPlaySound(unsigned int sound, unsigned char stream)
{
//	unsigned int temp;
	unsigned long temp1,temp2,temp3;
	stream &= 0x3;	//Limit value to allowable range
	// Find the sound by reading appropriate 256 byte page from index
	//sound values are 0 through 8191
	//for sound value = 8191 (0x1FFF)  we want Flash address 0x0000FF00

	SPISendInstruction(0x33 , (sound * 8) );		//8 Byte read


	//That page is now in SPIRxBuffer
	//Look up the address & length of this sound
	temp1 = SPIRxBuffer[1];
	temp2 = SPIRxBuffer[2];
	temp3 = SPIRxBuffer[3];
	AudioStreamAddress[stream] = (temp1<<8) + (temp2<<16) + (temp3<<24) + SPIRxBuffer[0];

	AudioStreamStartAddress[stream]=AudioStreamAddress[stream];
	temp1 = SPIRxBuffer[5];
	temp2 = SPIRxBuffer[6];
	temp3 = SPIRxBuffer[7];
	AudioStreamLength[stream] = (temp1<<8) + (temp2<<16) + (temp3<<24) + SPIRxBuffer[4];

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
}

//Play a sound routine - initiates the playing of a recorded sound
//All that is needed is to give it the sound number, stream & volume
//this routine starts from the beginning of the sound 
void EAudioPlaySound(unsigned char stream,unsigned int esound)
{
//	unsigned int temp;
	unsigned long temp1,temp2,temp3;
	stream &= 0x3;	//Limit value to allowable range
	// Find the sound by reading appropriate 256 byte page from index
	//sound values are 0 through 8191
	//for sound value = 8191 (0x1FFF)  we want Flash address 0x0000FF00


	SPISendInstruction(0x33 , (esound * 8) );		//8 Byte read


	//That page is now in SPIRxBuffer
	//Look up the address & length of this sound
	temp1 = SPIRxBuffer[1];
	temp2 = SPIRxBuffer[2];
	temp3 = SPIRxBuffer[3];
	AudioStreamAddress[stream] = (temp1<<8) + (temp2<<16) + (temp3<<24) + SPIRxBuffer[0];

	AudioStreamStartAddress[stream]=AudioStreamAddress[stream];
	temp1 = SPIRxBuffer[5];
	temp2 = SPIRxBuffer[6];
	temp3 = SPIRxBuffer[7];
	AudioStreamLength[stream] = (temp1<<8) + (temp2<<16) + (temp3<<24)+ SPIRxBuffer[0] ;

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
	unsigned int w1,w3;

	volint = volume1 * (AudioGlobalVolume+1);
	volume1 = volint >> 8;

	volint = volume2 * (AudioGlobalVolume+1);
	volume2 = volint >> 8;

	w1= node + (stream1<<8) + (stream2<<10) + (status1<<12) + (status2<<13);
	w3 = volume1 + (volume2<<8);
	CANQueueTxMessage(0x88,w1,timeout,w3,0);
}

void AudioSetNodesOnOff(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4)
{
	CANQueueTxMessage(0x084,w1,w2,w3,w4);
}

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



int t1,t2;

//DCI interrupt is hit 8000 times per second and queues up 2 16 bit words
//Used to time data to the slaves.
void __attribute__((__interrupt__,__auto_psv__)) _DCIInterrupt(void)
{

	
	//For master Operation, we only need a counter to time when to send out more data.
	//Most of the slave streaming code has been stripped. See slave code for more info.
	AudioStreamCounter[0] += 1;		// Count 2 more words sent in each stream 
	AudioStreamCounter[1] += 1;
	AudioStreamCounter[2] += 1;
	AudioStreamCounter[3] += 1;

	if ((AudioStreamCounter[0] & 0x3F)==0) AudioStreamTransmitFlag[0]=1+(AudioStreamCounter[0]>>6);	//every 64 passes (128 words sent to codec)
	if ((AudioStreamCounter[1] & 0x3F)==0) AudioStreamTransmitFlag[1]=1+(AudioStreamCounter[1]>>6);	//every 64 passes (128 words sent to codec)
	if ((AudioStreamCounter[2] & 0x3F)==0) AudioStreamTransmitFlag[2]=1+(AudioStreamCounter[2]>>6);	//every 64 passes (128 words sent to codec)
	if ((AudioStreamCounter[3] & 0x3F)==0) AudioStreamTransmitFlag[3]=1+(AudioStreamCounter[3]>>6);	//every 64 passes (128 words sent to codec)

	_DCIIF = 0;		// Clear the flag so it ain't stuck! 


	t1 = RXBUF0;
	t1 = RXBUF1;
	t1 = RXBUF2;
	t1 = RXBUF3;
		
TXBUF0 =	t2 ;			//Right channel
TXBUF1 =t2 ;			//Left channel							// Left Channel/
TXBUF2 =t2 ;			//Right Channel
TXBUF3 =	t2 ;			// Left channel							// Left Channel


}


void PlayInternalNodeSound(BYTE NodeNumber,BYTE InternalSoundNumber,BYTE Volume,BYTE Repeats,BYTE StreamWhenComplete,BYTE StreamVolumeWhenComplete,BYTE AudioStatusWhenComplete)
{

	CANQueueTxMessage(PLAY_INTERNAL_SOUND,(WORD)(InternalSoundNumber<<8) | (WORD)(NodeNumber)
											,	(WORD)(Repeats<<8) | (WORD)(Volume)
											,	(WORD)(StreamVolumeWhenComplete<<8) | (WORD)(StreamWhenComplete)
											,	 (WORD)(AudioStatusWhenComplete));	
	
	
	
}	

