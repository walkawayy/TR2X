#include "game/level.h"

#include "game/hwr.h"
#include "game/items.h"
#include "game/shell.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <libtrx/log.h>
#include <libtrx/memory.h>

#include <assert.h>

static void Level_Read(HANDLE handle, void *target, size_t size);
static int8_t Level_ReadS8(HANDLE handle);
static int16_t Level_ReadS16(HANDLE handle);
static int32_t Level_ReadS32(HANDLE handle);
static uint8_t Level_ReadU8(HANDLE handle);
static uint16_t Level_ReadU16(HANDLE handle);
static uint32_t Level_ReadU32(HANDLE handle);

static void __cdecl Level_LoadMeshBase(HANDLE handle);
static void __cdecl Level_LoadMeshes(HANDLE handle);
static int32_t __cdecl Level_LoadAnims(HANDLE handle, int32_t **frame_pointers);
static void __cdecl Level_LoadAnimChanges(HANDLE handle);
static void __cdecl Level_LoadAnimCommands(HANDLE handle);
static void __cdecl Level_LoadAnimBones(HANDLE handle);
static void __cdecl Level_LoadAnimFrames(HANDLE handle);
static void __cdecl Level_LoadObjectsImpl(HANDLE handle);
static void __cdecl Level_LoadStaticObjects(HANDLE handle);
static void __cdecl Level_LoadTextures(HANDLE handle);

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

static void __cdecl Level_LoadMeshBase(HANDLE handle)
{
    const int32_t num_meshes = Level_ReadS32(handle);
    LOG_INFO("%d meshes", num_meshes);
    g_MeshBase = game_malloc(sizeof(int16_t) * num_meshes, GBUF_MESHES);
    Level_Read(handle, g_MeshBase, sizeof(int16_t) * num_meshes);
}

static void __cdecl Level_LoadMeshes(HANDLE handle)
{
    const int32_t num_mesh_ptrs = Level_ReadS32(handle);
    LOG_INFO("%d mesh pointers", num_mesh_ptrs);
    int32_t *const mesh_indices =
        (int32_t *)Memory_Alloc(sizeof(int32_t) * num_mesh_ptrs);
    Level_Read(handle, mesh_indices, sizeof(int32_t) * num_mesh_ptrs);

    g_Meshes =
        game_malloc(sizeof(int16_t *) * num_mesh_ptrs, GBUF_MESH_POINTERS);
    for (int32_t i = 0; i < num_mesh_ptrs; i++) {
        g_Meshes[i] = &g_MeshBase[mesh_indices[i] / 2];
    }

    Memory_Free(mesh_indices);
}

static int32_t __cdecl Level_LoadAnims(HANDLE handle, int32_t **frame_pointers)
{
    const int32_t num_anims = Level_ReadS32(handle);
    LOG_INFO("%d anims", num_anims);
    g_Anims = game_malloc(sizeof(ANIM_STRUCT) * num_anims, GBUF_ANIMS);
    if (frame_pointers != NULL) {
        *frame_pointers = Memory_Alloc(sizeof(int32_t) * num_anims);
    }

    for (int32_t i = 0; i < num_anims; i++) {
        ANIM_STRUCT *const anim = &g_Anims[i];
        const int32_t frame_idx = Level_ReadS32(handle);
        if (frame_pointers != NULL) {
            (*frame_pointers)[i] = frame_idx;
        }
        anim->frame_ptr = NULL; // filled later by the animation frame loader
        anim->interpolation = Level_ReadS16(handle);
        anim->current_anim_state = Level_ReadS16(handle);
        anim->velocity = Level_ReadS32(handle);
        anim->acceleration = Level_ReadS32(handle);
        anim->frame_base = Level_ReadS16(handle);
        anim->frame_end = Level_ReadS16(handle);
        anim->jump_anim_num = Level_ReadS16(handle);
        anim->jump_frame_num = Level_ReadS16(handle);
        anim->num_changes = Level_ReadS16(handle);
        anim->change_idx = Level_ReadS16(handle);
        anim->num_commands = Level_ReadS16(handle);
        anim->command_idx = Level_ReadS16(handle);
    }
    return num_anims;
}

