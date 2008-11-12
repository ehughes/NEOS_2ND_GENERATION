#include "DataTypes.h"
#include "SystemVariables.h"
#include "config.h"

#ifndef _DIAGNOSTICS_H
#define _DIAGNOSTICS_H

extern volatile BYTE NumPongs;

typedef struct {
				
				BYTE Pongs;
				DWORD DataRate;
				WORD BusVoltage;
		
} NodeInfo;	


extern volatile NodeInfo MyNodes[NUM_BUTTONS];
extern BYTE DiagnosticsState;


void SystemsDiagnostics();
void ResetNodeInfo();

void EnterAddressingMode();

void OnSelectPressDiagnosticsMode(WORD sel);
void OnButtonPressDiagnosticsMode(BYTE button);

extern DWORD PacketsRecieved;
extern DWORD PacketsSent;
volatile extern BOOL LoopBackResponse;


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
#define POWER_SEQUENCE_NOTHING			13
#define POWER_SEQUENCE_BUTTONS			14
#define POWER_SEQUENCE_SCORE_DISPLAY	15
#define POWER_SEQUENCE_EVERYTHING		16
#define BOOTUP_SOUND					17
#define DATA_BUS_TEST					18
#define NEXT_DATA_BUS_TEST				19
#define CHECK_FOR_TEST_DATA_RESPONSE	20
#define DISPLAY_DATA_RATE_ERROR			21

#endif
