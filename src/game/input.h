#pragma once

#include "global/types.h"

typedef struct {
    uint16_t key[14];
} INPUT_LAYOUT;

extern INPUT_LAYOUT g_Layout[2];
extern bool g_ConflictLayout[14];

bool Input_Update(void);
int32_t __cdecl Input_GetDebounced(int32_t input);
