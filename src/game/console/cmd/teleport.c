#include "game/console/cmd/teleport.h"

#include "game/game_string.h"
#include "game/gameflow/gameflow_new.h"
#include "game/items.h"
#include "game/lara/lara_cheat.h"
#include "game/objects/names.h"
#include "game/objects/vars.h"
#include "game/random.h"
#include "global/const.h"
#include "global/vars.h"

#include <libtrx/game/objects/common.h>
#include <libtrx/strings.h>

#include <math.h>
#include <stdio.h>

static bool M_CanTargetObject(GAME_OBJECT_ID object_id);
static bool M_IsFloatRound(float num);
static COMMAND_RESULT M_Entrypoint(const char *args);

static bool M_CanTargetObject(const GAME_OBJECT_ID object_id)
{
    return !Object_IsObjectType(object_id, g_NullObjects)
        && !Object_IsObjectType(object_id, g_AnimObjects)
        && !Object_IsObjectType(object_id, g_InvObjects);
}

static inline bool M_IsFloatRound(const float num)
{
    return (fabsf(num) - roundf(num)) < 0.0001f;
}

static COMMAND_RESULT M_Entrypoint(const char *const args)
{
    if (g_GameInfo.current_level.type == GFL_TITLE
        || g_GameInfo.current_level.type == GFL_DEMO
        || g_GameInfo.current_level.type == GFL_CUTSCENE) {
        return CR_UNAVAILABLE;
    }

    if (!g_Objects[O_LARA].loaded || !g_LaraItem->hit_points) {
        return CR_UNAVAILABLE;
    }

    // X Y Z
    {
        float x, y, z;
        if (sscanf(args, "%f %f %f", &x, &y, &z) == 3) {
            if (M_IsFloatRound(x)) {
                x += 0.5f;
            }
            if (M_IsFloatRound(z)) {
                z += 0.5f;
            }

            if (Lara_Cheat_Teleport(x * WALL_L, y * WALL_L, z * WALL_L)) {
                Console_Log(GS(OSD_POS_SET_POS), x, y, z);
                return CR_SUCCESS;
            }

            Console_Log(GS(OSD_POS_SET_POS_FAIL), x, y, z);
            return CR_FAILURE;
        }
    }

    // Room number
    {
        int16_t room_num = NO_ROOM_NEG;
        if (sscanf(args, "%hd", &room_num) == 1) {
            if (room_num < 0 || room_num >= g_RoomCount) {
                Console_Log(GS(OSD_INVALID_ROOM), room_num, g_RoomCount - 1);
                return CR_FAILURE;
            }

            const ROOM_INFO *const room = &g_Rooms[room_num];

            const int32_t x1 = room->pos.x + WALL_L;
            const int32_t x2 =
                (room->x_size << WALL_SHIFT) + room->pos.x - WALL_L;
            const int32_t y1 = room->min_floor;
            const int32_t y2 = room->max_ceiling;
            const int32_t z1 = room->pos.z + WALL_L;
            const int32_t z2 =
                (room->z_size << WALL_SHIFT) + room->pos.z - WALL_L;

            for (int32_t i = 0; i < 100; i++) {
                int32_t x = x1 + Random_GetControl() * (x2 - x1) / 0x7FFF;
                int32_t y = y1;
                int32_t z = z1 + Random_GetControl() * (z2 - z1) / 0x7FFF;
                if (Lara_Cheat_Teleport(x, y, z)) {
                    Console_Log(GS(OSD_POS_SET_ROOM), room_num);
                    return CR_SUCCESS;
                }
            }

            Console_Log(GS(OSD_POS_SET_ROOM_FAIL), room_num);
            return CR_FAILURE;
        }
    }

    // Nearest item of this name
    if (!String_Equivalent(args, "")) {
        int32_t match_count = 0;
        GAME_OBJECT_ID *matching_objs =
            Object_IdsFromName(args, &match_count, M_CanTargetObject);

        const ITEM_INFO *best_item = NULL;
        int32_t best_distance = INT32_MAX;

        for (int16_t item_num = 0; item_num < Item_GetTotalCount();
             item_num++) {
            const ITEM_INFO *const item = &g_Items[item_num];
            if (Object_IsObjectType(item->object_id, g_PickupObjects)
                && (item->status == IS_INVISIBLE
                    || item->status == IS_DEACTIVATED
                    || item->room_num == NO_ROOM)) {
                continue;
            }

            if (item->flags & IF_KILLED) {
                continue;
            }

            bool is_matched = false;
            for (int32_t i = 0; i < match_count; i++) {
                if (matching_objs[i] == item->object_id) {
                    is_matched = true;
                    break;
                }
            }
            if (!is_matched) {
                continue;
            }

            const int32_t distance = Item_GetDistance(item, &g_LaraItem->pos);
            if (distance < best_distance) {
                best_distance = distance;
                best_item = item;
            }
        }

        if (best_item != NULL) {
            if (Lara_Cheat_Teleport(
                    best_item->pos.x, best_item->pos.y - STEP_L,
                    best_item->pos.z)) {
                Console_Log(
                    GS(OSD_POS_SET_ITEM), Object_GetName(best_item->object_id));
            } else {
                Console_Log(
                    GS(OSD_POS_SET_ITEM_FAIL),
                    Object_GetName(best_item->object_id));
            }
            return CR_SUCCESS;
        } else {
            Console_Log(GS(OSD_POS_SET_ITEM_FAIL), args);
            return CR_FAILURE;
        }
    }

    return CR_BAD_INVOCATION;
}

CONSOLE_COMMAND g_Console_Cmd_Teleport = {
    .prefix = "tp",
    .proc = M_Entrypoint,
};
