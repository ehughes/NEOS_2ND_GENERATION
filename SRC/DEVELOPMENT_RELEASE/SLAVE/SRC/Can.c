#include <can.h>
#include "CANRoutines.h"
#include "SystemVariables.h"
#include <p30fxxxx.h>
#include "LED-Display.h"
#include "Audio.h"
#include "EEPROMRoutines.h"
#include "TimerRoutines.h"
#include "CANCommands.h"
#include "BoardSupport.h"
#include "stdlib.h"
#include "ADCRoutines.h"


CANMsg IncomingMessage;
CANMsg RxIRQCANMsg;
CANMsg TxIRQCANMsg;
CANMsg TempCANMsg;

volatile CANMessageQueue CANRxQueue,CANTxQueue;

#define CAN_TX_QUEUE_SIZE	256
#define CAN_RX_QUEUE_SIZE	256

CANMsg InternalTxMessageQueueStorage[CAN_TX_QUEUE_SIZE];
CANMsg InternalRxMessageQueueStorage[CAN_RX_QUEUE_SIZE];

#define PING_DELAY_TIMER GPTimer[0]
BYTE PingDelayTime;
WORD SupplyVoltageTemp;

void CANQueueInit()
{
	WORD i;
		//Init the software structures
	
	CANRxQueue.ReadPtr = 1;
	CANRxQueue.WritePtr = 1;
	CANRxQueue.QueueSize = CAN_RX_QUEUE_SIZE;
	CANRxQueue.Message = &InternalRxMessageQueueStorage[0];
		

	CANTxQueue.ReadPtr = 1;
	CANTxQueue.WritePtr = 1;
	CANTxQueue.QueueSize = CAN_TX_QUEUE_SIZE;
	CANTxQueue.Message = &InternalTxMessageQueueStorage[0];

	
	for(i=0;i<CAN_RX_QUEUE_SIZE;i++)
	{
		InitCANMsg(&CANRxQueue.Message[i]);	
		
	}
	
	for(i=0;i<CAN_TX_QUEUE_SIZE;i++)
	{
		InitCANMsg(&CANTxQueue.Message[i]);	
		
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
}

void __attribute__((__interrupt__,,__auto_psv__)) _C1Interrupt(void)
{
	_C1IF=0;    /* Clear interrupt flag to acknowledge */    
	//Check both Recieve Buffers
	WORD TempSID;
	
	if (C1INTFbits.RX0IF==1)
	{
		TempSID = C1RX0SID>>2;
		//only non audio streaming messages get into the queue
				
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
		C1INTFbits.RX0IF=0;
		C1RX0CON=0;		
	}
	if (C1INTFbits.RX1IF==1)
	{
			TempSID = C1RX0SID>>2;
		//only non audio streaming messages get into the queue
	
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


	if ((GetNumMessagesInQueue((CANMessageQueue *)&CANTxQueue)>0)	&& ((C1TX0CON & 0x0008) == 0))    // If data queued & buffer is available
		{
			C1INTFbits.TX0IF = 0;		// Flag as NOT free
			C1TX0DLC=0x0040;			// Data length =8
			
			CANMesssageDequeue((CANMessageQueue *)&CANTxQueue,&TxIRQCANMsg);
			C1TX0SID=CANMakeTxSID(TxIRQCANMsg.SID) ;
			C1TX0B1=TxIRQCANMsg.Data[0] | (TxIRQCANMsg.Data[1]<<8);					// Set the TXIDE bit 
			C1TX0B2=TxIRQCANMsg.Data[2] | (TxIRQCANMsg.Data[3]<<8);	
			C1TX0B3=TxIRQCANMsg.Data[4] | (TxIRQCANMsg.Data[5]<<8);	
			C1TX0B4=TxIRQCANMsg.Data[6] | (TxIRQCANMsg.Data[7]<<8);	
			C1TX0CON=0x0003;  // Priority 0, set TXREQ LAST
			C1TX0CON|=0x0008;  // Priority 0, set TXREQ LAST
		}
	if ((GetNumMessagesInQueue((CANMessageQueue *)&CANTxQueue)>0)	&& 	((C1TX1CON & 0x0008) == 0))    // If data queued & buffer is available
		{
		
			C1INTFbits.TX1IF = 0;		// Flag as NOT free
			C1TX1DLC=0x0040;			// Data length =8
		
			CANMesssageDequeue((CANMessageQueue *)&CANTxQueue,&TxIRQCANMsg);
			C1TX1SID=CANMakeTxSID(TxIRQCANMsg.SID) ;
			C1TX1B1=TxIRQCANMsg.Data[0] | (TxIRQCANMsg.Data[1]<<8);					// Set the TXIDE bit 
			C1TX1B2=TxIRQCANMsg.Data[2] | (TxIRQCANMsg.Data[3]<<8);	
			C1TX1B3=TxIRQCANMsg.Data[4] | (TxIRQCANMsg.Data[5]<<8);	
			C1TX1B4=TxIRQCANMsg.Data[6] | (TxIRQCANMsg.Data[7]<<8);	
			C1TX1CON=0x0003;  // Priority 0, set TXREQ LAST
			C1TX1CON|=0x0008;  // Priority 0, set TXREQ LAST
		}
	if ((GetNumMessagesInQueue((CANMessageQueue *)&CANTxQueue)>0)	&& ((C1TX2CON & 0x0008) == 0))    // If data queued & buffer is available
		{
		
			C1INTFbits.TX2IF = 0;		// Flag as NOT free
			C1TX2DLC=0x0040;			// Data length =8
			CANMesssageDequeue((CANMessageQueue *)&CANTxQueue,&TxIRQCANMsg);
			C1TX2SID=CANMakeTxSID(TxIRQCANMsg.SID) ;		
			C1TX2B1=TxIRQCANMsg.Data[0] | (TxIRQCANMsg.Data[1]<<8);			
			C1TX2B2=TxIRQCANMsg.Data[2] | (TxIRQCANMsg.Data[3]<<8);	
			C1TX2B3=TxIRQCANMsg.Data[4] | (TxIRQCANMsg.Data[5]<<8);	
			C1TX2B4=TxIRQCANMsg.Data[6] | (TxIRQCANMsg.Data[7]<<8);	
			C1TX2CON=0x0003;  // Priority 0, set TXREQ LAST
			C1TX2CON|=0x0008;  // Priority 0, set TXREQ LAST
		
		}
}


void CANInit(void)
{
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




void ResetCAN()
{	
		DisableIntCAN1;
		CANQueueInit();
		EnableIntCAN1;
	
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


// * Process pending receive messages */
void CANRxProcess(void)
{

	WORD AudioAddress;
	WORD AudioOffset;
	WORD indx;
	WORD Rand;
	WORD i,z,MessagesToProcess;
	WORD BufferBaseAddress;
	
	MessagesToProcess = GetNumMessagesInQueue((CANMessageQueue *)&CANRxQueue);
	
    for(z=0;z<MessagesToProcess;z++)
	{
		CANMesssageDequeue((CANMessageQueue *)&CANRxQueue,&IncomingMessage);
		

		if 	( (IncomingMessage.SID >= 0x100) && (IncomingMessage.SID <= 0x1FF) && ((MyAudioStream * 0x40) == (IncomingMessage.SID & 0xC0)) )		
		{
			//Copy sound to audio buffer if stream matches
			//New 11/6/06 - process 8 bit sound messages but put in buffer as 16 bit
			AudioAddress = (IncomingMessage.SID & 0x0F) * 8;   //they come 8 at a time
			AudioOffset = (IncomingMessage.SID & 0x20) * 4;  //If in upper half of buffer
			indx=AudioAddress+AudioOffset;
		
			//Look for automatic reset of audio stream read address
			if (AudioReadIndexReset != 0)
			{
				if (indx==0)				
					AudioReadIndexReset=2;	//Setting value to 2 indicates 0x00 was hit
				else if ((indx == 0x80) && (AudioReadIndexReset==2))
				{
					AudioReadIndex=0;
					AudioReadIndexReset=0;
				}
			}		
						
			AudioBuffer[indx++] = IncomingMessage.Data[0];
			AudioBuffer[indx++] = IncomingMessage.Data[1];
			AudioBuffer[indx++] = IncomingMessage.Data[2];
			AudioBuffer[indx++] = IncomingMessage.Data[3];
			AudioBuffer[indx++] = IncomingMessage.Data[4];
			AudioBuffer[indx++] = IncomingMessage.Data[5];
			AudioBuffer[indx++] = IncomingMessage.Data[6];
			AudioBuffer[indx++] = IncomingMessage.Data[7];

		}
		
		else if((IncomingMessage.SID>=GP_BUFFER_WRITE_LOWER_CAN_ID_BOUND) && (IncomingMessage.SID<=GP_BUFFER_WRITE_UPPPER_CAN_ID_BOUND) && (GeneralPurposeBufferAcceptanceFlag == TRUE))
		{
			InactivityTimer = INACTIVITYTIMEOUT;	//refresh only on NON-AUDIO messages

		     	BufferBaseAddress = (IncomingMessage.SID - GP_BUFFER_WRITE_LOWER_CAN_ID_BOUND) * 8;
		       	
		       	for(i=0;i<8;i++)
		       	{
			       GeneralPurposeBuffer[BufferBaseAddress + i] = IncomingMessage.Data[i];	
			    }
		       	
		     	InitCANMsg(&TempCANMsg);
				TempCANMsg.SID=GENERAL_PURPOSE_BUFFER_OPERATION;
				TempCANMsg.Data[0] = MyNodeNumber;
				TempCANMsg.Data[1] = ACK_CMD|WRITE_BUFFER_ACK;
				TempCANMsg.Data[2] = IncomingMessage.SID - GP_BUFFER_WRITE_LOWER_CAN_ID_BOUND;
				
				CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
				
		}
		
		else 
		{
			InactivityTimer = INACTIVITYTIMEOUT;	//refresh only on NON-AUDIO messages

			switch (IncomingMessage.SID)

				{
			case LED_COMMAND:
			
				if ( (IncomingMessage.Data[7] == MyNodeNumber) || (IncomingMessage.Data[7] == 0xFF))
						{
							WORD w1,w2,w3,w4;
							
							w1 = (WORD)(IncomingMessage.Data[0]) | (((WORD)(IncomingMessage.Data[1]))<<8);
							w2 = (WORD)(IncomingMessage.Data[2]) | (((WORD)(IncomingMessage.Data[3]))<<8);
							w3 = (WORD)(IncomingMessage.Data[4]) | (((WORD)(IncomingMessage.Data[5]))<<8);
							w4 = (WORD)(IncomingMessage.Data[6]) | (((WORD)(IncomingMessage.Data[7]))<<8);
							
							
							LEDColor[0] = w1 & 0x3F;
							LEDColor[1] = (w1 & 0x0FC0) >> 6;
							LEDColor[2] = ((w1 & 0xF0) >> 12) + ((w2 & 0x03) << 4);
						
							LEDStartColor[0]=LEDColor[0];
							LEDStartColor[1]=LEDColor[1];
							LEDStartColor[2]=LEDColor[2];
						
							LEDColorChange[0] = ((w2 & 0x00FC) >> 2 ) - LEDColor[0];
							LEDColorChange[1] = ((w2 & 0x3F00) >> 8) - LEDColor[1];
							LEDColorChange[2] = ( ((w2 & 0xC000) >> 14) +
								((w3 & 0x000F) << 2) ) - LEDColor[2];
						
							LEDTimer = (w3 & 0x3FF0) >> 4;	
							LEDFadeTime = ((w3 & 0xC000) >> 14) +
								((w4 & 0xFF) << 2);
							LEDFadeTimer= 0;
							
							LEDOverride =  FALSE;
							
							}
			break;
			
			case PLAY_AUDIO_STREAM:
			// Audio Enable Command with timeouts
			if 	(((IncomingMessage.Data[0]) == MyNodeNumber) || ((IncomingMessage.Data[0]) == 0xFF))
			{
				
				
				MyAudioStream = IncomingMessage.Data[1] & 0x3;
				AudioTimeoutStream=(IncomingMessage.Data[1] >>2) & 0x3;									
				AudioOn = (IncomingMessage.Data[1] >> 4) & 0x1;
				AudioTimeoutStatus=(IncomingMessage.Data[1] >>5) & 0x1;	
				AudioTimeout= (WORD)(IncomingMessage.Data[2]) | ((WORD)(IncomingMessage.Data[3])<<8);
				MyAudioVolume = IncomingMessage.Data[4];
				AudioTimeoutVolume=IncomingMessage.Data[5];
				AudioTimer=0;
				
			}
			break;


			//Sync Messages, keep
			case SYNC_NODE_TO_STREAM:
			// Reset AudioReadIndex WHEN STREAM DATA ARRIVES & WRITES AUDIO AT 0x80
			if (IncomingMessage.Data[0] == MyNodeNumber) 
			{
				AudioReadIndexReset=1;
			}
			break;

			case SYNC_ALL_TO_STREAM:
			if (MyAudioStream == (IncomingMessage.Data[0] & 0x3))
			{
				AudioReadIndexReset=1;
			}
			break; 


			case  NOOP:			// No operation code
				break;

		
			case  AUDIO_OFF_ALL_NODES:
				// Audio off all nodes 
				AudioOn=0;			// ALL AUDIO OFF
				AudioTimeout=0xFFFF;
			break;
	

			case PLAY_INTERNAL_SOUND:
			 
				if((IncomingMessage.Data[0] == MyNodeNumber) || ( IncomingMessage.Data[0] == 0xFF)  )
				{
			
					PlayInternalSound(IncomingMessage.Data[1],
									 IncomingMessage.Data[2],
									 IncomingMessage.Data[3], 
									 IncomingMessage.Data[4]&0x3,
									 IncomingMessage.Data[5], 
								     IncomingMessage.Data[6]&0x1);
				}
				
				
			break;
			
			case RESET_SLAVES:
			// Reset all
				ResetAll();
			break;

			case SET_SLAVE_TO_ADDRESS:
			// Set slave address when slave button pushed
			//and store to eeprom
				DiagTentativeAddress=IncomingMessage.Data[0];
				DiagAddressSetup=TRUE;
			
			break;

			case CANCEL_SET_SLAVE_TO_ADDRESS:
			//Cancel diagnostic 
				DiagAddressSetup=FALSE;
			break;
			
			case NODE_OPERATIONS:
					
					if((IncomingMessage.Data[1] == MyNodeNumber))
					{
						switch(IncomingMessage.Data[0])
						{
							
							case NODE_READ_SUPPLY_VOLTAGE:
							 {
							   		SupplyVoltageTemp = ADCRead();
							        		
							   		InitCANMsg(&TempCANMsg);
									TempCANMsg.SID=NODE_OPERATIONS;
									TempCANMsg.Data[0] = NODE_SUPPLY_VOLTAGE_RESPONSE;
									TempCANMsg.Data[1] = MyNodeNumber;
									TempCANMsg.Data[2] = SupplyVoltageTemp;
									TempCANMsg.Data[3] = SupplyVoltageTemp>>8;
									
									CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
								   	
								}
							
							break;
							
							case NODE_LOOP_BACK:
								
									InitCANMsg(&TempCANMsg);
									TempCANMsg.SID=NODE_OPERATIONS;
									TempCANMsg.Data[0] = NODE_LOOP_BACK_RESPONSE;
									TempCANMsg.Data[1] = MyNodeNumber;
									TempCANMsg.Data[2] = IncomingMessage.Data[2];
									TempCANMsg.Data[3] = IncomingMessage.Data[3];
									TempCANMsg.Data[4] = IncomingMessage.Data[4];
									TempCANMsg.Data[5] = IncomingMessage.Data[5];
									TempCANMsg.Data[6] = IncomingMessage.Data[6];
									TempCANMsg.Data[7] = IncomingMessage.Data[7];
									CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
							break;
							 
							case NODE_PING:
							
							LED_YELLOW;
							
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_OPERATIONS;
							TempCANMsg.Data[0] = NODE_PONG;
							TempCANMsg.Data[1] = MyNodeNumber;
							TempCANMsg.Data[2] = rand();
							TempCANMsg.Data[3] = rand();
							TempCANMsg.Data[4] = rand();
							TempCANMsg.Data[5] = rand();
							TempCANMsg.Data[6] = rand();
							TempCANMsg.Data[7] = rand();
							
							PING_DELAY_TIMER=0;
							PingDelayTime = 50 + (rand()&0x7f);
							
							while(PING_DELAY_TIMER < PingDelayTime)
							{
								CLEAR_WATCHDOG;	
							}
							
							CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
						
						    LED_GREEN;
						
							
							break;
								
							case NODE_STATISCALLY_ACCEPT_UID:
							Rand=rand();
							if(Rand<0x8000)
							{
								MyUID = ((DWORD)(IncomingMessage.Data[2])) + 
										((DWORD)(IncomingMessage.Data[3])<<8) +
										((DWORD)(IncomingMessage.Data[4])<<16) +
										((DWORD)(IncomingMessage.Data[5])<<24); 
								
							}
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_OPERATIONS;
							TempCANMsg.Data[0] = NODE_PONG;
							TempCANMsg.Data[1] = MyNodeNumber;
							TempCANMsg.Data[2] = MyUID;
							TempCANMsg.Data[3] = MyUID>>8;
							TempCANMsg.Data[4] = MyUID>>16;
							TempCANMsg.Data[5] = MyUID>>24;
							TempCANMsg.Data[6] = 0;
							TempCANMsg.Data[7] = 0;
							CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
						
							break;
							
							case NODE_UNCONDITIONALLY_ACCEPT_UID: 
						
								MyUID = ((DWORD)(IncomingMessage.Data[2])) + 
										((DWORD)(IncomingMessage.Data[3])<<8) +
										((DWORD)(IncomingMessage.Data[4])<<16) +
										((DWORD)(IncomingMessage.Data[5])<<24); 
								
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_OPERATIONS;
							TempCANMsg.Data[0] = NODE_PONG;
							TempCANMsg.Data[1] = MyNodeNumber;
							TempCANMsg.Data[2] = MyUID;
							TempCANMsg.Data[3] = MyUID>>8;
							TempCANMsg.Data[4] = MyUID>>16;
							TempCANMsg.Data[5] = MyUID>>24;
							TempCANMsg.Data[6] = 0;
							TempCANMsg.Data[7] = 0;
							CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
						
							break;
									
							default:
							break;
						}
					}
				
				break;
		
		    	case GENERAL_PURPOSE_BUFFER_OPERATION:
			
					if(IncomingMessage.Data[0] == MyNodeNumber)
					{
						switch(IncomingMessage.Data[1])
							{
							 	case  CLEAR_BUFFER:	
							 		for(i=0;i<GP_BUFFER_SIZE;i++)
							 		{
								 		GeneralPurposeBuffer[i] = 0;	
								 	}
							
								InitCANMsg(&TempCANMsg);
								TempCANMsg.SID=GENERAL_PURPOSE_BUFFER_OPERATION;
								TempCANMsg.Data[0] = MyNodeNumber;
								TempCANMsg.Data[1] = (ACK_CMD|CLEAR_BUFFER);
								CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
							
							 			
					        	break;
					        	
					         	case  SET_BUFFER_ACCEPTANCE_FLAG:
					         		GeneralPurposeBufferAcceptanceFlag = TRUE;
					         		InitCANMsg(&TempCANMsg);
									TempCANMsg.SID=GENERAL_PURPOSE_BUFFER_OPERATION;
									TempCANMsg.Data[0] = MyNodeNumber;
									TempCANMsg.Data[1] = (ACK_CMD|SET_BUFFER_ACCEPTANCE_FLAG);
									CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
								
		
							 	break;
					        	
					        	case  CLEAR_BUFFER_ACCEPTANCE_FLAG:
					        		GeneralPurposeBufferAcceptanceFlag = FALSE;
					        		InitCANMsg(&TempCANMsg);
									TempCANMsg.SID=GENERAL_PURPOSE_BUFFER_OPERATION;
									TempCANMsg.Data[0] = MyNodeNumber;
									TempCANMsg.Data[1] = (ACK_CMD|CLEAR_BUFFER_ACCEPTANCE_FLAG);
									CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
								
					        	break;
					        	
					         	case  READ_BUFFER_SEGMENT:	
					         	
					         		if(GeneralPurposeBufferAcceptanceFlag == TRUE)
					         		{
						         		BufferBaseAddress = (IncomingMessage.Data[2]) * 8;
						         		InitCANMsg(&TempCANMsg);
										TempCANMsg.SID=GP_BUFFER_READ_LOWER_CAN_ID_BOUND + (IncomingMessage.Data[2]);
										for(i=0;i<8;i++)
										{
											TempCANMsg.Data[i] = GeneralPurposeBuffer[BufferBaseAddress+i];
										}
										
										CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
									
						         			
						         	}
			         		}
		
	         		}
	         	break;
	        			
          	  	case   READ_BUFFER_ENTIRE_BUFFER:
          	  	break;
          	  	
          	  	default:
          	  	break;
			
			
		
		}
		}
	}
}






/* Transmitted SID word is in a strange format with 3 blank bytes in the middle */
WORD CANMakeTxSID (WORD SID_Unformatted)
{
return ( ((SID_Unformatted & 0x3F) << 2) + ((SID_Unformatted & 0x7C0) << 5) );
}