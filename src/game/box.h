#pragma once

#include "global/types.h"

#define BOX_BLOCKED 0x4000
#define BOX_BLOCKED_SEARCH 0x8000

int32_t __cdecl Box_SearchLOT(struct LOT_INFO *lot, int32_t expansion);
int32_t __cdecl Box_UpdateLOT(struct LOT_INFO *lot, int32_t expansion);
void __cdecl Box_TargetBox(struct LOT_INFO *lot, int16_t box_num);
int32_t __cdecl Box_StalkBox(
    const struct ITEM_INFO *item, const struct ITEM_INFO *enemy,
    int16_t box_num);
