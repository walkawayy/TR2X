#include "game/level.h"

#include "game/hwr.h"
#include "game/items.h"
#include "game/shell.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "specific/s_audio_sample.h"

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
            handle, num_pages * texture_size_16_bit, NULL, FILE_CURRENT);
        return true;
    }

    char *const base = GlobalAlloc(
        GMEM_FIXED,
        num_pages * (is_16_bit ? texture_size_16_bit : texture_size_8_bit));
    if (base == NULL) {
        return false;
    }

    if (is_16_bit) {
        SetFilePointer(
            handle, num_pages * texture_size_8_bit, NULL, FILE_CURRENT);
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
            handle, num_pages * texture_size_16_bit, NULL, FILE_CURRENT);
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
        Shell_ExitSystem("Too many textures");
        return;
    }

    g_TextureInfoCount = num_textures;
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
        uint16_t *const uv = &g_TextureInfo[i].uv[0].u;
        uint8_t byte = 0;
        for (int32_t j = 0; j < 8; j++) {
            if ((uv[j] & 0x80) != 0) {
                uv[j] |= 0xFF;
                byte |= 1 << j;
            } else {
                uv[j] &= 0xFF00;
            }
        }
        g_LabTextureUVFlag[i] = byte;
    }

    AdjustTextureUVs(true);
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
    return true;
}

