#include "game/gun/gun_pistols.h"

#include "game/gun/gun_misc.h"
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
