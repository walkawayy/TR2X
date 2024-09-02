#include "game/console_cmd.h"

#include "game/console.h"
#include "game/items.h"
#include "game/lara/lara_cheat.h"
#include "game/random.h"
#include "game/sound.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <libtrx/strings.h>

#include <math.h>
#include <stdio.h>

static bool Console_Cmd_IsFloatRound(float num);
static COMMAND_RESULT Console_Cmd_Pos(const char *args);
static COMMAND_RESULT Console_Cmd_Teleport(const char *args);
static COMMAND_RESULT Console_Cmd_SetHealth(const char *args);
static COMMAND_RESULT Console_Cmd_Heal(const char *args);
static COMMAND_RESULT Console_Cmd_Fly(const char *const args);
static COMMAND_RESULT Console_Cmd_EndLevel(const char *args);
static COMMAND_RESULT Console_Cmd_StartLevel(const char *args);
static COMMAND_RESULT Console_Cmd_LoadGame(const char *args);
static COMMAND_RESULT Console_Cmd_SaveGame(const char *args);
static COMMAND_RESULT Console_Cmd_StartDemo(const char *args);
static COMMAND_RESULT Console_Cmd_ExitToTitle(const char *args);
static COMMAND_RESULT Console_Cmd_ExitGame(const char *args);
static COMMAND_RESULT Console_Cmd_Abortion(const char *args);

static inline bool Console_Cmd_IsFloatRound(const float num)
{
    return (fabsf(num) - roundf(num)) < 0.0001f;
}

