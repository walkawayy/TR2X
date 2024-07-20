#include "game/items.h"

#include "game/camera.h"
#include "game/math.h"
#include "game/matrix.h"
#include "game/room.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <assert.h>

static OBJECT_BOUNDS Item_ConvertBounds(const int16_t *bounds_in);

static OBJECT_BOUNDS Item_ConvertBounds(const int16_t *const bounds_in)
{
    // TODO: remove this conversion utility once we gain control over its
    // incoming arguments
    return (OBJECT_BOUNDS) {
        .shift = {
            .min = {
                .x = bounds_in[0],
                .y = bounds_in[2],
                .z = bounds_in[4],
            },
            .max = {
                .x = bounds_in[1],
                .y = bounds_in[3],
                .z = bounds_in[5],
            },
        },
        .rot = {
            .min = {
                .x = bounds_in[6],
                .y = bounds_in[8],
                .z = bounds_in[10],
            },
            .max = {
                .x = bounds_in[7],
                .y = bounds_in[9],
                .z = bounds_in[11],
            },
        },
    };
}

void __cdecl Item_InitialiseArray(const int32_t num_items)
{
    assert(num_items > 0);
    g_NextItemFree = g_LevelItemCount;
    g_PrevItemActive = NO_ITEM;
    g_NextItemActive = NO_ITEM;
    for (int i = g_NextItemFree; i < num_items - 1; i++) {
        ITEM_INFO *const item = &g_Items[i];
        item->active = 1;
        item->next_item = i + 1;
    }
    g_Items[num_items - 1].next_item = NO_ITEM;
}

int16_t __cdecl Item_Create(void)
{
    const int16_t result = g_NextItemFree;
    if (result != NO_ITEM) {
        g_Items[result].flags = 0;
        g_NextItemFree = g_Items[result].next_item;
    }
    return result;
}

void __cdecl Item_Kill(const int16_t item_num)
{
    Item_RemoveActive(item_num);
    Item_RemoveDrawn(item_num);

    ITEM_INFO *const item = &g_Items[item_num];
    if (item == g_Lara.target) {
        g_Lara.target = NULL;
    }

    if (item_num < g_LevelItemCount) {
        item->flags |= IF_KILLED;
    } else {
        item->next_item = g_NextItemFree;
        g_NextItemFree = item_num;
    }
}

void __cdecl Item_Initialise(const int16_t item_num)
{
    ITEM_INFO *const item = &g_Items[item_num];
    item->anim_num = g_Objects[item->object_num].anim_idx;
    item->frame_num = g_Anims[item->anim_num].frame_base;
    item->goal_anim_state = g_Anims[item->anim_num].current_anim_state;
    item->current_anim_state = item->goal_anim_state;
    item->required_anim_state = 0;
    item->rot.x = 0;
    item->rot.z = 0;
    item->speed = 0;
    item->fall_speed = 0;
    item->hit_points = g_Objects[item->object_num].hit_points;
    item->timer = 0;
    item->mesh_bits = 0xFFFFFFFF;
    item->touch_bits = 0;
    item->data = NULL;

    item->active = 0;
    item->status = IS_INACTIVE;
    item->gravity = 0;
    item->hit_status = 0;
    item->collidable = 1;
    item->looked_at = 0;
    item->killed = 0;

    if ((item->flags & IF_INVISIBLE) != 0) {
        item->status = IS_INVISIBLE;
        item->flags &= ~IF_INVISIBLE;
    } else if (g_Objects[item->object_num].intelligent) {
        item->status = IS_INVISIBLE;
    }

    if (item->flags & IF_KILLED) {
        item->killed = 1;
        item->flags &= ~IF_KILLED;
    }

    if ((item->flags & IF_CODE_BITS) == IF_CODE_BITS) {
        item->flags &= ~IF_CODE_BITS;
        item->flags |= IF_REVERSE;
        Item_AddActive(item_num);
        item->status = IS_ACTIVE;
    }

    ROOM_INFO *const room = &g_Rooms[item->room_num];
    item->next_item = room->item_num;
    room->item_num = item_num;

    const int32_t dx = (item->pos.x - room->pos.x) >> WALL_SHIFT;
    const int32_t dz = (item->pos.z - room->pos.z) >> WALL_SHIFT;
    const FLOOR_INFO *const floor = &room->floor[dx * room->x_size + dz];
    item->floor = floor->floor << 8;

    if (g_SaveGame.bonus_flag && !g_IsDemoLevelType) {
        item->hit_points *= 2;
    }

    if (g_Objects[item->object_num].initialise != NULL) {
        g_Objects[item->object_num].initialise(item_num);
    }
}

