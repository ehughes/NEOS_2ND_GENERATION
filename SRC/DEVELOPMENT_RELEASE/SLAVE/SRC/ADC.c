#include <adc12.h>
#include "ADCRoutines.h"
#include "SystemVariables.h"
#include <p30fxxxx.h>

void ADCInit(void)
{
	
	ADCON1bits.ADON=0;
	OpenADC12(ADC_MODULE_ON & ADC_IDLE_STOP & ADC_FORMAT_INTG & ADC_CLK_AUTO & ADC_AUTO_SAMPLING_ON & ADC_SAMP_ON,
	ADC_VREF_AVDD_AVSS & ADC_SCAN_ON & ADC_ALT_BUF_OFF & ADC_ALT_INPUT_OFF & ADC_SAMPLES_PER_INT_16,
	ADC_SAMPLE_TIME_16 & ADC_CONV_CLK_INTERNAL_RC & ADC_CONV_CLK_16Tcy,
	ENABLE_AN2_ANA, 
	SKIP_SCAN_AN0 & SKIP_SCAN_AN1 & SKIP_SCAN_AN3 &
	SKIP_SCAN_AN4 & SKIP_SCAN_AN5 & SKIP_SCAN_AN6 & SKIP_SCAN_AN7 &
	SKIP_SCAN_AN8 & SKIP_SCAN_AN9 & SKIP_SCAN_AN10 & SKIP_SCAN_AN11 &
	SKIP_SCAN_AN12 & SKIP_SCAN_AN13 & SKIP_SCAN_AN14 & SKIP_SCAN_AN15);

	SetChanADC12(ADC_CH0_POS_SAMPLEA_AN2 & 	ADC_CH0_NEG_SAMPLEA_NVREF);
	
}

unsigned int ADCRead(void)
{
	unsigned int u;

	u=ReadADC12(0);
	u += 217;				//Add equivalent of 0.7V diode drop

	return(u);

	//4096 = 13.2V so it is 310 counts per volt

}


//void __attribute__((__interrupt__,__auto_psv__)) _ADCInterrupt(void)
void __attribute__((__interrupt__,__auto_psv__)) _ADCInterrupt(void)
{
        
}
