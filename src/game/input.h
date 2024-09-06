#pragma once

#include "global/types.h"

typedef enum {
    // clang-format off
    INPUT_ROLE_UP          = 0,
    INPUT_ROLE_FORWARD     = INPUT_ROLE_UP,
    INPUT_ROLE_DOWN        = 1,
    INPUT_ROLE_BACK        = INPUT_ROLE_DOWN,
    INPUT_ROLE_LEFT        = 2,
    INPUT_ROLE_RIGHT       = 3,
    INPUT_ROLE_STEP_LEFT   = 4,
    INPUT_ROLE_STEP_RIGHT  = 5,
    INPUT_ROLE_SLOW        = 6,
    INPUT_ROLE_JUMP        = 7,
    INPUT_ROLE_ACTION      = 8,
    INPUT_ROLE_DRAW_WEAPON = 9,
    INPUT_ROLE_FLARE       = 10,
    INPUT_ROLE_LOOK        = 11,
    INPUT_ROLE_ROLL        = 12,
    INPUT_ROLE_OPTION      = 13,
    INPUT_ROLE_NUMBER_OF   = 14,
    // clang-format on
} INPUT_ROLE;

typedef struct {
    uint16_t key[INPUT_ROLE_NUMBER_OF];
} INPUT_LAYOUT;

extern INPUT_LAYOUT g_Layout[2];
extern bool g_ConflictLayout[INPUT_ROLE_NUMBER_OF];

bool Input_Update(void);
int32_t __cdecl Input_GetDebounced(int32_t input);
