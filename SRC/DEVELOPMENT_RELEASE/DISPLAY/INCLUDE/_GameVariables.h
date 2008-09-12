#define RED 63,0,0
#define GREEN 0,63,0
#define YELLOW 63,63,0
#define AMBER 63,20,0
#define LIME 20,63,0
#define LEDOFF 0,0,0


#define GLOBALVOLUMEINIT 0xFF	// Initialized value of global volume
#define BACKGROUNDVOLUMEINIT 40 	//00-FF background volume
#define	BACKGROUNDVOLHI	 90		//Higher low volume
#define BACKGROUNDVOLLO	 40
#define BACKGROUNDVOLMUSIC 30	//Background for music games
#define VOLUMEPHASE4	128
#define VOLUMESNORE	128

#define GAME_ROOT_GAME0 	0x00

#define GAME_BUTTON_ADDRESS 0x80
#define GAME_DIAGNOSTIC     0x81
#define GAME_FULL_POWER		0x82


#define INIT		0x0


extern unsigned int GameScores[2];				//Player 0 & Player 1 scores 
extern unsigned int GameAchievementLevels[4];	//Score achievement levels
extern unsigned char GameActiveButton[6];		//Active button group 0 & 1
extern unsigned char GameButtonGroup1[16];		//Buttons existing in group 1
extern unsigned char GameButtonGroupLeft[16];
extern unsigned char GameButtonGroupRight[16];
//unsigned char GameButtonGroup2[32];		//Buttons existing in group 2
extern unsigned int GameState;			//For use by game
extern unsigned int GameTimer1;		// counts 100 per sec
extern unsigned int GameTimeout1;		//Times to end of game */
extern unsigned int GameTimer2;		// counts 100 per sec
extern unsigned int GameTimeout2;		//Times to end of game */
extern unsigned int GameTimer3;
extern unsigned int GameTimeout3;		//Times to end of game */
extern unsigned char GameSelected;  // Game being played
extern unsigned char GamePlayers;	// Number of players (1 or 2)
extern unsigned char GameLevel;		// Level 1,2 or 3

extern unsigned char GameNodeState[16];
extern unsigned int GameNodeTimer[16];
extern unsigned char GamePhase;
extern unsigned char GameVariable[6];
extern unsigned int GameInteger[4];
extern unsigned char GameArray[16];
extern unsigned char __attribute__ ((space(auto_psv))) NextButtonTable[16][8];

extern unsigned int GameShowTimer;
extern unsigned int GameShowIndex;

extern unsigned long GameCount;		//Total number of games played

extern unsigned char AudioBackgroundVolume;
extern unsigned char ScoreBrightness;

extern unsigned char GameSelectedLast;  //0-8 game selection button number
extern unsigned char GamePlayersLast;	// 0 or 1 (note this is not offest by 1)
extern unsigned char GameLevelLast;		//0-2 (note that this is not offset by 1)



extern void GameScoreAdd(int index, int adder);

