#include "game/level.h"

#include "game/hwr.h"
#include "game/shell.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <assert.h>

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

BOOL __cdecl Level_LoadRooms(HANDLE handle)
{
    DWORD bytes_read;

    ReadFileSync(handle, &g_RoomCount, sizeof(int16_t), &bytes_read, NULL);
    if (g_RoomCount > MAX_ROOMS) {
        Shell_ExitSystem("Too many rooms");
        return false;
    }

    g_Rooms = game_malloc(sizeof(ROOM_INFO) * g_RoomCount, GBUF_ROOM_INFOS);
    assert(g_Rooms != NULL);

    for (int32_t i = 0; i < g_RoomCount; i++) {
        ROOM_INFO *const r = &g_Rooms[i];

        ReadFileSync(handle, &r->pos.x, sizeof(int32_t), &bytes_read, NULL);
        r->pos.y = 0;
        ReadFileSync(handle, &r->pos.z, sizeof(int32_t), &bytes_read, NULL);

        ReadFileSync(handle, &r->min_floor, sizeof(int32_t), &bytes_read, NULL);
        ReadFileSync(
            handle, &r->max_ceiling, sizeof(int32_t), &bytes_read, NULL);

        int32_t data_size;
        ReadFileSync(handle, &data_size, sizeof(int32_t), &bytes_read, NULL);
        r->data = game_malloc(sizeof(int16_t) * data_size, GBUF_ROOM_MESH);
        ReadFileSync(
            handle, r->data, sizeof(int16_t) * data_size, &bytes_read, NULL);

        int16_t num_doors;
        ReadFileSync(handle, &num_doors, sizeof(int16_t), &bytes_read, NULL);
        if (num_doors <= 0) {
            r->doors = NULL;
        } else {
            r->doors = game_malloc(
                sizeof(DOOR_INFO) * num_doors + sizeof(DOOR_INFOS),
                GBUF_ROOM_DOOR);
            r->doors->count = num_doors;
            ReadFileSync(
                handle, r->doors->door, sizeof(DOOR_INFO) * num_doors,
                &bytes_read, NULL);
        }

        ReadFileSync(handle, &r->z_size, sizeof(int16_t), &bytes_read, NULL);
        ReadFileSync(handle, &r->x_size, sizeof(int16_t), &bytes_read, NULL);

        r->sector = game_malloc(
            sizeof(SECTOR_INFO) * r->z_size * r->x_size, GBUF_ROOM_FLOOR);
        for (int32_t i = 0; i < r->z_size * r->x_size; i++) {
            SECTOR_INFO *const sector = &r->sector[i];
            ReadFileSync(
                handle, &sector->idx, sizeof(uint16_t), &bytes_read, NULL);
            ReadFileSync(
                handle, &sector->box, sizeof(int16_t), &bytes_read, NULL);
            ReadFileSync(
                handle, &sector->pit_room, sizeof(uint8_t), &bytes_read, NULL);
            ReadFileSync(
                handle, &sector->floor, sizeof(int8_t), &bytes_read, NULL);
            ReadFileSync(
                handle, &sector->sky_room, sizeof(uint8_t), &bytes_read, NULL);
            ReadFileSync(
                handle, &sector->ceiling, sizeof(int8_t), &bytes_read, NULL);
        }

        ReadFileSync(handle, &r->ambient1, sizeof(int16_t), &bytes_read, NULL);
        ReadFileSync(handle, &r->ambient2, sizeof(int16_t), &bytes_read, NULL);
        ReadFileSync(
            handle, &r->light_mode, sizeof(int16_t), &bytes_read, NULL);

        ReadFileSync(
            handle, &r->num_lights, sizeof(int16_t), &bytes_read, NULL);
        if (!r->num_lights) {
            r->light = NULL;
        } else {
            r->light = game_malloc(
                sizeof(LIGHT_INFO) * r->num_lights, GBUF_ROOM_LIGHTS);
            for (int32_t i = 0; i < r->num_lights; i++) {
                LIGHT_INFO *const light = &r->light[i];
                ReadFileSync(
                    handle, &light->x, sizeof(int32_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &light->y, sizeof(int32_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &light->z, sizeof(int32_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &light->intensity1, sizeof(int16_t), &bytes_read,
                    NULL);
                ReadFileSync(
                    handle, &light->intensity2, sizeof(int16_t), &bytes_read,
                    NULL);
                ReadFileSync(
                    handle, &light->falloff1, sizeof(int32_t), &bytes_read,
                    NULL);
                ReadFileSync(
                    handle, &light->falloff2, sizeof(int32_t), &bytes_read,
                    NULL);
            }
        }

        ReadFileSync(
            handle, &r->num_meshes, sizeof(int16_t), &bytes_read, NULL);
        if (!r->num_meshes) {
            r->mesh = NULL;
        } else {
            r->mesh = game_malloc(
                sizeof(MESH_INFO) * r->num_meshes, GBUF_ROOM_STATIC_MESH_INFOS);
            for (int32_t i = 0; i < r->num_meshes; i++) {
                MESH_INFO *const mesh = &r->mesh[i];
                ReadFileSync(
                    handle, &mesh->x, sizeof(int32_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &mesh->y, sizeof(int32_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &mesh->z, sizeof(int32_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &mesh->y_rot, sizeof(int16_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &mesh->shade1, sizeof(int16_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &mesh->shade2, sizeof(int16_t), &bytes_read, NULL);
                ReadFileSync(
                    handle, &mesh->static_num, sizeof(int16_t), &bytes_read,
                    NULL);
            }
        }

        ReadFileSync(
            handle, &r->flipped_room, sizeof(int16_t), &bytes_read, NULL);
        ReadFileSync(handle, &r->flags, sizeof(uint16_t), &bytes_read, NULL);

        r->bound_active = 0;
        r->bound_left = g_PhdWinMaxX;
        r->bound_top = g_PhdWinMaxY;
        r->bound_bottom = 0;
        r->bound_right = 0;
        r->item_num = NO_ITEM;
        r->fx_num = NO_ITEM;
    }

    int32_t floor_data_size;
    ReadFileSync(handle, &floor_data_size, sizeof(int32_t), &bytes_read, NULL);
    g_FloorData =
        game_malloc(sizeof(int16_t) * floor_data_size, GBUF_FLOOR_DATA);
    ReadFileSync(
        handle, g_FloorData, sizeof(int16_t) * floor_data_size, &bytes_read,
        NULL);

    return true;
}
