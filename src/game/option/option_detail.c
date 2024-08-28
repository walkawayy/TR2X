#include "game/option/option.h"
#include "game/text.h"
#include "global/funcs.h"
#include "global/vars.h"

void Option_Detail_Shutdown(INVENTORY_ITEM *const item)
{
    for (int32_t i = 0; i < 5; i++) {
        Text_Remove(g_DetailText[i]);
        g_DetailText[i] = NULL;
    }
}
