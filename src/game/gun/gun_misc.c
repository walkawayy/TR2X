#include "game/gun/gun_misc.h"

#include "game/los.h"
#include "game/math_misc.h"
#include "global/funcs.h"
#include "global/vars.h"

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
