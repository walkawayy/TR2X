#pragma once

#include <stdint.h>

typedef struct {
    const char *key;
    const char *value;
} GAMEFLOW_NEW_STRING_ENTRY;

typedef struct {
    GAMEFLOW_NEW_STRING_ENTRY *object_strings;
    GAMEFLOW_NEW_STRING_ENTRY *game_strings;
} GAMEFLOW_NEW_LEVEL;

typedef struct {
    int32_t level_count;
    GAMEFLOW_NEW_LEVEL *levels;
    GAMEFLOW_NEW_STRING_ENTRY *object_strings;
    GAMEFLOW_NEW_STRING_ENTRY *game_strings;
} GAMEFLOW_NEW;

extern GAMEFLOW_NEW g_GameflowNew;

void GF_N_LoadStrings(int32_t level_num);
