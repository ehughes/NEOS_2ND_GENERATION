#include "DataTypes.h"

#ifndef _PACKET_DETECT_H
#define _PACKET_DETECT_H

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



void ScanForIncomingMessages();
void ProcessMessage(BYTE *Message, WORD MessageLength);
void ScanForIncomingMessages();
void InitPacketDetect();
BYTE PacketDetect(BYTE DataIn);

#endif

