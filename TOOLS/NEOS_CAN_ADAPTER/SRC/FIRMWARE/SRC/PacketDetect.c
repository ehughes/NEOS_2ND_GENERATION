#include "PacketDetect.h"
#include "CRC.h"
#include "DataTypes.h"
#include "BoardSupport.h"
#include "Messages.h"
#include "CANRoutines.h"
#include "SystemVariables.h"

struct {

		 BYTE  Header1;
		 BYTE  Header2;
         WORD  Length;
		 BYTE  Payload[MAX_PACKET_SIZE];
		 WORD CRC;
       
       } IncomingPacket;

BYTE DetectState=SCAN_FOR_HEADER1;
WORD DataCnt=0;
WORD CheckCRC;

CANMsg TempCANMsg;

void ProcessMessage(BYTE *Message, WORD MessageLength)
{
	int i;
	
	if(MessageLength == 0)
	{
		return; 	
	}
	
	//First Byte is the Command
	
	switch(Message[0])
	{
		case TX_CAN_MSG:
		
			TempCANMsg.SID = (WORD)(Message[1])|(WORD)(Message[2]<<8);
		
			for(i=0;i<8;i++)
			{
				TempCANMsg.Data[i] = Message[5+i];
			}
		            
		     CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&TempCANMsg);        
             
             TX_LED_ON;
             
			break;

		case PING:

				TempBufToUSB[0] = 0xAA;
				TempBufToUSB[1] = 0xAA;
				TempBufToUSB[2] = 0x01;
				TempBufToUSB[3] = 0x00;
				TempBufToUSB[4] = CAN_INTERFACE_PONG;
						
				TempCRC = CalcCRC(&TempBufToUSB[0], 5);
		
				TempBufToUSB[5] = (BYTE)(TempCRC);
		        TempBufToUSB[6] = (BYTE)(TempCRC>>8);
		
		    	WriteBlockFT245(&TempBufToUSB[0],7);
		
			break;
			
		case SET_ID_EXCLUDE_RANGE:
		
				IDExcludeLow  = (WORD)(Message[1])|(WORD)(Message[2]<<8);
				IDExcludeHigh = (WORD)(Message[5])|(WORD)(Message[6]<<8);
	
			break;	
			
		case SET_CAN_TERMINATION:

				if(Message[1] == 0)
				{
					CAN_TERMINATION_OFF;
				}
				else
				{
					CAN_TERMINATION_ON;
				}
				
			break;
		
		case SYSTEM_RESET:
		
			SystemMode = RESET;
			
			break;
		
		default:
			break;
	}
	
	
}	

void ScanForIncomingMessages()
{

	while(FT245_DATA_AVAILABLE)
	{
		if(PacketDetect(ReadFT245()) == PACKET_DETECTED)
		{
			ProcessMessage(&IncomingPacket.Payload[0],IncomingPacket.Length);
		}
	
	}
}	

void InitPacketDetect()
{
	DetectState=SCAN_FOR_HEADER1;
	DataCnt=0;
	CheckCRC=0;
}

BYTE PacketDetect(BYTE DataIn)
{
   //Detect Status will be our return value to indicate if a packet is
   //available to copy.
   BYTE DetectStatus = 0;

    //implement out packet detection StateMachine
   switch(DetectState)
   {

      case SCAN_FOR_HEADER1:
         if(DataIn == H1)
			{
            	DetectState = SCAN_FOR_HEADER2;
     			IncomingPacket.Header1 = DataIn;	
			}   
	 	 else
         {
            DetectState = SCAN_FOR_HEADER1;
         }
         DetectStatus = NO_PACKET_DETECTED;

      break;

      case SCAN_FOR_HEADER2:
         if(DataIn == H2)
		  {
            DetectState = GRAB_LENGTH_LOW;
         	IncomingPacket.Header2 = DataIn;
	      }
		else
          {
            DetectState = SCAN_FOR_HEADER1;
          }
      break;

	
     case GRAB_LENGTH_LOW:
             
              IncomingPacket.Length = DataIn;
														
              DetectState = GRAB_LENGTH_HIGH;
               
              DetectStatus = NO_PACKET_DETECTED;

      break;
      
	case GRAB_LENGTH_HIGH:
              IncomingPacket.Length += ((WORD)DataIn)<<8;
			  DataCnt = 0;									
              DetectState = GRAB_PAYLOAD;
               
              DetectStatus = NO_PACKET_DETECTED;

      break;
	     

      case GRAB_PAYLOAD:
              IncomingPacket.Payload[DataCnt]=DataIn;
              DataCnt++;
			
              if(DataCnt == (IncomingPacket.Length))
                  DetectState = GRAB_CRC_LOW;
              else
                  DetectState = GRAB_PAYLOAD;

              DetectStatus = NO_PACKET_DETECTED;
      break;

      case GRAB_CRC_LOW:
              IncomingPacket.CRC = 0;
              IncomingPacket.CRC = (WORD)DataIn;
              DetectState = GRAB_CRC_HIGH;
              DetectStatus =  NO_PACKET_DETECTED;
      break;
      
      case GRAB_CRC_HIGH:
              IncomingPacket.CRC += (WORD)(DataIn)<<8;
              DetectState = SCAN_FOR_HEADER1;

			//Check CRC of everything (including header Bytes)

			 CheckCRC = CalcCRC(&IncomingPacket.Header1,IncomingPacket.Length+4);	

			  if(CheckCRC == IncomingPacket.CRC)
	           {
				   DetectStatus = PACKET_DETECTED;
    		  }
				else
			  {
				  DetectStatus = BAD_CRC;
				}
        break;
   

      default:
         DetectState = SCAN_FOR_HEADER1;
         DetectStatus = NO_PACKET_DETECTED;
      break;


   }


   return DetectStatus;

}
