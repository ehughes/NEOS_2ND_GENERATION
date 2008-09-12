#include "DataTypes.h"

#ifndef PACKET_PROCESS_H_
#define PACKET_PROCESS_H_


void InitPacketProcess();
void ProcessPacket();
void ProcessIncomingUSBMessages();




extern unsigned char PacketDetect(unsigned char DataIn);

void InitPacketDetect();


#endif


