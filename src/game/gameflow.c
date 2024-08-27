#include "game/gameflow.h"

#include "decomp/decomp.h"
#include "game/demo.h"
#include "game/gun/gun.h"
#include "game/overlay.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <libtrx/memory.h>

#include <stdio.h>

#define GF_CURRENT_VERSION 3

static GF_ADD_INV GF_ModifyInventory_GetGunAdder(LARA_GUN_TYPE gun_type);
static GF_ADD_INV GF_ModifyInventory_GetAmmoAdder(LARA_GUN_TYPE gun_type);
static GF_ADD_INV GF_ModifyInventory_GetItemAdder(GAME_OBJECT_ID object_id);
static void GF_ModifyInventory_GunOrAmmo(
    START_INFO *start, int32_t type, LARA_GUN_TYPE gun_type);
static void GF_ModifyInventory_Item(int32_t type, GAME_OBJECT_ID object_id);

static GF_ADD_INV GF_ModifyInventory_GetGunAdder(const LARA_GUN_TYPE gun_type)
{
    // clang-format off
    switch (gun_type) {
    case LGT_PISTOLS: return GF_ADD_INV_PISTOLS;
    case LGT_MAGNUMS: return GF_ADD_INV_MAGNUMS;
    case LGT_UZIS:    return GF_ADD_INV_UZIS;
    case LGT_SHOTGUN: return GF_ADD_INV_SHOTGUN;
    case LGT_HARPOON: return GF_ADD_INV_HARPOON;
    case LGT_M16:     return GF_ADD_INV_M16;
    case LGT_GRENADE: return GF_ADD_INV_GRENADE;
    default:          return (GF_ADD_INV)-1;
    }
    // clang-format on
}

static GF_ADD_INV GF_ModifyInventory_GetAmmoAdder(const LARA_GUN_TYPE gun_type)
{
    // clang-format off
    switch (gun_type) {
    case LGT_PISTOLS: return GF_ADD_INV_PISTOL_AMMO;
    case LGT_MAGNUMS: return GF_ADD_INV_MAGNUM_AMMO;
    case LGT_UZIS:    return GF_ADD_INV_UZI_AMMO;
    case LGT_SHOTGUN: return GF_ADD_INV_SHOTGUN_AMMO;
    case LGT_HARPOON: return GF_ADD_INV_HARPOON_AMMO;
    case LGT_M16:     return GF_ADD_INV_M16_AMMO;
    case LGT_GRENADE: return GF_ADD_INV_GRENADE_AMMO;
    default:          return (GF_ADD_INV)-1;
    }
    // clang-format on
}

static GF_ADD_INV GF_ModifyInventory_GetItemAdder(
    const GAME_OBJECT_ID object_id)
{
    // clang-format off
    switch (object_id) {
    case O_FLARE_ITEM:          return GF_ADD_INV_FLARES;
    case O_SMALL_MEDIPACK_ITEM: return GF_ADD_INV_SMALL_MEDI;
    case O_LARGE_MEDIPACK_ITEM: return GF_ADD_INV_LARGE_MEDI;
    case O_PICKUP_ITEM_1:       return GF_ADD_INV_PICKUP_1;
    case O_PICKUP_ITEM_2:       return GF_ADD_INV_PICKUP_2;
    case O_PUZZLE_ITEM_1:       return GF_ADD_INV_PUZZLE_1;
    case O_PUZZLE_ITEM_2:       return GF_ADD_INV_PUZZLE_2;
    case O_PUZZLE_ITEM_3:       return GF_ADD_INV_PUZZLE_3;
    case O_PUZZLE_ITEM_4:       return GF_ADD_INV_PUZZLE_4;
    case O_KEY_ITEM_1:          return GF_ADD_INV_KEY_1;
    case O_KEY_ITEM_2:          return GF_ADD_INV_KEY_2;
    case O_KEY_ITEM_3:          return GF_ADD_INV_KEY_3;
    case O_KEY_ITEM_4:          return GF_ADD_INV_KEY_4;
    default:                    return (GF_ADD_INV)-1;
    }
    // clang-format on
}

