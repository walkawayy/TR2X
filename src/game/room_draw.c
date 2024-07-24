#include "game/room_draw.h"

#include "game/matrix.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <libtrx/utils.h>

void __cdecl Room_GetBounds(void)
{
    while (g_BoundStart != g_BoundEnd) {
        const int16_t room_num = g_BoundRooms[g_BoundStart++ % MAX_BOUND_ROOMS];
        ROOM_INFO *const r = &g_Rooms[room_num];
        r->bound_active &= ~2;
        g_MidSort = (r->bound_active >> 8) + 1;

        if (r->test_left < r->bound_left) {
            r->bound_left = r->test_left;
        }
        if (r->test_top < r->bound_top) {
            r->bound_top = r->test_top;
        }
        if (r->test_right > r->bound_right) {
            r->bound_right = r->test_right;
        }
        if (r->test_bottom > r->bound_bottom) {
            r->bound_bottom = r->test_bottom;
        }

        if (!(r->bound_active & 1)) {
            // TODO: fix crash when drawing too many rooms
            g_DrawRoomsArray[g_DrawRoomsCount++] = room_num;
            r->bound_active |= 1;
            if (r->flags & RF_OUTSIDE) {
                g_Outside = RF_OUTSIDE;
            }
        }

        if (!(r->flags & RF_INSIDE)) {
            if (r->bound_left < g_OutsideLeft) {
                g_OutsideLeft = r->bound_left;
            }
            if (r->bound_right > g_OutsideRight) {
                g_OutsideRight = r->bound_right;
            }
            if (r->bound_top < g_OutsideTop) {
                g_OutsideTop = r->bound_top;
            }
            if (r->bound_bottom > g_OutsideBottom) {
                g_OutsideBottom = r->bound_bottom;
            }
        }

        if (r->doors == NULL) {
            continue;
        }

        Matrix_Push();
        Matrix_TranslateAbs(r->pos.x, r->pos.y, r->pos.z);
        for (int32_t i = 0; i < r->doors->count; i++) {
            const DOOR_INFO *const door = &r->doors->door[i];

            // clang-format off
            const XYZ_32 offset = {
                .x = door->x * (r->pos.x + door->vertex[0].x - g_W2VMatrix._03),
                .y = door->y * (r->pos.y + door->vertex[0].y - g_W2VMatrix._13),
                .z = door->z * (r->pos.z + door->vertex[0].z - g_W2VMatrix._23),
            };
            // clang-format on

            if (offset.x + offset.y + offset.z >= 0) {
                continue;
            }

            Room_SetBounds(&door->x, door->room, r);
        }
        Matrix_Pop();
    }
}