void __cdecl Item_RemoveActive(const int16_t item_num)
{
    ITEM_INFO *const item = &g_Items[item_num];
    if (!item->active) {
        return;
    }

    item->active = 0;

    int16_t link_num = g_NextItemActive;
    if (link_num == item_num) {
        g_NextItemActive = item->next_active;
        return;
    }

    while (link_num != NO_ITEM) {
        if (g_Items[link_num].next_active == item_num) {
            g_Items[link_num].next_active = item->next_active;
            return;
        }
        link_num = g_Items[link_num].next_active;
    }
}

void __cdecl Item_RemoveDrawn(const int16_t item_num)
{
    const ITEM_INFO *const item = &g_Items[item_num];
    if (item->room_num == NO_ROOM) {
        return;
    }

    int16_t link_num = g_Rooms[item->room_num].item_num;
    if (link_num == item_num) {
        g_Rooms[item->room_num].item_num = item->next_item;
        return;
    }

    while (link_num != NO_ITEM) {
        if (g_Items[link_num].next_item == item_num) {
            g_Items[link_num].next_item = item->next_item;
            return;
        }
        link_num = g_Items[link_num].next_item;
    }
}

void __cdecl Item_AddActive(const int16_t item_num)
{
    ITEM_INFO *const item = &g_Items[item_num];
    if (g_Objects[item->object_num].control == NULL) {
        item->status = IS_INACTIVE;
        return;
    }

    if (item->active) {
        return;
    }

    item->active = 1;
    item->next_active = g_NextItemActive;
    g_NextItemActive = item_num;
}

void __cdecl Item_NewRoom(const int16_t item_num, const int16_t room_num)
{
    ITEM_INFO *const item = &g_Items[item_num];
    ROOM_INFO *room = NULL;

    if (item->room_num != NO_ROOM) {
        room = &g_Rooms[item->room_num];

        int16_t link_num = room->item_num;
        if (link_num == item_num) {
            room->item_num = item->next_item;
        } else {
            while (link_num != NO_ITEM) {
                if (g_Items[link_num].next_item == item_num) {
                    g_Items[link_num].next_item = item->next_item;
                    break;
                }
                link_num = g_Items[link_num].next_item;
            }
        }
    }

    item->room_num = room_num;
    room = &g_Rooms[room_num];
    item->next_item = room->item_num;
    room->item_num = item_num;
}

int32_t __cdecl Item_GlobalReplace(
    const int32_t src_object_num, const int32_t dst_object_num)
{
    int32_t changed = 0;

    for (int i = 0; i < g_RoomCount; i++) {
        int16_t j = g_Rooms[i].item_num;
        while (j != NO_ITEM) {
            ITEM_INFO *const item = &g_Items[j];
            if (item->object_num == src_object_num) {
                item->object_num = dst_object_num;
                changed++;
            }
            j = item->next_item;
        }
    }

    return changed;
}

void __cdecl Item_ClearKilled(void)
{
    // Remove corpses and other killed items. Part of OG performance
    // improvements, generously used in Opera House and Barkhang Monastery
    int16_t link_num = g_PrevItemActive;
    while (link_num != NO_ITEM) {
        ITEM_INFO *const item = &g_Items[link_num];
        Item_Kill(link_num);
        link_num = item->next_active;
        item->next_active = NO_ITEM;
    }
    g_PrevItemActive = NO_ITEM;
}

bool __cdecl Item_Teleport(ITEM_INFO *item, int32_t x, int32_t y, int32_t z)
{
    int16_t room_num = Room_GetIndexFromPos(x, y, z);
    if (room_num == NO_ROOM) {
        return false;
    }
    FLOOR_INFO *const floor = Room_GetFloor(x, y, z, &room_num);
    const int16_t height = Room_GetHeight(floor, x, y, z);
    if (height != NO_HEIGHT) {
        item->pos.x = x;
        item->pos.y = y;
        item->pos.z = z;
        item->floor = height;
        if (item->room_num != room_num) {
            const int16_t item_num = item - g_Items;
            Item_NewRoom(item_num, room_num);
        }

        if (item->object_num == O_LARA) {
            Camera_ResetPosition();
        }
        return true;
    }
    return false;
}

bool Item_IsSmashable(const ITEM_INFO *item)
{
    return (item->object_num == O_WINDOW_1 || item->object_num == O_BELL);
}

