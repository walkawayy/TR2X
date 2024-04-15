#include "game/creature.h"

#include "game/box.h"
#include "game/math.h"
#include "game/random.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#define FRONT_ARC PHD_90
#define ESCAPE_CHANCE 2048
#define RECOVER_CHANCE 256
#define ATTACK_RANGE SQUARE(WALL_L * 3) // = 9437184

void __cdecl Creature_Initialise(const int16_t item_num)
{
    struct ITEM_INFO *const item = &g_Items[item_num];
    item->rot.y += (Random_GetControl() - PHD_90) >> 1;
    item->collidable = 1;
    item->data = 0;
}

int32_t __cdecl Creature_Activate(const int16_t item_num)
{
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
    struct CREATURE_INFO *const creature = (struct CREATURE_INFO *)item->data;
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
        const struct ROOM_INFO *const r = &g_Rooms[item->room_num];
        const int32_t z_floor = (item->pos.z - r->pos.z) >> WALL_SHIFT;
        const int32_t x_floor = (item->pos.x - r->pos.x) >> WALL_SHIFT;
        item->box_num = r->floor[z_floor + r->x_size * x_floor].box;
        info->zone_num = zone[item->box_num];
    }

    {
        const struct ROOM_INFO *const r = &g_Rooms[enemy->room_num];
        const int32_t z_floor = (enemy->pos.z - r->pos.z) >> WALL_SHIFT;
        const int32_t x_floor = (enemy->pos.x - r->pos.x) >> WALL_SHIFT;
        enemy->box_num = r->floor[z_floor + r->x_size * x_floor].box;
        info->enemy_zone_num = zone[enemy->box_num];
    }

    if (((g_Boxes[enemy->box_num].overlap_index & creature->lot.block_mask)
         != 0)
        || (creature->lot.node[item->box_num].search_num
            == (creature->lot.search_num | BOX_BLOCKED_SEARCH))) {
        info->enemy_zone_num |= BOX_BLOCKED;
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

void __cdecl Creature_Mood(
    const struct ITEM_INFO *item, const struct AI_INFO *info, int32_t violent)
{
    struct CREATURE_INFO *const creature = item->data;
    if (creature == NULL) {
        return;
    }

    const struct LOT_INFO *const lot = &creature->lot;
    const struct ITEM_INFO *enemy = creature->enemy;
    if (creature->lot.node[item->box_num].search_num
        == (creature->lot.search_num | 0x8000)) {
        creature->lot.required_box = NO_BOX;
    }
    if (creature->mood != MOOD_ATTACK && creature->lot.required_box != NO_BOX
        && !Box_ValidBox(item, info->zone_num, creature->lot.target_box)) {
        if (info->zone_num == info->enemy_zone_num) {
            creature->mood = MOOD_BORED;
        }
        creature->lot.required_box = NO_BOX;
    }

    const enum MOOD_TYPE mood = creature->mood;
    if (enemy == NULL) {
        creature->mood = MOOD_BORED;
        enemy = g_LaraItem;
    } else if (enemy->hit_points <= 0) {
        creature->mood = MOOD_BORED;
    } else if (violent) {
        switch (mood) {
        case MOOD_BORED:
        case MOOD_STALK:
            if (info->zone_num == info->enemy_zone_num) {
                creature->mood = MOOD_ATTACK;
            } else if (item->hit_status != 0) {
                creature->mood = MOOD_ESCAPE;
            }
            break;

        case MOOD_ATTACK:
            if (info->zone_num != info->enemy_zone_num) {
                creature->mood = MOOD_BORED;
            }
            break;

        case MOOD_ESCAPE:
            if (info->zone_num == info->enemy_zone_num) {
                creature->mood = MOOD_ATTACK;
            }
            break;
        }
    } else {
        switch (mood) {
        case MOOD_BORED:
        case MOOD_STALK:
            if (item->hit_status != 0
                && (Random_GetControl() < ESCAPE_CHANCE
                    || info->zone_num != info->enemy_zone_num)) {
                creature->mood = MOOD_ESCAPE;
            } else if (info->zone_num == info->enemy_zone_num) {
                if (info->distance < ATTACK_RANGE
                    || (creature->mood == MOOD_STALK
                        && creature->lot.required_box == NO_BOX)) {
                    creature->mood = MOOD_ATTACK;
                } else {
                    creature->mood = MOOD_STALK;
                }
            }
            break;

        case MOOD_ATTACK:
            if (item->hit_status != 0
                && (Random_GetControl() < ESCAPE_CHANCE
                    || info->zone_num != info->enemy_zone_num)) {
                creature->mood = MOOD_ESCAPE;
            } else if (info->zone_num != info->enemy_zone_num) {
                creature->mood = MOOD_BORED;
            }
            break;

        case MOOD_ESCAPE:
            if (info->zone_num == info->enemy_zone_num
                && Random_GetControl() < RECOVER_CHANCE) {
                creature->mood = MOOD_STALK;
            }
            break;
        }
    }

    if (mood != creature->mood) {
        if (mood == MOOD_ATTACK) {
            Box_TargetBox(&creature->lot, creature->lot.target_box);
        }
        creature->lot.required_box = NO_BOX;
    }

    switch (creature->mood) {
    case MOOD_BORED: {
        const int16_t box_num =
            lot->node[(creature->lot.zone_count * Random_GetControl()) >> 15]
                .box_num;
        if (Box_ValidBox(item, info->zone_num, box_num)) {
            if (Box_StalkBox(item, enemy, box_num) && creature->enemy != NULL
                && enemy->hit_points > 0) {
                Box_TargetBox(&creature->lot, box_num);
                creature->mood = MOOD_STALK;
            } else if (creature->lot.required_box == NO_BOX) {
                Box_TargetBox(&creature->lot, box_num);
            }
        }
        break;
    }

    case MOOD_ATTACK:
        creature->lot.target = enemy->pos;
        creature->lot.required_box = enemy->box_num;
        if (creature->lot.fly != 0 && g_Lara.water_status == LWS_ABOVE_WATER) {
            creature->lot.target.y += Item_GetBestFrame(enemy)[2];
        }
        break;

    case MOOD_ESCAPE: {
        const int16_t box_num =
            lot->node[(creature->lot.zone_count * Random_GetControl()) >> 15]
                .box_num;
        if (Box_ValidBox(item, info->zone_num, box_num)
            && creature->lot.required_box == NO_BOX) {
            if (Box_EscapeBox(item, enemy, box_num)) {
                Box_TargetBox(&creature->lot, box_num);
            } else if (
                info->zone_num == info->enemy_zone_num
                && Box_StalkBox(item, enemy, box_num)) {
                Box_TargetBox(&creature->lot, box_num);
                creature->mood = MOOD_STALK;
            }
        }
        break;
    }

    case MOOD_STALK: {
        int16_t box_num = creature->lot.required_box;
        if (box_num == NO_BOX || !Box_StalkBox(item, enemy, box_num)) {
            box_num =
                lot->node
                    [(creature->lot.zone_count * Random_GetControl()) >> 15]
                        .box_num;
            if (Box_ValidBox(item, info->zone_num, box_num)) {
                if (Box_StalkBox(item, enemy, box_num)) {
                    Box_TargetBox(&creature->lot, box_num);
                } else if (creature->lot.required_box == NO_BOX) {
                    Box_TargetBox(&creature->lot, box_num);
                    if (info->zone_num != info->enemy_zone_num) {
                        creature->mood = MOOD_BORED;
                    }
                }
            }
        }
        break;
    }
    }

    if (creature->lot.target_box == NO_BOX) {
        Box_TargetBox(&creature->lot, item->box_num);
    }
    Box_CalculateTarget(&creature->target, item, &creature->lot);
}
