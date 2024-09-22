#include "game/gameflow/gameflow_new.h"

#include "game/game_string.h"
#include "game/objects/names.h"
#include "global/enum_str.h"
#include "global/types.h"
#include "global/vars.h"

#include <libtrx/game/gameflow/types.h>
#include <libtrx/log.h>

#include <assert.h>

GAMEFLOW_NEW g_GameflowNew;
GAME_INFO g_GameInfo;

static void M_LoadObjectString(const char *key, const char *value);
static void M_LoadGameString(const char *key, const char *value);
static void M_LoadObjectStrings(const int32_t level_num);
static void M_LoadGameStrings(const int32_t level_num);

static void M_LoadObjectString(const char *const key, const char *const value)
{
    const GAME_OBJECT_ID object =
        ENUM_STRING_GET(GAME_OBJECT_ID, key, NO_OBJECT);
    if (object != NO_OBJECT) {
        Object_SetName(object, value);
    }
}

static void M_LoadGameString(const char *const key, const char *const value)
{
    if (!GameString_IsKnown(key)) {
        LOG_ERROR("Invalid game string key: %s", key);
    } else if (value == NULL) {
        LOG_ERROR("Invalid game string value: %s", key);
    } else {
        GameString_Define(key, value);
    }
}

static void M_LoadObjectStrings(const int32_t level_num)
{
    const GAMEFLOW_NEW *const gf = &g_GameflowNew;

    const GAMEFLOW_NEW_STRING_ENTRY *entry = gf->object_strings;
    while (entry != NULL && entry->key != NULL) {
        M_LoadObjectString(entry->key, entry->value);
        entry++;
    }

    if (level_num >= 0) {
        assert(level_num < gf->level_count);
        const GAMEFLOW_NEW_LEVEL *const level = &gf->levels[level_num];
        entry = level->object_strings;
        while (entry != NULL && entry->key != NULL) {
            M_LoadObjectString(entry->key, entry->value);
            entry++;
        }
    }
}

static void M_LoadGameStrings(const int32_t level_num)
{
    const GAMEFLOW_NEW *const gf = &g_GameflowNew;

    const GAMEFLOW_NEW_STRING_ENTRY *entry = gf->game_strings;
    while (entry != NULL && entry->key != NULL) {
        M_LoadGameString(entry->key, entry->value);
        entry++;
    }

    if (level_num >= 0) {
        assert(level_num < gf->level_count);
        const GAMEFLOW_NEW_LEVEL *const level = &gf->levels[level_num];
        entry = level->game_strings;
        while (entry != NULL && entry->key != NULL) {
            M_LoadGameString(entry->key, entry->value);
            entry++;
        }
    }
}

void GF_N_LoadStrings(const int32_t level_num)
{
    M_LoadObjectStrings(level_num);
    M_LoadGameStrings(level_num);
}

void Gameflow_OverrideCommand(const GAMEFLOW_COMMAND command)
{
    switch (command.action) {
    case GF_START_GAME:
        g_GF_OverrideDir = GFD_START_GAME | command.param;
        break;
    case GF_START_SAVED_GAME:
        g_GF_OverrideDir = GFD_START_SAVED_GAME | command.param;
        break;
    case GF_START_CINE:
        g_GF_OverrideDir = GFD_START_CINE;
        break;
    case GF_START_FMV:
        g_GF_OverrideDir = GFD_START_FMV;
        break;
    case GF_START_DEMO:
        g_GF_OverrideDir = GFD_START_DEMO;
        break;
    case GF_EXIT_TO_TITLE:
        g_GF_OverrideDir = GFD_EXIT_TO_TITLE;
        break;
    case GF_LEVEL_COMPLETE:
        g_GF_OverrideDir = GFD_LEVEL_COMPLETE;
        break;
    case GF_EXIT_GAME:
        g_GF_OverrideDir = GFD_EXIT_GAME;
        break;
    default:
        LOG_ERROR("Not implemented");
        assert(false);
        break;
    }
}
