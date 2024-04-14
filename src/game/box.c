#include "game/box.h"

#include "game/random.h"
#include "global/const.h"
#include "global/vars.h"
#include "util.h"

#define BOX_OVERLAP_INDEX 0x3FFF
#define BOX_SEARCH_NUM 0x7FFF
#define BOX_END_BIT 0x8000
#define BOX_NUM_BITS (~BOX_END_BIT) // = 0x7FFF
#define BOX_STALK_DIST 3 // tiles
#define BOX_ESCAPE_DIST 5 // tiles

int32_t __cdecl Box_SearchLOT(
    struct LOT_INFO *const lot, const int32_t expansion)
{
    int16_t *zone;
    if (lot->fly) {
        zone = g_FlyZone[g_FlipStatus];
    } else if (lot->step == STEP_L) {
        zone = g_GroundZone[lot->step >> 8][g_FlipStatus];
    }

    const int16_t search_zone = zone[lot->head];
    for (int i = 0; i < expansion; i++) {
        if (lot->head == NO_BOX) {
            lot->tail = NO_BOX;
            return false;
        }

        BOX_NODE *node = &lot->node[lot->head];
        const struct BOX_INFO *box = &g_Boxes[lot->head];

        bool done = false;
        int32_t index = box->overlap_index & BOX_OVERLAP_INDEX;
        while (!done) {
            int16_t box_num = g_Overlap[index++];
            if ((box_num & BOX_END_BIT) != 0) {
                done = true;
                box_num &= BOX_NUM_BITS;
            }

            if (search_zone != zone[box_num]) {
                continue;
            }

            const int32_t change = g_Boxes[box_num].height - box->height;
            if (change > lot->step || change < lot->drop) {
                continue;
            }

            struct BOX_NODE *const expand = &lot->node[box_num];
            if ((node->search_num & BOX_SEARCH_NUM)
                < (expand->search_num & BOX_SEARCH_NUM)) {
                continue;
            }

            if ((node->search_num & BOX_BLOCKED_SEARCH) != 0) {
                if ((node->search_num & BOX_SEARCH_NUM)
                    == (expand->search_num & BOX_SEARCH_NUM)) {
                    continue;
                }
                expand->search_num = node->search_num;
            } else {
                if ((node->search_num & BOX_SEARCH_NUM)
                        == (expand->search_num & BOX_SEARCH_NUM)
                    && (expand->search_num & BOX_BLOCKED_SEARCH) == 0) {
                    continue;
                }

                if ((g_Boxes[box_num].overlap_index & lot->block_mask) != 0) {
                    expand->search_num = node->search_num | BOX_BLOCKED_SEARCH;
                } else {
                    expand->search_num = node->search_num;
                    expand->exit_box = lot->head;
                }
            }

            if (expand->next_expansion == NO_BOX && box_num != lot->tail) {
                lot->node[lot->tail].next_expansion = box_num;
                lot->tail = box_num;
            }
        }

        lot->head = node->next_expansion;
        node->next_expansion = NO_BOX;
    }

    return true;
}

int32_t __cdecl Box_UpdateLOT(
    struct LOT_INFO *const lot, const int32_t expansion)
{
    if (lot->required_box == NO_BOX || lot->required_box == lot->target_box) {
        goto end;
    }

    lot->target_box = lot->required_box;
    struct BOX_NODE *const expand = &lot->node[lot->target_box];
    if (expand->next_expansion == NO_BOX && lot->tail != lot->target_box) {
        expand->next_expansion = lot->head;
        if (lot->head == NO_BOX) {
            lot->tail = lot->target_box;
        }
        lot->head = lot->target_box;
    }
    lot->search_num++;
    expand->search_num = lot->search_num;
    expand->exit_box = NO_BOX;

end:
    return Box_SearchLOT(lot, expansion);
}

void __cdecl Box_TargetBox(struct LOT_INFO *const lot, const int16_t box_num)
{
    const struct BOX_INFO *const box = &g_Boxes[box_num & BOX_NUM_BITS];

    lot->target.z = ((box->right - box->left - 1) >> (15 - WALL_SHIFT))
            * Random_GetControl()
        + (box->left << WALL_SHIFT) + WALL_L / 2;
    lot->target.x = ((box->bottom - box->top - 1) >> (15 - WALL_SHIFT))
            * Random_GetControl()
        + (box->top << WALL_SHIFT) + WALL_L / 2;
    lot->required_box = box_num & BOX_NUM_BITS;

    if (lot->fly != 0) {
        lot->target.y = box->height - STEP_L * 3 / 2;
    } else {
        lot->target.y = box->height;
    }
}

int32_t __cdecl Box_StalkBox(
    const struct ITEM_INFO *const item, const struct ITEM_INFO *const enemy,
    const int16_t box_num)
{
    const struct BOX_INFO *const box = &g_Boxes[box_num];

    const int32_t z =
        ((box->left + box->right) << (WALL_SHIFT - 1)) - enemy->pos.z;
    const int32_t x =
        ((box->top + box->bottom) << (WALL_SHIFT - 1)) - enemy->pos.x;

    const int32_t x_range = (box->bottom - box->top + BOX_STALK_DIST)
        << WALL_SHIFT;
    const int32_t z_range = (box->right - box->left + BOX_STALK_DIST)
        << WALL_SHIFT;
    if (x > x_range || x < -x_range || z > z_range || z < -z_range) {
        return false;
    }

    const int32_t enemy_quad = (enemy->rot.y >> 14) + 2;
    const int32_t box_quad = (z > 0) ? ((x > 0) ? 2 : 1) : ((x > 0) ? 3 : 0);
    if (enemy_quad == box_quad) {
        return false;
    }

    const int32_t baddie_quad = item->pos.z > enemy->pos.z
        ? (item->pos.x > x ? 2 : 1)
        : (item->pos.x > x ? 3 : 0);

    return enemy_quad == baddie_quad && ABS(enemy_quad - box_quad) == 2;
}

int32_t __cdecl Box_EscapeBox(
    const struct ITEM_INFO *const item, const struct ITEM_INFO *const enemy,
    const int16_t box_num)
{
    const struct BOX_INFO *const box = &g_Boxes[box_num];
    const int32_t x =
        ((box->bottom + box->top) << (WALL_SHIFT - 1)) - enemy->pos.x;
    const int32_t z =
        ((box->left + box->right) << (WALL_SHIFT - 1)) - enemy->pos.z;

    const int32_t x_range = BOX_ESCAPE_DIST << WALL_SHIFT;
    const int32_t z_range = BOX_ESCAPE_DIST << WALL_SHIFT;
    if (x > -x_range && x < x_range && z > -z_range && z < z_range) {
        return false;
    }

    return ((z > 0) == (item->pos.z > enemy->pos.z))
        || ((x > 0) == (item->pos.x > enemy->pos.x));
}
