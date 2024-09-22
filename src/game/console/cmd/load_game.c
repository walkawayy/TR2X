#include "game/console/cmd/load_game.h"

#include "game/game_string.h"
#include "global/vars.h"

#include <libtrx/strings.h>

static COMMAND_RESULT M_Entrypoint(const COMMAND_CONTEXT *ctx);

static COMMAND_RESULT M_Entrypoint(const COMMAND_CONTEXT *const ctx)
{
    int32_t slot_num;
    if (!String_ParseInteger(ctx->args, &slot_num)) {
        return CR_BAD_INVOCATION;
    }

    // convert 1-indexing to 0-indexing
    const int32_t slot_idx = slot_num - 1;

    if (slot_idx < 0 || slot_idx >= MAX_SAVE_SLOTS) {
        Console_Log(GS(OSD_INVALID_SAVE_SLOT), slot_num);
        return CR_FAILURE;
    }

    // TODO: replace this with a proper status check
    if (g_SavedLevels[slot_idx] <= 0) {
        Console_Log(GS(OSD_LOAD_GAME_FAIL_UNAVAILABLE_SLOT), slot_num);
        return CR_FAILURE;
    }

    g_GF_OverrideDir = GFD_START_SAVED_GAME | slot_idx;
    Console_Log(GS(OSD_LOAD_GAME), slot_num);
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_Console_Cmd_LoadGame = {
    .prefix = "load",
    .proc = M_Entrypoint,
};
