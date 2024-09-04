#include "game/lara/lara_cheat.h"

#include "game/camera.h"
#include "game/console.h"
#include "game/game_string.h"
#include "game/inventory.h"
#include "game/items.h"
#include "game/lara/lara_control.h"
#include "game/math.h"
#include "game/output.h"
#include "game/room.h"
#include "global/funcs.h"
#include "global/utils.h"
#include "global/vars.h"

#include <libtrx/utils.h>
#include <libtrx/vector.h>

void __cdecl Lara_Cheat_EndLevel(void)
{
    g_LevelComplete = true;
    Console_Log(GS(OSD_COMPLETE_LEVEL));
}

bool __cdecl Lara_Cheat_EnterFlyMode(void)
{
    if (g_LaraItem == NULL) {
        return false;
    }

    Lara_GetOffVehicle();

    if (g_Lara.water_status != LWS_UNDERWATER || g_LaraItem->hit_points <= 0) {
        g_LaraItem->pos.y -= STEP_L;
        g_LaraItem->current_anim_state = LS_SWIM;
        g_LaraItem->goal_anim_state = LS_SWIM;
        g_LaraItem->anim_num =
            g_Objects[O_LARA].anim_idx + LA_UNDERWATER_SWIM_FORWARD_DRIFT;
        g_LaraItem->frame_num = g_Anims[g_LaraItem->anim_num].frame_base;
        g_LaraItem->gravity = 0;
        g_LaraItem->rot.x = 30 * PHD_DEGREE;
        g_LaraItem->fall_speed = 30;
        g_Lara.head_x_rot = 0;
        g_Lara.head_y_rot = 0;
        g_Lara.torso_x_rot = 0;
        g_Lara.torso_y_rot = 0;
    }

    g_Lara.water_status = LWS_CHEAT;
    g_Lara.spaz_effect_count = 0;
    g_Lara.spaz_effect = NULL;
    g_Lara.hit_frame = 0;
    g_Lara.hit_direction = -1;
    g_Lara.air = LARA_MAX_AIR;
    g_Lara.death_timer = 0;
    g_Lara.mesh_effects = 0;
    g_Lara.extra_anim = 0;

    Lara_InitialiseMeshes(g_CurrentLevel);
    g_Camera.type = CAM_CHASE;
    Output_AlterFOV(GAME_FOV * PHD_DEGREE);

    Console_Log(GS(OSD_FLY_MODE_ON));
    return true;
}

bool __cdecl Lara_Cheat_ExitFlyMode(void)
{
    if (g_LaraItem == NULL) {
        return false;
    }

    const ROOM_INFO *const room = &g_Rooms[g_LaraItem->room_num];
    const bool room_submerged = (room->flags & RF_UNDERWATER) != 0;
    const int16_t water_height = Room_GetWaterHeight(
        g_LaraItem->pos.x, g_LaraItem->pos.y, g_LaraItem->pos.z,
        g_LaraItem->room_num);

    if (room_submerged || (water_height != NO_HEIGHT && water_height > 0)) {
        g_Lara.water_status = LWS_UNDERWATER;
    } else {
        g_Lara.water_status = LWS_ABOVE_WATER;
        g_LaraItem->anim_num = g_Objects[O_LARA].anim_idx + LA_STAND_STILL;
        g_LaraItem->frame_num = g_Anims[g_LaraItem->anim_num].frame_base;
        g_LaraItem->rot.x = 0;
        g_LaraItem->rot.z = 0;
        g_Lara.head_x_rot = 0;
        g_Lara.head_y_rot = 0;
        g_Lara.torso_x_rot = 0;
        g_Lara.torso_y_rot = 0;
    }
    g_Lara.gun_status = LGS_ARMLESS;

    Console_Log(GS(OSD_FLY_MODE_OFF));
    return true;
}

