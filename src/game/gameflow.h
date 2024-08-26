#pragma once

#include "global/types.h"

#include <stdint.h>

int32_t __cdecl GF_LoadScriptFile(const char *fname);
int32_t __cdecl GF_DoFrontendSequence(void);
int32_t __cdecl GF_DoLevelSequence(int32_t level, GF_LEVEL_TYPE type);
