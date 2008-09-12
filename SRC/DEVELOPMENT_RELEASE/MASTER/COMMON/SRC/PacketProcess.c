#include "PacketProcess.h"
#include "DataTypes.h"
#include "USB.h"
#include "USBCommands.h"
#include "SystemVariables.h"
#include "Flash.h"
#include "Audio.h"
#include "LED-Display.h"
#include "config.h"


	
	
#define MAX_PACKET_SIZE 512

#define H1 0xAA
#define H2 0xAA

//State Machine Macros
#define SCAN_FOR_HEADER1                0x00
#define SCAN_FOR_HEADER2                0x01
#define GRAB_LENGTH_LOW                 0x02
#define GRAB_LENGTH_HIGH                0x03
#define GRAB_PAYLOAD                    0x04
#define GRAB_CRC_LOW                    0x05
#define GRAB_CRC_HIGH                   0x06

//Return MACROS
#define NO_PACKET_DETECTED              0x00
#define PACKET_DETECTED                 0x01
#define BAD_CRC						    0x02

		
unsigned char PacketResponse [300];
unsigned short TempCRC;
unsigned char ReadCheck[256];
unsigned long Page;
unsigned char UnknownResponse [16];
unsigned char FirmwareResponse [16];
unsigned char WritingPageResponse [16];
unsigned char WriteCompleteResponse [16];
unsigned char WriteFailureResponse [16];

unsigned char FLASHStatusByte=0;
unsigned short ij;
unsigned short Retries = 0;




struct {

		 unsigned char  Header1;
		 unsigned char  Header2;
         unsigned short Length;
		 unsigned char  Payload[MAX_PACKET_SIZE];
		 unsigned short CRC;
       
       } IncomingPacket;

unsigned char DetectState=SCAN_FOR_HEADER1;
unsigned short DataCnt=0;
unsigned short CheckCRC;

void InitPacketDetect()
{

DetectState=SCAN_FOR_HEADER1;
DataCnt=0;
CheckCRC=0;
	
}



BYTE ReadTemp[256];

void InitPacketProcess()
{

Retries = 0;

// does some packet calculations for static response

FirmwareResponse[0] = 0xAA;
FirmwareResponse[1] = 0xAA;
FirmwareResponse[2] = 0x3;
FirmwareResponse[3] = 0x0;
FirmwareResponse[4] = GET_FIRMWARE_VERSION;
FirmwareResponse[5] = FIRMWARE_MAJOR;
FirmwareResponse[6] = FIRMWARE_MINOR;
TempCRC = CalcCRC( &FirmwareResponse[0], 7);
FirmwareResponse[7] = (unsigned char)TempCRC;
FirmwareResponse[8] = (unsigned char)(TempCRC>>8);

WritingPageResponse[0] = 0xAA;
WritingPageResponse[1] = 0xAA;
WritingPageResponse[2] = 0x1;
WritingPageResponse[3] = 0x0;
WritingPageResponse[4] =  WRITE_PAGE ;
TempCRC = CalcCRC( &WritingPageResponse[0], 5);
WritingPageResponse[5] = (unsigned char)TempCRC;
WritingPageResponse[6] = (unsigned char)(TempCRC>>8);

WriteCompleteResponse[0] = 0xAA;
WriteCompleteResponse[1] = 0xAA;
WriteCompleteResponse[2] = 0x1;
WriteCompleteResponse[3] = 0x0;
WriteCompleteResponse[4] =  WRITING_COMPLETE ;
TempCRC = CalcCRC( &WriteCompleteResponse[0], 5);
WriteCompleteResponse[5] = (unsigned char)TempCRC;
WriteCompleteResponse[6] = (unsigned char)(TempCRC>>8);

WriteFailureResponse[0] = 0xAA;
WriteFailureResponse[1] = 0xAA;
WriteFailureResponse[2] = 0x1;
WriteFailureResponse[3] = 0x0;
WriteFailureResponse[4] =  WRITING_ERROR ;
TempCRC = CalcCRC( &WriteCompleteResponse[0], 5);
WriteFailureResponse[5] = (unsigned char)TempCRC;
WriteFailureResponse[6] = (unsigned char)(TempCRC>>8);

UnknownResponse[0] = 0xAA;
UnknownResponse[1] = 0xAA;
UnknownResponse[2] = 0x1;
UnknownResponse[3] = 0x0;
UnknownResponse[4] =  UNKNOWN_COMMAND ;
TempCRC = CalcCRC( &UnknownResponse[0], 5);
UnknownResponse[5] = (unsigned char)TempCRC;
UnknownResponse[6] = (unsigned char)(TempCRC>>8);



}
		

