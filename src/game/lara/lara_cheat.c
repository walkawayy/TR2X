#include "game/lara/lara_cheat.h"

#include "game/console.h"
#include "game/inventory.h"
#include "game/lara/lara_control.h"
#include "game/output.h"
#include "game/room.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Lara_Cheat_EndLevel(void)
{
    g_LevelComplete = true;
    Console_Log("Level complete!");
}

bool __cdecl Lara_Cheat_EnterFlyMode(void)
{
    if (g_LaraItem == NULL) {
        return false;
    }

    if (g_Lara.water_status != LWS_UNDERWATER || g_LaraItem->hit_points <= 0) {
        g_LaraItem->pos.y -= STEP_L / 2;
        g_LaraItem->current_anim_state = LS_SWIM;
        g_LaraItem->goal_anim_state = LS_SWIM;
        g_LaraItem->anim_num =
            g_Objects[O_LARA].anim_idx + LA_UNDERWATER_SWIM_FORWARD_DRIFT;
        g_LaraItem->frame_num = g_Anims[g_LaraItem->anim_num].frame_base;
        g_LaraItem->gravity = 0;
        g_LaraItem->rot.x = 30 * PHD_DEGREE;
        g_LaraItem->fall_speed = 30;
        g_Lara.head_x_rot = 0;
        g_Lara.head_y_rot = 0;
        g_Lara.torso_x_rot = 0;
        g_Lara.torso_y_rot = 0;
    }

    if (g_Lara.skidoo != NO_ITEM) {
        ITEM_INFO *const vehicle = &g_Items[g_Lara.skidoo];
        vehicle->anim_num = g_Objects[vehicle->object_num].anim_idx;
        vehicle->frame_num = g_Anims[vehicle->anim_num].frame_base;
        g_Lara.skidoo = NO_ITEM;
    }

    g_Lara.water_status = LWS_CHEAT;
    g_Lara.spaz_effect_count = 0;
    g_Lara.spaz_effect = NULL;
    g_Lara.hit_frame = 0;
    g_Lara.hit_direction = -1;
    g_Lara.air = LARA_MAX_AIR;
    g_Lara.death_timer = 0;
    g_Lara.mesh_effects = 0;
    g_Lara.extra_anim = 0;

    Lara_InitialiseMeshes(g_CurrentLevel);
    g_Camera.type = CAM_CHASE;
    Output_AlterFOV(GAME_FOV * PHD_DEGREE);

    Console_Log("Fly mode enabled");
    return true;
}

bool __cdecl Lara_Cheat_ExitFlyMode(void)
{
    if (g_LaraItem == NULL) {
        return false;
    }

    const ROOM_INFO *const room = &g_Rooms[g_LaraItem->room_num];
    const bool room_submerged = (room->flags & RF_UNDERWATER) != 0;
    const int16_t water_height = Room_GetWaterHeight(
        g_LaraItem->pos.x, g_LaraItem->pos.y, g_LaraItem->pos.z,
        g_LaraItem->room_num);

    if (room_submerged || (water_height != NO_HEIGHT && water_height > 0)) {
        g_Lara.water_status = LWS_UNDERWATER;
    } else {
        g_Lara.water_status = LWS_ABOVE_WATER;
        g_LaraItem->anim_num = g_Objects[O_LARA].anim_idx + LA_STAND_STILL;
        g_LaraItem->frame_num = g_Anims[g_LaraItem->anim_num].frame_base;
        g_LaraItem->rot.x = 0;
        g_LaraItem->rot.z = 0;
        g_Lara.head_x_rot = 0;
        g_Lara.head_y_rot = 0;
        g_Lara.torso_x_rot = 0;
        g_Lara.torso_y_rot = 0;
    }
    g_Lara.gun_status = LGS_ARMLESS;

    Console_Log("Fly mode disabled");
    return true;
}

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
