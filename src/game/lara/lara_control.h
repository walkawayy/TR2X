#pragma once

#include "global/types.h"

void __cdecl Lara_HandleAboveWater(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_HandleSurface(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_HandleUnderwater(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_Control(int16_t item_num);
void __cdecl Lara_ControlExtra(int16_t item_num);

void __cdecl Lara_Animate(ITEM_INFO *item);

void __cdecl Lara_UseItem(int16_t object_num);

void __cdecl Lara_CheatGetStuff(void);

void __cdecl Lara_InitialiseLoad(int16_t item_num);

void __cdecl Lara_Initialise(INIT_LEVEL_TYPE type);
