#include <can.h>
#include "CANRoutines.h"
#include "BoardSupport.h"
#include "DataTypes.h"
#include <p30fxxxx.h>
#include "TimerRoutines.h"
#include "CRC.h"
#include "SystemVariables.h"
#include "Messages.h"


CANMsg IncomingCANMsg;
CANMsg RxIRQCANMsg;
CANMsg TxIRQCANMsg;

volatile CANMessageQueue CANRxQueue,CANTxQueue;

#define CAN_REJECTION_RANGE_LOW 	0x100
#define CAN_REJECTION_RANGE_HIGH	0x1FF


#define CAN_TX_QUEUE_SIZE	256
#define CAN_RX_QUEUE_SIZE	256

CANMsg InternalTxMessageQueueStorage[CAN_TX_QUEUE_SIZE];
CANMsg InternalRxMessageQueueStorage[CAN_RX_QUEUE_SIZE];


WORD IDExcludeHigh = 0x1FF;
WORD IDExcludeLow = 0x100;



void ResetCAN()
{
	CANQueueInit();
	EnableIntCAN1;
}	

void CANQueueInit()
{

	int i,j;
		//Init the software structures
	CANRxQueue.MessageCount = 0;
	CANRxQueue.ReadPtr = 0;
	CANRxQueue.WritePtr = 0;
	CANRxQueue.QueueSize = CAN_RX_QUEUE_SIZE;
	CANRxQueue.Message = &InternalRxMessageQueueStorage[0];
		
	CANTxQueue.MessageCount = 0;
	CANTxQueue.ReadPtr = 0;
	CANTxQueue.WritePtr = 0;
	CANTxQueue.QueueSize = CAN_TX_QUEUE_SIZE;
	CANTxQueue.Message = &InternalTxMessageQueueStorage[0];

	for(i=0;i<CANRxQueue.QueueSize;i++)
	{
		InternalTxMessageQueueStorage[i].SID = 0xFFFF;
		InternalRxMessageQueueStorage[i].SID = 0xFFFF;
		for(j=0;j<8;j++)
		{
		InternalTxMessageQueueStorage[i].Data[j] = 0xFF;
		InternalRxMessageQueueStorage[i].Data[j] = 0xFF;
		}
	}

}	

WORD GetNumMessagesInQueue(CANMessageQueue *MessageQueue)
{
	if(MessageQueue->ReadPtr > 	MessageQueue->WritePtr)
	{
		return (MessageQueue->QueueSize - MessageQueue->ReadPtr + MessageQueue->WritePtr);
	}
	else if (MessageQueue->WritePtr > MessageQueue->ReadPtr)
	{
		return 	(MessageQueue->WritePtr - MessageQueue->ReadPtr);
	}
	else
	{
		return 0;	
	}
	
}	

void CANMesssageEnqueue(CANMessageQueue *MessageQueue,CANMsg *Msg)
{
	BYTE i;
	

	
	MessageQueue->Message[MessageQueue->WritePtr].SID = Msg->SID;
	
	

	for(i=0;i<8;i++)
	{
		MessageQueue->Message[MessageQueue->WritePtr].Data[i] = Msg->Data[i];
	}
	
	
	MessageQueue->WritePtr++;
	
		if(MessageQueue->WritePtr >= MessageQueue->QueueSize)
		{
			MessageQueue->WritePtr = 0;	
		}
		
	MessageQueue->MessageCount++;
}


void CANMesssageDequeue(CANMessageQueue *MessageQueue,CANMsg * Msg)
{
	BYTE i;
	
	Msg->SID = MessageQueue->Message[MessageQueue->ReadPtr].SID;
			
		for(i=0;i<8;i++)
		{
			Msg->Data[i] = 	MessageQueue->Message[MessageQueue->ReadPtr].Data[i];
		}
		
	   
		
		MessageQueue->ReadPtr++;
		if(MessageQueue->ReadPtr >= MessageQueue->QueueSize)
		{
			MessageQueue->ReadPtr = 0;	
		}
		
		MessageQueue->MessageCount--;
}	


