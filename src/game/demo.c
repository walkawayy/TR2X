#include "game/demo.h"

#include "global/funcs.h"
#include "global/vars.h"

static int32_t m_DemoLevel = 0;

int32_t __cdecl Demo_Control(int32_t level_num)
{
    if (level_num < 0 && !g_GameFlow.num_demos) {
        return GFD_EXIT_TO_TITLE;
    }

    if (level_num < 0) {
        if (m_DemoLevel >= g_GameFlow.num_demos) {
            m_DemoLevel = 0;
        }
        level_num = g_GF_ValidDemos[m_DemoLevel];
        m_DemoLevel++;
    } else {
        m_DemoLevel = level_num;
    }

    return GF_DoLevelSequence(level_num, IL_DEMO);
}
