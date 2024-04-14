#pragma once

#include "global/types.h"

#define BOX_BLOCKED 0x4000
#define BOX_BLOCKED_SEARCH 0x8000

int32_t __cdecl Box_SearchLOT(struct LOT_INFO *lot, int32_t expansion);
int32_t __cdecl Box_UpdateLOT(struct LOT_INFO *lot, int32_t expansion);
