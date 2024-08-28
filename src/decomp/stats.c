#include "decomp/stats.h"

#include "game/input.h"
#include "game/music.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"

#include <stdio.h>

int32_t __cdecl LevelStats(const int32_t level_num)
{
    START_INFO *const start = &g_SaveGame.start[level_num];
    start->statistics.timer = g_SaveGame.statistics.timer;
    start->statistics.shots = g_SaveGame.statistics.shots;
    start->statistics.hits = g_SaveGame.statistics.hits;
    start->statistics.distance = g_SaveGame.statistics.distance;
    start->statistics.kills = g_SaveGame.statistics.kills;
    start->statistics.secrets = g_SaveGame.statistics.secrets;
    start->statistics.medipacks = g_SaveGame.statistics.medipacks;

    const int32_t sec = g_SaveGame.statistics.timer / FRAMES_PER_SECOND;
    char buffer[100];
    sprintf(buffer, "%02d:%02d:%02d", sec / 3600, (sec / 60) % 60, sec % 60);

    Music_Play(g_GameFlow.level_complete_track, false);

    TempVideoAdjust(g_HiRes, 1.0);
    FadeToPal(30, g_GamePalette8);
    Text_Init();
    S_CopyScreenToBuffer();

    while (g_Input & IN_SELECT) {
        Input_Update();
    }

    while (true) {
        S_InitialisePolyList(0);
        S_CopyBufferToScreen();

        Input_Update();

        if (g_GF_OverrideDir != (GAME_FLOW_DIR)-1) {
            break;
        }

        ShowStatsText(buffer, 0);
        Text_Draw();
        S_OutputPolyList();
        S_DumpScreen();

        if (g_InputDB & IN_SELECT) {
            break;
        }
    }

    CreateStartInfo(level_num + 1);
    g_SaveGame.current_level = level_num + 1;
    start->available = 0;
    S_FadeToBlack();
    TempVideoRemove();
    return 0;
}

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

        if (g_GF_OverrideDir != (GAME_FLOW_DIR)-1) {
            break;
        }

        ShowEndStatsText();
        Text_Draw();
        S_OutputPolyList();
        S_DumpScreen();

        if (g_InputDB & IN_SELECT) {
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
