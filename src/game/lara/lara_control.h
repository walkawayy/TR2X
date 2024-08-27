#pragma once

#include "global/types.h"

void __cdecl Lara_HandleAboveWater(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_HandleSurface(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_HandleUnderwater(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_Control(int16_t item_num);
void __cdecl Lara_ControlExtra(int16_t item_num);

void __cdecl Lara_Animate(ITEM_INFO *item);

void __cdecl Lara_UseItem(GAME_OBJECT_ID object_num);

void __cdecl Lara_InitialiseLoad(int16_t item_num);

void __cdecl Lara_Initialise(GF_LEVEL_TYPE type);

void __cdecl Lara_InitialiseInventory(int32_t level_num);

void __cdecl Lara_InitialiseMeshes(int32_t level_num);

void Lara_SwapSingleMesh(LARA_MESH mesh, GAME_OBJECT_ID);
