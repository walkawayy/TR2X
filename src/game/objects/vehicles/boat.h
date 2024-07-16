#pragma once

#include "global/types.h"

#include <stdint.h>

void __cdecl Boat_Initialise(int16_t item_num);
int32_t __cdecl Boat_CheckGeton(int16_t item_num, const COLL_INFO *coll);
void __cdecl Boat_Collision(int16_t item_num, ITEM_INFO *lara, COLL_INFO *coll);
int32_t __cdecl Boat_TestWaterHeight(
    const ITEM_INFO *item, int32_t z_off, int32_t x_off, XYZ_32 *pos);
void __cdecl Boat_DoShift(const int32_t boat_num);
void __cdecl Boat_DoWakeEffect(const ITEM_INFO *boat);
int32_t __cdecl Boat_DoDynamics(int32_t height, int32_t fall_speed, int32_t *y);
