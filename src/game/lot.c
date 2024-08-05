#include "game/lot.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl LOT_InitialiseArray(void)
{
    g_BaddieSlots =
        game_malloc(NUM_SLOTS * sizeof(CREATURE_INFO), GBUF_CREATURE_DATA);

    for (int32_t i = 0; i < NUM_SLOTS; i++) {
        CREATURE_INFO *const creature = &g_BaddieSlots[i];
        creature->item_num = NO_ITEM;
        creature->lot.node =
            game_malloc(g_BoxCount * sizeof(BOX_NODE), GBUF_CREATURE_LOT);
    }

    g_SlotsUsed = 0;
}