static void __cdecl Level_LoadAnimChanges(HANDLE handle)
{
    const int32_t num_anim_changes = Level_ReadS32(handle);
    LOG_INFO("%d anim changes", num_anim_changes);
    g_AnimChanges =
        game_malloc(sizeof(ANIM_CHANGE) * num_anim_changes, GBUF_STRUCTS);
    for (int32_t i = 0; i < num_anim_changes; i++) {
        ANIM_CHANGE *const change = &g_AnimChanges[i];
        change->goal_anim_state = Level_ReadS16(handle);
        change->num_ranges = Level_ReadS16(handle);
        change->range_idx = Level_ReadS16(handle);
    }
}

static void __cdecl Level_LoadAnimRanges(HANDLE handle)
{
    const int32_t num_anim_ranges = Level_ReadS32(handle);
    LOG_INFO("%d anim ranges", num_anim_ranges);
    g_AnimRanges =
        game_malloc(sizeof(ANIM_RANGE) * num_anim_ranges, GBUF_ANIM_RANGES);
    for (int32_t i = 0; i < num_anim_ranges; i++) {
        ANIM_RANGE *const range = &g_AnimRanges[i];
        range->start_frame = Level_ReadS16(handle);
        range->end_frame = Level_ReadS16(handle);
        range->link_anim_num = Level_ReadS16(handle);
        range->link_frame_num = Level_ReadS16(handle);
    }
}

static void __cdecl Level_LoadAnimCommands(HANDLE handle)
{
    const int32_t num_anim_commands = Level_ReadS32(handle);
    LOG_INFO("%d anim commands", num_anim_commands);
    g_AnimCommands =
        game_malloc(sizeof(int16_t) * num_anim_commands, GBUF_ANIM_COMMANDS);
    Level_Read(handle, g_AnimCommands, sizeof(int16_t) * num_anim_commands);
}

static void __cdecl Level_LoadAnimBones(HANDLE handle)
{
    const int32_t num_anim_bones = Level_ReadS32(handle);
    LOG_INFO("%d anim bones", num_anim_bones);
    g_AnimBones =
        game_malloc(sizeof(int32_t) * num_anim_bones, GBUF_ANIM_BONES);
    Level_Read(handle, g_AnimBones, sizeof(int32_t) * num_anim_bones);
}

static void __cdecl Level_LoadAnimFrames(HANDLE handle)
{
    const int32_t anim_frame_data_size = Level_ReadS32(handle);
    LOG_INFO("%d anim frame data size", anim_frame_data_size);
    g_AnimFrames =
        game_malloc(sizeof(int16_t) * anim_frame_data_size, GBUF_ANIM_FRAMES);
    // TODO: make me FRAME_INFO
    int16_t *ptr = (int16_t *)&g_AnimFrames[0];
    for (int32_t i = 0; i < anim_frame_data_size; i++) {
        ptr[i] = Level_ReadS16(handle);
    }
}

static void __cdecl Level_LoadObjectsImpl(HANDLE handle)
{
    const int32_t num_objects = Level_ReadS32(handle);
    LOG_INFO("%d objects", num_objects);
    for (int32_t i = 0; i < num_objects; i++) {
        const GAME_OBJECT_ID object_num = Level_ReadS32(handle);
        OBJECT_INFO *const object = &g_Objects[object_num];
        object->mesh_count = Level_ReadS16(handle);
        object->mesh_idx = Level_ReadS16(handle);
        object->bone_idx = Level_ReadS32(handle);
        const int32_t frame_idx = Level_ReadS32(handle);
        object->frame_base = ((int16_t *)g_AnimFrames) + frame_idx / 2;
        object->anim_idx = Level_ReadS16(handle);
        object->loaded = 1;
    }
}

