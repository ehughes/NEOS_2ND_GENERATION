#include "DataTypes.h"

#ifndef _LEDDISPLAY_H
#define _LEDDISPLAY_H

#define RED 63,0,0
#define GREEN 0,63,0
#define YELLOW 63,63,0
#define AMBER 63,20,0
#define LIME 20,63,0
#define LEDOFF 0,0,0

#define BLANK_SCORE 1000
#define BLANK_DIGIT 10

#define NO_FADE 		0x00
#define NO_LED_TIMEOUT 0x00

#define DISPLAY_ADDRESS	248


#define DISPLAY_VOL	0
#define DISPLAY_BRI 1
#define DISPLAY_ERR 7
#define DISPLAY_DIA 8


void LEDMaster(void);
void LEDRefresh(void);
void LEDEncode(unsigned int score1, unsigned int score2); //Encode 2 scores to 7 segment patterns
unsigned int LEDMake7Seg (unsigned int value);		//Convert 0-9 to 7-segment pattern
void LEDSendScoreP1P2(unsigned int node, unsigned int brightness, unsigned int players);
void LEDSendScores(unsigned int node, unsigned int brightness, unsigned int score1, unsigned int score2);
void LEDSendScoreAddress(unsigned int node, unsigned int brightness, unsigned int score1, unsigned int score2);
void LEDSendCharsandVar(unsigned int node, unsigned int brightness, unsigned int score1,unsigned int score2,unsigned char c1, unsigned char c2, unsigned char c3);
void LEDSendChars(unsigned int node, unsigned int brightness, unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4, unsigned char c5, unsigned char c6);
void LEDCANCommand(void);
void LEDTimerCheck(void);
void LEDSendMessage(unsigned char nodenumber, unsigned char redbrt, unsigned char greenbrt, unsigned char bluebrt,
unsigned char redfade, unsigned char greenfade, unsigned char bluefade,
unsigned int ledtime, unsigned int fadetime);
void LEDSendVariable(unsigned int indx, unsigned int value);
void ScoreSendLights(unsigned char nodenumber, unsigned int Data0to15, unsigned int Data16to19);
extern unsigned int LEDSegmentPattern[3];	// encoded 7-segment patterns for 6 digits
void LEDSendDigits(WORD node, BYTE brightness, BYTE d1, BYTE d2, BYTE d3, BYTE d4, BYTE d5, BYTE d6);


extern WORD P1ScoreUpdateTimer;
extern WORD P2ScoreUpdateTimer;


#define SCORE_NORMAL     0x0
#define SCORE_FLASHING   0x1
#define SCORE_BLANK      0x2

extern WORD P1ScoreUpdateTimer;
extern WORD P2ScoreUpdateTimer;

extern BYTE P1ScoreDisplayState;
extern BYTE P2ScoreDisplayState;
extern WORD P1ScoreOutput;
extern WORD P2ScoreOutput;
extern BOOL P1FlashState;
extern BOOL P2FlashState;

extern BYTE P1ScoreFlashTime;
extern BYTE P2ScoreFlashTime;

extern WORD ScoreManagerTimer;
extern BOOL ScoreManagerEnabled;
extern BYTE ScoreManagerUpdateRate;

void ScoreDisplayManager();
void InitScoreDisplayerManager();
void SyncDisplayFlashing();

#endif

