#include "game/lara/lara_cheat.h"

#include "game/inventory.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Lara_Cheat_GetStuff(void)
{
    Inv_AddItem(O_PISTOL_ITEM);
    Inv_AddItem(O_MAGNUM_ITEM);
    Inv_AddItem(O_UZI_ITEM);
    Inv_AddItem(O_SHOTGUN_ITEM);
    Inv_AddItem(O_HARPOON_ITEM);
    Inv_AddItem(O_M16_ITEM);
    Inv_AddItem(O_GRENADE_ITEM);

    Inv_AddItemNTimes(O_FLARES_ITEM, 10);
    Inv_AddItemNTimes(O_SMALL_MEDIPACK_ITEM, 10);
    Inv_AddItemNTimes(O_LARGE_MEDIPACK_ITEM, 10);

    g_Lara.magnum_ammo.ammo = 1000;
    g_Lara.uzi_ammo.ammo = 2000;
    g_Lara.shotgun_ammo.ammo = 300;
    g_Lara.harpoon_ammo.ammo = 300;
    g_Lara.m16_ammo.ammo = 300;
    g_Lara.grenade_ammo.ammo = 300;
}
