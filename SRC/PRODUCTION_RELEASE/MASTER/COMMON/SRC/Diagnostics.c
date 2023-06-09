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
#include "BoardSupport.h"
#include "EEPROMRoutines.h"

#define PING_DELAY_TIMER 		GPTimer[1]
#define BOOTUP_SOUND_TIMER		GPTimer[2]
#define BUTTON_BLINK_TIMER		GPTimer[3]
#define COMPLETION_WAIT_TIMER	GPTimer[4]
#define RESPONSE_RESET_TIMER	GPTimer[5]
#define DIAGNOSTIC_TIMER 		GPTimer[6]

#define PONG_RESPONSE_TIME 		400
#define PING_DELAY_TIME 		10  
#define DATA_RATE_THRES 		320
#define VBUS_10V				3100
#define VBUS_8V					2480

static CANMsg DiagnosticCANMsg;

volatile BYTE NumPongs;
volatile NodeInfo MyNodes[NUM_BUTTONS];

static WORD ErrorCode;
BYTE DiagnosticsState = INIT;
static BYTE PingRetries;
static BOOL NodesAddressed;
static BYTE NodeToPing;
static BYTE NodeToAddress = 0;
static DWORD BusVoltage = 0;
static WORD BusVoltageMeasurementCnt = 0;
static BYTE PowerSequenceButton=0;
DWORD PacketsRecieved = 0;
DWORD PacketsSent = 0;
static DWORD AverageBusSpeed;

volatile BOOL LoopBackResponse = FALSE;
volatile BYTE NodeTestData[8];
volatile BOOL WaitingForTestDataResponse = FALSE;

BYTE DataRateMeasurementCount = 0;
BYTE CurrentNodeForBusMeasurement = 0;
static BYTE ErrorFlash = 0;


void DiagnosticsPlayButtonFeebackSound();
void PlayBootupSound();
void WriteTestDataToNode(BYTE Node);
void RequestNodeVoltages();
void ResetNodeDataRateInfo();
void ResetNodeBusVoltageInfo();
WORD GetVoltageDrop();

