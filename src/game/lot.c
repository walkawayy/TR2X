#include "game/lot.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#include <assert.h>

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

void __cdecl LOT_DisableBaddieAI(const int16_t item_num)
{
    CREATURE_INFO *creature;

    if (item_num == g_Lara.item_num) {
        creature = g_Lara.creature;
        g_Lara.creature = NULL;
    } else {
        ITEM_INFO *const item = &g_Items[item_num];
        creature = (CREATURE_INFO *)item->data;
        item->data = NULL;
    }

    if (creature != NULL) {
        creature->item_num = NO_ITEM;
        g_SlotsUsed--;
    }
}

bool __cdecl LOT_EnableBaddieAI(const int16_t item_num, const bool always)
{
    if (g_Lara.item_num == item_num) {
        if (g_Lara.creature != NULL) {
            return true;
        }
    } else if (g_Items[item_num].data != NULL) {
        return true;
    }

    if (g_SlotsUsed < NUM_SLOTS) {
        for (int32_t slot = 0; slot < NUM_SLOTS; slot++) {
            if (g_BaddieSlots->item_num == NO_ITEM) {
                InitialiseSlot(item_num, slot);
                return true;
            }
        }
        assert(false);
    }

    int32_t worst_dist = 0;
    if (!always) {
        const ITEM_INFO *const item = &g_Items[item_num];
        const int32_t dx = (item->pos.x - g_Camera.pos.pos.x) >> 8;
        const int32_t dy = (item->pos.y - g_Camera.pos.pos.y) >> 8;
        const int32_t dz = (item->pos.z - g_Camera.pos.pos.z) >> 8;
        worst_dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);
    }

    int32_t worst_slot = -1;
    for (int32_t slot = 0; slot < NUM_SLOTS; slot++) {
        const int32_t item_num = g_BaddieSlots[slot].item_num;
        const ITEM_INFO *const item = &g_Items[item_num];
        const int32_t dx = (item->pos.x - g_Camera.pos.pos.x) >> 8;
        const int32_t dy = (item->pos.y - g_Camera.pos.pos.y) >> 8;
        const int32_t dz = (item->pos.z - g_Camera.pos.pos.z) >> 8;
        const int32_t dist = SQUARE(dx) + SQUARE(dy) + SQUARE(dz);
        if (dist > worst_dist) {
            worst_dist = dist;
            worst_slot = slot;
        }
    }

    if (worst_slot < 0) {
        return false;
    }

    const CREATURE_INFO *const creature = &g_BaddieSlots[worst_slot];
    g_Items[creature->item_num].status = IS_INVISIBLE;
    LOT_DisableBaddieAI(creature->item_num);
    InitialiseSlot(item_num, worst_slot);
    return true;
}