void __attribute__((__interrupt__,,__auto_psv__)) _C1Interrupt(void)
{
	_C1IF=0;    /* Clear interrupt flag to acknowledge */    
	//Check both Recieve Buffers
	WORD TempSID;
	BYTE TempVal;
	
	if (C1INTFbits.RX0IF==1)
	{ 
		TempSID = C1RX0SID>>2;
		TempVal = C1RX0B1 &0xFF;
		
		if((TempSID == 0x2f2) && (TempVal == 0x07 || TempVal == 0x08))
		{
		 //Reject the Node Operation  NODE_LOOP_BACK && RESPONSE Messages	
			
		}
	
		//only non audio streaming messages get into the queue
		else if(!((TempSID>=CAN_REJECTION_RANGE_LOW) && (TempSID<=CAN_REJECTION_RANGE_HIGH)))
		{
						
			RxIRQCANMsg.SID = TempSID;
			RxIRQCANMsg.Data[0] =  C1RX0B1 &0xFF;
		RxIRQCANMsg.Data[1] = (C1RX0B1>>8) &0xFF;
				RxIRQCANMsg.Data[2] =  C1RX0B2 &0xFF;
			RxIRQCANMsg.Data[3] = (C1RX0B2>>8) &0xFF;
			RxIRQCANMsg.Data[4] =  C1RX0B3&0xFF;
			RxIRQCANMsg.Data[5] = (C1RX0B3>>8) &0xFF;
			RxIRQCANMsg.Data[6] =	 C1RX0B4 &0xFF;
			RxIRQCANMsg.Data[7] = (C1RX0B4>>8) &0xFF;
			CANMesssageEnqueue((CANMessageQueue *)&CANRxQueue,&RxIRQCANMsg);
			
			
			/*	
			TempBufToUSB[0] = 0xAA;
			TempBufToUSB[1] = 0xAA;
			TempBufToUSB[2] = 13;
			TempBufToUSB[3] = 0;
			TempBufToUSB[4] = RX_CAN_MSG;
			TempBufToUSB[5] = (BYTE)(TempSID);
			TempBufToUSB[6] = (BYTE)(TempSID>>8);
			TempBufToUSB[7] = 0;
			TempBufToUSB[8] = 0;
			
			TempBufToUSB[9] =  C1RX0B1 &0xFF;
			TempBufToUSB[10] = (C1RX0B1>>8) &0xFF;
			TempBufToUSB[11] = C1RX0B2 &0xFF;
			TempBufToUSB[12] = (C1RX0B2>>8) &0xFF;
			TempBufToUSB[13] = C1RX0B3&0xFF;
			TempBufToUSB[14] = (C1RX0B3>>8) &0xFF;
			TempBufToUSB[15] = C1RX0B4 &0xFF;
			TempBufToUSB[16] = (C1RX0B4>>8) &0xFF;
		
			TempCRC =  CalcCRC(&TempBufToUSB[0], 17);
			TempBufToUSB[17] = (BYTE)(TempCRC);
			TempBufToUSB[18] = (BYTE)(TempCRC>>8);
		
			WriteBlockFT245(&TempBufToUSB[0],19);
			*/
			RX_LED_ON;
			
			
		}
				
		C1INTFbits.RX0IF=0;
		C1RX0CON=0;		
	}
	if (C1INTFbits.RX1IF==1)
	{
			TempSID = C1RX0SID>>2;
		//only non audio streaming messages get into the queue
		if(!((TempSID>=CAN_REJECTION_RANGE_LOW) && (TempSID<=CAN_REJECTION_RANGE_HIGH)))
		{
		
			RxIRQCANMsg.SID = TempSID;
			RxIRQCANMsg.Data[0] =  C1RX0B1 &0xFF;
			RxIRQCANMsg.Data[1] = (C1RX0B1>>8) &0xFF;
			RxIRQCANMsg.Data[2] =  C1RX0B2 &0xFF;
			RxIRQCANMsg.Data[3] = (C1RX0B2>>8) &0xFF;
			RxIRQCANMsg.Data[4] =  C1RX0B3&0xFF;
			RxIRQCANMsg.Data[5] = (C1RX0B3>>8) &0xFF;
			RxIRQCANMsg.Data[6] =	 C1RX0B4 &0xFF;
			RxIRQCANMsg.Data[7] = (C1RX0B4>>8) &0xFF;
			CANMesssageEnqueue((CANMessageQueue *)&CANRxQueue,&RxIRQCANMsg);
			
			
			/*
			TempBufToUSB[0] = 0xAA;
			TempBufToUSB[1] = 0xAA;
			TempBufToUSB[2] = 13;
			TempBufToUSB[3] = 0;
			TempBufToUSB[4] = RX_CAN_MSG;
			TempBufToUSB[5] = (BYTE)(TempSID);
			TempBufToUSB[6] = (BYTE)(TempSID>>8);
			TempBufToUSB[7] = 0;
			TempBufToUSB[8] = 0;
			
			TempBufToUSB[9] =  C1RX0B1 &0xFF;
			TempBufToUSB[10] = (C1RX0B1>>8) &0xFF;
			TempBufToUSB[11] = C1RX0B2 &0xFF;
			TempBufToUSB[12] = (C1RX0B2>>8) &0xFF;
			TempBufToUSB[13] = C1RX0B3&0xFF;
			TempBufToUSB[14] = (C1RX0B3>>8) &0xFF;
			TempBufToUSB[15] = C1RX0B4 &0xFF;
			TempBufToUSB[16] = (C1RX0B4>>8) &0xFF;
		
			TempCRC =  CalcCRC(&TempBufToUSB[0], 17);
			TempBufToUSB[17] = (BYTE)(TempCRC);
			TempBufToUSB[18] = (BYTE)(TempCRC>>8);
		
			WriteBlockFT245(&TempBufToUSB[0],19);*/
			RX_LED_ON;
		}

		C1INTFbits.RX1IF=0;
		C1RX1CON=0;		
	}
	if (C1INTFbits.ERRIF==1)
	{
		CAN1AbortAll();
		C1INTFbits.RX0OVR=0; //RX1OVR must be cleared too in order to clear interrupt
		C1INTFbits.RX1OVR=0; //RX1OVR must be cleared too in order to clear interrupt
		C1INTFbits.ERRIF=0;	
	}
}



