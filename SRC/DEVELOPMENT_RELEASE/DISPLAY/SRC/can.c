#include <can.h>
#include "CANRoutines.h"
#include "SystemVariables.h"
#include <p30fxxxx.h>
#include "LED-Display.h"


/* CAN variables */
/* Ring buffer for Pending CAN transmit messages */
unsigned int CANTxBuff[175][5];	/* Up to 64 pending 5-word messages */
unsigned int CANTxBuffNextUp=0;		/* Index of next one up to TRANSMIT */
unsigned int CANTxBuffNextAvailable=0;	/* Index of next available location to fill */
/* Note if NextUp = NextAvailable, there is nothing in queue */
/* Put a new message in NextAvialable and increment NextAvailable */
/* If NextAvailable != NextUp then send NextUp & then inc NextUp */

unsigned int CANRxBuff[64][5]; // Up to 64 received messages
unsigned int CANRxBuffNextAvailable=0;	//Next available location to receive data
unsigned int CANRxNextToProcess=0;	//Next that needs processed

unsigned char CANTxBuffDifferentialMax=0;
unsigned char CANRxBuffDifferentialMax=0;

int DiagTentativeAddress=0;
unsigned char DiagAddressSetup=0;



/* CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN */
/* CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN */
/* CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN */
/* CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN */
/* CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN CAN */
void CANInit(void)
{
	/* Set request for configuration mode */
	CAN1SetOperationModeNoWait(CAN_IDLE_CON &  /* CAN on in idle mode */
	CAN_MASTERCLOCK_1 &					/* FCAN = FCY (16.384 MHZ) */
	CAN_REQ_OPERMODE_CONFIG &
	CAN_CAPTURE_DIS_NO_WAIT);		/* Disable capture */

	/* Waits for config to happen & confirm I think */
	while(C1CTRLbits.OPMODE <=3);
	
	/* Load configuration register */
	CAN1Initialize(CAN_SYNC_JUMP_WIDTH1 & /* Sync Jump = 1 TQ */
	CAN_BAUD_PRE_SCALE(1),			/* Pre-scale 1, TQ = 118ns */
	CAN_WAKEUP_BY_FILTER_DIS &		/* No filter on wake-up */
	CAN_PHASE_SEG2_TQ(3) &			/*Sync seg=1+prop seg=2+phase1=2+phase2=3 = 8TQ total */
	CAN_PHASE_SEG1_TQ(2) &			/* Phase seg 2 > jump width */
	CAN_PROPAGATIONTIME_SEG_TQ(2) &	
	CAN_SEG2_FREE_PROG &
	CAN_SAMPLE1TIME);			/* take one sample at sample point (not 3) */
	
	/* Load Acceptance filter register */
	CAN1SetFilter(0, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */
	CAN1SetFilter(1, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */
	CAN1SetFilter(2, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */
	CAN1SetFilter(3, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */
	CAN1SetFilter(4, CAN_FILTER_SID(0) &
	CAN_RX_EID_DIS, CAN_FILTER_EID(0));	/* Disable extended mask */

	/* Load mask filter register */
	CAN1SetMask(0, CAN_MASK_SID(0) &
	CAN_IGNORE_FILTER_TYPE, CAN_MASK_EID(0));
	CAN1SetMask(1, CAN_MASK_SID(0) &
	CAN_IGNORE_FILTER_TYPE, CAN_MASK_EID(0));


	CAN1SetTXMode(0,
	CAN_TX_STOP_REQ &
	CAN_TX_PRIORITY_HIGH );
	CAN1SetTXMode(1,
	CAN_TX_STOP_REQ &
	CAN_TX_PRIORITY_HIGH );
	CAN1SetTXMode(2,
	CAN_TX_STOP_REQ &
	CAN_TX_PRIORITY_HIGH );

	CAN1SetRXMode(0,
	CAN_RXFUL_CLEAR &
	CAN_BUF0_DBLBUFFER_EN);
	CAN1SetRXMode(1,
	CAN_RXFUL_CLEAR &
	CAN_BUF0_DBLBUFFER_EN);


	/* Enable interrupts, highest priority */
	ConfigIntCAN1(CAN_INDI_INVMESS_DIS & 
		CAN_INDI_WAK_DIS &
		CAN_INDI_ERR_EN &
		CAN_INDI_TXB2_DIS &
		CAN_INDI_TXB1_DIS &
		CAN_INDI_TXB0_DIS &
		CAN_INDI_RXB1_EN &
		CAN_INDI_RXB0_EN,
		CAN_INT_ENABLE &
		CAN_INT_PRI_6);

	CAN1SetOperationMode(CAN_IDLE_CON &  /* CAN on in idle mode */
	CAN_MASTERCLOCK_1 &					/* FCAN = FCY (16.384 MHZ) */
	CAN_REQ_OPERMODE_NOR &			/* Change to _LOOPBK to test */
	CAN_CAPTURE_DIS );

	while (C1CTRLbits.OPMODE !=0);	/* Wait for loopback mode to take */
}



/* CAN interrupt routine has to be fast - just recieve stuff into 
ring buffer */

/* 6/21/06 CAN receive interrupt is 5uS once every 122-123uS while transmitting one audio
audio stream */

/* the 32 messages needed to fill the 128 word audio buffer is taking 
dead on 4ms. The buffer empties every 8ms, so this is half the time!
*/

//void __attribute__((__interrupt__,__auto_psv__)) _C1Interrupt(void)
void __attribute__((__interrupt__,,__auto_psv__)) _C1Interrupt(void)
{
//	PORTAbits.RA13=1;    // Temporary to check interrupt time
	_C1IF=0;    /* Clear interrupt flag to acknowledge */    
// Parse the interrupt fast - maybe use the jump table?
	if (C1INTFbits.RX0IF==1)
	{
		CANRxBuff[CANRxBuffNextAvailable][0]=C1RX0SID;	//Standard Identifier
//		cantrap=CANRxBuff[CANRxBuffNextAvailable][0];
		CANRxBuff[CANRxBuffNextAvailable][1]=C1RX0B1;	//Bytes 1 & 0
		CANRxBuff[CANRxBuffNextAvailable][2]=C1RX0B2;	//Bytes 3 & 2
		CANRxBuff[CANRxBuffNextAvailable][3]=C1RX0B3;	//Bytes 5 & 4
		CANRxBuff[CANRxBuffNextAvailable][4]=C1RX0B4;	//Bytes 7 & 6
		CANRxBuffNextAvailable = ((CANRxBuffNextAvailable+1));
		if (CANRxBuffNextAvailable > CANRXBUFFMAX) CANRxBuffNextAvailable=0;
		C1INTFbits.RX0IF=0;
//		C1RX0CONbits.RXFUL=0;
		C1RX0CON=0;		
//Maybe should check data length code C1RX0DLC register?
	}
	if (C1INTFbits.RX1IF==1)
	{
		CANRxBuff[CANRxBuffNextAvailable][0]=C1RX1SID;	//Standard Identifier
//		cantrap=CANRxBuff[CANRxBuffNextAvailable][0];
		CANRxBuff[CANRxBuffNextAvailable][1]=C1RX1B1;	//Bytes 1 & 0
		CANRxBuff[CANRxBuffNextAvailable][2]=C1RX1B2;	//Bytes 3 & 2
		CANRxBuff[CANRxBuffNextAvailable][3]=C1RX1B3;	//Bytes 5 & 4
		CANRxBuff[CANRxBuffNextAvailable][4]=C1RX1B4;	//Bytes 7 & 6
		CANRxBuffNextAvailable = ((CANRxBuffNextAvailable+1) );
		if (CANRxBuffNextAvailable > CANRXBUFFMAX) CANRxBuffNextAvailable=0;
		C1INTFbits.RX1IF=0;
//		C1RX1CONbits.RXFUL=0;  (this might work)
		C1RX1CON=0;		
	}
	if (C1INTFbits.ERRIF==1)
	{
		CAN1AbortAll();
		C1INTFbits.RX0OVR=0; //RX1OVR must be cleared too in order to clear interrupt
		C1INTFbits.RX1OVR=0; //RX1OVR must be cleared too in order to clear interrupt
		C1INTFbits.ERRIF=0;	
	}
//	PORTAbits.RA13=0;    // Temporary to check interrupt time

}



/* Routine to check if anything is in CAN transmit queue that needs started */
// Note highest numbered buffer sends first

//Some code could be saved here by making the copt yo master RX buffer a subroutine

void CANTransmitCheck(void)
{
	if ((CANTxBuffNextUp != CANTxBuffNextAvailable)	&&
		((C1TX2CON & 0x0008) == 0))    // If data queued & buffer is available
		{
			if (CANTxBuff[CANTxBuffNextUp][0]>0)
			{
//			PORTAbits.RA15=1;     // Temporary to check interrupt time
			C1INTFbits.TX2IF = 0;		// Flag as NOT free
			C1TX2DLC=0x0040;			// Data length =8
			C1TX2SID=CANTxBuff[CANTxBuffNextUp][0] ;		//Get data next up to xmit & send it 
//			CANTxBuff[CANTxBuffNextUp][0] +=1 ;		//Get data next up to xmit & send it 
			C1TX2B1=CANTxBuff[CANTxBuffNextUp][1];				// Set the TXIDE bit 
			C1TX2B2=CANTxBuff[CANTxBuffNextUp][2];
			C1TX2B3=CANTxBuff[CANTxBuffNextUp][3];
			C1TX2B4=CANTxBuff[CANTxBuffNextUp][4];
			C1TX2CON=0x0008;  // Priority 0, set TXREQ LAST
			DisableIntCAN1;		//Neceaaary because int affects CANRxBuffNextAvailableValue
//			PORTAbits.RA15=0;     // Temporary to check interrupt time
//			if (Slave==0)		//If this IS the master put it in the RX buffer HERE too
//			{
				CANRxBuff[CANRxBuffNextAvailable][0]=(CANTxBuff[CANTxBuffNextUp][0] & 0xFF) +
					((CANTxBuff[CANTxBuffNextUp][0] & 0xF800) >> 3); 	//Standard Identifier
//		cantrap=CANRxBuff[CANRxBuffNextAvailable][0];
				CANRxBuff[CANRxBuffNextAvailable][1]=CANTxBuff[CANTxBuffNextUp][1];	//Bytes 1 & 0
				CANRxBuff[CANRxBuffNextAvailable][2]=CANTxBuff[CANTxBuffNextUp][2];	//Bytes 3 & 2
				CANRxBuff[CANRxBuffNextAvailable][3]=CANTxBuff[CANTxBuffNextUp][3];	//Bytes 5 & 4
				CANRxBuff[CANRxBuffNextAvailable][4]=CANTxBuff[CANTxBuffNextUp][4];	//Bytes 7 & 6
				CANRxBuffNextAvailable = ((CANRxBuffNextAvailable+1) );
				if (CANRxBuffNextAvailable > CANRXBUFFMAX) CANRxBuffNextAvailable=0;

//			} 
			EnableIntCAN1;
			}
			CANTxBuffNextUp = ((CANTxBuffNextUp+1) );
			if (CANTxBuffNextUp > CANTXBUFFMAX) CANTxBuffNextUp=0;

		}
	if ((CANTxBuffNextUp != CANTxBuffNextAvailable)	&&
		((C1TX1CON & 0x0008) == 0))    // If data queued & buffer is available
		{
			if (CANTxBuff[CANTxBuffNextUp][0]>0)
			{
//			PORTAbits.RA15=1;     // Temporary to check interrupt time
			C1INTFbits.TX1IF = 0;		// Flag as NOT free
			C1TX1DLC=0x0040;			// Data length =8
			C1TX1SID=CANTxBuff[CANTxBuffNextUp][0] ;		//Get data next up to xmit & send it 
//			CANTxBuff[CANTxBuffNextUp][0] +=1 ;		//Get data next up to xmit & send it 
			C1TX1B1=CANTxBuff[CANTxBuffNextUp][1];				// Set the TXIDE bit 
			C1TX1B2=CANTxBuff[CANTxBuffNextUp][2];
			C1TX1B3=CANTxBuff[CANTxBuffNextUp][3];
			C1TX1B4=CANTxBuff[CANTxBuffNextUp][4];
			C1TX1CON=0x0008;  // Priority 0, set TXREQ LAST
//			PORTAbits.RA15=0;     // Temporary to check interrupt time
			DisableIntCAN1;		//Neceaaary because int affects CANRxBuffNextAvailableValue
//			if (Slave==0)		//If this IS the master put it in the RX buffer HERE too//			{
				CANRxBuff[CANRxBuffNextAvailable][0]=(CANTxBuff[CANTxBuffNextUp][0] & 0xFF) +
					((CANTxBuff[CANTxBuffNextUp][0] & 0xF800) >> 3); 	//Standard Identifier
//		cantrap=CANRxBuff[CANRxBuffNextAvailable][0];
				CANRxBuff[CANRxBuffNextAvailable][1]=CANTxBuff[CANTxBuffNextUp][1];	//Bytes 1 & 0
				CANRxBuff[CANRxBuffNextAvailable][2]=CANTxBuff[CANTxBuffNextUp][2];	//Bytes 3 & 2
				CANRxBuff[CANRxBuffNextAvailable][3]=CANTxBuff[CANTxBuffNextUp][3];	//Bytes 5 & 4
				CANRxBuff[CANRxBuffNextAvailable][4]=CANTxBuff[CANTxBuffNextUp][4];	//Bytes 7 & 6
				CANRxBuffNextAvailable = ((CANRxBuffNextAvailable+1) );
				if (CANRxBuffNextAvailable > CANRXBUFFMAX) CANRxBuffNextAvailable=0;

//			} 
			EnableIntCAN1;		//Neceaaary because int affects CANRxBuffNextAvailableValue
			}
			CANTxBuffNextUp = ((CANTxBuffNextUp+1) );
			if (CANTxBuffNextUp > CANTXBUFFMAX) CANTxBuffNextUp=0;

		}
	if ((CANTxBuffNextUp != CANTxBuffNextAvailable)	&&
		((C1TX0CON & 0x0008) == 0))    // If data queued & buffer is available
		{
			if (CANTxBuff[CANTxBuffNextUp][0]>0)
			{
//			PORTAbits.RA15=1;     // Temporary to check interrupt time
			C1INTFbits.TX0IF = 0;		// Flag as NOT free
			C1TX0DLC=0x0040;			// Data length =8
			C1TX0SID=CANTxBuff[CANTxBuffNextUp][0] ;		//Get data next up to xmit & send it 
//			CANTxBuff[CANTxBuffNextUp][0] +=1 ;		//Get data next up to xmit & send it 
			C1TX0B1=CANTxBuff[CANTxBuffNextUp][1];				// Set the TXIDE bit 
			C1TX0B2=CANTxBuff[CANTxBuffNextUp][2];
			C1TX0B3=CANTxBuff[CANTxBuffNextUp][3];
			C1TX0B4=CANTxBuff[CANTxBuffNextUp][4];
			C1TX0CON=0x0008;  // Priority 0, set TXREQ LAST
//			PORTAbits.RA15=0;     // Temporary to check interrupt time
			DisableIntCAN1;		//Neceaaary because int affects CANRxBuffNextAvailableValue
//			if (Slave==0)		//If this IS the master put it in the RX buffer HERE too
//			{
				CANRxBuff[CANRxBuffNextAvailable][0]=(CANTxBuff[CANTxBuffNextUp][0] & 0xFF) +
					((CANTxBuff[CANTxBuffNextUp][0] & 0xF800) >> 3); 	//Standard Identifier
//		cantrap=CANRxBuff[CANRxBuffNextAvailable][0];
				CANRxBuff[CANRxBuffNextAvailable][1]=CANTxBuff[CANTxBuffNextUp][1];	//Bytes 1 & 0
				CANRxBuff[CANRxBuffNextAvailable][2]=CANTxBuff[CANTxBuffNextUp][2];	//Bytes 3 & 2
				CANRxBuff[CANRxBuffNextAvailable][3]=CANTxBuff[CANTxBuffNextUp][3];	//Bytes 5 & 4
				CANRxBuff[CANRxBuffNextAvailable][4]=CANTxBuff[CANTxBuffNextUp][4];	//Bytes 7 & 6
				CANRxBuffNextAvailable = ((CANRxBuffNextAvailable+1) );
				if (CANRxBuffNextAvailable > CANRXBUFFMAX) CANRxBuffNextAvailable=0;
//			} 
			EnableIntCAN1;		//Neceaaary because int affects CANRxBuffNextAvailableValue
			}
			CANTxBuffNextUp = ((CANTxBuffNextUp+1) );
			if (CANTxBuffNextUp > CANTXBUFFMAX) CANTxBuffNextUp=0;

		}



}




// * Process pending receive messages */
void CANRxProcess(void)
{
	unsigned int sid;
	unsigned int mask,mask2;
	unsigned int temp;
	unsigned int AudioAddress;
	unsigned int AudioOffset;
	unsigned int indx;

//	long long1;		// Signed long for doing audio math


		int diff;
		diff = CANRxBuffNextAvailable-CANRxNextToProcess;
		if (diff <0)
			diff += (CANRXBUFFMAX+1);
		if (diff > CANRxBuffDifferentialMax)
			CANRxBuffDifferentialMax=diff;


	while (CANRxBuff[CANRxNextToProcess][0] !=0)
	{
		sid = CANRxBuff[CANRxNextToProcess][0]/4;
		if (sid < 0x100)
			InactivityTimer = INACTIVITYTIMEOUT;	//refresh only on NON-AUDIO messages

		if 	( (sid >= 0x100) && (sid <= 0x1FF) && ((MyAudioStream * 0x40) == (sid & 0xC0)) )		
		{
			//Copy sound to audio buffer if stream matches
			//New 11/6/06 - process 8 bit sound messages but put in buffer as 16 bit
			AudioAddress = (sid & 0x0F) * 8;   //they come 8 at a time
			AudioOffset = (sid & 0x20) * 4;  //If in upper half of buffer
			indx=AudioAddress+AudioOffset;
		
			//Look for automatic reset of audio stream read address
			if (AudioReadIndexReset != 0)
			{
				if (indx==0)				
					AudioReadIndexReset=2;	//Setting value to 2 indicates 0x00 was hit
				else if ((indx == 0x80) && (AudioReadIndexReset==2))
				{
					AudioReadIndex=0;
					AudioReadIndexReset=0;
				}
			}		
						
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][1] & 0xFF) ;
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][1] & 0xFF00) >> 8;
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][2] & 0xFF) ;
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][2] & 0xFF00) >> 8;
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][3] & 0xFF) ;
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][3] & 0xFF00) >> 8;
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][4] & 0xFF) ;
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][4] & 0xFF00) >> 8;

