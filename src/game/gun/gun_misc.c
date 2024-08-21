#include "game/gun/gun_misc.h"

#include "game/los.h"
#include "game/math_misc.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#define NEAR_ANGLE (PHD_DEGREE * 15) // = 2730

void __cdecl Gun_TargetInfo(const WEAPON_INFO *const winfo)
{
    if (!g_Lara.target) {
        g_Lara.left_arm.lock = 0;
        g_Lara.right_arm.lock = 0;
        g_Lara.target_angles[0] = 0;
        g_Lara.target_angles[1] = 0;
        return;
    }

    GAME_VECTOR start;
    start.pos.x = g_LaraItem->pos.x;
    start.pos.y = g_LaraItem->pos.y - 650;
    start.pos.z = g_LaraItem->pos.z;
    start.room_num = g_LaraItem->room_num;

    GAME_VECTOR target;
    Gun_FindTargetPoint(g_Lara.target, &target);

    int16_t angles[2];
    // clang-format off
    Math_GetVectorAngles(
        target.pos.x - start.pos.x,
        target.pos.y - start.pos.y,
        target.pos.z - start.pos.z,
        angles);
    // clang-format on

    angles[0] -= g_LaraItem->rot.y;
    angles[1] -= g_LaraItem->rot.x;

    if (!LOS_Check(&start, &target)) {
        g_Lara.right_arm.lock = 0;
        g_Lara.left_arm.lock = 0;
    } else if (
        angles[0] >= winfo->lock_angles[0] && angles[0] <= winfo->lock_angles[1]
        && angles[1] >= winfo->lock_angles[2]
        && angles[1] <= winfo->lock_angles[3]) {
        g_Lara.right_arm.lock = 1;
        g_Lara.left_arm.lock = 1;
    } else {
        if (g_Lara.left_arm.lock
            && (angles[0] < winfo->left_angles[0]
                || angles[0] > winfo->left_angles[1]
                || angles[1] < winfo->left_angles[2]
                || angles[1] > winfo->left_angles[3])) {
            g_Lara.left_arm.lock = 0;
        }
        if (g_Lara.right_arm.lock
            && (angles[0] < winfo->right_angles[0]
                || angles[0] > winfo->right_angles[1]
                || angles[1] < winfo->right_angles[2]
                || angles[1] > winfo->right_angles[3])) {
            g_Lara.right_arm.lock = 0;
        }
    }

    g_Lara.target_angles[0] = angles[0];
    g_Lara.target_angles[1] = angles[1];
}

void __cdecl Gun_GetNewTarget(const WEAPON_INFO *const winfo)
{
    GAME_VECTOR start;
    start.pos.x = g_LaraItem->pos.x;
    start.pos.y = g_LaraItem->pos.y - 650;
    start.pos.z = g_LaraItem->pos.z;
    start.room_num = g_LaraItem->room_num;

    int16_t best_y_rot = 0x7FFF;
    int32_t best_dist = 0x7FFFFFFF;
    ITEM_INFO *best_target = NULL;

    const int16_t max_dist = winfo->target_dist;
    for (int32_t i = 0; i < NUM_SLOTS; i++) {
        const int16_t item_num = g_BaddieSlots[i].item_num;
        if (item_num == NO_ITEM || item_num == g_Lara.item_num) {
            continue;
        }

        ITEM_INFO *const item = &g_Items[item_num];
        if (item->hit_points <= 0) {
            continue;
        }

        const int32_t dx = item->pos.x - start.pos.x;
        const int32_t dy = item->pos.y - start.pos.y;
        const int32_t dz = item->pos.z - start.pos.z;
        if (ABS(dx) > max_dist || ABS(dy) > max_dist || ABS(dz) > max_dist) {
            continue;
        }

        const int32_t dist = SQUARE(dz) + SQUARE(dy) + SQUARE(dx);
        if (dist >= SQUARE(max_dist)) {
            continue;
        }

        GAME_VECTOR target;
        Gun_FindTargetPoint(item, &target);
        if (!LOS_Check(&start, &target)) {
            continue;
        }

        int16_t angles[2];
        Math_GetVectorAngles(
            target.pos.x - start.pos.x, target.pos.y - start.pos.y,
            target.pos.z - start.pos.z, angles);
        angles[0] -= g_Lara.torso_y_rot + g_LaraItem->rot.y;
        angles[1] -= g_Lara.torso_x_rot + g_LaraItem->rot.x;

        if (angles[0] >= winfo->lock_angles[0]
            && angles[0] <= winfo->lock_angles[1]
            && angles[1] >= winfo->lock_angles[2]
            && angles[1] <= winfo->lock_angles[3]) {
            const int16_t y_rot = ABS(angles[0]);
            if (y_rot < best_y_rot + NEAR_ANGLE && dist < best_dist) {
                best_dist = dist;
                best_y_rot = y_rot;
                best_target = item;
            }
        }
    }

    g_Lara.target = best_target;
    Gun_TargetInfo(winfo);
}