/* Routine to check if anything is in CAN transmit queue that needs started */
// Note highest numbered buffer sends first and this routine uses all three message buffers

void CANTransmitCheck(void)
{


	if ((CANTxQueue.MessageCount>0)	&& ((C1TX0CON & 0x0008) == 0))    // If data queued & buffer is available
		{
			C1INTFbits.TX0IF = 0;		// Flag as NOT free
			C1TX0DLC=0x0040;			// Data length =8
			
			CANMesssageDequeue((CANMessageQueue *)&CANTxQueue,&TxIRQCANMsg);
			C1TX0SID=CANMakeTxSID(TxIRQCANMsg.SID) ;
			C1TX0B1=TxIRQCANMsg.Data[0] | (TxIRQCANMsg.Data[1]<<8);					// Set the TXIDE bit 
			C1TX0B2=TxIRQCANMsg.Data[2] | (TxIRQCANMsg.Data[3]<<8);	
			C1TX0B3=TxIRQCANMsg.Data[4] | (TxIRQCANMsg.Data[5]<<8);	
			C1TX0B4=TxIRQCANMsg.Data[6] | (TxIRQCANMsg.Data[7]<<8);	
			C1TX0CON=0x0008;  // Priority 0, set TXREQ LAST
		}

	if ((CANTxQueue.MessageCount>0)	&& 	((C1TX1CON & 0x0008) == 0))    // If data queued & buffer is available
		{
		
			C1INTFbits.TX1IF = 0;		// Flag as NOT free
			C1TX1DLC=0x0040;			// Data length =8
		
			CANMesssageDequeue((CANMessageQueue *)&CANTxQueue,&TxIRQCANMsg);
			C1TX1SID=CANMakeTxSID(TxIRQCANMsg.SID) ;
			C1TX1B1=TxIRQCANMsg.Data[0] | (TxIRQCANMsg.Data[1]<<8);					// Set the TXIDE bit 
			C1TX1B2=TxIRQCANMsg.Data[2] | (TxIRQCANMsg.Data[3]<<8);	
			C1TX1B3=TxIRQCANMsg.Data[4] | (TxIRQCANMsg.Data[5]<<8);	
			C1TX1B4=TxIRQCANMsg.Data[6] | (TxIRQCANMsg.Data[7]<<8);	
			C1TX1CON=0x0008;  // Priority 0, set TXREQ LAST
		}
	if ((CANTxQueue.MessageCount>0)	&& ((C1TX2CON & 0x0008) == 0))    // If data queued & buffer is available
		{
		
			C1INTFbits.TX2IF = 0;		// Flag as NOT free
			C1TX2DLC=0x0040;			// Data length =8
			CANMesssageDequeue((CANMessageQueue *)&CANTxQueue,&TxIRQCANMsg);
			C1TX2SID=CANMakeTxSID(TxIRQCANMsg.SID) ;		
			C1TX2B1=TxIRQCANMsg.Data[0] | (TxIRQCANMsg.Data[1]<<8);			
			C1TX2B2=TxIRQCANMsg.Data[2] | (TxIRQCANMsg.Data[3]<<8);	
			C1TX2B3=TxIRQCANMsg.Data[4] | (TxIRQCANMsg.Data[5]<<8);	
			C1TX2B4=TxIRQCANMsg.Data[6] | (TxIRQCANMsg.Data[7]<<8);	
			C1TX2CON=0x0008;  // Priority 0, set TXREQ LAST
		
		}
}


