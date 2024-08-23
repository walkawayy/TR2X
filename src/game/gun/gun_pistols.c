#include "game/gun/gun_pistols.h"

#include "game/gun/gun_misc.h"
#include "game/sound.h"
#include "global/funcs.h"
#include "global/vars.h"

void __cdecl Gun_Pistols_SetArmInfo(LARA_ARM *const arm, const int32_t frame)
{
    const int16_t anim_idx = g_Objects[O_LARA_PISTOLS].anim_idx;

    if (frame < LF_G_UNDRAW_START) {
        arm->anim_num = anim_idx;
    } else if (frame <= LF_G_UNDRAW_END) {
        arm->anim_num = anim_idx + 1;
    } else if (frame <= LF_G_DRAW_END) {
        arm->anim_num = anim_idx + 2;
    } else {
        arm->anim_num = anim_idx + 3;
    }

    arm->frame_num = frame;
    arm->frame_base = g_Anims[arm->anim_num].frame_ptr;
}

void __cdecl Gun_Pistols_Draw(const LARA_GUN_TYPE weapon_type)
{
    int16_t frame = g_Lara.left_arm.frame_num + 1;

    if (!(frame >= LF_G_UNDRAW_START && frame <= LF_G_DRAW_END)) {
        frame = LF_G_UNDRAW_START;
    } else if (frame == LF_G_DRAW_START) {
        Gun_Pistols_DrawMeshes(weapon_type);
        Sound_Effect(SFX_LARA_DRAW, &g_LaraItem->pos, SPM_NORMAL);
    } else if (frame == LF_G_DRAW_END) {
        Gun_Pistols_Ready(weapon_type);
        frame = LF_G_AIM_START;
    }

    Gun_Pistols_SetArmInfo(&g_Lara.right_arm, frame);
    Gun_Pistols_SetArmInfo(&g_Lara.left_arm, frame);
}