unsigned char PacketDetect(unsigned char DataIn)
{
   //Detect Status will be our return value to indicate if a packet is
   //available to copy.
   unsigned char DetectStatus = 0;

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
              IncomingPacket.Length += ((unsigned short)DataIn)<<8;
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
              IncomingPacket.CRC = (unsigned short)DataIn;
              DetectState = GRAB_CRC_HIGH;
              DetectStatus =  NO_PACKET_DETECTED;
      break;
      
      case GRAB_CRC_HIGH:
              IncomingPacket.CRC += (unsigned short)(DataIn)<<8;
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



void ProcessPacket()
{
	WORD SoundNum=0;
	WORD SoundLen = 0;
	BYTE SoundStream = 0;

	//This next state machine will process the packet and look for valid commands
	//The first byte indicates a command
	switch(IncomingPacket.Payload[0])
	{
		case PLAY_SOUND:
		if(IncomingPacket.Length == 6)
			{
				SoundNum = IncomingPacket.Payload[1];
				SoundNum |= (WORD)(IncomingPacket.Payload[2]<<8);
				
				SoundLen = IncomingPacket.Payload[3];
				SoundLen |= (WORD)(IncomingPacket.Payload[4]<<8);
				
				SoundStream = IncomingPacket.Payload[5];
				
				AudioNodeEnable(0xFF,SoundStream,SoundStream,AUDIO_ON_BEFORE_TIMEOUT,SoundLen,0xFF,0x00);
				LEDSendMessage(0xFF,RED,RED,100,0);	
				EAudioPlaySound(SoundStream,SoundNum);
			}
		break;
		
		case GET_FIRMWARE_VERSION:
		
			if(IncomingPacket.Length == 1)
			{
				UartTx(&FirmwareResponse[0],9);
			}	
			
			else
			{
				UartTx(&UnknownResponse[0],7);
			}
		
				
		break;
		
			
		case SWITCH_SYSTEM_MODE:
		
			if(IncomingPacket.Length == 2)
			{
				SystemMode =	IncomingPacket.Payload[1] ;
			}	
							
		break;
	
			
		
	case READ_PAGE:
								
			if(IncomingPacket.Length == 3)
			{
		   	
	 	
			
			PacketResponse[0]=0xAA;
			PacketResponse[1]=0xAA;
			PacketResponse[2]=0x01;
			PacketResponse[3]=0x01;
			PacketResponse[4] = READ_PAGE;
			
			
			//Make the LSB Zero as we are doing page aligned read
			Page =  (unsigned long)(IncomingPacket.Payload[1])<<8;
			Page |= ((unsigned long)(IncomingPacket.Payload[2])<<16)& 0xFF0000;
						
			M25PXX_READ(Page,&PacketResponse[5]);
				
			TempCRC=CalcCRC(&PacketResponse[0],261);
						
			PacketResponse[261] = (unsigned char)TempCRC;
			PacketResponse[262] = (unsigned char)(TempCRC>>8);
			
			UartTx(&PacketResponse[0],263);
			

						
													
			}			
			else
			{
				UartTx(&UnknownResponse[0],7);
			}
		
				
		break;	
		
	case WRITE_PAGE:
								
			if(IncomingPacket.Length == 260)
			{
		  		     Page = 0;				   					
					//Make the LSB Zero as we are doing page aligned read
					Page =  (unsigned long)(IncomingPacket.Payload[1])<<8;
					Page |= (((unsigned long)(IncomingPacket.Payload[2])<<16)& 0xFF0000);
					Page |= (((unsigned long)(IncomingPacket.Payload[3])<<24)& 0xFF000000);
					
			//THis code is very spaghetti like
				//	Retries = 0;
					
		//	FlashRetry:	
			
				M25PXX_PP(Page,&IncomingPacket.Payload[4]);
				 	
				
				BlockUntilFlashOperationComplete(Page);	
						
				/*	M25PXX_READ(Page, &ReadCheck[0]);	
						
					for (ij=0;ij<256;ij++)
					{
						
						if ( ReadCheck[ij] != IncomingPacket.Payload[ij+3])
						{
							Retries++;
							if(Retries < MAX_FLASH_RETRIES)
							{
								goto FlashRetry;
							}
							else
							{
								//Send a Message that Flash write Failed	
								while(TxBusy()>0)
									{
									}
											
									UartTx(&WriteFailureResponse[0],7);
								goto FlashComplete;
							}
							
						}
					}	
						*/
					while(TxBusy()>0)
						{
						}
											
						UartTx(&WriteCompleteResponse[0],7);
								
			//	FlashComplete:
								
						ij=0;
							
					
												
			}			
			else
			{
				UartTx(&UnknownResponse[0],7);
			}
		
				
		break;
	
		
		
		case SECTOR_ERASE:
								
			if(IncomingPacket.Length == 3)
			{
		   	   	
					//Make the LSB Zero as we are doing page aligned read
					Page = 0;
					Page = ((DWORD)(IncomingPacket.Payload[1])<<16) | ((DWORD)(IncomingPacket.Payload[2])<<24);
								
					M25PXX_SE(Page);
					
					//Block until write complete.
					
					BlockUntilFlashOperationComplete(Page);
						
						
						while(TxBusy()>0)
							{
							}
											
							UartTx(&WriteCompleteResponse[0],7);
								
												
			}			
			else
			{
				UartTx(&UnknownResponse[0],7);
			}
		
				
		break;
		
				
		case SOFTWARE_RESET:
	
		break;
		
	
		default:
				UartTx(&UnknownResponse[0],7);
				
		break;	
	}	
	
	
	
}
	
	

void ProcessIncomingUSBMessages()
{
	WORD NumBytesToRead,i;
	
	 NumBytesToRead = RxAvailable();
      
	  if(NumBytesToRead>0)
	  {
	  	
	  	if(NumBytesToRead>256)
	  	{
	  		
	  	CopyFromRxBuf(&ReadTemp[0],256);
	  	}
	  	
	  	else
	  	{
	  	
	  	CopyFromRxBuf(&ReadTemp[0],NumBytesToRead);
	  	
	  	
	  	}
	  	
	  	for(i=0;i<NumBytesToRead;i++)
	  	{
	  		if(PacketDetect(ReadTemp[i])==PACKET_DETECTED)
	  	 		{
	  	 			ProcessPacket();
	  	 		}
	  	}
	  	
	  }
	 
	
	  
}




		
		
		
	
	
