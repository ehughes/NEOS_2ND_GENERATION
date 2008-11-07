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
#define DISPLAY_ERR 2
#define DISPLAY_DIA 3
#define DISPLAY_VER 4

void LEDMaster(void);
void LEDRefresh(void);
void LEDEncode(WORD  score1, WORD  score2); //Encode 2 scores to 7 segment patterns
WORD  LEDMake7Seg (WORD  value);		//Convert 0-9 to 7-segment pattern
void LEDSendScoreP1P2(WORD  node, WORD  brightness, WORD  players);
void LEDSendScores(WORD  node, WORD  brightness, WORD  score1, WORD  score2);
void LEDSendScoreAddress(WORD  node, WORD  brightness, WORD  score1, WORD  score2);
void LEDSendCHARsandVar(WORD  node, WORD  brightness, WORD  score1,WORD  score2,BYTE  c1, BYTE  c2, BYTE  c3);
void LEDSendCHARs(WORD  node, WORD  brightness, BYTE  c1, BYTE  c2, BYTE  c3, BYTE  c4, BYTE  c5, BYTE  c6);
void LEDCANCommand(void);
void LEDTimerCheck(void);
void LEDSendMessage(BYTE  nodenumber, BYTE  redbrt, BYTE  greenbrt, BYTE  bluebrt,
BYTE  redfade, BYTE  greenfade, BYTE  bluefade,
WORD  ledtime, WORD  fadetime);
void LEDSendVariable(WORD  indx, WORD  value);
void ScoreSendLights(BYTE  nodenumber, WORD  Data0to15, WORD  Data16to19);
extern WORD  LEDSegmentPattern[3];	// encoded 7-segment patterns for 6 digits
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