static void GF_ModifyInventory_GunOrAmmo(
    START_INFO *const start, const int32_t type, const LARA_GUN_TYPE gun_type)
{
    const GAME_OBJECT_ID gun_item = Gun_GetGunObject(gun_type);
    const GAME_OBJECT_ID ammo_item = Gun_GetAmmoObject(gun_type);
    const int32_t ammo_qty = Gun_GetAmmoQuantity(gun_type);
    AMMO_INFO *const ammo_info = Gun_GetAmmoInfo(gun_type);

    const GF_ADD_INV gun_adder = GF_ModifyInventory_GetGunAdder(gun_type);
    const GF_ADD_INV ammo_adder = GF_ModifyInventory_GetAmmoAdder(gun_type);

    if (Inv_RequestItem(gun_item)) {
        if (type == 1) {
            ammo_info->ammo += ammo_qty * g_GF_SecretInvItems[ammo_adder];
            for (int32_t i = 0; i < g_GF_SecretInvItems[ammo_adder]; i++) {
                Overlay_AddDisplayPickup(ammo_item);
            }
        } else if (type == 0) {
            ammo_info->ammo += ammo_qty * g_GF_Add2InvItems[ammo_adder];
        }
    } else if (
        (type == 0 && g_GF_Add2InvItems[gun_adder])
        || (type == 1 && g_GF_SecretInvItems[gun_adder])) {

        // clang-format off
        // TODO: consider moving this to Inv_AddItem
        switch (gun_type) {
        case LGT_PISTOLS: start->has_pistols = 1; break;
        case LGT_MAGNUMS: start->has_magnums = 1; break;
        case LGT_UZIS:    start->has_uzis = 1;    break;
        case LGT_SHOTGUN: start->has_shotgun = 1; break;
        case LGT_HARPOON: start->has_harpoon = 1; break;
        case LGT_M16:     start->has_m16 = 1;     break;
        case LGT_GRENADE: start->has_grenade = 1; break;
        default: break;
        }
        // clang-format on

        Inv_AddItem(gun_item);

        if (type == 1) {
            ammo_info->ammo += ammo_qty * g_GF_SecretInvItems[ammo_adder];
            Overlay_AddDisplayPickup(gun_item);
            for (int32_t i = 0; i < g_GF_SecretInvItems[ammo_adder]; i++) {
                Overlay_AddDisplayPickup(ammo_item);
            }
        } else if (type == 0) {
            ammo_info->ammo += ammo_qty * g_GF_Add2InvItems[ammo_adder];
        }
    } else if (type == 1) {
        for (int32_t i = 0; i < g_GF_SecretInvItems[ammo_adder]; i++) {
            Inv_AddItem(ammo_item);
            Overlay_AddDisplayPickup(ammo_item);
        }
    } else if (type == 0) {
        for (int32_t i = 0; i < g_GF_Add2InvItems[ammo_adder]; i++) {
            Inv_AddItem(ammo_item);
        }
    }
}

static void GF_ModifyInventory_Item(
    const int32_t type, const GAME_OBJECT_ID object_id)
{
    const GF_ADD_INV item_adder = GF_ModifyInventory_GetItemAdder(object_id);
    int32_t qty = 0;
    if (type == 1) {
        qty = g_GF_SecretInvItems[item_adder];
    } else if (type == 0) {
        qty = g_GF_Add2InvItems[item_adder];
    }

    for (int32_t i = 0; i < qty; i++) {
        Inv_AddItem(object_id);
        if (type == 1) {
            Overlay_AddDisplayPickup(object_id);
        }
    }
}

