#pragma once

#include "global/types.h"

#include <stdint.h>

void __cdecl Boat_Initialise(int16_t item_num);
int32_t __cdecl Boat_CheckGeton(int16_t item_num, const COLL_INFO *coll);
