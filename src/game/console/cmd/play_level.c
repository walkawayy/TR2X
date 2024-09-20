#include "game/console/cmd/play_level.h"

#include "game/game_string.h"
#include "global/vars.h"

#include <libtrx/strings.h>

#include <stdio.h>

static COMMAND_RESULT M_Entrypoint(const char *args);

static COMMAND_RESULT M_Entrypoint(const char *const args)
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
        Console_Log(GS(OSD_INVALID_LEVEL));
        return CR_FAILURE;
    }

    if (level_to_load != -1) {
        g_GF_OverrideDir = GFD_START_GAME | level_to_load;
        Console_Log(GS(OSD_PLAY_LEVEL), g_GF_LevelNames[level_to_load]);
        return CR_SUCCESS;
    }

    return CR_BAD_INVOCATION;
}

CONSOLE_COMMAND g_Console_Cmd_PlayLevel = {
    .prefix = "play|level",
    .proc = M_Entrypoint,
};
