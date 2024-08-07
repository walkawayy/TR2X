#include "game/level.h"

#include "game/hwr.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

BOOL __cdecl Level_LoadTexturePages(HANDLE handle)
{
    const bool is_16_bit = g_TextureFormat.bpp >= 16;
    const int32_t texture_size = TEXTURE_PAGE_WIDTH * TEXTURE_PAGE_HEIGHT;
    const int32_t texture_size_8_bit = texture_size * sizeof(uint8_t);
    const int32_t texture_size_16_bit = texture_size * sizeof(uint16_t);

    int32_t num_pages;
    DWORD bytes_read;
    ReadFileSync(handle, &num_pages, sizeof(int32_t), &bytes_read, NULL);

    if (g_SavedAppSettings.render_mode == RM_SOFTWARE) {
        for (int32_t i = 0; i < num_pages; i++) {
            if (g_TexturePageBuffer8[i] == NULL) {
                g_TexturePageBuffer8[i] =
                    game_malloc(texture_size, GBUF_TEXTURE_PAGES);
            }
            ReadFileSync(
                handle, g_TexturePageBuffer8[i], texture_size, &bytes_read,
                NULL);
        }
        // skip 16-bit texture pages
        SetFilePointer(
            handle, num_pages * texture_size_16_bit, 0, FILE_CURRENT);
        return true;
    }

    char *const base = GlobalAlloc(
        GMEM_FIXED,
        num_pages * (is_16_bit ? texture_size_16_bit : texture_size_8_bit));
    if (base == NULL) {
        return false;
    }

    if (is_16_bit) {
        SetFilePointer(handle, num_pages * texture_size_8_bit, 0, FILE_CURRENT);
        char *ptr = base;
        for (int32_t i = 0; i < num_pages; i++) {
            ReadFileSync(handle, ptr, texture_size_16_bit, &bytes_read, NULL);
            ptr += texture_size_16_bit;
        }
        HWR_LoadTexturePages(num_pages, base, NULL);
    } else {
        char *ptr = base;
        for (int32_t i = 0; i < num_pages; i++) {
            ReadFileSync(handle, ptr, texture_size_8_bit, &bytes_read, NULL);
            ptr += texture_size_8_bit;
        }
        SetFilePointer(
            handle, num_pages * texture_size_16_bit, 0, FILE_CURRENT);
        HWR_LoadTexturePages((int32_t)num_pages, base, g_GamePalette8);
    }

    g_TexturePageCount = num_pages;
    GlobalFree(base);
    return true;
}
