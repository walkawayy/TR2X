#include "game/objects/vehicles/boat.h"

#include "game/items.h"
#include "game/math.h"
#include "global/funcs.h"
#include "global/vars.h"

#define BOAT_GETON_LW_ANIM 0
#define BOAT_GETON_RW_ANIM 8
#define BOAT_GETON_J_ANIM 6
#define BOAT_GETON_START 1

void __cdecl Boat_Initialise(const int16_t item_num)
{
    BOAT_INFO *boat_data = game_malloc(sizeof(BOAT_INFO), GBUF_TEMP_ALLOC);
    boat_data->boat_turn = 0;
    boat_data->left_fallspeed = 0;
    boat_data->right_fallspeed = 0;
    boat_data->tilt_angle = 0;
    boat_data->extra_rotation = 0;
    boat_data->water = 0;
    boat_data->pitch = 0;

    ITEM_INFO *const boat = &g_Items[item_num];
    boat->data = boat_data;
}

int32_t __cdecl Boat_CheckGeton(
    const int16_t item_num, const COLL_INFO *const coll)
{
    if (g_Lara.gun_status != LGS_ARMLESS) {
        return 0;
    }

    ITEM_INFO *const boat = &g_Items[item_num];
    const ITEM_INFO *const lara = g_LaraItem;
    const int32_t dist =
        ((lara->pos.z - boat->pos.z) * Math_Cos(-boat->rot.y)
         - (lara->pos.x - boat->pos.x) * Math_Sin(-boat->rot.y))
        >> W2V_SHIFT;

    if (dist > 200) {
        return 0;
    }

    int32_t geton = 0;
    const int16_t rot = boat->rot.y - lara->rot.y;

    if (g_Lara.water_status == LWS_SURFACE || g_Lara.water_status == LWS_WADE) {
        if ((g_Input & IN_ACTION) == 0 || lara->gravity || boat->speed) {
            return 0;
        }

        if (rot > PHD_45 && rot < PHD_135) {
            geton = 1;
        } else if (rot > -PHD_135 && rot < -PHD_45) {
            geton = 2;
        }
    } else if (g_Lara.water_status == LWS_ABOVE_WATER) {
        int16_t fall_speed = lara->fall_speed;
        if (fall_speed > 0) {
            if (rot > -PHD_135 && rot < PHD_135 && lara->pos.y > boat->pos.y) {
                geton = 3;
            }
        } else if (!fall_speed && rot > -PHD_135 && rot < PHD_135) {
            if (lara->pos.x == boat->pos.x && lara->pos.y == boat->pos.y
                && lara->pos.z == boat->pos.z) {
                geton = 4;
            } else {
                geton = 3;
            }
        }
    }

    if (!geton) {
        return 0;
    }

    if (!Item_TestBoundsCollide(boat, lara, coll->radius)) {
        return 0;
    }

    if (!TestCollision(boat, lara)) {
        return 0;
    }

    return geton;
}

void __cdecl Boat_Collision(
    const int16_t item_num, ITEM_INFO *const lara, COLL_INFO *const coll)
{
    if (lara->hit_points < 0 || g_Lara.skidoo != NO_ITEM) {
        return;
    }

    const int32_t geton = Boat_CheckGeton(item_num, coll);
    if (!geton) {
        coll->enable_baddie_push = 1;
        Object_Collision(item_num, lara, coll);
        return;
    }

    g_Lara.skidoo = item_num;

    switch (geton) {
    case 1:
        lara->anim_num = g_Objects[O_LARA_BOAT].anim_idx + BOAT_GETON_RW_ANIM;
        break;
    case 2:
        lara->anim_num = g_Objects[O_LARA_BOAT].anim_idx + BOAT_GETON_LW_ANIM;
        break;
    case 3:
        lara->anim_num = g_Objects[O_LARA_BOAT].anim_idx + BOAT_GETON_J_ANIM;
        break;
    default:
        lara->anim_num = g_Objects[O_LARA_BOAT].anim_idx + BOAT_GETON_START;
        break;
    }

    g_Lara.water_status = LWS_ABOVE_WATER;

    ITEM_INFO *const boat = &g_Items[item_num];

    lara->pos.x = boat->pos.x;
    lara->pos.y = boat->pos.y - 5;
    lara->pos.z = boat->pos.z;
    lara->gravity = 0;
    lara->rot.x = 0;
    lara->rot.y = boat->rot.y;
    lara->rot.z = 0;
    lara->speed = 0;
    lara->fall_speed = 0;
    lara->goal_anim_state = 0;
    lara->current_anim_state = 0;
    lara->frame_num = g_Anims[lara->anim_num].frame_base;

    if (lara->room_num != boat->room_num) {
        Item_NewRoom(g_Lara.item_num, boat->room_num);
    }

    Item_Animate(lara);
    if (boat->status != IS_ACTIVE) {
        Item_AddActive(item_num);
        boat->status = IS_ACTIVE;
    }
}
