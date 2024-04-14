#include "game/creature.h"

#include "game/random.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Creature_Initialise(const int16_t item_num)
{
    struct ITEM_INFO *const item = &g_Items[item_num];
    item->rot.y += (Random_GetControl() - PHD_90) >> 1;
    item->collidable = 1;
    item->data = 0;
}

int32_t __cdecl Creature_Activate(const int16_t item_num)
{
    int32_t result; // eax

    struct ITEM_INFO *const item = &g_Items[item_num];
    if (item->status != IS_INVISIBLE) {
        return true;
    }

    if (!LOT_EnableBaddieAI(item_num, false)) {
        return false;
    }

    item->status = IS_ACTIVE;
    return true;
}
