#include "game/background.h"

#include "global/funcs.h"
#include "global/vars.h"

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
