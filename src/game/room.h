#pragma once

#include "global/types.h"

#include <stdint.h>

int16_t Room_GetIndexFromPos(int32_t x, int32_t y, int32_t z);
int32_t __cdecl Room_FindGridShift(int32_t src, int32_t dst);
void __cdecl Room_GetNearbyRooms(
    int32_t x, int32_t y, int32_t z, int32_t r, int32_t h, int16_t room_num);
void __cdecl Room_GetNewRoom(int32_t x, int32_t y, int32_t z, int16_t room_num);
int16_t __cdecl Room_GetTiltType(
    const FLOOR_INFO *floor, int32_t x, int32_t y, int32_t z);
