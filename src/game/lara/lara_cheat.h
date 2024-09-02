#pragma once

#include <stdbool.h>
#include <stdint.h>

void __cdecl Lara_Cheat_EndLevel(void);
bool __cdecl Lara_Cheat_EnterFlyMode(void);
bool __cdecl Lara_Cheat_ExitFlyMode(void);
void __cdecl Lara_Cheat_GetStuff(void);
bool Lara_Cheat_OpenNearestDoor(void);
bool Lara_Cheat_Teleport(int32_t x, int32_t y, int32_t z);
bool Lara_Cheat_KillEnemy(int16_t item_num);
