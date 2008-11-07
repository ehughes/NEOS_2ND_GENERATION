#include "_GameVariables.h"
#include "DataTypes.h"
#include "LED-Display.h"



BYTE GameSelected=0;  // Game being played
BYTE GamePlayers=0;	// Number of players (1 or 2)

WORD Player1Score=0;
WORD Player2Score=0;

WORD GameState;			//For use by game

DWORD GameCount=0;		//Total number of games played

GameSettings CurrentGameSettings;




//To be deprecated








