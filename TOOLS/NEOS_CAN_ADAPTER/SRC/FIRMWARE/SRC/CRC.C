#include "CRC.h"
#include "DataTypes.h"

// Calculate the CRC for a packet using
// the CCITT 16bit algorithm (X^16 + X^12 + X^5 + 1)

WORD i,crc;

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



