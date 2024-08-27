#pragma once

#include "global/types.h"

#include <stdint.h>

int32_t __cdecl GF_LoadScriptFile(const char *fname);
int32_t __cdecl GF_DoFrontendSequence(void);
int32_t __cdecl GF_DoLevelSequence(int32_t level, GF_LEVEL_TYPE type);
int32_t __cdecl GF_InterpretSequence(
    const int16_t *ptr, GF_LEVEL_TYPE type, int32_t seq_type);
void __cdecl GF_ModifyInventory(int32_t level, int32_t type);
