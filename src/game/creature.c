#include "game/creature.h"

#include "game/random.h"
#include "global/const.h"
#include "global/vars.h"

void __cdecl Creature_Initialise(const int16_t item_num)
{
    struct ITEM_INFO *const item = &g_Items[item_num];
    item->rot.y += (Random_GetControl() - PHD_90) >> 1;
    item->collidable = 1;
    item->data = 0;
}