/*			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][1] & 0xFF) * (AudioVolume);
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][1] >> 8) * (AudioVolume);
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][2] & 0xFF) * (AudioVolume);
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][2] >> 8) * (AudioVolume);
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][3] & 0xFF) * (AudioVolume);
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][3] >> 8) * (AudioVolume);
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][4] & 0xFF) * (AudioVolume);
			AudioBuffer[indx++] = (CANRxBuff[CANRxNextToProcess][4] >> 8) * (AudioVolume);
*/

/*
	// Now 12 bit audio in 26 messages 
			AudioAddress = (sid & 0x1F) * 5;   //they come 5 words at a time
			AudioOffset = (sid & 0x20) * 4;  //If in upper half of buffer
			// Note that last message would contain indexes 126,127,128,129,130
			// But we don't want any beyond 127 
//			DisableIntDCI;
			indx=AudioAddress+AudioOffset;
			if (AudioAddress < 126)
			{
				long1  = (CANRxBuff[CANRxNextToProcess][1] & 0x0FFF) << 4;		
				AudioBuffer[indx++] = (long1* (long)(AudioVolume)) >>8;
//				AudioBuffer[indx++] = (long1);

				long1 = ((CANRxBuff[CANRxNextToProcess][1] & 0xF000) >> 8) + ((CANRxBuff[CANRxNextToProcess][2] & 0x00FF) << 8);
				AudioBuffer[indx++] = (long1* (long)(AudioVolume)) >>8;
//				AudioBuffer[indx++] = (long1);

				long1 =((CANRxBuff[CANRxNextToProcess][2] & 0xFF00) >> 4) + ((CANRxBuff[CANRxNextToProcess][3] & 0x000F) << 12);			
				AudioBuffer[indx++] = (long1* (long)(AudioVolume)) >>8;
//				AudioBuffer[indx++] = (long1);
			}
			if (AudioAddress < 124)  
			{
				long1 = CANRxBuff[CANRxNextToProcess][3] & 0xFFF0;
				AudioBuffer[indx++] = (long1* (long)(AudioVolume)) >>8;
//				AudioBuffer[indx++] = (long1);

				long1 = (CANRxBuff[CANRxNextToProcess][4] & 0x0FFF) << 4;
				AudioBuffer[indx++] = (long1* (long)(AudioVolume)) >>8;
//				AudioBuffer[indx] = (long1);
			}
//			EnableIntDCI;

*/

