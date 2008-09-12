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
void ScoreSendLights(unsigned char nodenumber, unsigned int Data16to19, unsigned int Data0to15);
void LEDSendVariable(unsigned int indx, unsigned int value);






