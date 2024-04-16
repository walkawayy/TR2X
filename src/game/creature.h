#pragma once

#include "global/types.h"

#include <stdint.h>

void __cdecl Creature_Initialise(int16_t item_num);
int32_t __cdecl Creature_Activate(int16_t item_num);
void __cdecl Creature_AIInfo(struct ITEM_INFO *item, struct AI_INFO *info);
void __cdecl Creature_Mood(
    const struct ITEM_INFO *item, const struct AI_INFO *info, int32_t violent);
int32_t __cdecl Creature_CheckBaddieOverlap(int16_t item_num);
void __cdecl Creature_Die(int16_t item_num, bool explode);
int32_t __cdecl Creature_Animate(int16_t item_num, int16_t angle, int16_t tilt);
int16_t __cdecl Creature_Turn(struct ITEM_INFO *item, int16_t max_turn);
