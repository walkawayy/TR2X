#include "game/console/cmd/save_game.h"

#include "decomp/decomp.h"
#include "game/game_string.h"
#include "game/gameflow/gameflow_new.h"
#include "global/funcs.h"
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
        return CR_BAD_INVOCATION;
    }

    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    if (g_LaraItem == NULL || g_LaraItem->hit_points <= 0) {
        return CR_UNAVAILABLE;
    }

    CreateSaveGameInfo();
    S_SaveGame(&g_SaveGame, sizeof(SAVEGAME_INFO), slot_idx);
    GetSavedGamesList(&g_LoadGameRequester);
    Console_Log(GS(OSD_SAVE_GAME), slot_num);
    return CR_SUCCESS;
}

CONSOLE_COMMAND g_Console_Cmd_SaveGame = {
    .prefix = "save",
    .proc = M_Entrypoint,
};