void __cdecl Item_ShiftCol(ITEM_INFO *const item, COLL_INFO *const coll)
{
    item->pos.x += coll->shift.x;
    item->pos.y += coll->shift.y;
    item->pos.z += coll->shift.z;
    coll->shift.z = 0;
    coll->shift.y = 0;
    coll->shift.x = 0;
}

void __cdecl Item_UpdateRoom(ITEM_INFO *const item, const int32_t height)
{
    int32_t x = item->pos.x;
    int32_t y = height + item->pos.y;
    int32_t z = item->pos.z;

    int16_t room_num = item->room_num;
    const FLOOR_INFO *const floor = Room_GetFloor(x, y, z, &room_num);
    item->floor = Room_GetHeight(floor, x, y, z);
    if (item->room_num != room_num) {
        Item_NewRoom(g_Lara.item_num, room_num);
    }
}

int32_t __cdecl Item_TestBoundsCollide(
    const ITEM_INFO *const src_item, const ITEM_INFO *const dst_item,
    const int32_t radius)
{
    const int16_t *const src_bounds = Item_GetBestFrame(src_item);
    const int16_t *const dst_bounds = Item_GetBestFrame(dst_item);

    if (src_item->pos.y + src_bounds[FBBOX_MAX_Y]
            <= dst_item->pos.y + dst_bounds[FBBOX_MIN_Y]
        || src_item->pos.y + src_bounds[FBBOX_MIN_Y]
            >= dst_item->pos.y + dst_bounds[FBBOX_MAX_Y]) {
        return false;
    }

    const int32_t c = Math_Cos(src_item->rot.y);
    const int32_t s = Math_Sin(src_item->rot.y);
    const int32_t dx = dst_item->pos.x - src_item->pos.x;
    const int32_t dz = dst_item->pos.z - src_item->pos.z;
    const int32_t rx = (c * dx - s * dz) >> W2V_SHIFT;
    const int32_t rz = (c * dz + s * dx) >> W2V_SHIFT;

    // clang-format off
    return (
        rx >= src_bounds[FBBOX_MIN_X] - radius &&
        rx <= src_bounds[FBBOX_MAX_X] + radius &&
        rz >= src_bounds[FBBOX_MIN_Z] - radius &&
        rz <= src_bounds[FBBOX_MAX_Z] + radius);
    // clang-format on
}

int32_t __cdecl Item_TestPosition(
    const int16_t *const bounds_in, const ITEM_INFO *const src_item,
    const ITEM_INFO *const dst_item)
{
    const OBJECT_BOUNDS bounds = Item_ConvertBounds(bounds_in);

    const XYZ_16 rot = {
        .x = dst_item->rot.x - src_item->rot.x,
        .y = dst_item->rot.y - src_item->rot.y,
        .z = dst_item->rot.z - src_item->rot.z,
    };
    const XYZ_32 dist = {
        .x = dst_item->pos.x - src_item->pos.x,
        .y = dst_item->pos.y - src_item->pos.y,
        .z = dst_item->pos.z - src_item->pos.z,
    };

    // clang-format off
    if (rot.x < bounds.rot.min.x ||
        rot.x > bounds.rot.max.x ||
        rot.y < bounds.rot.min.y ||
        rot.y > bounds.rot.max.y ||
        rot.z < bounds.rot.min.z ||
        rot.z > bounds.rot.max.z
    ) {
        return false;
    }
    // clang-format on

    Matrix_PushUnit();
    Matrix_RotYXZ(src_item->rot.y, src_item->rot.x, src_item->rot.z);
    const MATRIX *const m = g_MatrixPtr;
    const XYZ_32 shift = {
        .x = (dist.x * m->_00 + dist.y * m->_10 + dist.z * m->_20) >> W2V_SHIFT,
        .y = (dist.x * m->_01 + dist.y * m->_11 + dist.z * m->_21) >> W2V_SHIFT,
        .z = (dist.x * m->_02 + dist.y * m->_12 + dist.z * m->_22) >> W2V_SHIFT,
    };
    Matrix_Pop();

    // clang-format off
    return (
        shift.x >= bounds.shift.min.x &&
        shift.x <= bounds.shift.max.x &&
        shift.y >= bounds.shift.min.y &&
        shift.y <= bounds.shift.max.y &&
        shift.z >= bounds.shift.min.z &&
        shift.z <= bounds.shift.max.z
    );
    // clang-format on
}
