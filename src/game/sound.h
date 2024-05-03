#pragma once

#include "global/types.h"

void __cdecl Sound_SetMasterVolume(int32_t volume);
void __cdecl Sound_UpdateEffects(void);
void __cdecl Sound_Effect(int32_t sample_id, const XYZ_32 *pos, uint32_t flags);
