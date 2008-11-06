#ifndef _INTERNAL_SOUNDS_H
#define _INTERNAL_SOUNDS_H

#define INTERNAL_SOUND_POSITIVE_FEEDBACK	0
#define INTERNAL_SOUND_SELECTION			1
#define INTERNAL_SOUND_FF_WATER_HIT			2
#define INTERNAL_SOUND_FF_WATER_MISS		3
#define INTERNAL_SOUND_FF_FIRE_APPEAR		4
#define INTERNAL_SOUND_FF_FIRE_OUT			5

#define POSITIVE_FEEDBACK_LENGTH 4967
extern const unsigned char PositiveFeedback[POSITIVE_FEEDBACK_LENGTH] __attribute__((space(__auto_psv__)));

#define FIRE_HIT_LENGTH 2055
extern const unsigned char FireHit[2055] __attribute__((space(__auto_psv__)));

#define FIRE_MISS_LENGTH 645
extern const unsigned char FireMiss[FIRE_MISS_LENGTH] __attribute__((space(__auto_psv__)));

#define FIRE_APPEAR_LENGTH 7691
extern const unsigned char FireAppear[FIRE_APPEAR_LENGTH] __attribute__((space(__auto_psv__)));

#define FIRE_OUT_LENGTH 3436
extern const unsigned char FireOut[FIRE_OUT_LENGTH] __attribute__((space(__auto_psv__)));


#define SELECTION_LENGTH 3347
extern const unsigned char Selection[SELECTION_LENGTH] __attribute__((space(__auto_psv__)));



#endif



