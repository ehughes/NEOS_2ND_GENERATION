#include <can.h>
#include "CANRoutines.h"
#include "SystemVariables.h"
#include <p30fxxxx.h>
#include "LED-Display.h"
#include "_RootButtonHandler.h"
#include "CANCommands.h"
#include "TimerRoutines.h"
#include "FLASH.h"
#include "stdlib.h"
#include "Diagnostics.h"
#include "_GameVariables.h"
#include "ADCRoutines.h"

CANMsg TempCANMsg;
CANMsg IncomingCANMsg;
CANMsg RxIRQCANMsg;
CANMsg TxIRQCANMsg;

volatile CANMessageQueue CANRxQueue,CANTxQueue;

#define CAN_REJECTION_RANGE_LOW 	0x100
#define CAN_REJECTION_RANGE_HIGH	0x1FF


#define CAN_TX_QUEUE_SIZE	148
#define CAN_RX_QUEUE_SIZE	128

CANMsg InternalTxMessageQueueStorage[CAN_TX_QUEUE_SIZE];
CANMsg InternalRxMessageQueueStorage[CAN_RX_QUEUE_SIZE];

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




// * Process pending receive messages */
void CANRxProcess(void)
{
	WORD i,z,MessagesToProcess;
	WORD BufferBaseAddress;
	DWORD FlashAddress;
	
	MessagesToProcess = GetNumMessagesInQueue((CANMessageQueue *)&CANRxQueue);
	
    for(z=0;z<MessagesToProcess;z++)
	{
		
		CANMesssageDequeue((CANMessageQueue *)&CANRxQueue,&IncomingCANMsg);
		
		if((IncomingCANMsg.SID>=GP_BUFFER_WRITE_LOWER_CAN_ID_BOUND) && (IncomingCANMsg.SID<=GP_BUFFER_WRITE_UPPPER_CAN_ID_BOUND) && (GeneralPurposeBufferAcceptanceFlag == TRUE))
		{
		     	BufferBaseAddress = (IncomingCANMsg.SID - GP_BUFFER_WRITE_LOWER_CAN_ID_BOUND) * 8;
		       	
		       	for(i=0;i<8;i++)
		       	{
			       GeneralPurposeBuffer[BufferBaseAddress + i] = IncomingCANMsg.Data[i];	
			    }
		       	
		     	InitCANMsg(&TempCANMsg);
				TempCANMsg.SID=GENERAL_PURPOSE_BUFFER_OPERATION;
				TempCANMsg.Data[0] = MASTER_NODE_ADDRESS;
				TempCANMsg.Data[1] = ACK_CMD|WRITE_BUFFER_ACK;
				TempCANMsg.Data[2] = IncomingCANMsg.SID - GP_BUFFER_WRITE_LOWER_CAN_ID_BOUND;
				
				CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
				
		}
	
	    else
	    {
	    	switch (IncomingCANMsg.SID)
			{
		
				case NODE_OPERATIONS:
				
				if(IncomingCANMsg.Data[0] == NODE_PONG)
				{
					NumPongs++;
					
					MyNodes[IncomingCANMsg.Data[1]].Pongs++;
				}
				
				if(IncomingCANMsg.Data[0] == NODE_LOOP_BACK_RESPONSE)
				{
					PacketsRecieved++;
					LoopBackResponse = TRUE;
					
					
				}
				
				
				if((IncomingCANMsg.Data[0] == NODE_SUPPLY_VOLTAGE_RESPONSE) && (IncomingCANMsg.Data[1]!= MASTER_NODE_ADDRESS))
				{
					MyNodes[IncomingCANMsg.Data[1]].BusVoltage = ((WORD)(IncomingCANMsg.Data[3])<<8)  + ((WORD)(IncomingCANMsg.Data[2]));
					
				}
				
				if(IncomingCANMsg.Data[1] == MASTER_NODE_ADDRESS)
					{
								
						switch(IncomingCANMsg.Data[0])
							{
								case NODE_LOOP_BACK:
									InitCANMsg(&TempCANMsg);
									TempCANMsg.SID=NODE_OPERATIONS;
									TempCANMsg.Data[0] = NODE_LOOP_BACK_RESPONSE;
									TempCANMsg.Data[1] = MASTER_NODE_ADDRESS;
									TempCANMsg.Data[2] = IncomingCANMsg.Data[2];
									TempCANMsg.Data[3] = IncomingCANMsg.Data[3];
									TempCANMsg.Data[4] = IncomingCANMsg.Data[4];
									TempCANMsg.Data[5] = IncomingCANMsg.Data[5];
									TempCANMsg.Data[6] = IncomingCANMsg.Data[6];
									TempCANMsg.Data[7] = IncomingCANMsg.Data[7];
									CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
								break;
						
								case  NODE_READ_SUPPLY_VOLTAGE:
									SupplyVoltageTemp = ADCRead();
									InitCANMsg(&TempCANMsg);
									TempCANMsg.SID=NODE_OPERATIONS;
									TempCANMsg.Data[0] = NODE_SUPPLY_VOLTAGE_RESPONSE;
									TempCANMsg.Data[1] = MASTER_NODE_ADDRESS;
									TempCANMsg.Data[2] = SupplyVoltageTemp;
									TempCANMsg.Data[3] = SupplyVoltageTemp>>8;
									CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
								break;
								
								default:
								break;
							}
						}
					break;	
							
				/*	
					if((IncomingCANMsg.Data[1] == MASTER_NODE_ADDRESS) || (IncomingCANMsg.Data[1] == 0xFF))
					{
						switch(IncomingCANMsg.Data[0])
						{
							case NODE_PING:
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_OPERATIONS;
							TempCANMsg.Data[0] = NODE_PONG;
							TempCANMsg.Data[1] = MASTER_NODE_ADDRESS;
							TempCANMsg.Data[2] = MyUID;
							TempCANMsg.Data[3] = MyUID>>8;
							TempCANMsg.Data[4] = MyUID>>16;
							TempCANMsg.Data[5] = MyUID>>24;
							TempCANMsg.Data[6] = 0;
							TempCANMsg.Data[7] = 0;
							CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
						
						                      
								
							break;
								
							case NODE_STATISCALLY_ACCEPT_UID:
							
							if(rand()<0x8000)
							{
								MyUID = ((DWORD)(IncomingCANMsg.Data[2])) + 
										((DWORD)(IncomingCANMsg.Data[3])<<8) +
										((DWORD)(IncomingCANMsg.Data[4])<<16) +
										((DWORD)(IncomingCANMsg.Data[5])<<24); 
								
							}
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_OPERATIONS;
							TempCANMsg.Data[0] = NODE_PONG;
							TempCANMsg.Data[1] = MASTER_NODE_ADDRESS;
							TempCANMsg.Data[2] = MyUID;
							TempCANMsg.Data[3] = MyUID>>8;
							TempCANMsg.Data[4] = MyUID>>16;
							TempCANMsg.Data[5] = MyUID>>24;
							TempCANMsg.Data[6] = 0;
							TempCANMsg.Data[7] = 0;
							CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
						
							break;
							
							case NODE_UNCONDITIONALLY_ACCEPT_UID: 
						
								MyUID = ((DWORD)(IncomingCANMsg.Data[2])) + 
										((DWORD)(IncomingCANMsg.Data[3])<<8) +
										((DWORD)(IncomingCANMsg.Data[4])<<16) +
										((DWORD)(IncomingCANMsg.Data[5])<<24); 
								
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_OPERATIONS;
							TempCANMsg.Data[0] = NODE_PONG;
							TempCANMsg.Data[1] = MASTER_NODE_ADDRESS;
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
				*/
			
		
		    	case GENERAL_PURPOSE_BUFFER_OPERATION:
			
					if(IncomingCANMsg.Data[0] == MASTER_NODE_ADDRESS)
					{
						switch(IncomingCANMsg.Data[1])
							{
							 	case  CLEAR_BUFFER:	
							 		for(i=0;i<GP_BUFFER_SIZE;i++)
							 		{
								 		GeneralPurposeBuffer[i] = 0;	
								 	}
							
								InitCANMsg(&TempCANMsg);
								TempCANMsg.SID=GENERAL_PURPOSE_BUFFER_OPERATION;
								TempCANMsg.Data[0] = MASTER_NODE_ADDRESS;
								TempCANMsg.Data[1] = (ACK_CMD|CLEAR_BUFFER);
								CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
							
							 			
					        	break;
					        	
					         	case  SET_BUFFER_ACCEPTANCE_FLAG:
					         		GeneralPurposeBufferAcceptanceFlag = TRUE;
					         		InitCANMsg(&TempCANMsg);
									TempCANMsg.SID=GENERAL_PURPOSE_BUFFER_OPERATION;
									TempCANMsg.Data[0] = MASTER_NODE_ADDRESS;
									TempCANMsg.Data[1] = (ACK_CMD|SET_BUFFER_ACCEPTANCE_FLAG);
									CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
								
		
							 	break;
					        	
					        	case  CLEAR_BUFFER_ACCEPTANCE_FLAG:
					        		GeneralPurposeBufferAcceptanceFlag = FALSE;
					        		InitCANMsg(&TempCANMsg);
									TempCANMsg.SID=GENERAL_PURPOSE_BUFFER_OPERATION;
									TempCANMsg.Data[0] = MASTER_NODE_ADDRESS;
									TempCANMsg.Data[1] = (ACK_CMD|CLEAR_BUFFER_ACCEPTANCE_FLAG);
									CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
							   	break;
					        	
					         	case  READ_BUFFER_SEGMENT:	
					         	
					         		if(GeneralPurposeBufferAcceptanceFlag == TRUE)
					         		{
						         		BufferBaseAddress = (IncomingCANMsg.Data[2]) * 8;
						         		InitCANMsg(&TempCANMsg);
										TempCANMsg.SID=GP_BUFFER_READ_LOWER_CAN_ID_BOUND + (IncomingCANMsg.Data[2]);
										for(i=0;i<8;i++)
										{
											TempCANMsg.Data[i] = GeneralPurposeBuffer[BufferBaseAddress+i];
										}
										
										CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
									
						         			
						         	}
						      
			         	}
			         	
	         	break;
	        			
          	  	case   READ_BUFFER_ENTIRE_BUFFER:
          	  	break;
          	  	
          	  	default:
          	  	break;
	        			
			
				
			}
			
			break;
			
		
			
		    case NODE_FLASH_MEMORY_OPERATION:
		    
		    if(IncomingCANMsg.Data[0] == MASTER_NODE_ADDRESS)
		    {
			    FlashAddress =  ((DWORD)(IncomingCANMsg.Data[2]))+
							    ((DWORD)(IncomingCANMsg.Data[3])<<8)+
							    ((DWORD)(IncomingCANMsg.Data[4])<<16)+
							    ((DWORD)(IncomingCANMsg.Data[5])<<24);
							    
				switch(IncomingCANMsg.Data[1])
				{
					
					case WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER:
							M25PXX_PP(FlashAddress,&GeneralPurposeBuffer[0]);
							BlockUntilFlashOperationComplete(FlashAddress);	
							
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_FLASH_MEMORY_OPERATION;
							TempCANMsg.Data[0] = MASTER_NODE_ADDRESS;
							TempCANMsg.Data[1] = (ACK_CMD|WRITE_PAGE_FROM_GENERAL_PURPOSE_BUFFER);
							TempCANMsg.Data[2] = FlashAddress;
							TempCANMsg.Data[3] = FlashAddress>>8;
							TempCANMsg.Data[4] = FlashAddress>>16;
							TempCANMsg.Data[5] = FlashAddress>>24;
							CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
					
					break;
								
					case READ_PAGE_INTO_GENERAL_PURPOSE_BUFFER:
					
							M25PXX_READ(FlashAddress,&GeneralPurposeBuffer[0]);
							M25PXX_PP(FlashAddress,&GeneralPurposeBuffer[0]);
							
							BlockUntilFlashOperationComplete(FlashAddress);	
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_FLASH_MEMORY_OPERATION;
							TempCANMsg.Data[0] = MASTER_NODE_ADDRESS;
							TempCANMsg.Data[1] = (ACK_CMD|READ_PAGE_INTO_GENERAL_PURPOSE_BUFFER);
							TempCANMsg.Data[2] = FlashAddress;
							TempCANMsg.Data[3] = FlashAddress>>8;
							TempCANMsg.Data[4] = FlashAddress>>16;
							TempCANMsg.Data[5] = FlashAddress>>24;
							CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
						
					break;
					
					case ERASE_SECTOR:
					
							M25PXX_SE(FlashAddress);
							BlockUntilFlashOperationComplete(FlashAddress);
							InitCANMsg(&TempCANMsg);
							TempCANMsg.SID=NODE_FLASH_MEMORY_OPERATION;
							TempCANMsg.Data[0] = MASTER_NODE_ADDRESS;
							TempCANMsg.Data[1] = (ACK_CMD|ERASE_SECTOR);
							TempCANMsg.Data[2] = FlashAddress;
							TempCANMsg.Data[3] = FlashAddress>>8;
							TempCANMsg.Data[4] = FlashAddress>>16;
							TempCANMsg.Data[5] = FlashAddress>>24;
							CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);
						
					break;
					
					default:
					break;
					
				}
			}
		    
		    break;
		    
			case SET_MASTER_SYSTEM_MODE:
			
				SystemMode = IncomingCANMsg.Data[0];
				DiagnosticsState = INIT;
			break;
			
	
			
			case  NOOP:			// No operation code
				break;
	
			// button press on & off messages
			case SLAVE_BUTTON_PRESSED:
				OnButtonPress(IncomingCANMsg.Data[0]);
			break;
			
			case SLAVE_BUTTON_RELEASED:
			break;

			// Score display button change
			case SCORE_BUTTONS_CHANGED:
		
					ScoreBoardButtons[0] = IncomingCANMsg.Data[2];
					ScoreBoardButtons[1] = IncomingCANMsg.Data[3];
					ScoreBoardButtons[2] = IncomingCANMsg.Data[4];	
					SelectSwitchCheck();		
				
			break;

		}
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



/* Queue a 4-word message to CAN TX buffer */
void CANQueueTxMessage(WORD sid, WORD word1,
WORD word2, WORD word3, WORD word4)
{
	
	CANMsg Temp;
	
	
		Temp.SID = sid;
		
	
			Temp.Data[0] =   word1 &0xFF;
			Temp.Data[1] = ( word1>>8) &0xFF;
			Temp.Data[2] =   word2 &0xFF;
			Temp.Data[3] = ( word2>>8) &0xFF;
			Temp.Data[4] =   word3&0xFF;
			Temp.Data[5] = ( word3>>8) &0xFF;
			Temp.Data[6] =   word4 &0xFF;
			Temp.Data[7] = ( word4>>8) &0xFF;
		
		CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&Temp);
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

