#include "game/console/cmd/exit_game.h"

#include "global/vars.h"

static COMMAND_RESULT M_Entrypoint(const char *args);

static COMMAND_RESULT M_Entrypoint(const char *const args)
{
    g_GF_OverrideDir = GFD_EXIT_GAME;
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_Console_Cmd_ExitGame = {
    .prefix = "exit",
    .proc = M_Entrypoint,
};
