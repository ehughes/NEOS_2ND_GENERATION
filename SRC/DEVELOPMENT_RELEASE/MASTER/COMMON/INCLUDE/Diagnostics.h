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

void OnSelectPressDiagnosticsMode(unsigned int sel);
void OnButtonPressDiagnosticsMode(unsigned char button);

extern DWORD PacketsRecieved;
extern DWORD PacketsSent;
volatile extern BOOL LoopBackResponse;

#endif
