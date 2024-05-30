#include "game/background.h"

#include "game/hwr.h"
#include "global/funcs.h"
#include "global/vars.h"

#define TEXTURE_WIDTH 256
#define TEXTURE_HEIGHT 256

void __cdecl BGND_Make640x480(uint8_t *bitmap, RGB_888 *palette)
{
    if (g_TextureFormat.bpp >= 16) {
        g_BGND_PaletteIndex = -1;
    } else {
        g_BGND_PaletteIndex = CreateTexturePalette(palette);
    }

    const int32_t buf_size = 640 * 480 * 2;
    uint8_t *buf = game_malloc(buf_size, GBUF_TEMP_ALLOC);
    UT_MemBlt(buf, 0, 0, 256, 256, 256, bitmap, 0, 0, 640);
    BGND_AddTexture(0, buf, g_BGND_PaletteIndex, palette);

    UT_MemBlt(buf, 0, 0, 256, 256, 256, bitmap, 256, 0, 640);
    BGND_AddTexture(1, buf, g_BGND_PaletteIndex, palette);

    UT_MemBlt(buf, 0, 0, 128, 256, 256, bitmap, 512, 0, 640);
    UT_MemBlt(buf, 128, 0, 128, 224, 256, bitmap, 512, 256, 640);
    BGND_AddTexture(2, buf, g_BGND_PaletteIndex, palette);

    UT_MemBlt(buf, 0, 0, 256, 224, 256, bitmap, 0, 256, 640);
    BGND_AddTexture(3, buf, g_BGND_PaletteIndex, palette);

    UT_MemBlt(buf, 0, 0, 256, 224, 256, bitmap, 256, 256, 640);
    BGND_AddTexture(4, buf, g_BGND_PaletteIndex, palette);

    game_free(buf_size);

    BGND_GetPageHandles();

    g_BGND_PictureIsReady = true;
}

int32_t __cdecl BGND_AddTexture(
    const int32_t tile_idx, uint8_t *const bitmap, const int32_t pal_index,
    const RGB_888 *const bmp_pal)
{
    int32_t page_index;
    if (pal_index < 0) {
        uint8_t *bmp_src = &bitmap[TEXTURE_WIDTH * TEXTURE_HEIGHT];
        uint16_t *bmp_dst =
            &((uint16_t *)bitmap)[TEXTURE_WIDTH * TEXTURE_HEIGHT];
        for (int i = 0; i < TEXTURE_WIDTH * TEXTURE_HEIGHT; i++) {
            bmp_src--;
            bmp_dst--;

            const RGB_888 *const color = &bmp_pal[*bmp_src];

            *bmp_dst = (1 << 15) | (((uint16_t)color->red >> 3) << 10)
                | (((uint16_t)color->green >> 3) << 5)
                | (((uint16_t)color->blue >> 3));
        }

        page_index = AddTexturePage16(256, 256, bmp_src);
    } else {
        page_index = AddTexturePage8(256, 256, bitmap, pal_index);
    }

    g_BGND_TexturePageIndexes[tile_idx] = page_index >= 0 ? page_index : -1;
    return page_index;
}

void __cdecl BGND_GetPageHandles(void)
{
    for (int32_t i = 0; i < BGND_MAX_TEXTURE_PAGES; i++) {
        g_BGND_PageHandles[i] = g_BGND_TexturePageIndexes[i] >= 0
            ? GetTexturePageHandle(g_BGND_TexturePageIndexes[i])
            : 0;
    }
}

void __cdecl BGND_DrawInGameBlack(void)
{
    HWR_EnableZBuffer(false, false);
    DrawQuad(
        (float)g_PhdWinMinX, (float)g_PhdWinMinY, (float)g_PhdWinWidth,
        (float)g_PhdWinHeight, 0);
    HWR_EnableZBuffer(true, true);
}

void __cdecl DrawQuad(
    const float sx, const float sy, const float width, const float height,
    const D3DCOLOR color)
{
    D3DTLVERTEX vertex[4];

    vertex[0].sx = sx;
    vertex[0].sy = sy;

    vertex[1].sx = sx + width;
    vertex[1].sy = sy;

    vertex[2].sx = sx;
    vertex[2].sy = sy + height;

    vertex[3].sx = sx + width;
    vertex[3].sy = sy + height;

    for (int32_t i = 0; i < 4; i++) {
        vertex[i].sz = 0;
        vertex[i].rhw = g_FltRhwONearZ;
        vertex[i].color = RGBA_SETALPHA(color, 0xFF);
        vertex[i].specular = 0;
    }

    HWR_TexSource(0);
    HWR_EnableColorKey(false);
    HWR_DrawPrimitive(D3DPT_TRIANGLESTRIP, &vertex, 4, true);
}
