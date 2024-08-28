#include "game/option/option.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/vars.h"

void Option_Controls_Shutdown(INVENTORY_ITEM *const item)
{
    for (int32_t i = 0; i < 14; i++) {
        Text_Remove(g_ControlsTextA[i]);
        g_ControlsTextA[i] = NULL;

        Text_Remove(g_ControlsTextB[i]);
        g_ControlsTextB[i] = NULL;
    }

    for (int32_t i = 0; i < 2; i++) {
        Text_Remove(g_ControlsText[i]);
        g_ControlsText[i] = NULL;
    }
}
