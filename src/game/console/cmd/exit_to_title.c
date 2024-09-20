#include "game/console/cmd/exit_to_title.h"

#include "global/vars.h"

static COMMAND_RESULT M_Entrypoint(const char *args);

static COMMAND_RESULT M_Entrypoint(const char *const args)
{
    g_GF_OverrideDir = GFD_EXIT_TO_TITLE;
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_Console_Cmd_ExitToTitle = {
    .prefix = "title",
    .proc = M_Entrypoint,
};
