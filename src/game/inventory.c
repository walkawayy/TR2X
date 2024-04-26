#include "game/inventory.h"

#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"

void __cdecl Inv_Construct(void)
{
    S_SetupAboveWater(0);

    if (g_InventoryMode != INV_TITLE_MODE) {
        TempVideoAdjust(g_HiRes, 1.0);
    }

    g_PhdWinRight = g_PhdWinMaxX;
    g_PhdWinLeft = 0;
    g_PhdWinTop = 0;
    g_PhdWinBottom = g_PhdWinMaxY;

    g_IsInventoryActive = 1;
    g_InventoryChosen = 0;

    for (int i = 0; i < 8; i++) {
        g_InventoryExtraData[i] = 0;
    }

    g_InvOptionObjectsCount = 3; // TODO: don't hardcode me
    if (g_InventoryMode == INV_TITLE_MODE && g_GameFlow.gym_enabled) {
        g_InvOptionObjectsCount++;
    }

    for (int32_t i = 0; i < g_InvMainObjectsCount; i++) {
        INVENTORY_ITEM *const inv_item = &g_Inv_MainList[i];
        inv_item->meshes_drawn = inv_item->meshes_sel;
        inv_item->current_frame = 0;
        inv_item->goal_frame = 0;
        inv_item->anim_count = 0;
        inv_item->y_rot = 0;
    }

    for (int32_t i = 0; i < g_InvOptionObjectsCount; i++) {
        INVENTORY_ITEM *const inv_item = &g_Inv_OptionList[i];
        inv_item->current_frame = 0;
        inv_item->goal_frame = 0;
        inv_item->anim_count = 0;
        inv_item->y_rot = 0;
    }

    g_InvMainCurrent = 0;

    if (g_GymInvOpenEnabled && g_InventoryMode == INV_TITLE_MODE
        && !g_GameFlow.load_save_disabled && g_GameFlow.gym_enabled) {
        g_InvOptionCurrent = 3; // TODO: don't hardcode me
        g_SoundOptionLine = 0;
    } else {
        g_InvOptionCurrent = 0;
        g_GymInvOpenEnabled = 0;
        g_SoundOptionLine = 0;
    }
}
