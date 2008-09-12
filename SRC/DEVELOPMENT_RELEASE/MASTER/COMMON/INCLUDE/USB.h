#ifndef _USB_H
#define _USB_H


#define RX_BUFFER_SIZE      1024

WORD UartTx(unsigned char *Buf, unsigned short len);
void InitUSB(void);
BYTE GrabSingleRxByte();
WORD CopyFromRxBuf(unsigned char *Buf, unsigned int Cnt);
WORD RxAvailable();
WORD TxBusy();
void FlushRxBuf();
void FlushTxBuf();

WORD CalcCRC(BYTE *data, WORD length);

#endif

