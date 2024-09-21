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
    ITEM_INFO *item, XYZ_32 *vec, FRAME_INFO *frame1, FRAME_INFO *frame2,
    int32_t frac, int32_t rate);

void __cdecl Lara_BaddieCollision(ITEM_INFO *lara_item, COLL_INFO *coll);
void __cdecl Lara_TakeHit(ITEM_INFO *lara_item, const COLL_INFO *coll);
void __cdecl Lara_Push(
    const ITEM_INFO *item, ITEM_INFO *lara_item, COLL_INFO *coll, bool spaz_on,
    bool big_push);
int32_t __cdecl Lara_MovePosition(
    XYZ_32 *vec, ITEM_INFO *item, ITEM_INFO *lara_item);
int32_t __cdecl Lara_IsNearItem(const XYZ_32 *pos, int32_t distance);

int32_t __cdecl Lara_TestClimb(
    int32_t x, int32_t y, int32_t z, int32_t x_front, int32_t z_front,
    int32_t item_height, int16_t item_room, int32_t *shift);

int32_t __cdecl Lara_TestClimbPos(
    const ITEM_INFO *item, int32_t front, int32_t right, int32_t origin,
    int32_t height, int32_t *shift);

void __cdecl Lara_DoClimbLeftRight(
    ITEM_INFO *item, const COLL_INFO *coll, int32_t result, int32_t shift);

int32_t __cdecl Lara_TestClimbUpPos(
    const ITEM_INFO *item, int32_t front, int32_t right, int32_t *shift,
    int32_t *ledge);

int32_t __cdecl Lara_GetWaterDepth(
    int32_t x, int32_t y, int32_t z, int16_t room_num);

void __cdecl Lara_TestWaterDepth(ITEM_INFO *item, const COLL_INFO *coll);

void __cdecl Lara_SwimCollision(ITEM_INFO *item, COLL_INFO *coll);

void __cdecl Lara_WaterCurrent(COLL_INFO *coll);

void __cdecl Lara_CatchFire(void);

void __cdecl Lara_TouchLava(ITEM_INFO *item);
