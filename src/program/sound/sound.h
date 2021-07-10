#ifndef __SOUND_H__
#define __SOUND_H__

#include <soloud_c.h>

typedef struct
{
    Soloud *soloud;
} Sound;

Sound soundCreate();
void soundDestroy(Sound this);

#endif