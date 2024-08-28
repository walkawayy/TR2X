#include "game/input.h"

#include "game/console.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "specific/s_input.h"

bool Input_Update(void)
{
    bool result = S_Input_Update();
    g_InputDB = GetDebouncedInput(g_Input);
    if (Console_IsOpened()) {
        if (g_InputDB & IN_DESELECT) {
            Console_Close();
        } else if (g_InputDB & IN_SELECT) {
            Console_Confirm();
        }

        g_Input = 0;
        g_InputDB = 0;
    }
    return result;
}
