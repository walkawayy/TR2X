#include "game/gun/gun_rifle.h"

#include "config.h"
#include "game/gun/gun.h"
#include "game/gun/gun_misc.h"
#include "game/items.h"
#include "game/lara/lara_misc.h"
#include "game/math.h"
#include "game/random.h"
#include "game/sound.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <libtrx/utils.h>

void __cdecl Gun_Rifle_DrawMeshes(const LARA_GUN_TYPE weapon_type)
{
    Gun_SetLaraHandRMesh(weapon_type);
    g_Lara.back_gun = O_LARA;
}

void __cdecl Gun_Rifle_UndrawMeshes(const LARA_GUN_TYPE weapon_type)
{
    Gun_SetLaraHandRMesh(LGT_UNARMED);
    g_Lara.back_gun = Gun_GetWeaponAnim(weapon_type);
}

void __cdecl Gun_Rifle_Ready(const LARA_GUN_TYPE weapon_type)
{
    g_Lara.gun_status = LGS_READY;
    g_Lara.target = NULL;

    g_Lara.left_arm.frame_base =
        g_Objects[Gun_GetWeaponAnim(weapon_type)].frame_base;
    g_Lara.left_arm.frame_num = LF_G_AIM_START;
    g_Lara.left_arm.lock = 0;
    g_Lara.left_arm.rot.x = 0;
    g_Lara.left_arm.rot.y = 0;
    g_Lara.left_arm.rot.z = 0;

    g_Lara.right_arm.frame_base =
        g_Objects[Gun_GetWeaponAnim(weapon_type)].frame_base;
    g_Lara.right_arm.frame_num = LF_G_AIM_START;
    g_Lara.right_arm.lock = 0;
    g_Lara.right_arm.rot.x = 0;
    g_Lara.right_arm.rot.y = 0;
    g_Lara.right_arm.rot.z = 0;
}

void __cdecl Gun_Rifle_Control(const LARA_GUN_TYPE weapon_type)
{
    const WEAPON_INFO *const winfo = &g_Weapons[weapon_type];

    if (g_Input & IN_ACTION) {
        Gun_TargetInfo(winfo);
    } else {
        g_Lara.target = NULL;
    }

    if (g_Lara.target == NULL) {
        Gun_GetNewTarget(winfo);
    }

    Gun_AimWeapon(winfo, &g_Lara.left_arm);

    if (g_Lara.left_arm.lock) {
        g_Lara.head_x_rot = 0;
        g_Lara.head_y_rot = 0;
        g_Lara.torso_x_rot = g_Lara.left_arm.rot.x;
        g_Lara.torso_y_rot = g_Lara.left_arm.rot.y;
    }

    Gun_Rifle_Animate(weapon_type);

    if (g_Lara.right_arm.flash_gun
        && (weapon_type == LGT_SHOTGUN || weapon_type == LGT_M16)) {
        const int32_t c = Math_Cos(g_LaraItem->rot.y);
        const int32_t s = Math_Sin(g_LaraItem->rot.y);
        const int32_t x = g_LaraItem->pos.x + (s >> (W2V_SHIFT - 10));
        const int32_t y = g_LaraItem->pos.y - WALL_L / 2;
        const int32_t z = g_LaraItem->pos.z + (c >> (W2V_SHIFT - 10));
        AddDynamicLight(x, y, z, 12, 11);
    }
}

void __cdecl Gun_Rifle_FireShotgun(void)
{
    bool fired = false;

    int16_t angles[2];
    angles[0] = g_Lara.left_arm.rot.y + g_LaraItem->rot.y;
    angles[1] = g_Lara.left_arm.rot.x;

    for (int32_t i = 0; i < SHOTGUN_AMMO_CLIP; i++) {
        int16_t dangles[2];
        dangles[0] = angles[0]
            + SHOTGUN_PELLET_SCATTER * (Random_GetControl() - 0x4000) / 0x10000;
        dangles[1] = angles[1]
            + SHOTGUN_PELLET_SCATTER * (Random_GetControl() - 0x4000) / 0x10000;
        if (Gun_FireWeapon(LGT_SHOTGUN, g_Lara.target, g_LaraItem, dangles)) {
            fired = true;
        }
    }

    if (fired) {
        g_Lara.right_arm.flash_gun = g_Weapons[LGT_SHOTGUN].flash_time;
        Sound_Effect(
            g_Weapons[LGT_SHOTGUN].sample_num, &g_LaraItem->pos, SPM_NORMAL);
    }
}

void __cdecl Gun_Rifle_FireM16(const bool running)
{
    int16_t angles[2];
    angles[0] = g_Lara.left_arm.rot.y + g_LaraItem->rot.y;
    angles[1] = g_Lara.left_arm.rot.x;

    if (g_Config.gameplay.fix_m16_accuracy) {
        if (running) {
            g_Weapons[LGT_M16].shot_accuracy = PHD_DEGREE * 12;
            g_Weapons[LGT_M16].damage = 1;
        } else {
            g_Weapons[LGT_M16].shot_accuracy = PHD_DEGREE * 4;
            g_Weapons[LGT_M16].damage = 3;
        }
    }

    if (Gun_FireWeapon(LGT_M16, g_Lara.target, g_LaraItem, angles)) {
        g_Lara.right_arm.flash_gun = g_Weapons[LGT_M16].flash_time;
    }
}

void __cdecl Gun_Rifle_FireHarpoon(void)
{
    if (g_Lara.harpoon_ammo.ammo <= 0) {
        return;
    }

    const int16_t item_num = Item_Create();
    if (item_num == NO_ITEM) {
        return;
    }

    ITEM_INFO *const item = &g_Items[item_num];
    item->object_num = O_HARPOON_BOLT;
    item->room_num = g_LaraItem->room_num;

    XYZ_32 offset = {
        .x = -2,
        .y = 373,
        .z = 77,
    };

    Lara_GetJointAbsPosition(&offset, LM_HAND_R);
    item->pos.x = offset.x;
    item->pos.y = offset.y;
    item->pos.z = offset.z;
    Item_Initialise(item_num);

    if (g_Lara.target != NULL) {
        GAME_VECTOR lara_vec;
        Gun_FindTargetPoint(g_Lara.target, &lara_vec);
        const int32_t dx = lara_vec.pos.x - item->pos.x;
        const int32_t dz = lara_vec.pos.z - item->pos.z;
        const int32_t dy = lara_vec.pos.y - item->pos.y;
        const int32_t dxz = Math_Sqrt(SQUARE(dx) + SQUARE(dz));
        item->rot.y = Math_Atan(dz, dx);
        item->rot.x = -Math_Atan(dxz, dy);
        item->rot.z = 0;
    } else {
        item->rot.x = g_Lara.left_arm.rot.x + g_LaraItem->rot.x;
        item->rot.y = g_Lara.left_arm.rot.y + g_LaraItem->rot.y;
        item->rot.z = 0;
    }

    item->fall_speed =
        (-HARPOON_BOLT_SPEED * Math_Sin(item->rot.x)) >> W2V_SHIFT;
    item->speed = (HARPOON_BOLT_SPEED * Math_Cos(item->rot.x)) >> W2V_SHIFT;
    Item_AddActive(item_num);

    if (!g_SaveGame.bonus_flag) {
        g_Lara.harpoon_ammo.ammo--;
    }
    g_SaveGame.statistics.shots++;
}
