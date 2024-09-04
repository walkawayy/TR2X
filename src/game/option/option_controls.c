#include "game/option/option.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Option_Controls_FlashConflicts(void)
{
    for (int32_t i = 0; i < 14; i++) {
        Text_Flash(g_ControlsTextA[i], 0, 0);
        for (int32_t j = 0; j < 14; j++) {
            const uint16_t key1 = g_Layout[g_LayoutPage].key[i];
            const uint16_t key2 = g_Layout[g_LayoutPage].key[j];
            if (i != j && key1 == key2) {
                Text_Flash(g_ControlsTextA[i], 1, 20);
            }
        }
    }
}

void __cdecl Option_Controls_DefaultConflict(void)
{
    for (int32_t i = 0; i < 14; i++) {
        g_ConflictLayout[i] = false;
        for (int32_t j = 0; j < 14; j++) {
            uint16_t key1 = g_Layout[0].key[i];
            uint16_t key2 = g_Layout[1].key[j];
            if (key1 == key2) {
                g_ConflictLayout[i] = true;
            }
        }
    }
}

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
