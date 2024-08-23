#include "game/gun/gun_rifle.h"

#include "game/gun/gun.h"

void __cdecl Gun_Rifle_DrawMeshes(const LARA_GUN_TYPE weapon_type)
{
    Gun_SetLaraHandRMesh(weapon_type);
    g_Lara.back_gun = O_LARA;
}
