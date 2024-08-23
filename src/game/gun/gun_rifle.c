#include "game/gun/gun_rifle.h"

#include "game/gun/gun.h"
#include "game/gun/gun_misc.h"
#include "global/vars.h"

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
