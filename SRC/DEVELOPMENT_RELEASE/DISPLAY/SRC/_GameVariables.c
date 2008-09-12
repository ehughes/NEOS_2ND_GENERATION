#include "_GameVariables.h"

unsigned int GameScores[2];				//Player 0 & Player 1 scores 
unsigned int GameAchievementLevels[4];
unsigned char GameActiveButton[6];		//Active button group 0 & 1
unsigned char GameButtonGroup1[16];		//Buttons existing in group 1
unsigned char GameButtonGroupLeft[16];
unsigned char GameButtonGroupRight[16];
//unsigned char GameButtonGroup2[32];		//Buttons existing in group 2
unsigned int GameState;			//For use by game
unsigned int GameTimer1;		// counts 100 per sec
unsigned int GameTimeout1;		//Times to end of game */
unsigned int GameTimer2;		// counts 100 per sec
unsigned int GameTimeout2;		//Times to end of game */
unsigned int GameTimer3;
unsigned int GameTimeout3;		//Times to end of game */
unsigned char GameSelected=0;  // Game being played
unsigned char GamePlayers=0;	// Number of players (1 or 2)
unsigned char GameLevel=0;		// Level 1,2 or 3

unsigned char GameSelectedLast=0;  //0-8 game selection button number
unsigned char GamePlayersLast=0;	// 0 or 1 (note this is not offest by 1)
unsigned char GameLevelLast=0;		//0-2 (note that this is not offset by 1)

unsigned char GameNodeState[16];
unsigned int GameNodeTimer[16];
unsigned char GamePhase;
unsigned char GameVariable[6];
unsigned int GameInteger[4];
unsigned char GameArray[16];
unsigned long GameCount=0;		//Total number of games played

unsigned int GameShowTimer=0;
unsigned int GameShowIndex=0;



void GameScoreAdd(int index, int adder)
{
	GameScores[index] += adder;
	if (GameScores[index] > 999)
		GameScores[index] = 999;
}