#define PLAY_BOOTUP_SOUND			DiagnosticsState = BOOTUP_SOUND;PlayBootupSound()

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
		
			RED_LED_ON;
			GREEN_LED_OFF;
		
			#ifdef SKIP_DIAGNOSTICS
			
				PLAY_BOOTUP_SOUND;

			#else
				DiagnosticsState = CHECK_FOR_ALL_NODES_PRESENT;
			#endif
		break;
		
		
		case DATA_BUS_TEST:
			DiagnosticsState = NEXT_DATA_BUS_TEST;
			InhibitAudio = TRUE;
			DataRateMeasurementCount = 0;
			CurrentNodeForBusMeasurement = 0;
			ResetNodeDataRateInfo();
		break;
		
	
		
		case NEXT_DATA_BUS_TEST:
		
		
			if(DataRateMeasurementCount < 4)
			{
				LEDSendVariable(DISPLAY_DIA, DIAGNOSTIC_DATA_RATE_BASE_CODE + CurrentNodeForBusMeasurement);
				LEDSendMessage(CurrentNodeForBusMeasurement,YELLOW,YELLOW,0,0);
				
				LoopBackResponse = TRUE;
				WriteTestDataToNode(CurrentNodeForBusMeasurement);
				DiagnosticsState = CHECK_FOR_TEST_DATA_RESPONSE;
				PacketsRecieved = 0;
				PacketsSent = 0;
				DIAGNOSTIC_TIMER = 0;
			}
			else
			{
				MyNodes[CurrentNodeForBusMeasurement].DataRate = MyNodes[CurrentNodeForBusMeasurement].DataRate/100;
				
				if(MyNodes[CurrentNodeForBusMeasurement].DataRate < DATA_RATE_THRES)
				{
					DiagnosticsState = DISPLAY_DATA_RATE_ERROR;	
					DIAGNOSTIC_TIMER = 0xFFFE;
					
				}
				else
				{
					LEDSendMessage(CurrentNodeForBusMeasurement,LEDOFF,LEDOFF,0,0);
					CurrentNodeForBusMeasurement++;
					if(CurrentNodeForBusMeasurement >= NUM_BUTTONS)
					{
							InhibitAudio = FALSE;
						DiagnosticsState = CHECK_POWER;
					}
					else
					{
						DiagnosticsState = NEXT_DATA_BUS_TEST;
						ResetNodeDataRateInfo();
						DataRateMeasurementCount = 0;
						
					}
				}
			}
		
	
		break;
		
			
		case CHECK_FOR_TEST_DATA_RESPONSE:
		
			if(DIAGNOSTIC_TIMER >= 25)
			{
				DIAGNOSTIC_TIMER = 0;
				AverageBusSpeed	= (PacketsSent + PacketsRecieved) *32 ;
				
				if(AverageBusSpeed > MyNodes[CurrentNodeForBusMeasurement].DataRate)
				{
				   MyNodes[CurrentNodeForBusMeasurement].DataRate = AverageBusSpeed;
				}
			
			
				DiagnosticsState = NEXT_DATA_BUS_TEST;
				DataRateMeasurementCount++;
			}
		 	if(	LoopBackResponse == TRUE)
		 	{
				 WriteTestDataToNode(CurrentNodeForBusMeasurement);
				 LoopBackResponse = FALSE;
				 PacketsSent++;
			}
		break;
		
		case DISPLAY_DATA_RATE_ERROR:
		
		if(DIAGNOSTIC_TIMER>100)
		{
			DIAGNOSTIC_TIMER = 0;
			
			if(ErrorFlash == 0)
			{
				ErrorFlash = 1;	
				LEDSendVariable(DISPLAY_ERR, DIAGNOSTIC_DATA_RATE_BASE_CODE + CurrentNodeForBusMeasurement);
				
			}
			else
			{
				ErrorFlash = 0;	
				LEDSendVariable(DISPLAY_ERR, MyNodes[CurrentNodeForBusMeasurement].DataRate);
			}
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
		
				
					SystemMode = SYSTEM_DIAGNOSTICS;
				
					#ifdef SKIP_200_TESTS
						PLAY_BOOTUP_SOUND;
					#else
						DiagnosticsState = CHECK_POWER;
					#endif

				}
				else
				{
				
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
		
		if(DIAGNOSTIC_TIMER > 50)
		{
			DIAGNOSTIC_TIMER = 0;
			LEDSendVariable(DISPLAY_ERR, ErrorCode);
		}
		break;
		
		
		case ADDRESS_BUTTONS:
			DiagnosticsPlayButtonFeebackSound(ENABLE_ALL);
			ResetLeds();
			DiagnosticsState=INIT_ADDRESS_ON_DISPLAY;
			NodeToAddress = 0;
			
		break;
		
		case INIT_ADDRESS_ON_DISPLAY:
			LEDSendScoreAddress(DISPLAY_ADDRESS,0xFF,NodeToAddress,NodeToAddress);
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
		    SystemMode = GAME_ACTIVE;
			DiagnosticsState = INIT;
		break;
		
		
		case CHECK_POWER:
	
			//First make sure everything is off
			
			ResetLeds();
			ScoreSendLights(DISPLAY_ADDRESS, 0x00,0x00);
			CANQueueTxMessage(0x70,DISPLAY_ADDRESS + (0x00 << 8),0x0000,0x0000,0x0000);
			LEDSendVariable(DISPLAY_DIA, POWER_ERROR_BASE_CODE);
			DiagnosticsState = POWER_SEQUENCE_NOTHING;
			PowerSequenceButton = 0;
			DIAGNOSTIC_TIMER = 0;
			BusVoltage = 0;
			BusVoltageMeasurementCnt = 0;
							
		break;
		
		
		case POWER_SEQUENCE_NOTHING:
		
			if(DIAGNOSTIC_TIMER > 5)
				{
					DIAGNOSTIC_TIMER = 0;
					BusVoltage += ADCRead();
					RequestNodeVoltages();
					BusVoltageMeasurementCnt++;	
				}
						
			if(BusVoltageMeasurementCnt >15 )
				{
					BusVoltage = BusVoltage>>4;
					DIAGNOSTIC_TIMER = 0;
					
					if(BusVoltage>VBUS_8V)
					{
						PowerSequenceButton = 0;
						LEDSendMessage(PowerSequenceButton,YELLOW,YELLOW,0,0);
						DiagnosticsState = POWER_SEQUENCE_BUTTONS;
						LEDSendMessage(PowerSequenceButton,YELLOW,YELLOW,0,0);
						BusVoltage = 0;
						DIAGNOSTIC_TIMER = 0;
						BusVoltageMeasurementCnt=0;
						LEDSendVariable(DISPLAY_DIA, POWER_ERROR_BASE_CODE + PowerSequenceButton);
				
					}
					else
					{
						ErrorCode = POWER_ERROR_BASE_CODE;
						DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;	
					}
				}
				
	      break;
		
		
		
		case POWER_SEQUENCE_BUTTONS:
		
			if(DIAGNOSTIC_TIMER > 5)
			{
				DIAGNOSTIC_TIMER = 0;
				BusVoltage += ADCRead();
				RequestNodeVoltages();
				BusVoltageMeasurementCnt++;	
				LEDSendVariable(DISPLAY_DIA, POWER_ERROR_BASE_CODE + PowerSequenceButton);
			}
					
			if(BusVoltageMeasurementCnt >15 )
			{
				BusVoltage = BusVoltage>>4;
				DIAGNOSTIC_TIMER = 0;
				
				if(BusVoltage>VBUS_8V)
					{
						PowerSequenceButton++;
					
						if(	PowerSequenceButton == NUM_BUTTONS)
						{
							ScoreSendLights(DISPLAY_ADDRESS, 0xFFFF,0xFFFF);
							CANQueueTxMessage(0x70,DISPLAY_ADDRESS + (0xFF << 8),0xFFFF,0xFFFF,0xFFFF);
							DiagnosticsState = POWER_SEQUENCE_SCORE_DISPLAY;
							BusVoltage = 0;
						
						}
						else
						{
							for(i=0;i<PowerSequenceButton+1;i++)
							{
								LEDSendMessage(PowerSequenceButton,YELLOW,YELLOW,0xFFFF,0x0000);
								LEDSendVariable(DISPLAY_DIA, POWER_ERROR_BASE_CODE + PowerSequenceButton);
								LEDSendMessage(PowerSequenceButton,YELLOW,YELLOW,0xFFFF,0x0000);
							}
						}
						
						BusVoltage = 0;
						DIAGNOSTIC_TIMER = 0;
						BusVoltageMeasurementCnt=0;
				
					}
				else
					{
						ErrorCode = POWER_ERROR_BASE_CODE + PowerSequenceButton + 1;
						DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;	
					}
				
				}
				
				
		break;
		
		
		case POWER_SEQUENCE_SCORE_DISPLAY:
		
			if(DIAGNOSTIC_TIMER > 5)
			{
				DIAGNOSTIC_TIMER = 0;
				BusVoltage += ADCRead();
				RequestNodeVoltages();
				BusVoltageMeasurementCnt++;	
			}
					
			if(BusVoltageMeasurementCnt >15 )
			{
				BusVoltage = BusVoltage>>4;
				DIAGNOSTIC_TIMER = 0;
				
				if(BusVoltage>VBUS_8V)
					{
						BOOTUP_SOUND_TIMER = 0;
						DiagnosticsState = POWER_SEQUENCE_EVERYTHING;
						BusVoltage = 0;
						DIAGNOSTIC_TIMER = 0;
						BusVoltageMeasurementCnt=0;
						LEDSendVariable(DISPLAY_DIA, POWER_ERROR_BASE_CODE + POWER_SEQUENCE_EVERYTHING_CODE);
						
					}
				else
					{
						ErrorCode = POWER_ERROR_BASE_CODE + POWER_SEQUENCE_DISPLAY_CODE;
						DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;	
					}
			}
	
		break;
		
		case POWER_SEQUENCE_EVERYTHING:
		
			if(DIAGNOSTIC_TIMER > 5)
			{
				DIAGNOSTIC_TIMER = 0;
				BusVoltage += ADCRead();
				BusVoltageMeasurementCnt++;	
				RequestNodeVoltages();
			}
					
			if(BusVoltageMeasurementCnt >15 )
			{
				BusVoltage = BusVoltage>>4;
				DIAGNOSTIC_TIMER = 0;
				
			   if(BusVoltage>VBUS_8V)
				{						
					if(GetVoltageDrop() > VBUS_8V)
					{
						PLAY_BOOTUP_SOUND;
					}
					else
					{
						ErrorCode = POWER_ERROR_BASE_CODE + POWER_SEQUENCE_CHECK_BUS_DROP;
						DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;
					}
					
				}
				else
				{
					AudioOffAllNodes();
					ErrorCode = POWER_ERROR_BASE_CODE + POWER_SEQUENCE_EVERYTHING_CODE;
					DiagnosticsState = DISPLAY_ERROR_CODE_FOREVER;	
				}
			}
		break;
		
		
		case BOOTUP_SOUND:
		
			if(BOOTUP_SOUND_TIMER>BUILDUP_A_WAV_LENGTH)
			{
				ResetAudioAndLEDS();
				GameState =  INIT;
				GameSelected = GAME_ROOT_GAME0;
				SystemMode = GAME_ACTIVE;
				CANQueueTxMessage(CANCEL_SET_SLAVE_TO_ADDRESS ,0,0,0,0);	//Make sure they are out of setup!
				CANQueueTxMessage(CANCEL_SET_SLAVE_TO_ADDRESS ,0,0,0,0);	//Make sure they are out of setup!
				
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

void OnButtonPressDiagnosticsMode(BYTE button)
{
	switch(DiagnosticsState)
	{
		case WAIT_FOR_ADDRESSING_REPONSE:
		
		
			LEDSendMessage(NodeToAddress,GREEN,GREEN,50,0);
			//Audio feedback
			DiagnosticsPlayButtonFeebackSound(NodeToAddress);
			NodeToAddress++;
			LEDSendScoreAddress(248,0xFF,NodeToAddress,NodeToAddress);
			COMPLETION_WAIT_TIMER=0;
			DiagnosticsState=CHECK_FOR_ADDRESSING_COMPLETION;
	
		break;
		
		default:
		break;
	}

}


void OnSelectPressDiagnosticsMode(WORD sel)
{

}

void DiagnosticsPlayButtonFeebackSound()
{

  PlayInternalNodeSound(0xFF,INTERNAL_SOUND_POSITIVE_FEEDBACK,0xFF,1,0,0,0);

}	


void PlayBootupSound()
{

 	BYTE k;
 	
 	for(k=0;k<3;k++)
	{
	AudioNodeEnable((2*k),0,0,AUDIO_ON_BEFORE_TIMEOUT,BUILDUP_A_WAV_LENGTH,AudioGlobalVolume,0);
	SendNodeNOP();
	AudioNodeEnable(((2*k)+1),1,1,AUDIO_ON_BEFORE_TIMEOUT,BUILDUP_B_WAV_LENGTH,AudioGlobalVolume,0);
	SendNodeNOP();
 	}
	InhibitAudio = FALSE;               
	SendNodeNOP();	
	EAudioPlaySound(0,BUILDUP_A_WAV );
	SendNodeNOP();
	EAudioPlaySound(1,BUILDUP_B_WAV);
		
	BOOTUP_SOUND_TIMER = 0;

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



void RequestNodeVoltages()
{
	BYTE i;
	
	for(i=0;i<NUM_BUTTONS;i++)
	{
		InitCANMsg(&DiagnosticCANMsg);
		DiagnosticCANMsg.SID = NODE_OPERATIONS;
		DiagnosticCANMsg.Data[1] = i;
		DiagnosticCANMsg.Data[0] = NODE_READ_SUPPLY_VOLTAGE;		
		CANMesssageEnqueue((CANMessageQueue *)&CANTxQueue,&DiagnosticCANMsg);	
	}
}	

WORD GetVoltageDrop()
{
	WORD MinVoltage = 0xFFFF;
	BYTE i;
	
	for(i=0;i<NUM_BUTTONS;i++)
	{
		if(MyNodes[i].BusVoltage < MinVoltage)
		{
			MinVoltage = MyNodes[i].BusVoltage;
		}
	}
	
	return MinVoltage;
	
}	

