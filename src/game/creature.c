#include "game/creature.h"

#include "game/box.h"
#include "game/items.h"
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
#define MAX_X_ROT (20 * PHD_DEGREE) // = 3640
#define MAX_TILT (3 * PHD_DEGREE) // = 546
#define MAX_HEAD_CHANGE (5 * PHD_DEGREE) // = 910
#define HEAD_ARC 0x3000 // = 12288

void __cdecl Creature_Initialise(const int16_t item_num)
{
    ITEM_INFO *const item = &g_Items[item_num];
    item->rot.y += (Random_GetControl() - PHD_90) >> 1;
    item->collidable = 1;
    item->data = 0;
}

int32_t __cdecl Creature_Activate(const int16_t item_num)
{
    ITEM_INFO *const item = &g_Items[item_num];
    if (item->status != IS_INVISIBLE) {
        return true;
    }

    if (!LOT_EnableBaddieAI(item_num, false)) {
        return false;
    }

    item->status = IS_ACTIVE;
    return true;
}

void __cdecl Creature_AIInfo(ITEM_INFO *const item, AI_INFO *const info)
{
    CREATURE_INFO *const creature = (CREATURE_INFO *)item->data;
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

    ITEM_INFO *enemy = creature->enemy;
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
            == (creature->lot.search_num | BOX_BLOCKED_SEARCH))) {
        info->enemy_zone_num |= BOX_BLOCKED;
    }

    const OBJECT_INFO *const object = &g_Objects[item->object_num];
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
    const ITEM_INFO *item, const AI_INFO *info, int32_t violent)
{
    CREATURE_INFO *const creature = item->data;
    if (creature == NULL) {
        return;
    }

    const LOT_INFO *const lot = &creature->lot;
    const ITEM_INFO *enemy = creature->enemy;
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

    const MOOD_TYPE mood = creature->mood;
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

int32_t __cdecl Creature_CheckBaddieOverlap(const int16_t item_num)
{
    ITEM_INFO *item = &g_Items[item_num];

    const int32_t x = item->pos.x;
    const int32_t y = item->pos.y;
    const int32_t z = item->pos.z;
    const int32_t radius = SQUARE(g_Objects[item->object_num].radius);

    int16_t link = g_Rooms[item->room_num].item_num;
    while (link != NO_ITEM && link != item_num) {
        item = &g_Items[link];
        if (item != g_LaraItem && item->status == IS_ACTIVE
            && item->speed != 0) {
            const int32_t distance =
                (SQUARE(item->pos.z - z) + SQUARE(item->pos.y - y)
                 + SQUARE(item->pos.x - x));
            if (distance < radius) {
                return true;
            }
        }

        link = item->next_item;
    }

    return false;
}

void __cdecl Creature_Die(const int16_t item_num, const bool explode)
{
    ITEM_INFO *item = &g_Items[item_num];
    item->collidable = 0;
    item->hit_points = DONT_TARGET;
    if (explode) {
        Effect_ExplodingDeath(item_num, -1, 0);
        Item_Kill(item_num);
    } else {
        Item_RemoveActive(item_num);
    }
    DisableBaddieAI(item_num);
    item->flags |= IF_ONE_SHOT;

    if (item->killed) {
        item->next_active = g_PrevItemActive;
        g_PrevItemActive = item_num;
    }

    int16_t pickup_num = item->carried_item;
    while (pickup_num != NO_ITEM) {
        ITEM_INFO *const pickup = &g_Items[pickup_num];
        pickup->pos = item->pos;
        Item_NewRoom(pickup_num, item->room_num);
        pickup_num = pickup->carried_item;
    }
}

int32_t __cdecl Creature_Animate(
    const int16_t item_num, const int16_t angle, const int16_t tilt)
{
    ITEM_INFO *item = &g_Items[item_num];
    CREATURE_INFO *const creature = item->data;
    if (creature == NULL) {
        return false;
    }

    const LOT_INFO *const lot = &creature->lot;
    const XYZ_32 old = item->pos;

    int16_t *zone;
    if (creature->lot.fly != 0) {
        zone = g_FlyZone[g_FlipStatus];
    } else {
        zone = g_GroundZone[creature->lot.step / STEP_L][g_FlipStatus];
    }

    if (!g_Objects[item->object_num].water_creature) {
        int16_t room_num = item->room_num;
        Room_GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_num);
        if (room_num != item->room_num) {
            Item_NewRoom(item_num, room_num);
        }
    }

    Item_Animate(item);
    if (item->status == IS_DEACTIVATED) {
        Creature_Die(item_num, false);
        return false;
    }

    const int16_t *const bounds = Item_GetBoundsAccurate(item);
    int32_t y = item->pos.y + bounds[2];

    int16_t room_num = item->room_num;
    const FLOOR_INFO *floor =
        Room_GetFloor(item->pos.x, y, item->pos.z, &room_num);
    int32_t height = g_Boxes[floor->box].height;
    int16_t next_box = lot->node[floor->box].exit_box;
    int32_t next_height =
        next_box != NO_BOX ? g_Boxes[next_box].height : height;

    const int32_t box_height = g_Boxes[item->box_num].height;
    if (floor->box == NO_BOX || zone[item->box_num] != zone[floor->box]
        || box_height - height > lot->step || box_height - height < lot->drop) {
        const int32_t pos_x = item->pos.x >> WALL_SHIFT;
        const int32_t shift_x = old.x >> WALL_SHIFT;
        const int32_t shift_z = old.z >> WALL_SHIFT;

        if (pos_x < shift_x) {
            item->pos.x = old.x & (~(WALL_L - 1));
        } else if (pos_x > shift_x) {
            item->pos.x = old.x | (WALL_L - 1);
        }

        if (pos_x < shift_z) {
            item->pos.z = old.z & (~(WALL_L - 1));
        } else if (pos_x > shift_z) {
            item->pos.z = old.z | (WALL_L - 1);
        }

        floor = Room_GetFloor(item->pos.x, y, item->pos.z, &room_num);
        height = g_Boxes[floor->box].height;
        next_box = lot->node[floor->box].exit_box;
        next_height = next_box != NO_BOX ? g_Boxes[next_box].height : height;
    }

    int32_t x = item->pos.x;
    int32_t z = item->pos.z;
    int32_t pos_x = x & (WALL_L - 1);
    int32_t pos_z = z & (WALL_L - 1);
    int32_t shift_x = 0;
    int32_t shift_z = 0;
    int32_t radius = g_Objects[item->object_num].radius;

    if (pos_z < radius) {
        if (Box_BadFloor(
                x, y, z - radius, height, next_height, room_num, lot)) {
            shift_z = radius - pos_z;
        }

        if (pos_x < radius) {
            if (Box_BadFloor(
                    x - radius, y, z, height, next_height, room_num, lot)) {
                shift_x = radius - pos_x;
            } else if (
                !shift_z
                && Box_BadFloor(
                    x - radius, y, z - radius, height, next_height, room_num,
                    lot)) {
                if (item->rot.y > -PHD_135 && item->rot.y < PHD_45) {
                    shift_z = radius - pos_z;
                } else {
                    shift_x = radius - pos_x;
                }
            }
        } else if (pos_x > WALL_L - radius) {
            if (Box_BadFloor(
                    x + radius, y, z, height, next_height, room_num, lot)) {
                shift_x = WALL_L - radius - pos_x;
            } else if (
                !shift_z
                && Box_BadFloor(
                    x + radius, y, z - radius, height, next_height, room_num,
                    lot)) {
                if (item->rot.y > -PHD_45 && item->rot.y < PHD_135) {
                    shift_z = radius - pos_z;
                } else {
                    shift_x = WALL_L - radius - pos_x;
                }
            }
        }
    } else if (pos_z > WALL_L - radius) {
        if (Box_BadFloor(
                x, y, z + radius, height, next_height, room_num, lot)) {
            shift_z = WALL_L - radius - pos_z;
        }

        if (pos_x < radius) {
            if (Box_BadFloor(
                    x - radius, y, z, height, next_height, room_num, lot)) {
                shift_x = radius - pos_x;
            } else if (
                !shift_z
                && Box_BadFloor(
                    x - radius, y, z + radius, height, next_height, room_num,
                    lot)) {
                if (item->rot.y > -PHD_45 && item->rot.y < PHD_135) {
                    shift_x = radius - pos_x;
                } else {
                    shift_z = WALL_L - radius - pos_z;
                }
            }
        } else if (pos_x > WALL_L - radius) {
            if (Box_BadFloor(
                    x + radius, y, z, height, next_height, room_num, lot)) {
                shift_x = WALL_L - radius - pos_x;
            } else if (
                !shift_z
                && Box_BadFloor(
                    x + radius, y, z + radius, height, next_height, room_num,
                    lot)) {
                if (item->rot.y > -PHD_135 && item->rot.y < PHD_45) {
                    shift_x = WALL_L - radius - pos_x;
                } else {
                    shift_z = WALL_L - radius - pos_z;
                }
            }
        }
    } else if (pos_x < radius) {
        if (Box_BadFloor(
                x - radius, y, z, height, next_height, room_num, lot)) {
            shift_x = radius - pos_x;
        }
    } else if (pos_x > WALL_L - radius) {
        if (Box_BadFloor(
                x + radius, y, z, height, next_height, room_num, lot)) {
            shift_x = WALL_L - radius - pos_x;
        }
    }

    item->pos.x += shift_x;
    item->pos.z += shift_z;

    if (shift_x || shift_z) {
        floor = Room_GetFloor(item->pos.x, y, item->pos.z, &room_num);
        item->rot.y += angle;
        if (tilt != 0) {
            Creature_Tilt(item, tilt * 2);
        }
    }

    if (Creature_CheckBaddieOverlap(item_num)) {
        item->pos = old;
        return true;
    }

    if (lot->fly != 0) {
        int32_t dy = creature->target.y - item->pos.y;
        CLAMP(dy, -lot->fly, lot->fly);

        height = Room_GetHeight(floor, item->pos.x, y, item->pos.z);
        if (item->pos.y + dy <= height) {
            const int32_t ceiling =
                Room_GetCeiling(floor, item->pos.x, y, item->pos.z);
            const int32_t min_y = item->object_num == O_SHARK ? 128 : bounds[2];
            if (item->pos.y + min_y + dy < ceiling) {
                if (item->pos.y + min_y < ceiling) {
                    item->pos.x = old.x;
                    item->pos.z = old.z;
                    dy = lot->fly;
                } else {
                    dy = 0;
                }
            }
        } else if (item->pos.y <= height) {
            item->pos.y = height;
            dy = 0;
        } else {
            item->pos.x = old.x;
            item->pos.z = old.z;
            dy = -lot->fly;
        }

        item->pos.y += dy;
        floor = Room_GetFloor(item->pos.x, y, item->pos.z, &room_num);
        item->floor = Room_GetHeight(floor, item->pos.x, y, item->pos.z);

        int16_t angle = item->speed != 0 ? Math_Atan(item->speed, -dy) : 0;
        CLAMP(angle, -MAX_X_ROT, MAX_X_ROT);

        if (angle < item->rot.x - PHD_DEGREE) {
            item->rot.x -= PHD_DEGREE;
        } else if (angle > item->rot.x + PHD_DEGREE) {
            item->rot.x += PHD_DEGREE;
        } else {
            item->rot.x = angle;
        }
    } else {
        const FLOOR_INFO *const floor =
            Room_GetFloor(item->pos.x, item->pos.y, item->pos.z, &room_num);
        item->floor =
            Room_GetHeight(floor, item->pos.x, item->pos.y, item->pos.z);

        if (item->pos.y > item->floor) {
            item->pos.y = item->floor;
        } else if (item->floor - item->pos.y > STEP_L / 4) {
            item->pos.y += STEP_L / 4;
        } else if (item->pos.y < item->floor) {
            item->pos.y = item->floor;
        }
        item->rot.x = 0;
    }

    if (!g_Objects[item->object_num].water_creature) {
        Room_GetFloor(
            item->pos.x, item->pos.y - WALL_L / 2, item->pos.z, &room_num);
        if ((g_Rooms[room_num].flags & RF_UNDERWATER) != 0) {
            item->hit_points = 0;
        }
    }

    if (item->room_num != room_num) {
        Item_NewRoom(item_num, room_num);
    }
    return true;
}

