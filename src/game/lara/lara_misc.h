#pragma once

#include "global/types.h"

void __cdecl Lara_GetCollisionInfo(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_SlideSlope(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_HitCeiling(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_DeflectEdge(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_DeflectEdgeJump(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_SlideEdgeJump(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_TestWall(
    ITEM_INFO *item, int32_t front, int32_t right, int32_t down);

int32_t __cdecl Lara_TestHangOnClimbWall(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_TestClimbStance(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_HangTest(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_TestEdgeCatch(
    ITEM_INFO *item, COLL_INFO *coll, int32_t *edge);

int32_t __cdecl Lara_TestHangJumpUp(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_TestHangJump(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_TestHangSwingIn(ITEM_INFO *item, PHD_ANGLE angle);

int32_t __cdecl Lara_TestVault(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_TestSlide(ITEM_INFO *item, COLL_INFO *coll);

int16_t __cdecl Lara_FloorFront(ITEM_INFO *item, int16_t ang, int32_t dist);

int32_t __cdecl Lara_LandedBad(ITEM_INFO *item, COLL_INFO *coll);

int32_t __cdecl Lara_CheckForLetGo(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_GetJointAbsPosition(XYZ_32 *vec, int32_t joint);

void __cdecl Lara_GetJointAbsPosition_I(
    ITEM_INFO *item, XYZ_32 *vec, int16_t *frame1, int16_t *frame2,
    int32_t frac, int32_t rate);
