#include "DataTypes.h"

#ifndef _CANROUTINES_H
#define _CANROUTINES_H


void CANInit(void);
void CANTransmitCheck(void);
void CANRxProcess(void);
void ResetCAN();
WORD CANMakeTxSID (WORD SID_Unformatted);
void __attribute__((__interrupt__)) _C1Interrupt(void);





extern WORD IDExcludeHigh;
extern WORD IDExcludeLow;

typedef struct {
			
			WORD	SID;
			BYTE	Data[8];	
			
			} CANMsg;
			
typedef struct {
	
	     CANMsg *Message;
		 WORD MessageCount;
		 WORD ReadPtr;
		 WORD WritePtr;
		 WORD QueueSize;
		 
} CANMessageQueue;
	
	
extern volatile CANMessageQueue CANRxQueue,CANTxQueue;
		
extern void InitCANMsg(CANMsg *Msg);
extern void CANQueueInit();
extern void CANMesssageEnqueue(CANMessageQueue *MessageQueue,CANMsg *Msg);
extern void CANMesssageDequeue(CANMessageQueue *MessageQueue,CANMsg *Msg);
extern volatile CANMessageQueue CANRxQueue,CANTxQueue;

WORD GetNumMessagesInQueue(CANMessageQueue *MessageQueue);

#endif

