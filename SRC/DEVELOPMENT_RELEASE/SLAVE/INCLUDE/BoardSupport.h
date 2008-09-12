#include <p30fxxxx.h>

#define BUTTON_LOCATION     0x02
#define GREEN_LED_LOCATION 	0x02
#define RED_LED_LOCATION 	0x01

#define LED_RED    LEDOverride=TRUE;LATD|=RED_LED_LOCATION;LATD&=~GREEN_LED_LOCATION
#define LED_GREEN  LEDOverride=TRUE;LATD&=~RED_LED_LOCATION;LATD|=GREEN_LED_LOCATION
#define LED_YELLOW LEDOverride=TRUE;LATD|=RED_LED_LOCATION;LATD|=GREEN_LED_LOCATION
#define LED_OFF    LATD&=~RED_LED_LOCATION;LATD&=~GREEN_LED_LOCATION;LEDOverride=FALSE;

void PortInit(void);
void ReadOptionJumpers(void);
void RegulatorControl(void);
void ResetAll();


