#pragma once

#include "global/types.h"

#define BGND_MAX_TEXTURE_PAGES 5

void __cdecl BGND_Make640x480(uint8_t *bitmap, RGB_888 *palette);
int32_t __cdecl BGND_AddTexture(
    int32_t tile_idx, uint8_t *bitmap, int32_t pal_index,
    const RGB_888 *bmp_pal);
void __cdecl BGND_GetPageHandles(void);
