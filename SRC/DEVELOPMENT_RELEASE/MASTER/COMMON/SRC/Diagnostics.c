#include "CANRoutines.h"
#include "TimerRoutines.h"
#include "DataTypes.h"
#include "CANCommands.h"
#include "SystemVariables.h"
#include "_GameVariables.h"
#include "Diagnostics.h"
#include "ErrorCodes.h"
#include "LED-Display.h"
#include "SystemUtilities.h"
#include "GameMapping.h"
#include "Audio.h"
#include "SoundMapping.h"
#include "InternalButtonSoundMapping.h"
#include "ADCRoutines.h"
#include "stdlib.h"

CANMsg DiagnosticCANMsg;

#define DIAGNOSTIC_TIMER GPTimer[8]
#define PING_DELAY_TIMER GPTimer[1]
#define BOOTUP_SOUND_TIMER GPTimer[2]

#define BUTTON_BLINK_TIMER		GPTimer[3]
#define COMPLETION_WAIT_TIMER	GPTimer[4]
#define RESPONSE_RESET_TIMER	GPTimer[5]


#define PONG_RESPONSE_TIME 	400
#define PING_DELAY_TIME 10  

volatile BYTE NumPongs;
volatile NodeInfo MyNodes[NUM_BUTTONS];

WORD ErrorCode;
BYTE DiagnosticsState = INIT;
BYTE PingRetries;
BOOL NodesAddressed;
BYTE NodeToPing;
BYTE NodeToAddress = 0;
WORD BusVoltage = 0;
WORD BusVoltageMeasurementCnt = 0;
BYTE PowerSequenceState=0;
DWORD PacketsRecieved = 0;
DWORD PacketsSent = 0;

DWORD AverageBusSpeed;
volatile BOOL LoopBackResponse = FALSE;

volatile BYTE NodeTestData[8];
volatile BOOL WaitingForTestDataResponse = FALSE;

BYTE DataRateMeasurementCount = 0;
BYTE CurrentNodeForBusMeasurement = 0;


#define CHECK_FOR_ALL_NODES_PRESENT 	1
#define PING_NODES						2
#define WAIT_FOR_PONGS					3
#define DISPLAY_ERROR_CODE_FOREVER		4
#define ADDRESS_BUTTONS					5
#define INIT_ADDRESS_ON_DISPLAY 		6
#define ADDRESS_NEXT_NODE	 			7
#define WAIT_FOR_ADDRESSING_REPONSE	 	8
#define CHECK_FOR_ADDRESSING_COMPLETION 9 
#define EXIT_ADDRESS_SETUP 				10
#define CHECK_POWER						11
#define POWER_SEQUENCE					12

#define DATA_BUS_TEST					13
#define NEXT_DATA_BUS_TEST			14
#define CHECK_FOR_TEST_DATA_RESPONSE	15


#define CMD_SET_BUFFER_ACCEPTANCE_FLAG		16
#define CMD_CLEAR_BUFFER_ACCEPTANCE_FLAG	17
#define CMD_READ_NODE_GP_BUFFER				18

#define VBUS_10V				3100

void DiagnosticsPlayButtonFeebackSound();
void PlayBootupSound();
void WriteTestDataToNode(BYTE Node);


void ResetNodeDataRateInfo();
void ResetNodeBusVoltageInfo();


void EnterAddressingMode()
{
	
SystemMode = SYSTEM_DIAGNOSTICS;
DiagnosticsState = 	ADDRESS_BUTTONS;	
	
}	

