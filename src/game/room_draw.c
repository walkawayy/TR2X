#include "game/room_draw.h"

#include "game/matrix.h"
#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"

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
