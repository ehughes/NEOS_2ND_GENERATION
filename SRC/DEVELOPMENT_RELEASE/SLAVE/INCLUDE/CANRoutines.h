#include "DataTypes.h"

#ifndef _CANROUTINES_H
#define _CANROUTINES_H

void CANInit(void);
void CANTransmitCheck(void);
void CANRxProcess(void);
WORD CANMakeTxSID (WORD SID_Unformatted);

void __attribute__((__interrupt__)) _C1Interrupt(void);
void ResetCAN();

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

#endif




















