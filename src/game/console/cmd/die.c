#include "game/console/cmd/die.h"

#include "decomp/effects.h"
#include "game/sound.h"
#include "global/vars.h"

static COMMAND_RESULT M_Entrypoint(const char *args);

static COMMAND_RESULT M_Entrypoint(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    if (g_LaraItem->hit_points <= 0) {
        return CR_UNAVAILABLE;
    }

    Sound_Effect(SFX_LARA_FALL, &g_LaraItem->pos, SPM_NORMAL);
    Effect_ExplodingDeath(g_Lara.item_num, -1, 1);

    g_LaraItem->hit_points = 0;
    g_LaraItem->flags |= IF_INVISIBLE;
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_Console_Cmd_Die = {
    .prefix = "abortion|natlastinks",
    .proc = M_Entrypoint,
};
