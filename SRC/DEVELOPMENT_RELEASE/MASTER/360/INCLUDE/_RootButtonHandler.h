#ifndef _ROOTBUTTONHANDLER_H
#define  _ROOTBUTTONHANDLER_H

void OnSelectPress(unsigned int button);
void OnButtonRelease(unsigned char button);
void OnButtonPress(unsigned char button);
void OnButtonReleaseNull(unsigned char button);
void MasterButtonCheck(void);

void SelectSwitchCheck(void);
void OnMasterSwitchPress(void);

extern unsigned char ScoreBoardButtons[3];	// status of piezo bits
extern unsigned char ScoreBoardButtonsLast[3];	//Previous state

#endif


