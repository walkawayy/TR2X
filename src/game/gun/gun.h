#pragma once

#include "global/types.h"

void __cdecl Gun_Control(void);
void __cdecl Gun_InitialiseNewWeapon(void);
int32_t __cdecl Gun_GetWeaponAnim(LARA_GUN_TYPE gun_type);

void Gun_SetLaraBackMesh(LARA_GUN_TYPE weapon_type);
void Gun_SetLaraHandLMesh(LARA_GUN_TYPE weapon_type);
void Gun_SetLaraHandRMesh(LARA_GUN_TYPE weapon_type);
void Gun_SetLaraHolsterLMesh(LARA_GUN_TYPE weapon_type);
void Gun_SetLaraHolsterRMesh(LARA_GUN_TYPE weapon_type);