BOOL __cdecl Level_LoadSprites(HANDLE handle)
{
    const int32_t num_sprites = Level_ReadS32(handle);
    LOG_DEBUG("%d sprites", num_sprites);
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
    LOG_DEBUG("%d statics", num_statics);
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
    LOG_DEBUG("%d items", num_items);
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

BOOL __cdecl Level_LoadCameras(HANDLE handle)
{
    g_NumCameras = Level_ReadS32(handle);
    LOG_DEBUG("%d fixed cameras", g_NumCameras);
    if (!g_NumCameras) {
        return true;
    }

    g_Camera.fixed =
        game_malloc(sizeof(OBJECT_VECTOR) * g_NumCameras, GBUF_CAMERAS);
    for (int32_t i = 0; i < g_NumCameras; i++) {
        OBJECT_VECTOR *const camera = &g_Camera.fixed[i];
        camera->x = Level_ReadS32(handle);
        camera->y = Level_ReadS32(handle);
        camera->z = Level_ReadS32(handle);
        camera->data = Level_ReadS16(handle);
        camera->flags = Level_ReadS16(handle);
    }
    return true;
}

BOOL __cdecl Level_LoadSoundEffects(HANDLE handle)
{
    g_SoundEffectCount = Level_ReadS32(handle);
    LOG_DEBUG("%d sound effects", g_SoundEffectCount);
    if (!g_SoundEffectCount) {
        return true;
    }

    g_SoundEffects =
        game_malloc(sizeof(OBJECT_VECTOR) * g_SoundEffectCount, GBUF_SOUND_FX);
    for (int32_t i = 0; i < g_SoundEffectCount; i++) {
        OBJECT_VECTOR *const effect = &g_SoundEffects[i];
        effect->x = Level_ReadS32(handle);
        effect->y = Level_ReadS32(handle);
        effect->z = Level_ReadS32(handle);
        effect->data = Level_ReadS16(handle);
        effect->flags = Level_ReadS16(handle);
    }
    return true;
}

BOOL __cdecl Level_LoadBoxes(HANDLE handle)
{
    g_BoxCount = Level_ReadS32(handle);
    g_Boxes = game_malloc(sizeof(BOX_INFO) * g_BoxCount, GBUF_BOXES);
    for (int32_t i = 0; i < g_BoxCount; i++) {
        BOX_INFO *const box = &g_Boxes[i];
        box->left = Level_ReadU8(handle);
        box->right = Level_ReadU8(handle);
        box->top = Level_ReadU8(handle);
        box->bottom = Level_ReadU8(handle);
        box->height = Level_ReadS16(handle);
        box->overlap_index = Level_ReadS16(handle);
    }

    const int32_t num_overlaps = Level_ReadS32(handle);
    g_Overlap = game_malloc(sizeof(uint16_t) * num_overlaps, GBUF_OVERLAPS);
    Level_Read(handle, g_Overlap, sizeof(uint16_t) * num_overlaps);

    for (int32_t i = 0; i < 2; i++) {
        for (int32_t j = 0; j < 4; j++) {
            const bool skip = j == 2
                || (j == 1 && !g_Objects[O_SPIDER_or_WOLF].loaded
                    && !g_Objects[O_SKIDOO_ARMED].loaded)
                || (j == 3 && !g_Objects[O_YETI].loaded
                    && !g_Objects[O_WORKER_3].loaded);

            if (skip) {
                SetFilePointer(
                    handle, sizeof(int16_t) * g_BoxCount, NULL, FILE_CURRENT);
                continue;
            }

            g_GroundZone[j][i] =
                game_malloc(sizeof(int16_t) * g_BoxCount, GBUF_GROUND_ZONE);
            Level_Read(
                handle, g_GroundZone[j][i], sizeof(int16_t) * g_BoxCount);
        }

        g_FlyZone[i] = game_malloc(sizeof(int16_t) * g_BoxCount, GBUF_FLY_ZONE);
        Level_Read(handle, g_FlyZone[i], sizeof(int16_t) * g_BoxCount);
    }

    return true;
}

BOOL __cdecl Level_LoadAnimatedTextures(HANDLE handle)
{
    const int32_t num_ranges = Level_ReadS32(handle);
    g_AnimTextureRanges = game_malloc(
        sizeof(int16_t) * num_ranges, GBUF_ANIMATING_TEXTURE_RANGES);
    Level_Read(handle, g_AnimTextureRanges, sizeof(int16_t) * num_ranges);
    return true;
}

BOOL __cdecl Level_LoadCinematic(HANDLE handle)
{
    g_NumCineFrames = Level_ReadS16(handle);
    if (g_NumCineFrames <= 0) {
        g_CineLoaded = false;
        return true;
    }

    g_CineData = game_malloc(
        sizeof(CINE_FRAME) * g_NumCineFrames, GBUF_CINEMATIC_FRAMES);
    for (int32_t i = 0; i < g_NumCineFrames; i++) {
        CINE_FRAME *const frame = &g_CineData[i];
        frame->tx = Level_ReadS16(handle);
        frame->ty = Level_ReadS16(handle);
        frame->tz = Level_ReadS16(handle);
        frame->cx = Level_ReadS16(handle);
        frame->cy = Level_ReadS16(handle);
        frame->cz = Level_ReadS16(handle);
        frame->fov = Level_ReadS16(handle);
        frame->roll = Level_ReadS16(handle);
    }
    g_CineLoaded = true;
    return true;
}

BOOL __cdecl Level_LoadDemo(HANDLE handle)
{
    g_DemoCount = 0;

    // TODO: is the allocation necessary if there's no demo data?
    // TODO: do not hardcode the allocation size
    g_DemoPtr = game_malloc(36000, GBUF_LOAD_DEMO_BUFFER);

    const int32_t demo_size = Level_ReadU16(handle);
    LOG_DEBUG("%d demo input size", demo_size);
    if (!demo_size) {
        g_IsDemoLoaded = false;
        return true;
    }

    Level_Read(handle, g_DemoPtr, demo_size);
    g_IsDemoLoaded = true;
    return true;
}

void __cdecl Level_LoadDemoExternal(const char *const level_name)
{
    char file_name[MAX_PATH];
    strcpy(file_name, level_name);
    ChangeFileNameExtension(file_name, "DEM");

    HANDLE handle = CreateFileA(
        file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        return;
    }

    // TODO: do not hardcode the allocation size
    DWORD bytes_read;
    ReadFileSync(handle, g_DemoPtr, 36000, &bytes_read, 0);
    g_IsDemoLoaded = bytes_read != 0;
    CloseHandle(handle);
}

BOOL __cdecl Level_LoadSamples(HANDLE handle)
{
    g_SoundIsActive = false;
    if (!S_Audio_Sample_IsEnabled()) {
        return true;
    }

    S_Audio_Sample_CloseAllTracks();

    Level_Read(handle, g_SampleLUT, sizeof(int16_t) * SFX_NUMBER_OF);
    g_NumSampleInfos = Level_ReadS32(handle);
    LOG_DEBUG("%d sample infos", g_NumSampleInfos);
    if (!g_NumSampleInfos) {
        return false;
    }

    g_SampleInfos =
        game_malloc(sizeof(SAMPLE_INFO) * g_NumSampleInfos, GBUF_SAMPLE_INFOS);
    for (int32_t i = 0; i < g_NumSampleInfos; i++) {
        SAMPLE_INFO *const sample_info = &g_SampleInfos[i];
        sample_info->number = Level_ReadS16(handle);
        sample_info->volume = Level_ReadS16(handle);
        sample_info->randomness = Level_ReadS16(handle);
        sample_info->flags = Level_ReadS16(handle);
    }

    const int32_t num_samples = Level_ReadS32(handle);
    LOG_DEBUG("%d samples", num_samples);
    if (!num_samples) {
        return false;
    }
    int32_t *sample_offsets = Memory_Alloc(sizeof(int32_t) * num_samples);
    Level_Read(handle, sample_offsets, sizeof(int32_t) * num_samples);

    const char *const file_name = "data\\main.sfx";
    const char *const full_path = GetFullPath(file_name);
    LOG_DEBUG("Loading samples from %s", full_path);
    HANDLE sfx_handle = CreateFileA(
        full_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        NULL);

    if (sfx_handle == INVALID_HANDLE_VALUE) {
        Shell_ExitSystemFmt(
            "Could not open %s file: 0x%x", file_name, GetLastError());
        return false;
    }

    // TODO: refactor these WAVE/RIFF shenanigans
    int32_t sample_id = 0;
    for (int32_t i = 0; sample_id < num_samples; i++) {
        char header[0x2C];
        Level_Read(sfx_handle, header, 0x2C);
        if (*(int32_t *)(header + 0) != 0x46464952
            || *(int32_t *)(header + 8) != 0x45564157
            || *(int32_t *)(header + 36) != 0x61746164) {
            LOG_ERROR("Unexpected sample header for sample %d", i);
            Memory_FreePointer(&sample_offsets);
            return false;
        }
        const int32_t data_size = *(int32_t *)(header + 0x28);
        const int32_t aligned_size = (data_size + 1) & ~1;

        *(int16_t *)(header + 16) = 0;
        if (sample_offsets[sample_id] != i) {
            SetFilePointer(sfx_handle, aligned_size, NULL, FILE_CURRENT);
            continue;
        }

        char *sample_data = Memory_Alloc(aligned_size);
        Level_Read(sfx_handle, sample_data, aligned_size);
        // TODO: do not reconstruct the header in S_Audio_Sample_Load, just
        // pass the entire sample directly
        if (!S_Audio_Sample_Load(
                sample_id, (LPWAVEFORMATEX)(header + 20), sample_data,
                data_size)) {
            return false;
        }
        Memory_FreePointer(&sample_data);

        sample_id++;
    }

    Memory_FreePointer(&sample_offsets);
    CloseHandle(sfx_handle);
    g_SoundIsActive = true;
    return true;
}

bool __cdecl Level_Load(const char *const file_name, const int32_t level_num)
{
    init_game_malloc();

    const char *full_path = GetFullPath(file_name);
    strcpy(g_LevelFileName, full_path);

    HANDLE handle = CreateFileA(
        full_path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        Shell_ExitSystemFmt(
            "Could not open %s (level %d)", full_path, level_num);
        return false;
    }

    const int32_t version = Level_ReadS32(handle);
    if (version > 45) {
        Shell_ExitSystemFmt(
            "FATAL: Level %d (%s) requires a new TOMB2.EXE (version %d) to run",
            level_num, full_path, file_name);
        return false;
    }

    if (version < 45) {
        Shell_ExitSystemFmt(
            "FATAL: Level %d (%s) is OUT OF DATE (version %d). COPY NEW "
            "EDITORS AND REMAKE LEVEL",
            level_num, full_path, file_name);
        return false;
    }

    g_LevelFilePalettesOffset = SetFilePointer(handle, 0, NULL, FILE_CURRENT);
    if (!Level_LoadPalettes(handle))
        return false;

    g_LevelFileTexPagesOffset = SetFilePointer(handle, 0, NULL, FILE_CURRENT);
    if (!Level_LoadTexturePages(handle))
        return false;

    SetFilePointer(handle, 4, NULL, FILE_CURRENT);
    if (!Level_LoadRooms(handle))
        return false;
    if (!Level_LoadObjects(handle))
        return false;
    if (!Level_LoadSprites(handle))
        return false;
    if (!Level_LoadCameras(handle))
        return false;
    if (!Level_LoadSoundEffects(handle))
        return false;
    if (!Level_LoadBoxes(handle))
        return false;
    if (!Level_LoadAnimatedTextures(handle))
        return false;
    if (!Level_LoadItems(handle))
        return false;

    g_LevelFileDepthQOffset = SetFilePointer(handle, 0, NULL, FILE_CURRENT);
    if (!Level_LoadDepthQ(handle))
        return false;

    if (!Level_LoadCinematic(handle))
        return false;
    if (!Level_LoadDemo(handle))
        return false;
    if (!Level_LoadSamples(handle))
        return false;

    Level_LoadDemoExternal(full_path);
    CloseHandle(handle);
    return true;
}