static void __cdecl Level_LoadStaticObjects(HANDLE handle)
{
    const int32_t num_static_objects = Level_ReadS32(handle);
    LOG_INFO("%d static objects", num_static_objects);
    for (int32_t i = 0; i < num_static_objects; i++) {
        const int32_t static_num = Level_ReadS32(handle);
        STATIC_INFO *static_obj = &g_StaticObjects[static_num];
        static_obj->mesh_idx = Level_ReadS16(handle);
        static_obj->draw_bounds.min_x = Level_ReadS16(handle);
        static_obj->draw_bounds.max_x = Level_ReadS16(handle);
        static_obj->draw_bounds.min_y = Level_ReadS16(handle);
        static_obj->draw_bounds.max_y = Level_ReadS16(handle);
        static_obj->draw_bounds.min_z = Level_ReadS16(handle);
        static_obj->draw_bounds.max_z = Level_ReadS16(handle);
        static_obj->collision_bounds.min_x = Level_ReadS16(handle);
        static_obj->collision_bounds.max_x = Level_ReadS16(handle);
        static_obj->collision_bounds.min_y = Level_ReadS16(handle);
        static_obj->collision_bounds.max_y = Level_ReadS16(handle);
        static_obj->collision_bounds.min_z = Level_ReadS16(handle);
        static_obj->collision_bounds.max_z = Level_ReadS16(handle);
        static_obj->flags = Level_ReadU16(handle);
    }
}

static void __cdecl Level_LoadTextures(HANDLE handle)
{
    const int32_t num_textures = Level_ReadS32(handle);
    LOG_INFO("%d textures", num_textures);
    if (num_textures > MAX_TEXTURES) {
        Shell_ExitSystem("Too many rooms");
        return;
    }

    for (int32_t i = 0; i < num_textures; i++) {
        PHD_TEXTURE *texture = &g_TextureInfo[i];
        texture->draw_type = Level_ReadU16(handle);
        texture->tex_page = Level_ReadU16(handle);
        for (int32_t j = 0; j < 4; j++) {
            texture->uv[j].u = Level_ReadU16(handle);
            texture->uv[j].v = Level_ReadU16(handle);
        }
    }

    for (int32_t i = 0; i < num_textures; i++) {
        uint16_t *uv = (uint16_t *)&(g_TextureInfo[i].uv[0].u);
        uint8_t byte = 0;
        for (int32_t bit = 0; bit < 8; bit++) {
            if (*uv & 0x80) {
                *uv |= 0xFF;
                byte |= (1 << bit);
            } else {
                *uv &= 0xFF00;
            }
            uv++;
        }
        g_LabTextureUVFlag[i] = byte;
    }
}

BOOL __cdecl Level_LoadObjects(HANDLE handle)
{
    Level_LoadMeshBase(handle);
    Level_LoadMeshes(handle);

    int32_t *frame_pointers = NULL;
    const int32_t num_anims = Level_LoadAnims(handle, &frame_pointers);
    Level_LoadAnimChanges(handle);
    Level_LoadAnimRanges(handle);
    Level_LoadAnimCommands(handle);
    Level_LoadAnimBones(handle);
    Level_LoadAnimFrames(handle);

    for (int32_t i = 0; i < num_anims; i++) {
        ANIM_STRUCT *const anim = &g_Anims[i];
        // TODO: this is horrible
        anim->frame_ptr = ((int16_t *)g_AnimFrames) + frame_pointers[i] / 2;
    }
    Memory_FreePointer(&frame_pointers);

    Level_LoadObjectsImpl(handle);
    InitialiseObjects();
    Level_LoadStaticObjects(handle);
    Level_LoadTextures(handle);

    AdjustTextureUVs(1);
    return true;
}

BOOL __cdecl Level_LoadSprites(HANDLE handle)
{
    const int32_t num_sprites = Level_ReadS32(handle);
    for (int32_t i = 0; i < num_sprites; i++) {
        PHD_SPRITE *const sprite = &g_PhdSprites[i];
        sprite->tex_page = Level_ReadU16(handle);
        sprite->offset = Level_ReadU16(handle);
        sprite->width = Level_ReadU16(handle);
        sprite->height = Level_ReadU16(handle);
        sprite->x0 = Level_ReadS16(handle);
        sprite->y0 = Level_ReadS16(handle);
        sprite->x1 = Level_ReadS16(handle);
        sprite->y1 = Level_ReadS16(handle);
    }

    const int32_t num_statics = Level_ReadS32(handle);
    for (int32_t i = 0; i < num_statics; i++) {
        int32_t object_id = Level_ReadS32(handle);
        if (object_id >= O_NUMBER_OF) {
            object_id -= O_NUMBER_OF;
            STATIC_INFO *const static_object = &g_StaticObjects[object_id];
            SetFilePointer(handle, sizeof(int16_t), NULL, FILE_CURRENT);
            static_object->mesh_idx = Level_ReadS16(handle);
        } else {
            OBJECT_INFO *const object = &g_Objects[object_id];
            object->mesh_count = Level_ReadS16(handle);
            object->mesh_idx = Level_ReadS16(handle);
            object->loaded = 1;
        }
    }

    return true;
}

