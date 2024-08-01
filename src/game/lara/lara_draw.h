#pragma once

#include "global/types.h"

void __cdecl Lara_Draw(const ITEM_INFO *item);
void __cdecl Lara_Draw_I(
    const ITEM_INFO *item, const FRAME_INFO *frame1, const FRAME_INFO *frame2,
    int32_t frac, int32_t rate);
