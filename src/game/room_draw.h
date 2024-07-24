#pragma once

#include "global/types.h"

void __cdecl Room_GetBounds(void);
void __cdecl Room_SetBounds(
    const int16_t *obj_ptr, int32_t room_num, const ROOM_INFO *parent);
void __cdecl Room_Clip(const ROOM_INFO *r);
