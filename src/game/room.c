#include "game/room.h"

#include "game/shell.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#include <assert.h>

int16_t Room_GetIndexFromPos(const int32_t x, const int32_t y, const int32_t z)
{
    for (int32_t i = 0; i < g_RoomCount; i++) {
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
        if (FLOORDATA_TYPE(floor_data[0]) == FT_TILT) {
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

int32_t __cdecl Room_GetWaterHeight(
    const int32_t x, const int32_t y, const int32_t z, int16_t room_num)
{
    const FLOOR_INFO *floor = NULL;
    const ROOM_INFO *r = NULL;

    do {
        r = &g_Rooms[room_num];
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
        room_num = Room_GetDoor(floor);
    } while (room_num != (uint8_t)NO_ROOM);

    if (r->flags & RF_UNDERWATER) {
        while (floor->sky_room != (uint8_t)NO_ROOM) {
            r = &g_Rooms[floor->sky_room];
            if (!(r->flags & RF_UNDERWATER)) {
                break;
            }
            const int32_t x_floor = (z - r->pos.z) >> WALL_SHIFT;
            const int32_t y_floor = (x - r->pos.x) >> WALL_SHIFT;
            floor = &r->floor[x_floor + y_floor * r->x_size];
        }
        return floor->ceiling << 8;
    } else {
        while (floor->pit_room != (uint8_t)NO_ROOM) {
            r = &g_Rooms[floor->pit_room];
            if (r->flags & RF_UNDERWATER) {
                return floor->floor << 8;
            }
            const int32_t x_floor = (z - r->pos.z) >> WALL_SHIFT;
            const int32_t y_floor = (x - r->pos.x) >> WALL_SHIFT;
            floor = &r->floor[x_floor + y_floor * r->x_size];
        }
        return NO_HEIGHT;
    }
}

int32_t __cdecl Room_GetHeight(
    const FLOOR_INFO *floor, const int32_t x, const int32_t y, const int32_t z)
{
    g_HeightType = 0;
    g_TriggerIndex = NULL;

    while (floor->pit_room != (uint8_t)NO_ROOM) {
        const ROOM_INFO *const r = &g_Rooms[floor->pit_room];
        const int32_t x_floor = (z - r->pos.z) >> WALL_SHIFT;
        const int32_t y_floor = (x - r->pos.x) >> WALL_SHIFT;
        floor = &r->floor[x_floor + y_floor * r->x_size];
    }

    int32_t height = floor->floor << 8;
    if (g_GF_NoFloor && g_GF_NoFloor == height) {
        height = 0x4000;
    }

    if (!floor->idx) {
        return height;
    }

    int16_t *data = &g_FloorData[floor->idx];
    while (true) {
        const int16_t fd_cmd = *data++;

        switch (FLOORDATA_TYPE(fd_cmd)) {
        case FT_DOOR:
        case FT_ROOF:
            data++;
            break;

        case FT_TILT: {
            const int32_t x_off = data[0] >> 8;
            const int32_t y_off = (int8_t)data[0];
            data++;

            if (!g_IsChunkyCamera || (ABS(x_off) <= 2 && ABS(y_off) <= 2)) {
                if (ABS(x_off) > 2 || ABS(y_off) > 2) {
                    g_HeightType = HT_BIG_SLOPE;
                } else {
                    g_HeightType = HT_SMALL_SLOPE;
                }

                if (x_off < 0) {
                    height -= (x_off * (z & (WALL_L - 1))) >> 2;
                } else {
                    height += (x_off * ((WALL_L - 1 - z) & (WALL_L - 1))) >> 2;
                }

                if (y_off < 0) {
                    height -= (y_off * (x & (WALL_L - 1))) >> 2;
                } else {
                    height += (y_off * ((WALL_L - 1 - x) & (WALL_L - 1))) >> 2;
                }
            }
            break;
        }

        case FT_TRIGGER:
            if (g_TriggerIndex == NULL) {
                g_TriggerIndex = data - 1;
            }
            data++;

            int16_t trigger;
            do {
                trigger = *data++;
                switch (TRIGGER_TYPE(trigger)) {
                case TO_OBJECT:
                    const ITEM_INFO *const item =
                        &g_Items[TRIGGER_VALUE(trigger)];
                    const OBJECT_INFO *object = &g_Objects[item->object_num];
                    if (object->floor) {
                        object->floor(item, x, y, z, &height);
                    }
                    break;

                case TO_CAMERA:
                    trigger = *data++;
                    break;
                }
            } while (!TRIGGER_IS_END(trigger));

            break;

        case FT_LAVA:
            g_TriggerIndex = data - 1;
            break;

        case FT_CLIMB:
            if (g_TriggerIndex == NULL) {
                g_TriggerIndex = data - 1;
            }
            break;

        default:
            Shell_ExitSystem("GetHeight(): Unknown floordata type");
            break;
        }

        if (FLOORDATA_IS_END(fd_cmd)) {
            break;
        }
    }

    return height;
}