/* Little test to catch the end 
			temp = AudioReadIndex;
			if ( (temp >= (indx-4)) &&
				(temp <= (indx)) && (AudioAddress != 0x7D) )
			{
				temp=0;   //do-nothing
			}				
*/

		}
		else 
		{
			switch (sid)
			{
			case 0x10:
			// LED Command 
			LEDCANCommand();
			break;
			
			//Score display digit segments
			case 0x70:
			if ((MyNodeNumber >=248) && (MyNodeNumber == (CANRxBuff[CANRxNextToProcess][1] & 0xFF)))
			{
				ScoreBrightness = CANRxBuff[CANRxNextToProcess][1] >> 8;
				LEDEnable[0] = (LEDEnable[0] & 0x8080) | CANRxBuff[CANRxNextToProcess][2];
				LEDEnable[1] = (LEDEnable[1] & 0x8080) | CANRxBuff[CANRxNextToProcess][3];
				LEDEnable[2] = (LEDEnable[2] & 0x8080) | CANRxBuff[CANRxNextToProcess][4];
				// Maybe add some way to flash them
			}
			break;			

			//Score display flash digits
			case 0x71:
			if ((MyNodeNumber >=248) && (MyNodeNumber == (CANRxBuff[CANRxNextToProcess][1] & 0xFF)))
			{
			LEDScoreOnTime = CANRxBuff[CANRxNextToProcess][2] & 0xFF;
			LEDScoreOffTime = CANRxBuff[CANRxNextToProcess][2] >> 8 ;
			LEDScoreFlashEnable[0] = CANRxBuff[CANRxNextToProcess][3];   //flashing mode
			LEDScoreFlashEnable[1] = CANRxBuff[CANRxNextToProcess][4];   //flashing mode
			}
			break;


			//Score display Lamp drivers
			case 0x72:
			if ((MyNodeNumber >=248) && (MyNodeNumber == (CANRxBuff[CANRxNextToProcess][1] & 0xFF)))
			{
				LEDEnable[0] &= 0x7F7F;
				LEDEnable[1] &= 0x7F7F;			//Temporarily zero these bits
				LEDEnable[2] &= 0x7F7F;			//Temporarily zero these bits

				temp = CANRxBuff[CANRxNextToProcess][2];
				if (temp & 0x1)
					LEDEnable[0] |= 0x80;
				if (temp & 0x2)
					LEDEnable[0] |= 0x8000;
				if (temp & 0x4)
					LEDEnable[1] |= 0x80;
				if (temp & 0x8)
					LEDEnable[1] |= 0x8000;
				if (temp & 0x10)
					LEDEnable[2] |= 0x80;
				if (temp & 0x20)
					LEDEnable[2] |= 0x8000;
				LEDEnable[3] = (CANRxBuff[CANRxNextToProcess][2] >> 6) +
					(CANRxBuff[CANRxNextToProcess][3] << 10);
			}
			break;			


			case 0x80:
			case 0x81:
			case 0x82:
			case 0x83:
			//Audio set multiple nodes to stream 0,1,2 or 3
			temp = (MyNodeNumber >> 4) +1;  //Byte where bit will be found
			mask = (0x0001 << (MyNodeNumber & 0x0F));  
			if (CANRxBuff[CANRxNextToProcess][temp] & mask)
			{
				MyAudioStream = sid & 0x3;
				AudioTimeout=0xFFFF;
			}
			break;

			case 0x84:
			//Audio set multiple Nodes on or off
			temp = (MyNodeNumber >> 4) +1;  //Byte where bit will be found
			mask = (0x0001 << (MyNodeNumber & 0x0F));  
			AudioTimeout=0xFFFF;
			if (CANRxBuff[CANRxNextToProcess][temp] & mask) 
				AudioOn=1;
			else
				AudioOn=0;
			break;

			case 0x85:
			//Audio set multiple Nodes to volume = MAX
			temp = (MyNodeNumber >> 4) +1;  //Byte where bit will be found
			mask = (0x0001 << (MyNodeNumber & 0x0F));  
			if (CANRxBuff[CANRxNextToProcess][temp] & mask) 
			{
				MyAudioVolume = 0xFF;
				AudioTimeout=0xFFFF;
			}
			break;

			case 0x86:
			//Audio set multiple Nodes to volume = 1/2
			temp = (MyNodeNumber >> 4) +1;  //Byte where bit will be found
			mask = (0x0001 << (MyNodeNumber & 0x0F));  
			if (CANRxBuff[CANRxNextToProcess][temp] & mask) 
			{
				MyAudioVolume = 0x7F;
				AudioTimeout=0xFFFF;
			}
			break;

			case 0x87:
			//Audio set multiple Nodes to volume = background
			temp = (MyNodeNumber >> 4) +1;  //Byte where bit will be found
			mask = (0x0001 << (MyNodeNumber & 0x0F));  
			if (CANRxBuff[CANRxNextToProcess][temp] & mask) 
			{
				MyAudioVolume = AudioBackgroundVolume;
				AudioTimeout=0xFFFF;
			}
			break;

			case 0x88:
			// Audio Enable Command with timeouts
			if 	((CANRxBuff[CANRxNextToProcess][1] & 0xFF) == MyNodeNumber)
			{
				AudioOn = (CANRxBuff[CANRxNextToProcess][1] >> 12) & 0x1;
				MyAudioStream = (CANRxBuff[CANRxNextToProcess][1] >>8) & 0x3;
				AudioTimeout=CANRxBuff[CANRxNextToProcess][2];
				MyAudioVolume = CANRxBuff[CANRxNextToProcess][3] & 0xFF;
				AudioTimer=0;
				AudioTimeoutStream=(CANRxBuff[CANRxNextToProcess][1] >>10) & 0x3;									
				AudioTimeoutStatus=(CANRxBuff[CANRxNextToProcess][1] >>13) & 0x1;									
				AudioTimeoutVolume=(CANRxBuff[CANRxNextToProcess][3] >>8);
			}
			break;

			case 0x89:
			// Reset AudioReadIndex WHEN STREAM DATA ARRIVES & WRITES AUDIO AT 0x80
			if ((CANRxBuff[CANRxNextToProcess][1] & 0xFF) == MyNodeNumber) 
			{
				AudioReadIndexReset=1;
			}
			break;

			case 0x8A:
			if (MyAudioStream == (CANRxBuff[CANRxNextToProcess][1] & 0x3))
			{
				AudioReadIndexReset=1;
			}
			break; 


			case 0x8C:			// No operation code
				break;

/*	Function deleted use 0x89/0x8A instead
			case 0x8D:
			// Reset audio read index for nodes playing a specific stream
			temp=CANRxBuff[CANRxNextToProcess][1] & 0x3;		//Stream address 0-3
			if (Slave==0)		//No matter what stream master is playing, reset stream counter here
			{
				DisableIntDCI;
				AudioStreamCounter[temp] =0;
				AudioStreamTransmitFlag[temp]=1;
				EnableIntDCI;		//OOPS - WAS MISSING
			}
			if (temp == MyAudioStream)
			{
//				DisableIntDCI;   // not needed
//				AudioReadIndex=(CANRxBuff[CANRxNextToProcess][2] & 0xFF);
				AudioReadIndex=(0x80);
//				AudioTemporaryMute=(CANRxBuff[CANRxNextToProcess][3] & 0x1);
//				EnableIntDCI;
			}
			break;  */
		
			case 0x8E:
				// Audio off all nodes 
				AudioOn=0;			// ALL AUDIO OFF
				AudioTimeout=0xFFFF;
				break;

			case 0x8F:
			// Audio Enable Command 
			if 	(CANRxBuff[CANRxNextToProcess][1] == MyNodeNumber)
			{
				AudioOn = CANRxBuff[CANRxNextToProcess][2] & 0x1;
				MyAudioVolume = CANRxBuff[CANRxNextToProcess][3] & 0xFF;
				MyAudioStream = CANRxBuff[CANRxNextToProcess][4] & 0x3;
				AudioTimeout=0xFFFF;		//Indicate there is no timeout

			}
			break;


			case 0xE0:
			// Reset all
			if (Slave !=0)
			{
				DisableIntCAN1;
				for (temp=0; temp <= CANRXBUFFMAX; temp+=1)
				{			
					CANRxBuff[temp][0]=0;	//Indicate all instructions null
				}
				CANRxNextToProcess=CANRXBUFFMAX;	//Will inc to 00
				CANRxBuffNextAvailable=0;
				EnableIntCAN1;
				CANTxBuffNextAvailable=0;
				CANTxBuffNextUp=0;			//Do this last incase TX int happened between instrs
				AudioOn=0;			// Sound off
				AudioTimer=0;
				AudioTimeout=0xFFFF;
				MyAudioVolume=0xFF;
				MyAudioStream=0;
				LEDColor[0]=0;		//LEDs off
				LEDColor[1]=0;
				LEDColor[2]=0;
				LEDTimer=0;
				LEDFadeTimer=0;
				LEDFadeTime=0;
			}
			break;


			case 0xE3:
			// Set slave address when slave button pushed
			//and store to eeprom
			if (Slave != 0)
			{
				DiagTentativeAddress=CANRxBuff[CANRxNextToProcess][1] & 0xFF;
				DiagAddressSetup=1;
			}
			break;


			case 0xE4:
			//Cancel diagnostic 
				DiagAddressSetup=0;
			break;
			

			// button press on & off messages
			case 0xF0:
			case 0xF1:
			mask = 0x1;			// Make a mask to find the switch bit 
			indx = 0x0;
			if (CANRxBuff[CANRxNextToProcess][1] == 247)	//if master button
			{
				if (sid == 0xF0)
					MasterSwitchState=1;	//Set bit on
				else	
					MasterSwitchState=0;  // clear bit off
			}
			else
			{
				for (temp=0; temp <=31; temp+=1)		// Loop through to find this node 
				{
					#ifdef ROWFAKE
						if (mask==0x2)
							mask2=0x10;
						else if (mask==0x4)
							mask2=0x100;
						else if (mask==0x8)
							mask2=0x1000;
						else 
							mask2=mask;
					#else
						mask2=mask;
					#endif
			
					if (CANRxBuff[CANRxNextToProcess][1] == temp)  // if node number found 
					{
						if (sid == 0xF0)
							NodeSwitchStates[indx] |= mask2;	//Set bit on
						else	
							NodeSwitchStates[indx] &= (mask2 ^ 0xFFFF);  // clear bit off
					}
					mask = mask << 1;		//rotate mask bit
					if (mask ==0)
					{
						mask=0x1;		//After 16 mask bits it will go to 0
						indx=1;	//restart the mask at 1 & increment the index
					}
				}
			}
			break;

			// Score display button change
			case 0xF3:
				if (ScoreDisplay !=1)	//only update receipt on NON score displays
				{
					Piezos[0] = CANRxBuff[CANRxNextToProcess][2] & 0xFF;
					Piezos[1] = (CANRxBuff[CANRxNextToProcess][2] >> 8) & 0xFF;
					Piezos[2] = CANRxBuff[CANRxNextToProcess][3] & 0xFF;			
				}
			break;


		}
		}

		CANRxBuff[CANRxNextToProcess][0] = 0;  // Zero out the sid to indicate this message is DONE
		CANRxNextToProcess = ((CANRxNextToProcess+1) );		//Process next command next time	
		if (CANRxNextToProcess > CANRXBUFFMAX) CANRxNextToProcess=0;

	}
}



