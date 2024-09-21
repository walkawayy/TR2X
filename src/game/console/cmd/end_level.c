#include "game/console/cmd/end_level.h"

#include "game/lara/cheat.h"

#include <string.h>

static COMMAND_RESULT M_Entrypoint(const char *args);

static COMMAND_RESULT M_Entrypoint(const char *const args)
{
    if (strcmp(args, "") == 0) {
        Lara_Cheat_EndLevel();
        return CR_SUCCESS;
    }
    return CR_FAILURE;
}

CONSOLE_COMMAND g_Console_Cmd_EndLevel = {
    .prefix = "endlevel",
    .proc = M_Entrypoint,
};