// TODO: inline me into GF_LoadScriptFile
BOOL __cdecl GF_LoadFromFile(const char *const file_name)
{
    DWORD bytes_read;

    const char *full_path = GetFullPath(file_name);
    HANDLE handle = CreateFileA(
        file_name, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL,
        NULL);
    if (handle == INVALID_HANDLE_VALUE) {
        return false;
    }

    ReadFileSync(
        handle, &g_GF_ScriptVersion, sizeof(int32_t), &bytes_read, NULL);
    if (g_GF_ScriptVersion != GF_CURRENT_VERSION) {
        return false;
    }

    ReadFileSync(handle, &g_GF_Description, 256, &bytes_read, NULL);

    {
        int16_t size;
        ReadFileSync(handle, &size, sizeof(size), &bytes_read, NULL);
        if (size != sizeof(GAME_FLOW)) {
            return false;
        }
        ReadFileSync(handle, &g_GameFlow, sizeof(GAME_FLOW), &bytes_read, NULL);
    }

    g_GF_LevelNames = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_LevelNames, &g_GF_LevelNamesBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_PicFilenames = Memory_Alloc(sizeof(char *) * g_GameFlow.num_pictures);
    if (!GF_ReadStringTable(
            g_GameFlow.num_pictures, g_GF_PicFilenames, &g_GF_PicFilenamesBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_TitleFileNames = Memory_Alloc(sizeof(char *) * g_GameFlow.num_titles);
    if (!GF_ReadStringTable(
            g_GameFlow.num_titles, g_GF_TitleFileNames, &g_GF_TitleFileNamesBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_FMVFilenames = Memory_Alloc(sizeof(char *) * g_GameFlow.num_fmvs);
    if (!GF_ReadStringTable(
            g_GameFlow.num_fmvs, g_GF_FMVFilenames, &g_GF_FMVFilenamesBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_LevelFileNames = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_LevelFileNames, &g_GF_LevelFileNamesBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_CutsceneFileNames =
        Memory_Alloc(sizeof(char *) * g_GameFlow.num_cutscenes);
    if (!GF_ReadStringTable(
            g_GameFlow.num_cutscenes, g_GF_CutsceneFileNames,
            &g_GF_CutsceneFileNamesBuf, &bytes_read, handle)) {
        return false;
    }

    ReadFileSync(
        handle, &g_GF_LevelOffsets,
        sizeof(int16_t) * (g_GameFlow.num_levels + 1), &bytes_read, NULL);
    {
        int16_t size;
        ReadFileSync(handle, &size, sizeof(int16_t), &bytes_read, NULL);
        g_GF_SequenceBuf = Memory_Alloc(size);
        ReadFileSync(handle, g_GF_SequenceBuf, size, &bytes_read, NULL);
    }

    g_GF_FrontendSequence = g_GF_SequenceBuf;
    for (int32_t i = 0; i < g_GameFlow.num_levels; i++) {
        g_GF_ScriptTable[i] = g_GF_SequenceBuf + (g_GF_LevelOffsets[i + 1] / 2);
    }

    ReadFileSync(
        handle, g_GF_ValidDemos, sizeof(int16_t) * g_GameFlow.num_demos,
        &bytes_read, NULL);

    int16_t size;
    ReadFileSync(handle, &size, sizeof(int16_t), &bytes_read, NULL);
    if (size != GF_S_GAME_NUMBER_OF) {
        return false;
    }

    g_GF_GameStrings = Memory_Alloc(sizeof(char *) * GF_S_GAME_NUMBER_OF);
    if (!GF_ReadStringTable(
            GF_S_GAME_NUMBER_OF, g_GF_GameStrings, &g_GF_GameStringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_PCStrings = Memory_Alloc(sizeof(char *) * GF_S_PC_NUMBER_OF);
    if (!GF_ReadStringTable(
            GF_S_PC_NUMBER_OF, g_GF_PCStrings, &g_GF_PCStringsBuf, &bytes_read,
            handle)) {
        return false;
    }

    g_GF_Pickup1Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Pickup1Strings, &g_GF_Pickup1StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Pickup2Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Pickup2Strings, &g_GF_Pickup2StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Puzzle1Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Puzzle1Strings, &g_GF_Puzzle1StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Puzzle2Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Puzzle2Strings, &g_GF_Puzzle2StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Puzzle3Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Puzzle3Strings, &g_GF_Puzzle3StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Puzzle4Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Puzzle4Strings, &g_GF_Puzzle4StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Key1Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Key1Strings, &g_GF_Key1StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Key2Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Key2Strings, &g_GF_Key2StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Key3Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Key3Strings, &g_GF_Key3StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    g_GF_Key4Strings = Memory_Alloc(sizeof(char *) * g_GameFlow.num_levels);
    if (!GF_ReadStringTable(
            g_GameFlow.num_levels, g_GF_Key4Strings, &g_GF_Key4StringsBuf,
            &bytes_read, handle)) {
        return false;
    }

    CloseHandle(handle);
    return true;
}

BOOL __cdecl GF_ReadStringTable(
    const int32_t count, char **const table, char **const buffer,
    LPDWORD bytes_read, HANDLE handle)
{
    ReadFileSync(
        handle, g_GF_LevelOffsets, sizeof(int16_t) * count, bytes_read, NULL);

    int16_t size;
    ReadFileSync(handle, &size, sizeof(int16_t), bytes_read, NULL);

    *buffer = Memory_Alloc(size);
    ReadFileSync(handle, *buffer, size, bytes_read, NULL);

    if (g_GameFlow.cyphered_strings) {
        for (int32_t i = 0; i < size; i++) {
            (*buffer)[i] ^= g_GameFlow.cypher_code;
        }
    }

    for (int32_t i = 0; i < count; i++) {
        const int32_t offset = g_GF_LevelOffsets[i];
        table[i] = &(*buffer)[offset];
    }

    return true;
}

int32_t __cdecl GF_LoadScriptFile(const char *const fname)
{
    g_GF_SunsetEnabled = false;

    if (!GF_LoadFromFile(fname)) {
        return false;
    }

    g_GameFlow.level_complete_track = MX_END_OF_LEVEL;

    g_Inv_Item_Stopwatch.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_STATISTICS];
    g_Inv_Item_Pistols.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PISTOLS];
    g_Inv_Item_Flare.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_FLARE];
    g_Inv_Item_Shotgun.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_SHOTGUN];
    g_Inv_Item_Magnums.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_MAGNUMS];
    g_Inv_Item_Uzis.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_UZIS];
    g_Inv_Item_Harpoon.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_HARPOON];
    g_Inv_Item_M16.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_M16];
    g_Inv_Item_Grenade.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_GRENADE];
    g_Inv_Item_PistolAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_PISTOL_AMMO];
    g_Inv_Item_ShotgunAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_SHOTGUN_AMMO];
    g_Inv_Item_MagnumAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_MAGNUM_AMMO];
    g_Inv_Item_UziAmmo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_UZI_AMMO];
    g_Inv_Item_HarpoonAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_HARPOON_AMMO];
    g_Inv_Item_M16Ammo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_M16_AMMO];
    g_Inv_Item_GrenadeAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_GRENADE_AMMO];
    g_Inv_Item_SmallMedi.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_SMALL_MEDIPACK];
    g_Inv_Item_LargeMedi.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_LARGE_MEDIPACK];
    g_Inv_Item_Pickup1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PICKUP];
    g_Inv_Item_Pickup2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PICKUP];
    g_Inv_Item_Puzzle1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle3.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle4.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Key1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key3.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key4.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];

    g_Inv_Item_Passport.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_GAME];
    g_Inv_Item_Graphics.string = g_GF_PCStrings[GF_S_PC_DETAIL_LEVELS];
    ;
    g_Inv_Item_Sound.string = g_GF_PCStrings[GF_S_PC_SOUND];
    g_Inv_Item_Controls.string = g_GF_PCStrings[GF_S_PC_CONTROLS];
    g_Inv_Item_Photo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_LARA_HOME];

    SetRequesterHeading(
        &g_LoadGameRequester, g_GF_GameStrings[GF_S_GAME_PASSPORT_SELECT_LEVEL],
        0, 0, 0);
    SetRequesterHeading(
        &g_SaveGameRequester, g_GF_GameStrings[GF_S_GAME_PASSPORT_SELECT_LEVEL],
        0, 0, 0);

    return true;
}

