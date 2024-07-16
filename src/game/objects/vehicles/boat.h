#pragma once

#include "global/types.h"

#include <stdint.h>

void __cdecl Boat_Initialise(int16_t item_num);
int32_t __cdecl Boat_CheckGeton(int16_t item_num, const COLL_INFO *coll);
void __cdecl Boat_Collision(int16_t item_num, ITEM_INFO *lara, COLL_INFO *coll);
int32_t __cdecl Boat_TestWaterHeight(
    const ITEM_INFO *item, int32_t z_off, int32_t x_off, XYZ_32 *pos);
