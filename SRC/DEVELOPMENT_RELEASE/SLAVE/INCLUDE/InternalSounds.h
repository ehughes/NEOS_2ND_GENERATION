#ifndef _INTERNAL_SOUNDS_H
#define _INTERNAL_SOUNDS_H


#define INTERNAL_SOUND_POSITIVE_FEEDBACK	0
#define INTERNAL_SOUND_SELECTION			1
#define INTERNAL_SOUND_FF_WATER_HIT			2
#define INTERNAL_SOUND_FF_WATER_MISS		3
#define INTERNAL_SOUND_FF_FIRE_ON			4



#define POSITIVE_FEEDBACK_LENGTH 4967
extern const unsigned char PositiveFeedback[POSITIVE_FEEDBACK_LENGTH] __attribute__((space(__auto_psv__)));

#define FF_WATER_HIT_LENGTH 7655
extern const unsigned char FF_WaterHit[FF_WATER_HIT_LENGTH] __attribute__((space(__auto_psv__)));

#define FF_WATER_MISS_LENGTH 1037
extern const unsigned char FF_WaterMiss[FF_WATER_MISS_LENGTH] __attribute__((space(__auto_psv__)));

#define FF_FIRE_ON_LENGTH 13868
extern const unsigned char FF_FireOn[FF_FIRE_ON_LENGTH] __attribute__((space(__auto_psv__)));

#define SELECTION_LENGTH 3347
extern const unsigned char Selection[SELECTION_LENGTH] __attribute__((space(__auto_psv__)));


#endif



