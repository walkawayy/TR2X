#pragma once

#include <stdbool.h>
#include <stdint.h>

void __cdecl LOT_InitialiseArray(void);
void __cdecl LOT_DisableBaddieAI(int16_t item_num);
bool __cdecl LOT_EnableBaddieAI(int16_t item_num, bool always);
