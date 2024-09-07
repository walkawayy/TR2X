#include "game/input.h"

#include "game/console.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "specific/s_input.h"

bool g_ConflictLayout[INPUT_ROLE_NUMBER_OF] = { false };

bool Input_Update(void)
{
    bool result = S_Input_Update();
    g_InputDB = Input_GetDebounced(g_Input);
    if (Console_IsOpened()) {
        if (g_InputDB & IN_DESELECT) {
            Console_Close();
        } else if (g_InputDB & IN_SELECT) {
            Console_Confirm();
        }

        g_Input = 0;
        g_InputDB = 0;
    }
    return result;
}

int32_t __cdecl Input_GetDebounced(const int32_t input)
{
    const int32_t result = input & ~g_OldInputDB;
    g_OldInputDB = input;
    return result;
}

uint16_t Input_GetAssignedKey(const int32_t layout, const INPUT_ROLE role)
{
    return g_Layout[layout].key[role];
}

void Input_AssignKey(
    const int32_t layout, const INPUT_ROLE role, const uint16_t key)
{
    g_Layout[layout].key[role] = key;
}

const char *Input_GetLayoutName(const int32_t layout)
{
    // clang-format off
    switch (layout) {
    case 0: return g_GF_PCStrings[GF_S_PC_DEFAULT_KEYS];
    case 1: return g_GF_PCStrings[GF_S_PC_USER_KEYS];
    default: return "";
    }
    // clang-format on
}

const char *Input_GetRoleName(const INPUT_ROLE role)
{
    // clang-format off
    switch (role) {
    case INPUT_ROLE_UP:          return g_GF_GameStrings[GF_S_GAME_KEYMAP_RUN];
    case INPUT_ROLE_DOWN:        return g_GF_GameStrings[GF_S_GAME_KEYMAP_BACK];
    case INPUT_ROLE_LEFT:        return g_GF_GameStrings[GF_S_GAME_KEYMAP_LEFT];
    case INPUT_ROLE_RIGHT:       return g_GF_GameStrings[GF_S_GAME_KEYMAP_RIGHT];
    case INPUT_ROLE_STEP_LEFT:   return g_GF_GameStrings[GF_S_GAME_KEYMAP_STEP_LEFT];
    case INPUT_ROLE_STEP_RIGHT:  return g_GF_GameStrings[GF_S_GAME_KEYMAP_STEP_RIGHT];
    case INPUT_ROLE_SLOW:        return g_GF_GameStrings[GF_S_GAME_KEYMAP_WALK];
    case INPUT_ROLE_JUMP:        return g_GF_GameStrings[GF_S_GAME_KEYMAP_JUMP];
    case INPUT_ROLE_ACTION:      return g_GF_GameStrings[GF_S_GAME_KEYMAP_ACTION];
    case INPUT_ROLE_DRAW_WEAPON: return g_GF_GameStrings[GF_S_GAME_KEYMAP_DRAW_WEAPON];
    case INPUT_ROLE_FLARE:       return g_GF_GameStrings[GF_S_GAME_KEYMAP_FLARE];
    case INPUT_ROLE_LOOK:        return g_GF_GameStrings[GF_S_GAME_KEYMAP_LOOK];
    case INPUT_ROLE_ROLL:        return g_GF_GameStrings[GF_S_GAME_KEYMAP_ROLL];
    case INPUT_ROLE_OPTION:      return g_GF_GameStrings[GF_S_GAME_KEYMAP_INVENTORY];
    default:                     return "";
    }
    // clang-format on
}

const char *Input_GetKeyName(const uint16_t key)
{
    return g_KeyNames[key];
}