void SystemsDiagnostics()
{
	BYTE i;
	
	switch (DiagnosticsState)
	{
		case INIT:
		
			DiagnosticsState = CHECK_FOR_ALL_NODES_PRESENT;
			//DiagnosticsState = DATA_BUS_TEST;
			
		break;
		
		
		case DATA_BUS_TEST:
			DiagnosticsState = NEXT_DATA_BUS_TEST;
			InhibitAudio = TRUE;
			DataRateMeasurementCount = 0;
			CurrentNodeForBusMeasurement = 0;
		break;
		
		
		case NEXT_DATA_BUS_TEST:
		
		
			if(DataRateMeasurementCount < 4)
			{
				LoopBackResponse = TRUE;
				WriteTestDataToNode(0);
				DiagnosticsState = CHECK_FOR_TEST_DATA_RESPONSE;
				PacketsRecieved = 0;
				PacketsSent = 0;
				DIAGNOSTIC_TIMER = 0;
			}
			else
			{
				LEDSendVariable(DISPLAY_DIA,  MyNodes[CurrentNodeForBusMeasurement].DataRate/100);
				DiagnosticsState = DATA_BUS_TEST;
				ResetNodeDataRateInfo();
			}
		
	
		break;
		
			
		case CHECK_FOR_TEST_DATA_RESPONSE:
		
			if(DIAGNOSTIC_TIMER >= 25)
			{
				DIAGNOSTIC_TIMER = 0;
				AverageBusSpeed	= (PacketsSent + PacketsRecieved) * 2;
				
				if(AverageBusSpeed > MyNodes[CurrentNodeForBusMeasurement].DataRate)
				{
				   MyNodes[CurrentNodeForBusMeasurement].DataRate = AverageBusSpeed;
				}
			
				DiagnosticsState = NEXT_DATA_BUS_TEST;
				DataRateMeasurementCount++;
			}
		 	if(	LoopBackResponse == TRUE)
		 	{
				 WriteTestDataToNode(0);
				 LoopBackResponse = FALSE;
				 PacketsSent++;
			}
		break;
		
			
		
		case CHECK_FOR_ALL_NODES_PRESENT:
			InhibitAudio = FALSE;
			PingRetries = 0;
			ResetNodeInfo();
			NodeToPing = 0;
			NodesAddressed = FALSE;
			DIAGNOSTIC_TIMER = 0;
			PING_DELAY_TIMER = 0;
			DiagnosticsState = PING_NODES;
		break;
				
		case PING_NODES:
		
			if(PING_DELAY_TIMER > PING_DELAY_TIME)
			{
				LEDSendVariable(DISPLAY_DIA, DIAGNOSTIC_PING_BASE_CODE + PingRetries + 1);
				PING_DELAY_TIMER = 0;
				InitCANMsg(&DiagnosticCANMsg);
				DiagnosticCANMsg.SID = NODE_OPERATIONS;
				DiagnosticCANMsg.Data[0] = NODE_PING;
				DiagnosticCANMsg.Data[1] = NodeToPing;
				CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&DiagnosticCANMsg);
				NodeToPing++;
				
				if(NodeToPing > (NUM_BUTTONS-1))
				{
					DiagnosticsState = WAIT_FOR_PONGS;
					DIAGNOSTIC_TIMER = 0;
				}
			}
		break;
		
		case WAIT_FOR_PONGS:
		
			if(NumPongs == NUM_BUTTONS)
			{
				//Check to see if if there are 8 unique addressed	
				NodesAddressed = TRUE;
				for(i=0;i<NUM_BUTTONS;i++)
				{
					if(MyNodes[i].Pongs != 1)	
					{
						NodesAddressed = FALSE;
					}
				}

				if(NodesAddressed == TRUE)
				{
					//Boot the Game, Everthing is OK!
				//	SystemMode = GAME_ACTIVE;
				//	GameState=BOOT;			
				//	GameSelected = GAME_ROOT_GAME0;
					DiagnosticsState = CHECK_POWER;
					SystemMode = SYSTEM_DIAGNOSTICS;
				
				}
				else
				{
					//If we are not addressed, go directly into the addressing Mode
					DiagnosticsState = ADDRESS_BUTTONS;
					SystemMode = SYSTEM_DIAGNOSTICS;
	   			}
			}
			 else
			 {
				if(DIAGNOSTIC_TIMER>PONG_RESPONSE_TIME)
					{
						DIAGNOSTIC_TIMER = 0;
						PingRetries++;
						if(PingRetries > 3)
						{
							//There isn't the correct number of nodes out there so display an error message and wait forever
							ErrorCode = PING_ERROR_BASE_CODE + 	NumPongs;
							SystemMode = SYSTEM_DIAGNOSTICS;
							DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;
						}
						else
						{
							NodeToPing = 0;
							PING_DELAY_TIMER = 0;
							ResetNodeInfo();
							DiagnosticsState = PING_NODES;
						}
						
					}
			 }
		
		break;
		
		
		case DISPLAY_ERROR_CODE_FOREVER:
		
			LEDSendVariable(DISPLAY_ERR, ErrorCode);
		
		break;
		
		
		case ADDRESS_BUTTONS:
			DiagnosticsPlayButtonFeebackSound(ENABLE_ALL);
			ResetLeds();
			DiagnosticsState=INIT_ADDRESS_ON_DISPLAY;
			NodeToAddress = 0;
			
		break;
		
		case INIT_ADDRESS_ON_DISPLAY:
			LEDSendScoreAddress(DISPLAY_ADDRESS,ScoreBrightness,NodeToAddress,NodeToAddress);
			DiagnosticsState=ADDRESS_NEXT_NODE;
		break;	
		
		case ADDRESS_NEXT_NODE:
			//If a node (or nodes!) already has this address, light it
			LEDSendMessage(NodeToAddress,RED,RED,100,0);		//Red if already addressed
			//Send out message to grab this address
			CANQueueTxMessage(SET_SLAVE_TO_ADDRESS,NodeToAddress,0,0,0);	//Send message to grab address
			BUTTON_BLINK_TIMER=0;
			DiagnosticsState=WAIT_FOR_ADDRESSING_REPONSE;
		break;
		
		case WAIT_FOR_ADDRESSING_REPONSE:
			//Flash light appropriately			
			if (BUTTON_BLINK_TIMER >= 50)
			{
				BUTTON_BLINK_TIMER=0;
				LEDSendMessage(NodeToAddress,RED,RED,25,0);
			}
		break;
		
		case CHECK_FOR_ADDRESSING_COMPLETION:
			//Wait for acknowledge beep/flash to end
			if (COMPLETION_WAIT_TIMER >= 20)
			{
				if (NodeToAddress >(NUM_BUTTONS-1))
					DiagnosticsState=EXIT_ADDRESS_SETUP;
				else
				DiagnosticsState=ADDRESS_NEXT_NODE;		//Go back & do another light
			}
		break;
		
		case EXIT_ADDRESS_SETUP:
			CANQueueTxMessage(CANCEL_SET_SLAVE_TO_ADDRESS ,0,0,0,0);	//Make sure they are out of setup!
			CANQueueTxMessage(CANCEL_SET_SLAVE_TO_ADDRESS ,0,0,0,0);	//Make sure they are out of setup!
			SystemMode = SYSTEM_DIAGNOSTICS;
			DiagnosticsState = INIT;
		break;
		
		
		case CHECK_POWER:
	
			//First make sure everything is off
			
			ResetLeds();
			ScoreSendLights(DISPLAY_ADDRESS, 0x00,0x00);
			CANQueueTxMessage(0x70,DISPLAY_ADDRESS + (0x00 << 8),0x0000,0x0000,0x0000);
			
			DiagnosticsState = POWER_SEQUENCE;
			PowerSequenceState = 0;
			DIAGNOSTIC_TIMER = 0;
			BusVoltage = 0;
			BusVoltageMeasurementCnt = 0;
							
		break;
		
		case POWER_SEQUENCE:
		
		if(DIAGNOSTIC_TIMER > 5)
		{
			DIAGNOSTIC_TIMER = 0;
			BusVoltage += ADCRead();
			BusVoltageMeasurementCnt++;	
		}
				
		if(BusVoltageMeasurementCnt >15 )
		{
			BusVoltage = BusVoltage>>4;
			DIAGNOSTIC_TIMER = 0;
					
			switch(PowerSequenceState)
			{
			 	case 0:case 1:case 2:case 3:case 4:case 5:case 6:case 7:
										
					if(BusVoltage>VBUS_10V)
					{
						LEDSendMessage(PowerSequenceState,YELLOW,YELLOW,0,0);
						PowerSequenceState++;	
					}
					else
					{
						ErrorCode = POWER_ERROR_BASE_CODE + PowerSequenceState;
						DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;	
					}
					//Test
				break;
				
				case 8:
					if(BusVoltage>VBUS_10V)
					{
						ScoreSendLights(DISPLAY_ADDRESS, 0xFFFF,0xFFFF);
						CANQueueTxMessage(0x70,DISPLAY_ADDRESS + (0xFF << 8),0xFFFF,0xFFFF,0xFFFF);
						PowerSequenceState++;	
					}
					else
					{
						ErrorCode = POWER_ERROR_BASE_CODE + PowerSequenceState;
						DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;	
					}
				break;
			
			
				case 9:
				
					if(BusVoltage>VBUS_10V)
					{
						PlayBootupSound();	
						PowerSequenceState++;	
						BOOTUP_SOUND_TIMER = 0;
					}
					else
					{
						ErrorCode = POWER_ERROR_BASE_CODE + PowerSequenceState;
						DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;	
					}
				
				break;
			
				case 10:
				
					if(BusVoltage>VBUS_10V)
					{						
						if(BOOTUP_SOUND_TIMER>BUILDUP_A_WAV_LENGTH )
						{
							ResetAudioAndLEDS();
							GameState =  INIT;
							GameSelected = GAME_ROOT_GAME0;
							SystemMode = GAME_ACTIVE;
							CANQueueTxMessage(CANCEL_SET_SLAVE_TO_ADDRESS ,0,0,0,0);	//Make sure they are out of setup!
							CANQueueTxMessage(CANCEL_SET_SLAVE_TO_ADDRESS ,0,0,0,0);	//Make sure they are out of setup!
						}
					}
					else
					{
						AudioOffAllNodes();
						ErrorCode = POWER_ERROR_BASE_CODE + PowerSequenceState;
						DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;	
					}
				
				break;
							
				default:
					PowerSequenceState =0;
					break;	
			}
			
			BusVoltage = 0;
			DIAGNOSTIC_TIMER = 0;
			BusVoltageMeasurementCnt=0;
		}
		
		break;
				
		default:
			SystemMode = SYSTEM_DIAGNOSTICS;
			DiagnosticsState = INIT;
		break;
	
	}
	
}	


