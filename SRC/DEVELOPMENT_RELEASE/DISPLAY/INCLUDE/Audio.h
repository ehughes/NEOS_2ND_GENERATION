


void AudioNodeEnableAll(unsigned char strm, unsigned char vol);
void AudioTimeoutCheck(void);
void AudioStreamCheck(void);
void AudioPlaySound(unsigned int sound, unsigned char stream);

void AudioEnable(unsigned char node,unsigned char status,
unsigned char volume, unsigned char stream);

void AudioOffAllNodes(void);
void AudioReSync(unsigned int node);

void AudioSetNodesToStream(unsigned char stream, unsigned int w1, 
unsigned int w2, unsigned int w3, unsigned int w4);

void AudioNodeEnable(unsigned char node, unsigned char stream1, unsigned char stream2,
	unsigned char status1, unsigned char status2, unsigned int timeout,
	unsigned char volume1, unsigned char volume2);

void AudioSetNodesOnOff(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4);


//void AudioSetNodesVolumeMax(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4);
//void AudioSetNodesVolumeHalf(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4);
//void AudioSetNodesVolumeBackground(unsigned int w1, unsigned int w2, unsigned int w3, unsigned int w4);

void DCIInit (void);


void __attribute__((__interrupt__)) _DCIInterrupt(void);


