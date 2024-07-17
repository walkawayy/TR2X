#include "game/collide.h"

#include "game/math.h"
#include "game/room.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Collide_GetCollisionInfo(
    COLL_INFO *const coll, const int32_t x_pos, const int32_t y_pos,
    const int32_t z_pos, int16_t room_num, const int32_t obj_height)
{
    coll->coll_type = 0;
    coll->shift.x = 0;
    coll->shift.y = 0;
    coll->shift.z = 0;
    coll->quadrant = Math_GetDirection(coll->facing);

    const FLOOR_INFO *floor;
    int32_t height;
    int32_t ceiling;
    int32_t x;
    int32_t z;
    const int32_t y = y_pos - obj_height;
    const int32_t y_top = y - 160;

    x = x_pos;
    z = z_pos;
    floor = Room_GetFloor(x, y_top, z, &room_num);
    height = Room_GetHeight(floor, x, y_top, z);
    if (height != NO_HEIGHT) {
        height -= y_pos;
    }
    ceiling = Room_GetCeiling(floor, x, y_top, z);
    if (ceiling != NO_HEIGHT) {
        ceiling -= y;
    }

    coll->side_mid.floor = height;
    coll->side_mid.ceiling = ceiling;
    coll->side_mid.type = g_HeightType;
    coll->trigger = g_TriggerIndex;

    const int16_t tilt = Room_GetTiltType(floor, x, g_LaraItem->pos.y, z);
    coll->z_tilt = tilt >> 8;
    coll->x_tilt = (int8_t)tilt;

    int32_t x_left;
    int32_t z_left;
    int32_t x_right;
    int32_t z_right;
    int32_t x_front;
    int32_t z_front;
    switch (coll->quadrant) {
    case DIR_NORTH:
        x_front = (coll->radius * Math_Sin(coll->facing)) >> W2V_SHIFT;
        z_front = coll->radius;
        x_left = -coll->radius;
        z_left = coll->radius;
        x_right = coll->radius;
        z_right = coll->radius;
        break;

    case DIR_EAST:
        x_front = coll->radius;
        z_front = (coll->radius * Math_Cos(coll->facing)) >> W2V_SHIFT;
        x_left = coll->radius;
        z_left = coll->radius;
        x_right = coll->radius;
        z_right = -coll->radius;
        break;

    case DIR_SOUTH:
        x_front = (coll->radius * Math_Sin(coll->facing)) >> W2V_SHIFT;
        z_front = -coll->radius;
        x_left = coll->radius;
        z_left = -coll->radius;
        x_right = -coll->radius;
        z_right = -coll->radius;
        break;

    case DIR_WEST:
        x_front = -coll->radius;
        z_front = (coll->radius * Math_Cos(coll->facing)) >> W2V_SHIFT;
        x_left = -coll->radius;
        z_left = -coll->radius;
        x_right = -coll->radius;
        z_right = coll->radius;
        break;

    default:
        x_front = 0;
        z_front = 0;
        x_left = 0;
        z_left = 0;
        x_right = 0;
        z_right = 0;
        break;
    }

    x = x_pos + x_front;
    z = z_pos + z_front;
    floor = Room_GetFloor(x, y_top, z, &room_num);
    height = Room_GetHeight(floor, x, y_top, z);
    if (height != NO_HEIGHT) {
        height -= y_pos;
    }
    ceiling = Room_GetCeiling(floor, x, y_top, z);
    if (ceiling != NO_HEIGHT) {
        ceiling -= y;
    }

    coll->side_front.floor = height;
    coll->side_front.ceiling = ceiling;
    coll->side_front.type = g_HeightType;
    if (coll->slopes_are_walls && coll->side_front.type == HT_BIG_SLOPE
        && coll->side_front.floor < 0) {
        coll->side_front.floor = -32767;
    } else if (
        coll->slopes_are_pits && coll->side_front.type == HT_BIG_SLOPE
        && coll->side_front.floor > 0) {
        coll->side_front.floor = 512;
    } else if (
        coll->lava_is_pit && coll->side_front.floor > 0 && g_TriggerIndex
        && (g_TriggerIndex[0] & DATA_TYPE) == FT_LAVA) {
        coll->side_front.floor = 512;
    }

    x = x_pos + x_left;
    z = z_pos + z_left;
    floor = Room_GetFloor(x, y_top, z, &room_num);
    height = Room_GetHeight(floor, x, y_top, z);
    if (height != NO_HEIGHT) {
        height -= y_pos;
    }
    ceiling = Room_GetCeiling(floor, x, y_top, z);
    if (ceiling != NO_HEIGHT) {
        ceiling -= y;
    }

    coll->side_left.floor = height;
    coll->side_left.ceiling = ceiling;
    coll->side_left.type = g_HeightType;
    if (coll->slopes_are_walls && coll->side_left.type == HT_BIG_SLOPE
        && coll->side_left.floor < 0) {
        coll->side_left.floor = -32767;
    } else if (
        coll->slopes_are_pits && coll->side_left.type == HT_BIG_SLOPE
        && coll->side_left.floor > 0) {
        coll->side_left.floor = 512;
    } else if (
        coll->lava_is_pit && coll->side_left.floor > 0 && g_TriggerIndex
        && (g_TriggerIndex[0] & DATA_TYPE) == FT_LAVA) {
        coll->side_left.floor = 512;
    }

    x = x_pos + x_right;
    z = z_pos + z_right;
    floor = Room_GetFloor(x, y_top, z, &room_num);
    height = Room_GetHeight(floor, x, y_top, z);
    if (height != NO_HEIGHT) {
        height -= y_pos;
    }
    ceiling = Room_GetCeiling(floor, x, y_top, z);
    if (ceiling != NO_HEIGHT) {
        ceiling -= y;
    }

    coll->side_right.floor = height;
    coll->side_right.ceiling = ceiling;
    coll->side_right.type = g_HeightType;
    if (coll->slopes_are_walls && coll->side_right.type == HT_BIG_SLOPE
        && coll->side_right.floor < 0) {
        coll->side_right.floor = -32767;
    } else if (
        coll->slopes_are_pits && coll->side_right.type == HT_BIG_SLOPE
        && coll->side_right.floor > 0) {
        coll->side_right.floor = 512;
    } else if (
        coll->lava_is_pit && coll->side_right.floor > 0 && g_TriggerIndex
        && (g_TriggerIndex[0] & DATA_TYPE) == FT_LAVA) {
        coll->side_right.floor = 512;
    }

    if (Collide_CollideStaticObjects(
            coll, x_pos, y_pos, z_pos, room_num, obj_height)) {
        const XYZ_32 test_pos = {
            .x = x_pos + coll->shift.x,
            .y = y_pos,
            .z = z_pos + coll->shift.z,
        };
        floor = Room_GetFloor(test_pos.x, test_pos.y, test_pos.z, &room_num);
        if (Room_GetHeight(floor, test_pos.x, test_pos.y, test_pos.z)
                < test_pos.y - WALL_L / 2
            || Room_GetCeiling(floor, test_pos.x, test_pos.y, test_pos.z) > y) {
            coll->shift.x = -coll->shift.x;
            coll->shift.z = -coll->shift.z;
        }
    }

    if (coll->side_mid.floor == NO_HEIGHT) {
        coll->shift.x = coll->old.x - x_pos;
        coll->shift.y = coll->old.y - y_pos;
        coll->shift.z = coll->old.z - z_pos;
        coll->coll_type = COLL_FRONT;
        return;
    }

    if (coll->side_mid.floor - coll->side_mid.ceiling <= 0) {
        coll->shift.x = coll->old.x - x_pos;
        coll->shift.y = coll->old.y - y_pos;
        coll->shift.z = coll->old.z - z_pos;
        coll->coll_type = COLL_CLAMP;
        return;
    }

    if (coll->side_mid.ceiling >= 0) {
        coll->shift.y = coll->side_mid.ceiling;
        coll->coll_type = COLL_TOP;
    }

    if (coll->side_front.floor > coll->bad_pos
        || coll->side_front.floor < coll->bad_neg
        || coll->side_front.ceiling > coll->bad_ceiling) {
        switch (coll->quadrant) {
        case DIR_NORTH:
        case DIR_SOUTH:
            coll->shift.x = coll->old.x - x_pos;
            coll->shift.z = Room_FindGridShift(z_pos + z_front, z_pos);
            break;

        case DIR_EAST:
        case DIR_WEST:
            coll->shift.x = Room_FindGridShift(x_pos + x_front, x_pos);
            coll->shift.z = coll->old.z - z_pos;
            break;
        }

        coll->coll_type = COLL_FRONT;
        return;
    }

    if (coll->side_front.ceiling >= coll->bad_ceiling) {
        coll->shift.x = coll->old.x - x_pos;
        coll->shift.y = coll->old.y - y_pos;
        coll->shift.z = coll->old.z - z_pos;
        coll->coll_type = COLL_TOP_FRONT;
        return;
    }

    if (coll->side_left.floor > coll->bad_pos
        || coll->side_left.floor < coll->bad_neg) {
        switch (coll->quadrant) {
        case DIR_NORTH:
        case DIR_SOUTH:
            coll->shift.x = Room_FindGridShift(x_pos + x_left, x_pos + x_front);
            break;

        case DIR_EAST:
        case DIR_WEST:
            coll->shift.z = Room_FindGridShift(z_pos + z_left, z_pos + z_front);
            break;
        }

        coll->coll_type = COLL_LEFT;
        return;
    }

    if (coll->side_right.floor > coll->bad_pos
        || coll->side_right.floor < coll->bad_neg) {
        switch (coll->quadrant) {
        case DIR_NORTH:
        case DIR_SOUTH:
            coll->shift.x =
                Room_FindGridShift(x_pos + x_right, x_pos + x_front);
            break;

        case DIR_EAST:
        case DIR_WEST:
            coll->shift.z =
                Room_FindGridShift(z_pos + z_right, z_pos + z_front);
            break;
        }

        coll->coll_type = COLL_RIGHT;
        return;
    }
}
