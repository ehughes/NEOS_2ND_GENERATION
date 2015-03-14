#include "DataTypes.h"

#ifndef _USB_H
#define _USB_H

#define RX_BUFFER_SIZE      256

WORD UartTx(BYTE *Buf, WORD len);
void InitUSB(void);
BYTE GrabSingleRxByte();
WORD CopyFromRxBuf(BYTE *Buf, WORD Cnt);
WORD RxAvailable();
WORD TxBusy();
void FlushRxBuf();
void FlushTxBuf();

WORD CalcCRC(BYTE *data, WORD length);

#endif