/* Queue a 4-word message to CAN TX buffer */
void CANQueueTxMessage(unsigned int sid, unsigned int word1,
unsigned int word2, unsigned int word3, unsigned int word4)
{
		CANTxBuff[CANTxBuffNextAvailable][0] = CANMakeTxSID(sid);
		CANTxBuff[CANTxBuffNextAvailable][1] = word1;
		CANTxBuff[CANTxBuffNextAvailable][2] = word2;
		CANTxBuff[CANTxBuffNextAvailable][3] = word3;
		CANTxBuff[CANTxBuffNextAvailable][4] = word4;
		CANTxBuffNextAvailable = ((CANTxBuffNextAvailable+1) );
		if (CANTxBuffNextAvailable > CANTXBUFFMAX) CANTxBuffNextAvailable=0;

/* DIAGNOSTIC TO SEE HOW MUCH BUFFER IS IN USE */
		int diff;
		diff = CANTxBuffNextAvailable-CANTxBuffNextUp;
		if (diff <0)
			diff += (CANTXBUFFMAX+1);
		if (diff > CANTxBuffDifferentialMax)
			CANTxBuffDifferentialMax=diff;
		
}


/* Transmitted SID word is in a strange format with 3 blank bytes in the middle */
unsigned int CANMakeTxSID (unsigned int SID_Unformatted)
{
return ( ((SID_Unformatted & 0x3F) << 2) + ((SID_Unformatted & 0x7C0) << 5) );
}




