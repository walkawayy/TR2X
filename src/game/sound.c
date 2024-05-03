#include "game/sound.h"

#include "global/funcs.h"
#include "global/vars.h"

typedef enum {
    SF_FLIP = 0x40,
    SF_UNFLIP = 0x80,
} SOUND_FLAG;

void __cdecl Sound_SetMasterVolume(int32_t volume)
{
    g_MasterVolume = volume;
}

void __cdecl Sound_UpdateEffects(void)
{
    for (int32_t i = 0; i < g_SoundEffectCount; i++) {
        OBJECT_VECTOR *sound = &g_SoundEffects[i];
        if ((g_FlipStatus && (sound->flags & SF_FLIP))
            || (!g_FlipStatus && (sound->flags & SF_UNFLIP))) {
            Sound_Effect(
                sound->data, (XYZ_32 *)sound,
                0); // TODO: use proper pointer for this
        }
    }

    if (g_FlipEffect != -1) {
        g_EffectRoutines[g_FlipEffect](NULL);
    }

    Sound_EndScene();
}
