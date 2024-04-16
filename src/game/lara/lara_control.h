#pragma once

#include "global/types.h"

void __cdecl Lara_HandleAboveWater(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_HandleSurface(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_HandleUnderwater(ITEM_INFO *item, COLL_INFO *coll);
