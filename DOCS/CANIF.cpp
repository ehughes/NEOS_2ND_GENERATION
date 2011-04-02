//Data Type Declarations
#define BYTE unsigned char 		// BYTES are 8-bits
#define WORD unsigned short 	//WORDs are 2 bytes
#define DWORD unsigned long	   //DWORDs are 4 bytes   
#define BOOL BYTE	           //Booleans are bytes where 0 is false and non-zero is true

struct
{
	WORD Header = 0xAAAA;	// Message Start/Sync Header
	WORD PayloadLength;		//This the Length of the Payload only
	BYTE MessagePayload[PayloadLength]; //Byte array of the Payload
	WORD CRC;				//16-bit CRC CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1)
							//The CRC is calculated on entire message frame (Header+Length+Payload)
} MessageTransport;


//Example Routine for calculating the 
//CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1)

WORD CalcCRC(BYTE *data, WORD length)
{
	crc = 0xffff;
	for (i=0;i<length;i++)
	{
		crc = (unsigned char)(crc >> 8) | (crc << 8); 
		crc ^= data[i];
		crc ^= (unsigned char)(crc & 0xff) >> 4;
		crc ^= (crc << 8) << 4;
		crc ^= ((crc & 0xff) << 4) << 1;
	}
	return crc;
}

#define  CAN_INTERFACE_PING		0x00

struct 
{
	BYTE MessageId = CAN_INTERFACE_PING;
} CANInterfacePing;



#define  CAN_INTERFACE_PONG			0x11

struct 
{
	BYTE MessageId = CAN_INTERFACE_PONG;
} CANInterfacePong;


#define  CAN_INTERFACE_RESET 				0x01

struct 
{
	BYTE MessageId = CAN_INTERFACE_RESET;
} CANInterfaceReset;


#define  SET_ID_EXCLUDE_RANGE				0x05

struct 
{
	BYTE MessageId = SET_ID_EXCLUDE_RANGE;
	DWORD IDExcludeLow;  //Lower bound of the CAN message exclude range
	DWORD IDExcludeHigh; //Upper Bound of the CAN message exclude range
} SetIDExcludeRange;



#define  SET_CAN_TERMINATION		0x06

struct 
{
	BYTE MessageId = SET_CAN_TERMINATION;
	BOOL EnableInternalTermination;  // 0x00 (false)    -->	No Internal Termination
									 // non-zero (true) --> Can transciever on Adapter terminated to 100ohms
} SetCANTermination;


#define  TX_CAN_MSG					0x03

struct 
{
	BYTE MessageId = TX_CAN_MSG;
	DWORD CANId;						 // 11-bit CAN ID (Right Justified in SWORD)
	BYTE CanData[8]					 // 8-bytes of CAN Data
} TXCanMsg;



#define  RX_CAN_MSG					0x04

struct 
{
	BYTE MessageId = TX_CAN_MSG;
	DWORD CANId;						 // 11-bit CAN ID (Right Justified in SWORD)
	BYTE CanData[8]					 // 8-bytes of CAN Data
} TXCanMsg;

