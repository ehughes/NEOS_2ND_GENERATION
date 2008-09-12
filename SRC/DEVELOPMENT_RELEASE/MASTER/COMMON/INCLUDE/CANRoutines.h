#include "DataTypes.h"


void CANInit(void);
void CANTransmitCheck(void);
void CANRxProcess(void);
WORD CANMakeTxSID (WORD SID_Unformatted);
void CANQueueTxMessage(WORD SID, WORD word1,WORD word2, WORD word3, WORD word4);
void __attribute__((__interrupt__)) _C1Interrupt(void);

typedef struct {
			
				WORD	SID;
				BYTE	Data[8];	
			
				} CANMsg;
			
typedef struct {
	    
	    CANMsg *Message;
		volatile  WORD ReadPtr;
		volatile  WORD WritePtr;
		volatile  WORD QueueSize;
	
} CANMessageQueue;
		

extern void InitCANMsg(CANMsg *Msg);
void CANQueueInit();

WORD GetNumMessagesInQueue(CANMessageQueue *MessageQueue);
void CANMesssageEnqueue(CANMessageQueue *MessageQueue,CANMsg *Msg);
void CANMesssageDequeue(CANMessageQueue *MessageQueue,CANMsg *Msg);
	
extern volatile CANMessageQueue CANRxQueue,CANTxQueue;























