#include "game/option/option.h"

void __cdecl Option_ShutdownInventory(INVENTORY_ITEM *const item)
{
    switch (item->obj_num) {
    case O_PASSPORT_OPTION:
        Option_Passport_Shutdown(item);
        break;

    case O_DETAIL_OPTION:
        Option_Detail_Shutdown(item);
        break;

    case O_SOUND_OPTION:
        Option_Sound_Shutdown(item);
        break;

    case O_CONTROL_OPTION:
        Option_Controls_Shutdown(item);
        break;

    case O_COMPASS_OPTION:
        Option_Compass_Shutdown(item);
        break;
    }
}
