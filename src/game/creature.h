#pragma once

#include "global/types.h"

#include <stdint.h>

void __cdecl Creature_Initialise(int16_t item_num);
int32_t __cdecl Creature_Activate(int16_t item_num);
void __cdecl Creature_AIInfo(ITEM_INFO *item, AI_INFO *info);
void __cdecl Creature_Mood(
    const ITEM_INFO *item, const AI_INFO *info, int32_t violent);
int32_t __cdecl Creature_CheckBaddieOverlap(int16_t item_num);
void __cdecl Creature_Die(int16_t item_num, bool explode);
int32_t __cdecl Creature_Animate(int16_t item_num, int16_t angle, int16_t tilt);
int16_t __cdecl Creature_Turn(ITEM_INFO *item, int16_t max_turn);
void __cdecl Creature_Tilt(ITEM_INFO *item, int16_t angle);
void __cdecl Creature_Head(ITEM_INFO *item, int16_t required);
void __cdecl Creature_Neck(ITEM_INFO *item, int16_t required);
void __cdecl Creature_Float(int16_t item_num);
void __cdecl Creature_Underwater(ITEM_INFO *item, int32_t depth);
int16_t __cdecl Creature_Effect(
    const ITEM_INFO *item, const BITE_INFO *bite,
    int16_t(__cdecl *spawn)(
        int32_t x, int32_t y, int32_t z, int16_t speed, int16_t y_rot,
        int16_t room_num));