void CANRxProcess(void)
{
		
	if (GetNumMessagesInQueue((CANMessageQueue *)&CANRxQueue) > 0)
	{
		CANMesssageDequeue((CANMessageQueue *)&CANRxQueue,&IncomingCANMsg);
			
		if((IncomingCANMsg.SID>=IDExcludeLow) && (IncomingCANMsg.SID<=IDExcludeHigh))
		{
		}
		else 
		{
					
			TempBufToUSB[0] = 0xAA;
			TempBufToUSB[1] = 0xAA;
			TempBufToUSB[2] = 13;
			TempBufToUSB[3] = 0;
			TempBufToUSB[4] = RX_CAN_MSG;
			TempBufToUSB[5] = (BYTE)(IncomingCANMsg.SID);
			TempBufToUSB[6] = (BYTE)(IncomingCANMsg.SID>>8);
			TempBufToUSB[7] = 0;
			TempBufToUSB[8] = 0;
			
			TempBufToUSB[9] =  IncomingCANMsg.Data[0];
			TempBufToUSB[10] = IncomingCANMsg.Data[1];
			TempBufToUSB[11] = IncomingCANMsg.Data[2];
			TempBufToUSB[12] = IncomingCANMsg.Data[3];
			TempBufToUSB[13] = IncomingCANMsg.Data[4];
			TempBufToUSB[14] = IncomingCANMsg.Data[5];
			TempBufToUSB[15] = IncomingCANMsg.Data[6];
			TempBufToUSB[16] = IncomingCANMsg.Data[7];
		
			TempCRC =  CalcCRC(&TempBufToUSB[0], 17);
			TempBufToUSB[17] = (BYTE)(TempCRC);
			TempBufToUSB[18] = (BYTE)(TempCRC>>8);
		
			WriteBlockFT245(&TempBufToUSB[0],19);
			RX_LED_ON;
		}
	
	}
	
}



void InitCANMsg(CANMsg *Msg)
{
	BYTE i;
	
	for(i=0;i<8;i++)
	{
		Msg->Data[i]=0;	
	}
	
	Msg->SID = 0;
	
}	


