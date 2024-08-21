#pragma once

#include "global/types.h"

void __cdecl Gun_TargetInfo(const WEAPON_INFO *winfo);
void __cdecl Gun_GetNewTarget(const WEAPON_INFO *winfo);
int32_t __cdecl Gun_FireWeapon(
    LARA_GUN_TYPE weapon_type, ITEM_INFO *target, const ITEM_INFO *src,
    const PHD_ANGLE *angles);
