#include "game/gameflow.h"

#include "global/funcs.h"
#include "global/vars.h"

int32_t __cdecl GF_LoadScriptFile(const char *const fname)
{
    g_GF_SunsetEnabled = false;

    if (!S_LoadGameFlow(fname)) {
        return false;
    }

    g_GameFlow.level_complete_track = MX_END_OF_LEVEL;

    g_Inv_Item_Stopwatch.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_STATISTICS];
    g_Inv_Item_Pistols.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PISTOLS];
    g_Inv_Item_Flare.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_FLARE];
    g_Inv_Item_Shotgun.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_SHOTGUN];
    g_Inv_Item_Magnums.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_MAGNUMS];
    g_Inv_Item_Uzis.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_UZIS];
    g_Inv_Item_Harpoon.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_HARPOON];
    g_Inv_Item_M16.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_M16];
    g_Inv_Item_Grenade.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_GRENADE];
    g_Inv_Item_PistolAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_PISTOL_AMMO];
    g_Inv_Item_ShotgunAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_SHOTGUN_AMMO];
    g_Inv_Item_MagnumAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_MAGNUM_AMMO];
    g_Inv_Item_UziAmmo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_UZI_AMMO];
    g_Inv_Item_HarpoonAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_HARPOON_AMMO];
    g_Inv_Item_M16Ammo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_M16_AMMO];
    g_Inv_Item_GrenadeAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_GRENADE_AMMO];
    g_Inv_Item_SmallMedi.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_SMALL_MEDIPACK];
    g_Inv_Item_LargeMedi.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_LARGE_MEDIPACK];
    g_Inv_Item_Pickup1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PICKUP];
    g_Inv_Item_Pickup2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PICKUP];
    g_Inv_Item_Puzzle1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle3.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle4.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Key1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key3.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key4.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];

    g_Inv_Item_Passport.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_GAME];
    g_Inv_Item_Graphics.string = g_GF_PCStrings[GF_S_PC_DETAIL_LEVELS];
    ;
    g_Inv_Item_Sound.string = g_GF_PCStrings[GF_S_PC_SOUND];
    g_Inv_Item_Controls.string = g_GF_PCStrings[GF_S_PC_CONTROLS];
    g_Inv_Item_Photo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_LARA_HOME];

    SetRequesterHeading(
        &g_LoadGameRequester, g_GF_GameStrings[GF_S_GAME_PASSPORT_SELECT_LEVEL],
        0, 0, 0);
    SetRequesterHeading(
        &g_SaveGameRequester, g_GF_GameStrings[GF_S_GAME_PASSPORT_SELECT_LEVEL],
        0, 0, 0);

    return true;
}

int32_t __cdecl GF_DoFrontendSequence(void)
{
    const GAME_FLOW_DIR option =
        GF_InterpretSequence(g_GF_FrontendSequence, GFL_NORMAL, 1);
    return option == GFD_EXIT_GAME;
}
