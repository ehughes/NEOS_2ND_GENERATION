#ifndef _ROOTBUTTONHANDLER_H
#define  _ROOTBUTTONHANDLER_H

void OnSelectPress(WORD button);
void OnButtonRelease(BYTE button);
void OnButtonPress(BYTE button);
void OnButtonReleaseNull(BYTE button);
void MasterButtonCheck(void);

void SelectSwitchCheck(void);
void OnMasterSwitchPress(void);

extern BYTE ScoreBoardButtons[3];	// status of piezo bits
extern BYTE ScoreBoardButtonsLast[3];	//Previous state

#endif