int32_t __cdecl GF_DoFrontendSequence(void)
{
    const GAME_FLOW_DIR option =
        GF_InterpretSequence(g_GF_FrontendSequence, GFL_NORMAL, 1);
    return option == GFD_EXIT_GAME;
}

int32_t __cdecl GF_DoLevelSequence(
    const int32_t start_level, const GF_LEVEL_TYPE type)
{
    int32_t current_level = start_level;
    while (true) {
        if (current_level > g_GameFlow.num_levels - 1) {
            g_IsTitleLoaded = 0;
            return GFD_EXIT_TO_TITLE;
        }

        const int16_t *const ptr = g_GF_ScriptTable[current_level];
        const GAME_FLOW_DIR option = GF_InterpretSequence(ptr, type, 0);
        current_level++;

        if (g_GameFlow.single_level >= 0) {
            return option;
        }
        if ((option & ~0xFF) != GFD_LEVEL_COMPLETE) {
            return option;
        }
    }
}

int32_t __cdecl GF_InterpretSequence(
    const int16_t *ptr, GF_LEVEL_TYPE type, const int32_t seq_type)
{
    g_GF_NoFloor = false;
    g_GF_DeadlyWater = false;
    g_GF_SunsetEnabled = false;
    g_GF_LaraStartAnim = 0;
    g_GF_Kill2Complete = false;
    g_GF_RemoveAmmo = false;
    g_GF_RemoveWeapons = false;

    for (int32_t i = 0; i < GF_ADD_INV_NUMBER_OF; i++) {
        g_GF_SecretInvItems[i] = 0;
        g_GF_Add2InvItems[i] = 0;
    }

    g_GF_MusicTracks[0] = 2;
    g_CineTargetAngle = PHD_90;
    g_GF_NumSecrets = 3;

    int32_t ntracks = 0;
    GAME_FLOW_DIR option = GFD_EXIT_TO_TITLE;

    while (*ptr != GFE_END_SEQ) {
        switch (*ptr) {
        case GFE_PICTURE:
            ptr += 2;
            break;

        case GFE_LIST_START:
        case GFE_LIST_END:
            ptr++;
            break;

        case GFE_PLAY_FMV:
            if (type != GFL_SAVED) {
                if (ptr[2] == GFE_PLAY_FMV) {
                    if (S_IntroFMV(
                            g_GF_FMVFilenames[ptr[1]],
                            g_GF_FMVFilenames[ptr[3]])) {
                        return GFD_EXIT_GAME;
                    }
                    ptr += 2;
                } else if (S_PlayFMV(g_GF_FMVFilenames[ptr[1]])) {
                    return GFD_EXIT_GAME;
                }
            }
            ptr += 2;
            break;

        case GFE_START_LEVEL:
            if (ptr[1] > g_GameFlow.num_levels) {
                option = GFD_EXIT_TO_TITLE;
            } else if (type != GFL_STORY) {
                if (type == GFL_MIDSTORY) {
                    return GFD_EXIT_TO_TITLE;
                }
                option = StartGame(ptr[1], type);
                g_GF_StartGame = 0;
                if (type == GFL_SAVED) {
                    type = GFL_NORMAL;
                }
                if ((option & ~0xFF) != GFD_LEVEL_COMPLETE) {
                    return option;
                }
            }
            ptr += 2;
            break;

        case GFE_CUTSCENE:
            if (type != GFL_SAVED) {
                const int16_t level = g_CurrentLevel;
                const int32_t result = Game_Cutscene_Start(ptr[1]);
                g_CurrentLevel = level;
                if (result == 2
                    && (type == GFL_STORY || type == GFL_MIDSTORY)) {
                    return GFD_EXIT_TO_TITLE;
                }
                if (result == 3) {
                    return GFD_EXIT_GAME;
                }
            }
            ptr += 2;
            break;

        case GFE_LEVEL_COMPLETE:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                if (LevelStats(g_CurrentLevel)) {
                    return GFD_EXIT_TO_TITLE;
                }
                option = GFD_START_GAME | (g_CurrentLevel + 1);
            }
            ptr++;
            break;

        case GFE_DEMO_PLAY:
            if (type != GFL_SAVED && type != GFL_STORY
                && type != GFL_MIDSTORY) {
                return Demo_Start(ptr[1]);
            }
            ptr += 2;
            break;

        case GFE_JUMP_TO_SEQ:
            ptr += 2;
            break;

        case GFE_SET_TRACK:
            g_GF_MusicTracks[ntracks] = ptr[1];
            Game_SetCutsceneTrack(ptr[1]);
            ntracks++;
            ptr += 2;
            break;

        case GFE_SUNSET:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_SunsetEnabled = true;
            }
            ptr++;
            break;

        case GFE_LOADING_PIC:
            ptr += 2;
            break;

        case GFE_DEADLY_WATER:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_DeadlyWater = true;
            }
            ptr++;
            break;

        case GFE_REMOVE_WEAPONS:
            if (type != GFL_STORY && type != GFL_MIDSTORY
                && type != GFL_SAVED) {
                g_GF_RemoveWeapons = true;
            }
            ptr++;
            break;

        case GFE_GAME_COMPLETE:
            DisplayCredits();
            if (GameStats(g_CurrentLevel)) {
                return GFD_EXIT_TO_TITLE;
            }
            option = GFD_EXIT_TO_TITLE;
            ptr++;
            break;

        case GFE_CUT_ANGLE:
            if (type != GFL_SAVED) {
                g_CineTargetAngle = ptr[1];
            }
            ptr += 2;
            break;

        case GFE_NO_FLOOR:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_NoFloor = ptr[1];
            }
            ptr += 2;
            break;

        case GFE_ADD_TO_INV:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                if (ptr[1] < 1000) {
                    g_GF_SecretInvItems[ptr[1]]++;
                } else if (type != GFL_SAVED) {
                    g_GF_Add2InvItems[ptr[1] - 1000]++;
                }
            }
            ptr += 2;
            break;

        case GFE_START_ANIM:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_LaraStartAnim = ptr[1];
            }
            ptr += 2;
            break;

        case GFE_NUM_SECRETS:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_NumSecrets = ptr[1];
            }
            ptr += 2;
            break;

        case GFE_KILL_TO_COMPLETE:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_Kill2Complete = true;
            }
            ptr++;
            break;

        case GFE_REMOVE_AMMO:
            if (type != GFL_STORY && type != GFL_MIDSTORY
                && type != GFL_SAVED) {
                g_GF_RemoveAmmo = true;
            }
            ptr++;
            break;

        default:
            return GFD_EXIT_GAME;
        }
    }

    if (type == GFL_STORY || type == GFL_MIDSTORY) {
        return 0;
    }
    return option;
}