void ResetNodeDataRateInfo()
{
	BYTE i;
	
	for(i=0;i<NUM_BUTTONS;i++)
	{		
		MyNodes[i].DataRate = 0;
	}
	
}	

void ResetNodeBusVoltageInfo()
{
	BYTE i;
	
	for(i=0;i<NUM_BUTTONS;i++)
	{		
		MyNodes[i].DataRate = 0;
	}
	
}	


void ResetNodeInfo()
{
	BYTE i;
	NumPongs = 0;
	for(i=0;i<NUM_BUTTONS;i++)
	{		
		MyNodes[i].Pongs = 0;
		MyNodes[i].DataRate = 0;
		MyNodes[i].BusVoltage = 0;
		
	}
	
}	


//When a button press comes back acknowledging address, 
//Flash an acknowledge & move on to the next

void OnButtonPressDiagnosticsMode(unsigned char button)
{
	switch(DiagnosticsState)
	{
		case WAIT_FOR_ADDRESSING_REPONSE:
		
		
			LEDSendMessage(NodeToAddress,GREEN,GREEN,50,0);
			//Audio feedback
			DiagnosticsPlayButtonFeebackSound(NodeToAddress);
			NodeToAddress++;
			LEDSendScoreAddress(248,ScoreBrightness,NodeToAddress,NodeToAddress);
			COMPLETION_WAIT_TIMER=0;
			DiagnosticsState=CHECK_FOR_ADDRESSING_COMPLETION;
		break;
		
		default:
		break;
	}

}

