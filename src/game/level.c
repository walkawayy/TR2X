#include "game/level.h"

#include "game/hwr.h"
#include "game/shell.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <assert.h>

static void Level_Read(HANDLE handle, void *target, size_t size);
static int8_t Level_ReadS8(HANDLE handle);
static int16_t Level_ReadS16(HANDLE handle);
static int32_t Level_ReadS32(HANDLE handle);
static uint8_t Level_ReadU8(HANDLE handle);
static uint16_t Level_ReadU16(HANDLE handle);
static uint32_t Level_ReadU32(HANDLE handle);

static void Level_Read(HANDLE handle, void *target, size_t size)
{
    DWORD bytes_read;
    ReadFileSync(handle, target, size, &bytes_read, NULL);
}

static int8_t Level_ReadS8(HANDLE handle)
{
    int8_t result;
    Level_Read(handle, &result, sizeof(result));
    return result;
}

static int16_t Level_ReadS16(HANDLE handle)
{
    int16_t result;
    Level_Read(handle, &result, sizeof(result));
    return result;
}

static int32_t Level_ReadS32(HANDLE handle)
{
    int32_t result;
    Level_Read(handle, &result, sizeof(result));
    return result;
}

static uint8_t Level_ReadU8(HANDLE handle)
{
    uint8_t result;
    Level_Read(handle, &result, sizeof(result));
    return result;
}

static uint16_t Level_ReadU16(HANDLE handle)
{
    uint16_t result;
    Level_Read(handle, &result, sizeof(result));
    return result;
}

static uint32_t Level_ReadU32(HANDLE handle)
{
    uint32_t result;
    Level_Read(handle, &result, sizeof(result));
    return result;
}

BOOL __cdecl Level_LoadTexturePages(HANDLE handle)
{
    const bool is_16_bit = g_TextureFormat.bpp >= 16;
    const int32_t texture_size = TEXTURE_PAGE_WIDTH * TEXTURE_PAGE_HEIGHT;
    const int32_t texture_size_8_bit = texture_size * sizeof(uint8_t);
    const int32_t texture_size_16_bit = texture_size * sizeof(uint16_t);

    const int32_t num_pages = Level_ReadS32(handle);

    if (g_SavedAppSettings.render_mode == RM_SOFTWARE) {
        for (int32_t i = 0; i < num_pages; i++) {
            if (g_TexturePageBuffer8[i] == NULL) {
                g_TexturePageBuffer8[i] =
                    game_malloc(texture_size, GBUF_TEXTURE_PAGES);
            }
            Level_Read(handle, g_TexturePageBuffer8[i], texture_size);
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
            Level_Read(handle, ptr, texture_size_16_bit);
            ptr += texture_size_16_bit;
        }
        HWR_LoadTexturePages(num_pages, base, NULL);
    } else {
        char *ptr = base;
        for (int32_t i = 0; i < num_pages; i++) {
            Level_Read(handle, ptr, texture_size_8_bit);
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
    g_RoomCount = Level_ReadS16(handle);
    if (g_RoomCount > MAX_ROOMS) {
        Shell_ExitSystem("Too many rooms");
        return false;
    }

    g_Rooms = game_malloc(sizeof(ROOM_INFO) * g_RoomCount, GBUF_ROOM_INFOS);
    assert(g_Rooms != NULL);

    for (int32_t i = 0; i < g_RoomCount; i++) {
        ROOM_INFO *const r = &g_Rooms[i];

        r->pos.x = Level_ReadS32(handle);
        r->pos.y = 0;
        r->pos.z = Level_ReadS32(handle);

        r->min_floor = Level_ReadS32(handle);
        r->max_ceiling = Level_ReadS32(handle);

        const int32_t data_size = Level_ReadS32(handle);
        r->data = game_malloc(sizeof(int16_t) * data_size, GBUF_ROOM_MESH);
        Level_Read(handle, r->data, sizeof(int16_t) * data_size);

        const int16_t num_doors = Level_ReadS16(handle);
        if (num_doors <= 0) {
            r->doors = NULL;
        } else {
            r->doors = game_malloc(
                sizeof(DOOR_INFO) * num_doors + sizeof(DOOR_INFOS),
                GBUF_ROOM_DOOR);
            r->doors->count = num_doors;
            Level_Read(handle, r->doors->door, sizeof(DOOR_INFO) * num_doors);
        }

        r->z_size = Level_ReadS16(handle);
        r->x_size = Level_ReadS16(handle);

        r->sector = game_malloc(
            sizeof(SECTOR_INFO) * r->z_size * r->x_size, GBUF_ROOM_FLOOR);
        for (int32_t i = 0; i < r->z_size * r->x_size; i++) {
            SECTOR_INFO *const sector = &r->sector[i];
            sector->idx = Level_ReadU16(handle);
            sector->box = Level_ReadS16(handle);
            sector->pit_room = Level_ReadU8(handle);
            sector->floor = Level_ReadS8(handle);
            sector->sky_room = Level_ReadU8(handle);
            sector->ceiling = Level_ReadS8(handle);
        }

        r->ambient1 = Level_ReadS16(handle);
        r->ambient2 = Level_ReadS16(handle);
        r->light_mode = Level_ReadS16(handle);

        r->num_lights = Level_ReadS16(handle);
        if (!r->num_lights) {
            r->light = NULL;
        } else {
            r->light = game_malloc(
                sizeof(LIGHT_INFO) * r->num_lights, GBUF_ROOM_LIGHTS);
            for (int32_t i = 0; i < r->num_lights; i++) {
                LIGHT_INFO *const light = &r->light[i];
                light->x = Level_ReadS32(handle);
                light->y = Level_ReadS32(handle);
                light->z = Level_ReadS32(handle);
                light->intensity1 = Level_ReadS16(handle);
                light->intensity2 = Level_ReadS16(handle);
                light->falloff1 = Level_ReadS32(handle);
                light->falloff2 = Level_ReadS32(handle);
            }
        }

        r->num_meshes = Level_ReadS16(handle);
        if (!r->num_meshes) {
            r->mesh = NULL;
        } else {
            r->mesh = game_malloc(
                sizeof(MESH_INFO) * r->num_meshes, GBUF_ROOM_STATIC_MESH_INFOS);
            for (int32_t i = 0; i < r->num_meshes; i++) {
                MESH_INFO *const mesh = &r->mesh[i];
                mesh->x = Level_ReadS32(handle);
                mesh->y = Level_ReadS32(handle);
                mesh->z = Level_ReadS32(handle);
                mesh->y_rot = Level_ReadS16(handle);
                mesh->shade1 = Level_ReadS16(handle);
                mesh->shade2 = Level_ReadS16(handle);
                mesh->static_num = Level_ReadS16(handle);
            }
        }

        r->flipped_room = Level_ReadS16(handle);
        r->flags = Level_ReadU16(handle);

        r->bound_active = 0;
        r->bound_left = g_PhdWinMaxX;
        r->bound_top = g_PhdWinMaxY;
        r->bound_bottom = 0;
        r->bound_right = 0;
        r->item_num = NO_ITEM;
        r->fx_num = NO_ITEM;
    }

    const int32_t floor_data_size = Level_ReadS32(handle);
    g_FloorData =
        game_malloc(sizeof(int16_t) * floor_data_size, GBUF_FLOOR_DATA);
    Level_Read(handle, g_FloorData, sizeof(int16_t) * floor_data_size);

    return true;
}
