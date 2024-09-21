#include "game/effects.h"
#include "global/vars.h"

#include <libtrx/game/lara/misc.h>
#include <libtrx/game/math.h>

void __cdecl Lara_Extinguish(void)
{
    if (!g_Lara.burn) {
        return;
    }

    g_Lara.burn = 0;

    // put out flame objects
    int16_t fx_num = g_NextEffectActive;
    while (fx_num != NO_ITEM) {
        FX_INFO *const fx = &g_Effects[fx_num];
        const int16_t next_fx_num = fx->next_active;
        if (fx->object_id == O_FLAME && fx->counter < 0) {
            fx->counter = 0;
            Effect_Kill(fx_num);
        }
        fx_num = next_fx_num;
    }
}
