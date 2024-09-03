#include "game_string.h"

#include <libtrx/memory.h>
#include <libtrx/utils.h>

#include <assert.h>
#include <stddef.h>
#include <string.h>

static char *m_StringMap[GS_NUMBER_OF] = { 0 };

#undef GS_DEFINE
#define GS_DEFINE(id, str) str,
static const char *m_DefaultStringMap[GS_NUMBER_OF] = {
#include "game/game_string.def"
};

#undef GS_DEFINE
#define GS_DEFINE(id, str)                                                     \
    {                                                                          \
        QUOTE(id),                                                             \
        GS_##id,                                                               \
    },
ENUM_STRING_MAP ENUM_STRING_MAP(GAME_STRING_ID)[] = {
#include "game/game_string.def"
    { NULL, -1 }
};

void GameString_Set(const GAME_STRING_ID id, const char *const value)
{
    assert(id >= 0);
    assert(id < GS_NUMBER_OF);
    Memory_FreePointer(&m_StringMap[id]);
    m_StringMap[id] = Memory_DupStr(value);
}

const char *GameString_Get(const GAME_STRING_ID id)
{
    return m_StringMap[id] != NULL ? (const char *)m_StringMap[id]
                                   : m_DefaultStringMap[id];
}

GAME_STRING_ID GameString_IDFromEnum(const char *const key)
{
    return ENUM_STRING_GET(GAME_STRING_ID, key, GS_INVALID);
}