int16_t __cdecl Creature_Turn(ITEM_INFO *const item, int16_t max_turn)
{
    const CREATURE_INFO *const creature = item->data;
    if (creature == NULL || item->speed == 0 || max_turn == 0) {
        return 0;
    }

    const int32_t dx = creature->target.x - item->pos.x;
    const int32_t dz = creature->target.z - item->pos.z;

    int16_t angle = Math_Atan(dz, dx) - item->rot.y;
    if (angle > FRONT_ARC || angle < -FRONT_ARC) {
        const int32_t range = (item->speed << 14) / max_turn;
        if (SQUARE(dx) + SQUARE(dz) < SQUARE(range)) {
            max_turn /= 2;
        }
    }

    CLAMP(angle, -max_turn, max_turn);
    item->rot.y += angle;
    return angle;
}

void __cdecl Creature_Tilt(ITEM_INFO *const item, int16_t angle)
{
    angle = 4 * angle - item->rot.z;
    CLAMP(angle, -MAX_TILT, MAX_TILT);
    item->rot.z += angle;
}

void __cdecl Creature_Head(ITEM_INFO *item, int16_t required)
{
    CREATURE_INFO *const creature = item->data;
    if (creature == NULL) {
        return;
    }

    int16_t change = required - creature->head_rotation;
    CLAMP(change, -MAX_HEAD_CHANGE, MAX_HEAD_CHANGE);

    creature->head_rotation += change;
    CLAMP(creature->head_rotation, -HEAD_ARC, HEAD_ARC);
}

void __cdecl Creature_Neck(ITEM_INFO *const item, const int16_t required)
{
    CREATURE_INFO *const creature = item->data;
    if (creature == NULL) {
        return;
    }

    int16_t change = required - creature->neck_rotation;
    CLAMP(change, -MAX_HEAD_CHANGE, MAX_HEAD_CHANGE);

    creature->neck_rotation += change;
    CLAMP(creature->neck_rotation, -HEAD_ARC, HEAD_ARC);
}