BOOL __cdecl Level_LoadItems(HANDLE handle)
{
    const int32_t num_items = Level_ReadS32(handle);
    if (!num_items) {
        return true;
    }

    if (num_items > MAX_ITEMS) {
        Shell_ExitSystem("Too many items");
        return false;
    }

    g_Items = game_malloc(sizeof(ITEM_INFO) * MAX_ITEMS, GBUF_ITEMS);
    g_LevelItemCount = num_items;

    Item_InitialiseArray(MAX_ITEMS);

    for (int32_t i = 0; i < num_items; i++) {
        ITEM_INFO *const item = &g_Items[i];
        item->object_num = Level_ReadS16(handle);
        item->room_num = Level_ReadS16(handle);
        item->pos.x = Level_ReadS32(handle);
        item->pos.y = Level_ReadS32(handle);
        item->pos.z = Level_ReadS32(handle);
        item->rot.y = Level_ReadS16(handle);
        item->shade1 = Level_ReadS16(handle);
        item->shade2 = Level_ReadS16(handle);
        item->flags = Level_ReadS16(handle);
        if (item->object_num < 0 || item->object_num >= O_NUMBER_OF) {
            Shell_ExitSystemFmt(
                "Bad object number (%d) on item %d", item->object_num, i);
            return false;
        }
        Item_Initialise(i);
    }

    return true;
}

BOOL __cdecl Level_LoadDepthQ(HANDLE handle)
{
    for (int32_t i = 0; i < 32; i++) {
        Level_Read(handle, g_DepthQTable[i].index, sizeof(uint8_t) * 256);
        g_DepthQTable[i].index[0] = 0;
    }

    if (g_GameVid_IsWindowedVGA) {
        RGB_888 palette[256];
        CopyBitmapPalette(
            g_GamePalette8, g_DepthQTable[0].index, 32 * sizeof(DEPTHQ_ENTRY),
            palette);
        SyncSurfacePalettes(
            g_DepthQTable, 256, 32, 256, g_GamePalette8, g_DepthQTable, 256,
            palette, true);
        memcpy(g_GamePalette8, palette, sizeof(g_GamePalette8));

        for (int32_t i = 0; i < 256; i++) {
            g_DepthQIndex[i] = S_COLOUR(
                g_GamePalette8[i].red, g_GamePalette8[i].green,
                g_GamePalette8[i].blue);
        }
    } else {
        for (int32_t i = 0; i < 256; i++) {
            g_DepthQIndex[i] = g_DepthQTable[24].index[i];
        }
    }

    for (int32_t i = 0; i < 32; i++) {
        for (int32_t j = 0; j < 256; j++) {
            g_GouraudTable[j].index[i] = g_DepthQTable[i].index[j];
        }
    }

    g_IsWet = 0;
    for (int32_t i = 0; i < 256; i++) {
        g_WaterPalette[i].red = g_GamePalette8[i].red * 2 / 3;
        g_WaterPalette[i].green = g_GamePalette8[i].green * 2 / 3;
        g_WaterPalette[i].blue = g_GamePalette8[i].blue;
    }

    return true;
}

BOOL __cdecl Level_LoadPalettes(HANDLE handle)
{
    Level_Read(handle, g_GamePalette8, sizeof(RGB_888) * 256);

    g_GamePalette8[0].red = 0;
    g_GamePalette8[0].green = 0;
    g_GamePalette8[0].blue = 0;

    for (int32_t i = 1; i < 256; i++) {
        RGB_888 *col = &g_GamePalette8[i];
        col->red = (col->red << 2) | (col->red >> 4);
        col->green = (col->green << 2) | (col->green >> 4);
        col->blue = (col->blue << 2) | (col->blue >> 4);
    }

    Level_Read(handle, g_GamePalette16, sizeof(PALETTEENTRY) * 256);
    return true;
}