/* Transmitted SID word is in a strange format with 3 blank bytes in the middle */
WORD CANMakeTxSID (WORD SID_Unformatted)
{
return ( ((SID_Unformatted & 0x3F) << 2) + ((SID_Unformatted & 0x7C0) << 5) );
}

void CANInit(void)
{
	
CANQueueInit();

	/* Set request for configuration mode */
	CAN1SetOperationModeNoWait(CAN_IDLE_CON &  /* CAN on in idle mode */
	CAN_MASTERCLOCK_1 &					/* FCAN = FCY (16.384 MHZ) */
	CAN_REQ_OPERMODE_CONFIG &
	CAN_CAPTURE_DIS_NO_WAIT);		/* Disable capture */

	/* Waits for config to happen & confirm I think */
	while(C1CTRLbits.OPMODE <=3);
	
	/* Load configuration register */
	CAN1Initialize(CAN_SYNC_JUMP_WIDTH1 & /* Sync Jump = 1 TQ */
	CAN_BAUD_PRE_SCALE(1),			/* Pre-scale 1, TQ = 118ns */
	CAN_WAKEUP_BY_FILTER_DIS &		/* No filter on wake-up */
	CAN_PHASE_SEG2_TQ(3) &			/*Sync seg=1+prop seg=2+phase1=2+phase2=3 = 8TQ total */
	CAN_PHASE_SEG1_TQ(2) &			/* Phase seg 2 > jump width */
	CAN_PROPAGATIONTIME_SEG_TQ(2) &	
	CAN_SEG2_FREE_PROG &
	CAN_SAMPLE1TIME);			/* take one sample at sample point (not 3) */
	
	/* Load Acceptance filter register */
	CAN1SetFilter(0, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */
	CAN1SetFilter(1, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */
	CAN1SetFilter(2, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */
	CAN1SetFilter(3, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */
	CAN1SetFilter(4, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */

	/* Load mask filter register */
	CAN1SetMask(0, CAN_MASK_SID(0) &
	CAN_IGNORE_FILTER_TYPE, CAN_MASK_EID(0));
	CAN1SetMask(1, CAN_MASK_SID(0) &
	CAN_IGNORE_FILTER_TYPE, CAN_MASK_EID(0));


	CAN1SetTXMode(0,
	CAN_TX_STOP_REQ &
	CAN_TX_PRIORITY_HIGH );
	CAN1SetTXMode(1,
	CAN_TX_STOP_REQ &
	CAN_TX_PRIORITY_HIGH );
	CAN1SetTXMode(2,
	CAN_TX_STOP_REQ &
	CAN_TX_PRIORITY_HIGH );

	CAN1SetRXMode(0,
	CAN_RXFUL_CLEAR &
	CAN_BUF0_DBLBUFFER_EN);
	CAN1SetRXMode(1,
	CAN_RXFUL_CLEAR &
	CAN_BUF0_DBLBUFFER_EN);


	/* Enable interrupts, highest priority */
	ConfigIntCAN1(CAN_INDI_INVMESS_DIS & 
		CAN_INDI_WAK_DIS &
		CAN_INDI_ERR_EN &
		CAN_INDI_TXB2_DIS &
		CAN_INDI_TXB1_DIS &
		CAN_INDI_TXB0_DIS &
		CAN_INDI_RXB1_EN &
		CAN_INDI_RXB0_EN,
		CAN_INT_ENABLE &
		CAN_INT_PRI_6);

	CAN1SetOperationMode(CAN_IDLE_CON &  /* CAN on in idle mode */
	CAN_MASTERCLOCK_1 &					/* FCAN = FCY (16.384 MHZ) */
	CAN_REQ_OPERMODE_NOR &			/* Change to _LOOPBK to test */
	CAN_CAPTURE_DIS );

	while (C1CTRLbits.OPMODE !=0);	/* Wait for loopback mode to take */
}

