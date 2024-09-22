#include "game/console/cmd/exit_game.h"

#include "global/vars.h"

static COMMAND_RESULT M_Entrypoint(const COMMAND_CONTEXT *ctx);

static COMMAND_RESULT M_Entrypoint(const COMMAND_CONTEXT *const ctx)
{
    g_GF_OverrideDir = GFD_EXIT_GAME;
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_Console_Cmd_ExitGame = {
    .prefix = "exit",
    .proc = M_Entrypoint,
};
