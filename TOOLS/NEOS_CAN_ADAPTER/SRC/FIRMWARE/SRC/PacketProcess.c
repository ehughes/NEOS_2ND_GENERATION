#include "DataTypes.h"
#include "PacketDetect.h"
#include "PacketProcess.h"


struct {
			unsigned char Payload[512];
			unsigned short Length;
		} ActivePacket;
	
		





void ProcessPacket()
{

	GetPacket(&ActivePacket.Payload[0],&ActivePacket.Length);
	
	
	//This next state machine will process the packet and look for valid commands
	
	//The first byte indicates a command
	switch(ActivePacket.Payload[0])
	{
	
	}
	
}
	
	

void ProcessIncomingMessages()
{

	
	  
}




		
		
		
	
	
