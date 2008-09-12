
void LEDTimerCheck(void);
void ResetLED();


extern unsigned int LEDTimer;	//10ms down counter until LED shutoff 10 BITS LONG
extern unsigned int LEDFadeTimer; //10ms UP counter used for fading 10 BITS LONG
extern unsigned int LEDFadeTime;  //Limit for LED 10 BITS LONG

extern unsigned char LEDColor[3];	/* R,G,B 0-255 color values */
extern unsigned char LEDStartColor[3];	/* R,G,B 0-255 color values */
extern char LEDColorChange[3]; //SIGNED change values for fading colors
