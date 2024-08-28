#include "game/option/option.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/vars.h"

void Option_Sound_Shutdown(INVENTORY_ITEM *const item)
{
    for (int32_t i = 0; i < 4; i++) {
        Text_Remove(g_SoundText[i]);
        g_SoundText[i] = NULL;
    }
}
