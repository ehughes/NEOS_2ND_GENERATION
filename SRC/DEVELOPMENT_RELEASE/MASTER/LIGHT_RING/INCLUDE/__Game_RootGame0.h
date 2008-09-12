#ifndef __GAME_ROOTGAME0_H
#define __GAME_ROOTGAME0_H


extern unsigned char EasterEggTable[16][4];
extern unsigned char __attribute__ ((space(auto_psv))) GameSelectTable[9][2][3];

void Root_Game0 (void);
void OnButtonPressRootGame0(unsigned char button);
void OnMasterSwitchPressRootGame0(void);
void OnSelectPressRootGame0(unsigned char  button);


#endif