void __cdecl Room_SetBounds(
    const int16_t *obj_ptr, int32_t room_num, const ROOM_INFO *parent)
{
    ROOM_INFO *const r = &g_Rooms[room_num];
    const DOOR_INFO *const door = (const DOOR_INFO *)(obj_ptr - 1);

    // clang-format off
    if (r->bound_left <= parent->test_left &&
        r->bound_right >= parent->test_right &&
        r->bound_top <= parent->test_top &&
        r->bound_bottom >= parent->test_bottom
   ) {
        return;
    }
    // clang-format on

    const MATRIX *const m = g_MatrixPtr;
    int32_t left = parent->test_right;
    int32_t right = parent->test_left;
    int32_t bottom = parent->test_top;
    int32_t top = parent->test_bottom;

    DOOR_VBUF door_vbuf[4];
    int32_t z_behind = 0;
    int32_t z_too_far = 0;

    for (int32_t i = 0; i < 4; i++) {
        DOOR_VBUF *const dvbuf = &door_vbuf[i];
        const XYZ_16 *const dvtx = &door->vertex[i];
        const int32_t xv =
            dvtx->x * m->_00 + dvtx->y * m->_01 + dvtx->z * m->_02 + m->_03;
        const int32_t yv =
            dvtx->x * m->_10 + dvtx->y * m->_11 + dvtx->z * m->_12 + m->_13;
        const int32_t zv =
            dvtx->x * m->_20 + dvtx->y * m->_21 + dvtx->z * m->_22 + m->_23;
        dvbuf->xv = xv;
        dvbuf->yv = yv;
        dvbuf->zv = zv;

        if (zv <= 0) {
            z_behind++;
            continue;
        }

        if (zv > g_PhdFarZ) {
            z_too_far++;
        }

        int32_t xs;
        int32_t ys;
        const int32_t zp = zv / g_PhdPersp;
        if (zp) {
            xs = xv / zp + g_PhdWinCenterX;
            ys = yv / zp + g_PhdWinCenterY;
        } else {
            xs = xv < 0 ? g_PhdWinLeft : g_PhdWinRight;
            ys = yv < 0 ? g_PhdWinTop : g_PhdWinBottom;
        }

        if (xs - 1 < left) {
            left = xs - 1;
        }
        if (xs + 1 > right) {
            right = xs + 1;
        }
        if (ys - 1 < top) {
            top = ys - 1;
        }
        if (ys + 1 > bottom) {
            bottom = ys + 1;
        }
    }

    if (z_behind == 4 || z_too_far == 4) {
        return;
    }

    if (z_behind > 0) {
        const DOOR_VBUF *dest = &door_vbuf[0];
        const DOOR_VBUF *last = &door_vbuf[3];

        for (int32_t i = 0; i < 4; i++, last = dest++) {
            if ((dest->zv < 0) == (last->zv < 0)) {
                continue;
            }

            if (dest->xv < 0 && last->xv < 0) {
                left = 0;
            } else if (dest->xv > 0 && last->xv > 0) {
                right = g_PhdWinMaxX;
            } else {
                left = 0;
                right = g_PhdWinMaxX;
            }

            if (dest->yv < 0 && last->yv < 0) {
                top = 0;
            } else if (dest->yv > 0 && last->yv > 0) {
                bottom = g_PhdWinMaxY;
            } else {
                top = 0;
                bottom = g_PhdWinMaxY;
            }
        }
    }

    if (left < parent->test_left) {
        left = parent->test_left;
    }
    if (right > parent->test_right) {
        right = parent->test_right;
    }
    if (top < parent->test_top) {
        top = parent->test_top;
    }
    if (bottom > parent->test_bottom) {
        bottom = parent->test_bottom;
    }

    if (left >= right || top >= bottom) {
        return;
    }

    if (r->bound_active & 2) {
        if (left < r->test_left) {
            r->test_left = left;
        }
        if (top < r->test_top) {
            r->test_top = top;
        }
        if (right > r->test_right) {
            r->test_right = right;
        }
        if (bottom > r->test_bottom) {
            r->test_bottom = bottom;
        }
    } else {
        g_BoundRooms[g_BoundEnd++ % MAX_BOUND_ROOMS] = room_num;
        r->bound_active |= 2;
        r->bound_active += (int16_t)(g_MidSort << 8);
        r->test_left = left;
        r->test_right = right;
        r->test_top = top;
        r->test_bottom = bottom;
    }
}

