#include "game/room.h"

#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <assert.h>

int16_t Room_GetIndexFromPos(const int32_t x, const int32_t y, const int32_t z)
{
    for (int i = 0; i < g_RoomCount; i++) {
        const ROOM_INFO *const room = &g_Rooms[i];
        const int32_t x1 = room->pos.x + WALL_L;
        const int32_t x2 = room->pos.x + (room->y_size << WALL_SHIFT) - WALL_L;
        const int32_t y1 = room->max_ceiling;
        const int32_t y2 = room->min_floor;
        const int32_t z1 = room->pos.z + WALL_L;
        const int32_t z2 = room->pos.z + (room->x_size << WALL_SHIFT) - WALL_L;
        if (x >= x1 && x < x2 && y >= y1 && y <= y2 && z >= z1 && z < z2) {
            return i;
        }
    }
    return NO_ROOM;
}

int32_t __cdecl Room_FindGridShift(int32_t src, const int32_t dst)
{
    const int32_t src_w = src >> WALL_SHIFT;
    const int32_t dst_w = dst >> WALL_SHIFT;
    if (src_w == dst_w) {
        return 0;
    }

    src &= WALL_L - 1;
    if (dst_w > src_w) {
        return WALL_L - (src - 1);
    } else {
        return -(src + 1);
    }
}

void __cdecl Room_GetNearbyRooms(
    const int32_t x, const int32_t y, const int32_t z, const int32_t r,
    const int32_t h, const int16_t room_num)
{
    g_DrawRoomsArray[0] = room_num;
    g_DrawRoomsCount = 1;

    Room_GetNewRoom(r + x, y, r + z, room_num);
    Room_GetNewRoom(x - r, y, r + z, room_num);
    Room_GetNewRoom(r + x, y, z - r, room_num);
    Room_GetNewRoom(x - r, y, z - r, room_num);
    Room_GetNewRoom(r + x, y - h, r + z, room_num);
    Room_GetNewRoom(x - r, y - h, r + z, room_num);
    Room_GetNewRoom(r + x, y - h, z - r, room_num);
    Room_GetNewRoom(x - r, y - h, z - r, room_num);
}

void __cdecl Room_GetNewRoom(
    const int32_t x, const int32_t y, const int32_t z, int16_t room_num)
{
    Room_GetFloor(x, y, z, &room_num);

    for (int32_t i = 0; i < g_DrawRoomsCount; i++) {
        if (g_DrawRoomsArray[i] == room_num) {
            return;
        }
    }

    // TODO: fix crash when trying to draw too many rooms
    g_DrawRoomsArray[g_DrawRoomsCount++] = room_num;
}

int16_t __cdecl Room_GetTiltType(
    const FLOOR_INFO *floor, const int32_t x, const int32_t y, const int32_t z)
{
    while (floor->pit_room != (uint8_t)NO_ROOM) {
        const ROOM_INFO *const room = &g_Rooms[floor->pit_room];
        const int32_t dz = z - room->pos.z;
        const int32_t dx = x - room->pos.x;
        floor = &room->floor
                     [(dz >> WALL_SHIFT) + (dx >> WALL_SHIFT) * room->x_size];
    }

    if ((y + 512 >= (floor->floor << 8)) && floor->idx != 0) {
        const int16_t *floor_data = &g_FloorData[floor->idx];
        if ((floor_data[0] & DATA_TYPE) == FT_TILT) {
            return floor_data[1];
        }
    }

    return 0;
}

FLOOR_INFO *__cdecl Room_GetFloor(
    const int32_t x, const int32_t y, const int32_t z, int16_t *const room_num)
{
    FLOOR_INFO *floor = NULL;

    while (true) {
        const ROOM_INFO *r = &g_Rooms[*room_num];
        int32_t x_floor = (z - r->pos.z) >> WALL_SHIFT;
        int32_t y_floor = (x - r->pos.x) >> WALL_SHIFT;

        if (x_floor <= 0) {
            x_floor = 0;
            if (y_floor < 1) {
                y_floor = 1;
            } else if (y_floor > r->y_size - 2) {
                y_floor = r->y_size - 2;
            }
        } else if (x_floor >= r->x_size - 1) {
            x_floor = r->x_size - 1;
            if (y_floor < 1) {
                y_floor = 1;
            } else if (y_floor > r->y_size - 2) {
                y_floor = r->y_size - 2;
            }
        } else if (y_floor < 0) {
            y_floor = 0;
        } else if (y_floor >= r->y_size) {
            y_floor = r->y_size - 1;
        }

        floor = &r->floor[x_floor + y_floor * r->x_size];
        const int16_t door = Room_GetDoor(floor);
        if (door == (uint8_t)NO_ROOM) {
            break;
        }
        *room_num = door;
    }

    assert(floor != NULL);

    if (y >= (floor->floor << 8)) {
        while (floor->pit_room != (uint8_t)NO_ROOM) {
            *room_num = floor->pit_room;
            const ROOM_INFO *const r = &g_Rooms[*room_num];
            const int32_t x_floor = ((z - r->pos.z) >> WALL_SHIFT);
            const int32_t y_floor = ((x - r->pos.x) >> WALL_SHIFT);
            floor = &r->floor[x_floor + y_floor * r->x_size];
            if (y < (floor->floor << 8)) {
                break;
            }
        }
    } else if (y < (floor->ceiling << 8)) {
        while (floor->sky_room != (uint8_t)NO_ROOM) {
            *room_num = floor->sky_room;
            const ROOM_INFO *const r =
                &g_Rooms[(unsigned __int8)floor->sky_room];
            const int32_t x_floor = (z - r->pos.z) >> WALL_SHIFT;
            const int32_t y_floor = (x - r->pos.x) >> WALL_SHIFT;
            floor = &r->floor[x_floor + y_floor * r->x_size];
            if (y >= (floor->ceiling << 8)) {
                break;
            }
        }
    }

    return floor;
}