void __cdecl Lara_Cheat_GetStuff(void)
{
    Inv_AddItem(O_PISTOL_ITEM);
    Inv_AddItem(O_MAGNUM_ITEM);
    Inv_AddItem(O_UZI_ITEM);
    Inv_AddItem(O_SHOTGUN_ITEM);
    Inv_AddItem(O_HARPOON_ITEM);
    Inv_AddItem(O_M16_ITEM);
    Inv_AddItem(O_GRENADE_ITEM);

    Inv_AddItemNTimes(O_FLARES_ITEM, 10);
    Inv_AddItemNTimes(O_SMALL_MEDIPACK_ITEM, 10);
    Inv_AddItemNTimes(O_LARGE_MEDIPACK_ITEM, 10);

    g_Lara.magnum_ammo.ammo = 1000;
    g_Lara.uzi_ammo.ammo = 2000;
    g_Lara.shotgun_ammo.ammo = 300;
    g_Lara.harpoon_ammo.ammo = 300;
    g_Lara.m16_ammo.ammo = 300;
    g_Lara.grenade_ammo.ammo = 300;
}

bool Lara_Cheat_Teleport(int32_t x, int32_t y, int32_t z)
{
    int16_t room_num = Room_GetIndexFromPos(x, y, z);
    if (room_num == NO_ROOM) {
        return false;
    }

    const SECTOR_INFO *sector = Room_GetSector(x, y, z, &room_num);
    int16_t height = Room_GetHeight(sector, x, y, z);

    if (height == NO_HEIGHT) {
        // Sample a sphere of points around target x, y, z
        // and teleport to the first available location.
        VECTOR *const points = Vector_Create(sizeof(XYZ_32));

        const int32_t radius = 10;
        const int32_t unit = STEP_L;
        for (int32_t dx = -radius; dx <= radius; dx++) {
            for (int32_t dz = -radius; dz <= radius; dz++) {
                if (SQUARE(dx) + SQUARE(dz) > SQUARE(radius)) {
                    continue;
                }

                const XYZ_32 point = {
                    .x = ROUND_TO_SECTOR(x + dx * unit) + WALL_L / 2,
                    .y = y,
                    .z = ROUND_TO_SECTOR(z + dz * unit) + WALL_L / 2,
                };
                room_num = Room_GetIndexFromPos(point.x, point.y, point.z);
                if (room_num == NO_ROOM) {
                    continue;
                }
                sector = Room_GetSector(point.x, point.y, point.z, &room_num);
                height = Room_GetHeight(sector, point.x, point.y, point.z);
                if (height == NO_HEIGHT) {
                    continue;
                }
                Vector_Add(points, (void *)&point);
            }
        }

        int32_t best_distance = INT32_MAX;
        for (int32_t i = 0; i < points->count; i++) {
            const XYZ_32 *const point = (const XYZ_32 *)Vector_Get(points, i);
            const int32_t distance =
                XYZ_32_GetDistance(point, &g_LaraItem->pos);
            if (distance < best_distance) {
                best_distance = distance;
                x = point->x;
                y = point->y;
                z = point->z;
            }
        }

        Vector_Free(points);
        if (best_distance == INT32_MAX) {
            return false;
        }
    }

    room_num = Room_GetIndexFromPos(x, y, z);
    if (room_num == NO_ROOM) {
        return false;
    }
    sector = Room_GetSector(x, y, z, &room_num);
    height = Room_GetHeight(sector, x, y, z);
    if (height == NO_HEIGHT) {
        return false;
    }

    g_LaraItem->pos.x = x;
    g_LaraItem->pos.y = y;
    g_LaraItem->pos.z = z;
    g_LaraItem->floor = height;

    Lara_GetOffVehicle();

    if (g_LaraItem->room_num != room_num) {
        const int16_t item_num = g_LaraItem - g_Items;
        Item_NewRoom(item_num, room_num);
    }

    Camera_ResetPosition();
    return true;
}