static COMMAND_RESULT Console_Cmd_Pos(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    Console_Log(
        "Room: %d\nPosition: %.3f, %.3f, %.3f\nRotation: %.3f,%.3f,%.3f",
        g_LaraItem->room_num, g_LaraItem->pos.x / (float)WALL_L,
        g_LaraItem->pos.y / (float)WALL_L, g_LaraItem->pos.z / (float)WALL_L,
        g_LaraItem->rot.x * 360.0f / (float)PHD_ONE,
        g_LaraItem->rot.y * 360.0f / (float)PHD_ONE,
        g_LaraItem->rot.z * 360.0f / (float)PHD_ONE);
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Teleport(const char *const args)
{
    if (!g_Objects[O_LARA].loaded || !g_LaraItem->hit_points) {
        return CR_UNAVAILABLE;
    }

    // X Y Z
    {
        float x, y, z;
        if (sscanf(args, "%f %f %f", &x, &y, &z) == 3) {
            if (Console_Cmd_IsFloatRound(x)) {
                x += 0.5f;
            }
            if (Console_Cmd_IsFloatRound(z)) {
                z += 0.5f;
            }

            if (Item_Teleport(g_LaraItem, x * WALL_L, y * WALL_L, z * WALL_L)) {
                Console_Log("Teleported to position: %.3f %.3f %.3f", x, y, z);
                return CR_SUCCESS;
            }

            Console_Log(
                "Failed to teleport to position: %.3f %.3f %.3f", x, y, z);
            return CR_FAILURE;
        }
    }

    // Room number
    {
        int16_t room_num = NO_ROOM_NEG;
        if (sscanf(args, "%hd", &room_num) == 1) {
            if (room_num < 0 || room_num >= g_RoomCount) {
                Console_Log(
                    "Invalid room: %d. Valid rooms are 0-%d", room_num,
                    g_RoomCount - 1);
                return CR_FAILURE;
            }

            const ROOM_INFO *const room = &g_Rooms[room_num];

            const int32_t x1 = room->pos.x + WALL_L;
            const int32_t x2 =
                (room->x_size << WALL_SHIFT) + room->pos.x - WALL_L;
            const int32_t y1 = room->min_floor;
            const int32_t y2 = room->max_ceiling;
            const int32_t z1 = room->pos.z + WALL_L;
            const int32_t z2 =
                (room->z_size << WALL_SHIFT) + room->pos.z - WALL_L;

            for (int32_t i = 0; i < 100; i++) {
                int32_t x = x1 + Random_GetControl() * (x2 - x1) / 0x7FFF;
                int32_t y = y1;
                int32_t z = z1 + Random_GetControl() * (z2 - z1) / 0x7FFF;
                if (Item_Teleport(g_LaraItem, x, y, z)) {
                    Console_Log("Teleported to room: %d", room_num);
                    return CR_SUCCESS;
                }
            }

            Console_Log("Failed to teleport to room: %d", room_num);
            return CR_FAILURE;
        }
    }

    return CR_BAD_INVOCATION;
}

static COMMAND_RESULT Console_Cmd_SetHealth(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    if (strcmp(args, "") == 0) {
        Console_Log("Current Lara's health: %d", g_LaraItem->hit_points);
        return CR_SUCCESS;
    }

    int32_t hp;
    if (sscanf(args, "%d", &hp) != 1) {
        return CR_BAD_INVOCATION;
    }

    g_LaraItem->hit_points = hp;
    Console_Log("Lara's health set to %d", hp);
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Heal(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    if (g_LaraItem->hit_points == LARA_MAX_HITPOINTS) {
        Console_Log("Lara's already at full health");
        return CR_SUCCESS;
    }

    g_LaraItem->hit_points = LARA_MAX_HITPOINTS;
    Console_Log("Healed Lara back to full health");
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Fly(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }
    Lara_Cheat_EnterFlyMode();
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_EndLevel(const char *const args)
{
    if (strcmp(args, "") == 0) {
        Lara_Cheat_EndLevel();
        return CR_SUCCESS;
    }
    return CR_FAILURE;
}

static COMMAND_RESULT Console_Cmd_StartLevel(const char *const args)
{
    int32_t level_to_load = -1;

    if (level_to_load == -1) {
        int32_t num = 0;
        if (sscanf(args, "%d", &num) == 1) {
            level_to_load = num;
        }
    }

    if (level_to_load == -1 && strlen(args) >= 2) {
        for (int i = 0; i < g_GameFlow.num_levels; i++) {
            if (String_CaseSubstring(g_GF_LevelNames[i], args) != NULL) {
                level_to_load = i;
                break;
            }
        }
    }

    if (level_to_load == -1 && String_Equivalent(args, "gym")) {
        level_to_load = LV_GYM;
    }

    if (level_to_load >= g_GameFlow.num_levels) {
        Console_Log("Invalid level");
        return CR_FAILURE;
    }

    if (level_to_load != -1) {
        g_GF_OverrideDir = GFD_START_GAME | level_to_load;
        Console_Log("Loading %s", g_GF_LevelNames[level_to_load]);
        return CR_SUCCESS;
    }

    return CR_BAD_INVOCATION;
}

static COMMAND_RESULT Console_Cmd_LoadGame(const char *const args)
{
    int32_t slot_num;
    if (!String_ParseInteger(args, &slot_num)) {
        return CR_BAD_INVOCATION;
    }

    // convert 1-indexing to 0-indexing
    const int32_t slot_idx = slot_num - 1;

    if (slot_idx < 0 || slot_idx >= MAX_SAVE_SLOTS) {
        Console_Log("Invalid save slot %d", slot_num);
        return CR_FAILURE;
    }

    // TODO: replace this with a proper status check
    if (g_SavedLevels[slot_idx] <= 0) {
        Console_Log("Save slot %d is not available", slot_num);
        return CR_FAILURE;
    }

    g_GF_OverrideDir = GFD_START_SAVED_GAME | slot_idx;
    Console_Log("Loaded game from save slot %d", slot_num);
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_SaveGame(const char *const args)
{
    int32_t slot_num = -1;
    if (sscanf(args, "%d", &slot_num) != 1) {
        return CR_BAD_INVOCATION;
    }

    // convert 1-indexing to 0-indexing
    const int32_t slot_idx = slot_num - 1;

    if (slot_idx < 0 || slot_idx >= MAX_SAVE_SLOTS) {
        Console_Log("Invalid save slot %d", slot_num);
        return CR_BAD_INVOCATION;
    }

    if (g_LaraItem == NULL) {
        Console_Log("Cannot save the game in the current state", slot_num);
        return CR_UNAVAILABLE;
    }

    CreateSaveGameInfo();
    S_SaveGame(&g_SaveGame, sizeof(SAVEGAME_INFO), slot_idx);
    GetSavedGamesList(&g_LoadGameRequester);
    Console_Log("Saved game to save slot %d", slot_num);
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_StartDemo(const char *const args)
{
    g_GF_OverrideDir = GFD_START_DEMO;
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_ExitToTitle(const char *const args)
{
    g_GF_OverrideDir = GFD_EXIT_TO_TITLE;
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_ExitGame(const char *const args)
{
    g_GF_OverrideDir = GFD_EXIT_GAME;
    return CR_SUCCESS;
}

static COMMAND_RESULT Console_Cmd_Abortion(const char *const args)
{
    if (!g_Objects[O_LARA].loaded) {
        return CR_UNAVAILABLE;
    }

    if (g_LaraItem->hit_points <= 0) {
        return CR_UNAVAILABLE;
    }

    Sound_Effect(SFX_LARA_FALL, &g_LaraItem->pos, SPM_NORMAL);
    Effect_ExplodingDeath(g_Lara.item_num, -1, 1);

    g_LaraItem->hit_points = 0;
    g_LaraItem->flags |= IF_INVISIBLE;
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_ConsoleCommands[] = {
    { .prefix = "pos", .proc = Console_Cmd_Pos },
    { .prefix = "tp", .proc = Console_Cmd_Teleport },
    { .prefix = "hp", .proc = Console_Cmd_SetHealth },
    { .prefix = "heal", .proc = Console_Cmd_Heal },
    { .prefix = "fly", .proc = Console_Cmd_Fly },
    { .prefix = "endlevel", .proc = Console_Cmd_EndLevel },
    { .prefix = "play", .proc = Console_Cmd_StartLevel },
    { .prefix = "level", .proc = Console_Cmd_StartLevel },
    { .prefix = "load", .proc = Console_Cmd_LoadGame },
    { .prefix = "save", .proc = Console_Cmd_SaveGame },
    { .prefix = "demo", .proc = Console_Cmd_StartDemo },
    { .prefix = "title", .proc = Console_Cmd_ExitToTitle },
    { .prefix = "exit", .proc = Console_Cmd_ExitGame },
    { .prefix = "abortion", .proc = Console_Cmd_Abortion },
    { .prefix = "natlastinks", .proc = Console_Cmd_Abortion },
    { .prefix = NULL, .proc = NULL },
};