//Select button advances without defining an address
void OnSelectPressDiagnosticsMode(unsigned int sel)
{

}

void DiagnosticsPlayButtonFeebackSound()
{

  PlayInternalNodeSound(0xFF,INTERNAL_SOUND_POSITIVE_FEEDBACK,0xFF,1,0,0,0);

}	


void PlayBootupSound()
{

 	BYTE k;
 	for(k=0;k<4;k++)
	{
	AudioNodeEnable((2*k),0,0,AUDIO_ON_BEFORE_TIMEOUT,BUILDUP_A_WAV_LENGTH,0xFF,0);
	SendNodeNOP();
	AudioNodeEnable(((2*k)+1),1,1,AUDIO_ON_BEFORE_TIMEOUT,BUILDUP_B_WAV_LENGTH,0xFF,0);
	SendNodeNOP();
 	}
	
	SendNodeNOP();	
	EAudioPlaySound(0,BUILDUP_A_WAV );
	SendNodeNOP();
	EAudioPlaySound(1,BUILDUP_B_WAV);	

}	


void WriteTestDataToNode(BYTE Node)
{
	BYTE i;

	InitCANMsg(&DiagnosticCANMsg);

	DiagnosticCANMsg.SID = NODE_OPERATIONS;
	DiagnosticCANMsg.Data[1] = Node;
	DiagnosticCANMsg.Data[0] = NODE_LOOP_BACK;
	
	for(i=2;i<8;i++)
	{
		DiagnosticCANMsg.Data[i] = rand()*0xFF;
	}
	
	CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&DiagnosticCANMsg);
}	



