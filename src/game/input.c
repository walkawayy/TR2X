#include "game/input.h"

#include "game/console.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "specific/s_input.h"

bool Input_Update(void)
{
    bool result = S_Input_Update();
    if (Console_IsOpened()) {
        g_Input = 0;
    }
    return result;
}
