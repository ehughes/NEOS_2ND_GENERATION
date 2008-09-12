#include "DataTypes.h"

#ifndef __GAMEVARIABLES_H
#define __GAMEVARIABLES_H



#define ENABLE_ALL    0xFF


#define BUTTON_TIME		10
#define GLOBALVOLUMEINIT 0xFF	// Initialized value of global volume
#define BACKGROUNDVOLUMEINIT 40 	//00-FF background volume
#define	BACKGROUNDVOLHI	 90		//Higher low volume
#define BACKGROUNDVOLLO	 40
#define BACKGROUNDVOLMUSIC 30	//Background for music games
#define VOLUMEPHASE4	128
#define VOLUMESNORE	128


#define INIT		0x0
#define BOOT		0xF0
#define BOOT_WAIT	0xF1
#define BOOT_TIME	200

//Eli's
extern WORD Player1Score;
extern WORD Player2Score;

extern WORD GameState;			//For use by game

extern BYTE GameSelected;  // Game being played
extern BYTE GamePlayers;	// Number of players (1 or 2)


typedef struct{
	BYTE GameBackgroundMusicVolume;
	BYTE GameSoundEffectVolume;
	BYTE FinaleMusicVolume;
} GameSettings;

extern GameSettings CurrentGameSettings;

extern BYTE AudioBackgroundVolume;
extern BYTE ScoreBrightness;

extern unsigned long GameCount;		//Total number of games played

#define PLAYER_1 0x00
#define PLAYER_2 0x01




#endif