void __cdecl GF_ModifyInventory(const int32_t level, const int32_t type)
{
    START_INFO *const start = &g_SaveGame.start[level];

    if (!start->has_pistols && g_GF_Add2InvItems[GF_ADD_INV_PISTOLS]) {
        start->has_pistols = 1;
        Inv_AddItem(O_PISTOL_ITEM);
    }

    GF_ModifyInventory_GunOrAmmo(start, type, LGT_MAGNUMS);
    GF_ModifyInventory_GunOrAmmo(start, type, LGT_UZIS);
    GF_ModifyInventory_GunOrAmmo(start, type, LGT_SHOTGUN);
    GF_ModifyInventory_GunOrAmmo(start, type, LGT_HARPOON);
    GF_ModifyInventory_GunOrAmmo(start, type, LGT_M16);
    GF_ModifyInventory_GunOrAmmo(start, type, LGT_GRENADE);

    GF_ModifyInventory_Item(type, O_FLARE_ITEM);
    GF_ModifyInventory_Item(type, O_SMALL_MEDIPACK_ITEM);
    GF_ModifyInventory_Item(type, O_LARGE_MEDIPACK_ITEM);
    GF_ModifyInventory_Item(type, O_PICKUP_ITEM_1);
    GF_ModifyInventory_Item(type, O_PICKUP_ITEM_2);
    GF_ModifyInventory_Item(type, O_PUZZLE_ITEM_1);
    GF_ModifyInventory_Item(type, O_PUZZLE_ITEM_2);
    GF_ModifyInventory_Item(type, O_PUZZLE_ITEM_3);
    GF_ModifyInventory_Item(type, O_PUZZLE_ITEM_4);
    GF_ModifyInventory_Item(type, O_KEY_ITEM_1);
    GF_ModifyInventory_Item(type, O_KEY_ITEM_2);
    GF_ModifyInventory_Item(type, O_KEY_ITEM_3);
    GF_ModifyInventory_Item(type, O_KEY_ITEM_4);

    for (int32_t i = 0; i < GF_ADD_INV_NUMBER_OF; i++) {
        if (type == 1) {
            g_GF_SecretInvItems[i] = 0;
        } else if (type == 0) {
            g_GF_Add2InvItems[i] = 0;
        }
    }
}
