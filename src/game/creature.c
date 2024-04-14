#include "game/creature.h"

#include "game/math.h"
#include "game/random.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#define FRONT_ARC PHD_90
#define BLOCKED 0x4000
#define BLOCKED_SEARCH 0x8000

void __cdecl Creature_Initialise(const int16_t item_num)
{
    struct ITEM_INFO *const item = &g_Items[item_num];
    item->rot.y += (Random_GetControl() - PHD_90) >> 1;
    item->collidable = 1;
    item->data = 0;
}

int32_t __cdecl Creature_Activate(const int16_t item_num)
{
    int32_t result; // eax

    struct ITEM_INFO *const item = &g_Items[item_num];
    if (item->status != IS_INVISIBLE) {
        return true;
    }

    if (!LOT_EnableBaddieAI(item_num, false)) {
        return false;
    }

    item->status = IS_ACTIVE;
    return true;
}

void __cdecl Creature_AIInfo(
    struct ITEM_INFO *const item, struct AI_INFO *const info)
{
    CREATURE_INFO *creature = (CREATURE_INFO *)item->data;
    if (creature == NULL) {
        return;
    }

    switch (item->object_num) {
    case O_BANDIT_1:
    case O_BANDIT_2:
        Creature_GetBaddieTarget(creature->item_num, false);
        break;

    case O_MONK_1:
    case O_MONK_2:
        Creature_GetBaddieTarget(creature->item_num, true);
        break;

    default:
        creature->enemy = g_LaraItem;
        break;
    }

    struct ITEM_INFO *enemy = creature->enemy;
    if (enemy == NULL) {
        enemy = g_LaraItem;
    }

    int16_t *zone;
    if (creature->lot.fly) {
        zone = g_FlyZone[g_FlipStatus];
    } else {
        zone = g_GroundZone[creature->lot.step / STEP_L][g_FlipStatus];
    }

    {
        const ROOM_INFO *const r = &g_Rooms[item->room_num];
        const int32_t z_floor = (item->pos.z - r->pos.z) >> WALL_SHIFT;
        const int32_t x_floor = (item->pos.x - r->pos.x) >> WALL_SHIFT;
        item->box_num = r->floor[z_floor + r->x_size * x_floor].box;
        info->zone_num = zone[item->box_num];
    }

    {
        const ROOM_INFO *const r = &g_Rooms[enemy->room_num];
        const int32_t z_floor = (enemy->pos.z - r->pos.z) >> WALL_SHIFT;
        const int32_t x_floor = (enemy->pos.x - r->pos.x) >> WALL_SHIFT;
        enemy->box_num = r->floor[z_floor + r->x_size * x_floor].box;
        info->enemy_zone_num = zone[enemy->box_num];
    }

    if (((g_Boxes[enemy->box_num].overlap_index & creature->lot.block_mask)
         != 0)
        || (creature->lot.node[item->box_num].search_num
            == (creature->lot.search_num | BLOCKED_SEARCH))) {
        info->enemy_zone_num |= BLOCKED;
    }

    const struct OBJECT_INFO *const object = &g_Objects[item->object_num];
    const int32_t z = enemy->pos.z
        - ((object->pivot_length * Math_Cos(item->rot.y)) >> W2V_SHIFT)
        - item->pos.z;
    const int32_t x = enemy->pos.x
        - ((object->pivot_length * Math_Sin(item->rot.y)) >> W2V_SHIFT)
        - item->pos.x;
    PHD_ANGLE angle = Math_Atan(z, x);
    if (creature->enemy != NULL) {
        info->distance = SQUARE(x) + SQUARE(z);
    } else {
        info->distance = 0x7FFFFFFF;
    }

    info->angle = angle - item->rot.y;
    info->enemy_facing = angle - enemy->rot.y + PHD_180;
    info->ahead = info->angle > -FRONT_ARC && info->angle < FRONT_ARC;
    info->bite = info->ahead && enemy->hit_points > 0
        && ABS(enemy->pos.y - item->pos.y) <= STEP_L;
}
