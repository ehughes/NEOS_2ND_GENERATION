void UARTInit(void);
void UART1TxFormat (unsigned long addr, unsigned char command);
void UARTMakeChecksum (unsigned char array[] );
int UARTRxVerify( unsigned char array[] );
void UART1RxProcess (void);
void UART1ReplyCheck(void);




