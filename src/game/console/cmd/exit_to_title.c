#include "game/console/cmd/exit_to_title.h"

#include "global/vars.h"

static COMMAND_RESULT M_Entrypoint(const COMMAND_CONTEXT *ctx);

static COMMAND_RESULT M_Entrypoint(const COMMAND_CONTEXT *const ctx)
{
    g_GF_OverrideDir = GFD_EXIT_TO_TITLE;
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_Console_Cmd_ExitToTitle = {
    .prefix = "title",
    .proc = M_Entrypoint,
};
