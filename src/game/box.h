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
int32_t __cdecl Box_EscapeBox(
    const struct ITEM_INFO *item, const struct ITEM_INFO *enemy,
    int16_t box_num);
int32_t __cdecl Box_ValidBox(
    const struct ITEM_INFO *item, int16_t zone_num, int16_t box_num);
enum TARGET_TYPE __cdecl Box_CalculateTarget(
    struct XYZ_32 *target, const struct ITEM_INFO *item, struct LOT_INFO *lot);
int32_t __cdecl Box_BadFloor(
    int32_t x, int32_t y, int32_t z, int32_t box_height, int32_t next_height,
    int16_t room_num, const struct LOT_INFO *lot);
