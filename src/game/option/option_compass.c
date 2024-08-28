#include "game/option/option.h"
#include "global/funcs.h"
#include "global/vars.h"

void Option_Compass_Shutdown(INVENTORY_ITEM *const item)
{
    Remove_Requester(&g_StatsRequester);
}
