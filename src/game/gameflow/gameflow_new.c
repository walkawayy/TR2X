#include "game/gameflow/gameflow_new.h"

#include "game/game_string.h"
#include "global/enum_str.h"
#include "global/types.h"

#include <assert.h>

GAMEFLOW_NEW g_GameflowNew;

static void GF_N_LoadGameString(const char *key, const char *value);
static void GF_N_LoadGameStrings(const int32_t level_num);

static void GF_N_LoadGameString(const char *const key, const char *const value)
{
    const GAME_STRING_ID game_string =
        ENUM_STRING_GET(GAME_STRING_ID, key, GS_INVALID);
    if (game_string != GS_INVALID) {
        GameString_Set(game_string, value);
    }
}

static void GF_N_LoadGameStrings(const int32_t level_num)
{
    const GAMEFLOW_NEW *const gf = &g_GameflowNew;

    const GAMEFLOW_NEW_STRING_ENTRY *entry = gf->game_strings;
    while (entry != NULL && entry->key != NULL) {
        GF_N_LoadGameString(entry->key, entry->value);
        entry++;
    }

    if (level_num >= 0) {
        assert(level_num < gf->level_count);
        const GAMEFLOW_NEW_LEVEL *const level = &gf->levels[level_num];
        entry = level->game_strings;
        while (entry != NULL && entry->key != NULL) {
            GF_N_LoadGameString(entry->key, entry->value);
            entry++;
        }
    }
}

void GF_N_LoadStrings(const int32_t level_num)
{
    GF_N_LoadGameStrings(level_num);
}
