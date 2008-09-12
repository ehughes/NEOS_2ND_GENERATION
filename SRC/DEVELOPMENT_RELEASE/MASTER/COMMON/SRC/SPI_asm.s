
.include "P30F6014A.INC"

.global _asmSPITransfer

.section .text
_asmSPITransfer:
	bclr	SPI1STAT,#SPIROV	;clear any old read overflow
	mov	W0,SPI1BUF	;Write SPI buffer
	mov	#0x100,W1
SPITransferLoop1:
	btsc	SPI1STAT,#SPIRBF	;id rx not full
	bra	SPITransfer1
	dec	W1,W1
	bra	NZ,SPITransferLoop1
SPITransfer1:
	mov	SPI1BUF,W0	;Move read data into W0 for return
	mov	#0x100,W1
SPITransferLoop2:	
	btss	SPI1STAT,#SPITBF	;id rx not full
	bra	SPITransfer2
	dec	W1,W1
	bra	NZ,SPITransferLoop2
SPITransfer2:

	return		;return with W0 read result
	

	.end