void __cdecl Room_Clip(const ROOM_INFO *const r)
{
    int32_t xv[8];
    int32_t yv[8];
    int32_t zv[8];

    xv[0] = WALL_L;
    yv[0] = r->max_ceiling - r->pos.y;
    zv[0] = WALL_L;

    xv[1] = (r->y_size - 1) * WALL_L;
    yv[1] = r->max_ceiling - r->pos.y;
    zv[1] = WALL_L;

    xv[2] = (r->y_size - 1) * WALL_L;
    yv[2] = r->max_ceiling - r->pos.y;
    zv[2] = (r->x_size - 1) * WALL_L;

    xv[3] = WALL_L;
    yv[3] = r->max_ceiling - r->pos.y;
    zv[3] = (r->x_size - 1) * WALL_L;

    xv[4] = WALL_L;
    yv[4] = r->min_floor - r->pos.y;
    zv[4] = WALL_L;

    xv[5] = (r->y_size - 1) * WALL_L;
    yv[5] = r->min_floor - r->pos.y;
    zv[5] = WALL_L;

    xv[6] = (r->y_size - 1) * WALL_L;
    yv[6] = r->min_floor - r->pos.y;
    zv[6] = (r->x_size - 1) * WALL_L;

    xv[7] = WALL_L;
    yv[7] = r->min_floor - r->pos.y;
    zv[7] = (r->x_size - 1) * WALL_L;

    bool clip_room = false;
    bool clip[8];

    const MATRIX *const m = g_MatrixPtr;
    for (int32_t i = 0; i < 8; i++) {
        const int32_t x = xv[i];
        const int32_t y = yv[i];
        const int32_t z = zv[i];
        xv[i] = x * m->_00 + y * m->_01 + z * m->_02 + m->_03;
        yv[i] = x * m->_10 + y * m->_11 + z * m->_12 + m->_13;
        zv[i] = x * m->_20 + y * m->_21 + z * m->_22 + m->_23;
        if (zv[i] > g_PhdFarZ) {
            clip_room = true;
            clip[i] = true;
        } else {
            clip[i] = false;
        }
    }

    if (!clip_room) {
        return;
    }

    int32_t min_x = 0x10000000;
    int32_t min_y = 0x10000000;
    int32_t max_x = -0x10000000;
    int32_t max_y = -0x10000000;
    for (int32_t i = 0; i < 12; i++) {
        const int32_t p1 = g_BoxLines[i][0];
        const int32_t p2 = g_BoxLines[i][1];

        if (clip[p1] == clip[p2]) {
            continue;
        }

        const int32_t zdiv = (zv[p2] - zv[p1]) >> W2V_SHIFT;
        if (zdiv) {
            const int32_t znom = (g_PhdFarZ - zv[p1]) >> W2V_SHIFT;
            const int32_t x = xv[p1]
                + ((((xv[p2] - xv[p1]) >> W2V_SHIFT) * znom / zdiv)
                   << W2V_SHIFT);
            const int32_t y = yv[p1]
                + ((((yv[p2] - yv[p1]) >> W2V_SHIFT) * znom / zdiv)
                   << W2V_SHIFT);

            CLAMPG(min_x, x);
            CLAMPL(max_x, x);
            CLAMPG(min_y, y);
            CLAMPL(max_y, y);
        } else {
            CLAMPG(min_x, xv[p1]);
            CLAMPG(min_x, xv[p2]);
            CLAMPL(max_x, xv[p1]);
            CLAMPL(max_x, xv[p2]);
            CLAMPG(min_y, yv[p1]);
            CLAMPG(min_y, yv[p2]);
            CLAMPL(max_y, yv[p1]);
            CLAMPL(max_y, yv[p2]);
        }
    }

    const int32_t zp = g_PhdFarZ / g_PhdPersp;
    min_x = g_PhdWinCenterX + min_x / zp;
    min_y = g_PhdWinCenterY + min_y / zp;
    max_x = g_PhdWinCenterX + max_x / zp;
    max_y = g_PhdWinCenterY + max_y / zp;

    // clang-format off
    if (min_x > g_PhdWinRight ||
        min_y > g_PhdWinBottom ||
        max_x < g_PhdWinLeft ||
        max_y < g_PhdWinTop
    ) {
        return;
    }
    // clang-format on

    CLAMPL(min_x, g_PhdWinLeft);
    CLAMPL(min_y, g_PhdWinTop);
    CLAMPG(max_x, g_PhdWinRight);
    CLAMPG(max_y, g_PhdWinBottom);
    S_InsertBackPolygon(min_x, min_y, max_x, max_y);
}
