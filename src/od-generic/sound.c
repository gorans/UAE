 /*
  * UAE - The Un*x Amiga Emulator
  *
  * Support for the Mute sound system
  *
  * Copyright 1997 Bernd Schmidt
  */

#include "sysconfig.h"
#include "sysdeps.h"

#include "options.h"
#include "memory.h"
#include "custom.h"
#include "audio.h"
#include "gensound.h"
#include "sounddep/sound.h"

int init_sound (void)
{
    currprefs.produce_sound = 0;
    return 1;
}

int setup_sound (void)
{
    currprefs.produce_sound = 0;
    return 1;
}

void close_sound(void)
{
}
