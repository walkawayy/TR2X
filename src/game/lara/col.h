#pragma once

#include "global/types.h"

// TODO: make static
void __cdecl Lara_CollideStop(ITEM_INFO *item, const COLL_INFO *coll);
bool __cdecl Lara_Fallen(ITEM_INFO *item, const COLL_INFO *coll);
bool __cdecl Lara_TestWaterClimbOut(ITEM_INFO *item, const COLL_INFO *coll);
bool __cdecl Lara_TestWaterStepOut(ITEM_INFO *item, const COLL_INFO *coll);
void __cdecl Lara_SurfaceCollision(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_Col_Walk(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Run(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Stop(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_ForwardJump(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_FastBack(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_TurnRight(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_TurnLeft(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Death(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_FastFall(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Hang(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Reach(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Splat(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Land(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Compress(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Back(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_StepRight(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_StepLeft(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Slide(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_BackJump(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_RightJump(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_LeftJump(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_UpJump(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Fallback(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_HangLeft(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_HangRight(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_SlideBack(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Null(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Roll(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Roll2(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_SwanDive(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_FastDive(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Wade(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Default(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Jumper(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_ClimbLeft(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_ClimbRight(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_ClimbStance(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Climbing(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_ClimbDown(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_SurfSwim(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_SurfBack(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_SurfLeft(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_SurfRight(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_SurfTread(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_Swim(ITEM_INFO *item, COLL_INFO *coll);
void __cdecl Lara_Col_UWDeath(ITEM_INFO *item, COLL_INFO *coll);
