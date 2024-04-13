#include "game/objects/creatures/bird.h"

#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"

typedef enum BIRD_ANIM {
    BIRD_ANIM_EMPTY = 0,
    BIRD_ANIM_FLY = 1,
    BIRD_ANIM_STOP = 2,
    BIRD_ANIM_GLIDE = 3,
    BIRD_ANIM_FALL = 4,
    BIRD_ANIM_DEATH = 5,
    BIRD_ANIM_ATTACK = 6,
    BIRD_ANIM_EAT = 7,
} BIRD_ANIM;

#define BIRD_START_ANIM 5
#define CROW_START_ANIM 14

void __cdecl Bird_Initialise(const int16_t item_num)
{
    Creature_Initialise(item_num);
    struct ITEM_INFO *item = &g_Items[item_num];
    if (item->object_num == O_CROW) {
        item->anim_num = g_Objects[O_CROW].anim_idx + CROW_START_ANIM;
        item->frame_num = g_Anims[item->anim_num].frame_base;
        item->goal_anim_state = BIRD_ANIM_EAT;
        item->current_anim_state = BIRD_ANIM_EAT;
    } else {
        item->anim_num = g_Objects[O_EAGLE].anim_idx + BIRD_START_ANIM;
        item->frame_num = g_Anims[item->anim_num].frame_base;
        item->goal_anim_state = BIRD_ANIM_STOP;
        item->current_anim_state = BIRD_ANIM_STOP;
    }
}
