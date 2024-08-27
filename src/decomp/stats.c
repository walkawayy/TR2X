#include "decomp/stats.h"

#include "game/input.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"

int32_t __cdecl GameStats(const int32_t level_num)
{
    START_INFO *const start = &g_SaveGame.start[level_num];

    start->statistics.timer = g_SaveGame.statistics.timer;
    start->statistics.shots = g_SaveGame.statistics.shots;
    start->statistics.hits = g_SaveGame.statistics.hits;
    start->statistics.distance = g_SaveGame.statistics.distance;
    start->statistics.kills = g_SaveGame.statistics.kills;
    start->statistics.secrets = g_SaveGame.statistics.secrets;
    start->statistics.medipacks = g_SaveGame.statistics.medipacks;

    Text_Init();

    while (g_Input & IN_SELECT) {
        Input_Update();
    }

    while (true) {
        S_InitialisePolyList(0);
        S_CopyBufferToScreen();

        Input_Update();
        if (g_IsResetFlag) {
            g_Input = IN_SELECT;
        }
        g_InputDB = GetDebouncedInput(g_Input);

        ShowEndStatsText();
        Text_Draw();
        S_OutputPolyList();
        S_DumpScreen();

        if (g_Input & IN_SELECT) {
            break;
        }
    }

    g_SaveGame.bonus_flag = 1;
    for (int32_t level = LV_FIRST; level <= g_GameFlow.num_levels; level++) {
        ModifyStartInfo(level);
    }
    g_SaveGame.current_level = LV_FIRST;

    S_DontDisplayPicture();
    return 0;
}
