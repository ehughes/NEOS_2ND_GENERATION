

void CANInit(void);
void CANTransmitCheck(void);
void CANRxProcess(void);
unsigned int CANMakeTxSID (unsigned int SID_Unformatted);
void CANQueueTxMessage(unsigned int SID, unsigned int word1,
unsigned int word2, unsigned int word3, unsigned int word4);
void __attribute__((__interrupt__)) _C1Interrupt(void);



//***********************************************************************
//CAN Related
//***********************************************************************

//the highest differential I have seen in the tx buffer is 0x7B (123)
//But Light thief played better with this at 150 than at 135!
#define CANTXBUFFMAX 174
#define CANRXBUFFMAX 63 


/* Ring buffer for Pending CAN transmit messages */
extern unsigned int CANTxBuff[175][5];	/* Up to 64 pending 5-word messages */
extern unsigned int CANTxBuffNextUp;		/* Index of next one up to TRANSMIT */
extern unsigned int CANTxBuffNextAvailable;	/* Index of next available location to fill */
/* Note if NextUp = NextAvailable, there is nothing in queue */
/* Put a new message in NextAvialable and increment NextAvailable */
/* If NextAvailable != NextUp then send NextUp & then inc NextUp */

extern unsigned int CANRxBuff[64][5]; // Up to 64 received messages
extern unsigned int CANRxBuffNextAvailable;	//Next available location to receive data
extern unsigned int CANRxNextToProcess;	//Next that needs processed

extern unsigned char CANTxBuffDifferentialMax;
extern unsigned char CANRxBuffDifferentialMax;