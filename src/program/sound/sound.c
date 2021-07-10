#include "sound.h"

Sound soundCreate()
{
    Sound this = {0};

    this.soloud = Soloud_create();

    Soloud_initEx(this.soloud, SOLOUD_CLIP_ROUNDOFF | SOLOUD_ENABLE_VISUALIZATION,
                  SOLOUD_AUTO, SOLOUD_AUTO, SOLOUD_AUTO, 2);

    Soloud_setGlobalVolume(this.soloud, 4);

    return this;
}

void soundDestroy(Sound this)
{
    Soloud_deinit(this.soloud);
    Soloud_destroy(this.soloud);
}