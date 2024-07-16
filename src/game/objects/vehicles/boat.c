#include "game/objects/vehicles/boat.h"

#include "game/effects.h"
#include "game/items.h"
#include "game/math.h"
#include "game/random.h"
#include "game/sound.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

#define BOAT_GETON_LW_ANIM 0
#define BOAT_GETON_RW_ANIM 8
#define BOAT_GETON_J_ANIM 6
#define BOAT_GETON_START 1
#define BOAT_SIDE 300
#define BOAT_RADIUS 500
#define BOAT_MAX_SPEED 90
#define BOAT_MAX_BACK (-20)
#define BOAT_ACCELERATION 5
#define BOAT_WAKE 700
#define BOAT_FRONT 750
#define BOAT_TIP (BOAT_FRONT + 250)

typedef enum {
    GONDOLA_EMPTY = 0,
    GONDOLA_FLOATING = 1,
    GONDOLA_CRASH = 2,
    GONDOLA_SINK = 3,
    GONDOLA_LAND = 4,
} GONDOLA_ANIM;

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

int32_t __cdecl Boat_TestWaterHeight(
    const ITEM_INFO *const item, const int32_t z_off, const int32_t x_off,
    XYZ_32 *const pos)
{
    // clang-format off
    pos->y = item->pos.y
        + ((x_off * Math_Sin(item->rot.z)) >> W2V_SHIFT)
        - ((z_off * Math_Sin(item->rot.x)) >> W2V_SHIFT);
    // clang-format on

    const int32_t c = Math_Cos(item->rot.y);
    const int32_t s = Math_Sin(item->rot.y);
    pos->x = item->pos.x + ((x_off * c + z_off * s) >> W2V_SHIFT);
    pos->z = item->pos.z + ((z_off * c - x_off * s) >> W2V_SHIFT);

    int16_t room_num = item->room_num;
    Room_GetFloor(pos->x, pos->y, pos->z, &room_num);
    int32_t height = Room_GetWaterHeight(pos->x, pos->y, pos->z, room_num);
    if (height == NO_HEIGHT) {
        FLOOR_INFO *floor = Room_GetFloor(pos->x, pos->y, pos->z, &room_num);
        height = Room_GetHeight(floor, pos->x, pos->y, pos->z);
        if (height != NO_HEIGHT) {
            return height;
        }
    }

    return height - 5;
}

void __cdecl Boat_DoShift(const int32_t boat_num)
{
    ITEM_INFO *const boat = &g_Items[boat_num];
    int16_t item_num = g_Rooms[boat->room_num].item_num;

    while (item_num != NO_ITEM) {
        ITEM_INFO *item = &g_Items[item_num];

        if (item->object_num == O_BOAT && item_num != boat_num
            && g_Lara.skidoo != item_num) {
            const int32_t dx = item->pos.x - boat->pos.x;
            const int32_t dz = item->pos.z - boat->pos.z;
            const int32_t dist = SQUARE(dx) + SQUARE(dz);

            if (dist < SQUARE(BOAT_RADIUS * 2)) {
                boat->pos.x = item->pos.x - SQUARE(BOAT_RADIUS * 2) * dx / dist;
                boat->pos.z = item->pos.z - SQUARE(BOAT_RADIUS * 2) * dz / dist;
            }
            break;
        }

        if (item->object_num == O_GONDOLA
            && item->current_anim_state == GONDOLA_FLOATING) {
            const int32_t c = Math_Cos(item->rot.y);
            const int32_t s = Math_Sin(item->rot.y);
            const int32_t ix = item->pos.x - ((s * STEP_L * 2) >> W2V_SHIFT);
            const int32_t iz = item->pos.z - ((c * STEP_L * 2) >> W2V_SHIFT);
            const int32_t dx = ix - boat->pos.x;
            const int32_t dz = iz - boat->pos.z;
            const int32_t dist = SQUARE(dx) + SQUARE(dz);

            if (dist < SQUARE(BOAT_RADIUS * 2)) {
                if (boat->speed < BOAT_MAX_SPEED - 10) {
                    boat->pos.x = ix - SQUARE(BOAT_RADIUS * 2) * dx / dist;
                    boat->pos.z = iz - SQUARE(BOAT_RADIUS * 2) * dz / dist;
                } else if (item->pos.y - boat->pos.y < WALL_L * 2) {
                    Sound_Effect(337, &item->pos, 0);
                    item->goal_anim_state = GONDOLA_CRASH;
                }
            }
        }

        item_num = item->next_item;
    }
}

void __cdecl Boat_DoWakeEffect(const ITEM_INFO *const boat)
{
    g_MatrixPtr->_23 = 0;
    S_CalculateLight(boat->pos.x, boat->pos.y, boat->pos.z, boat->room_num);

    const int16_t frame =
        (Random_GetDraw() * g_Objects[O_WATER_SPRITE].mesh_count) >> 15;

    for (int32_t i = 0; i < 3; i++) {
        const int16_t fx_num = Effect_Create(boat->room_num);
        if (fx_num == NO_ITEM) {
            continue;
        }

        FX_INFO *const fx = &g_Effects[fx_num];
        fx->object_num = O_WATER_SPRITE;
        fx->room_num = boat->room_num;
        fx->frame_num = frame;

        const int32_t c = Math_Cos(boat->rot.y);
        const int32_t s = Math_Sin(boat->rot.y);
        const int32_t w = (1 - i) * BOAT_SIDE;
        const int32_t h = BOAT_WAKE;
        fx->pos.x = boat->pos.x + ((-c * w - s * h) >> W2V_SHIFT);
        fx->pos.y = boat->pos.y;
        fx->pos.z = boat->pos.z + ((-c * h + s * w) >> W2V_SHIFT);
        fx->rot.y = boat->rot.y + (i << W2V_SHIFT) - 0x4000;

        fx->counter = 20;
        fx->speed = boat->speed >> 2;
        if (boat->speed < 64) {
            fx->fall_speed = (Random_GetDraw() * (ABS(boat->speed) - 64)) >> 15;
        } else {
            fx->fall_speed = 0;
        }

        fx->shade = g_LsAdder - 768;
        CLAMPL(fx->shade, 0);
    }
}

int32_t __cdecl Boat_DoDynamics(
    const int32_t height, int32_t fall_speed, int32_t *const y)
{
    if (height > *y) {
        *y = fall_speed + *y;
        if (*y > height) {
            *y = height;
            fall_speed = 0;
        } else {
            fall_speed += GRAVITY;
        }
    } else {
        fall_speed += ((height - fall_speed - *y) >> 3);
        CLAMPL(fall_speed, -20);
        CLAMPG(*y, height);
    }

    return fall_speed;
}

int32_t __cdecl Boat_Dynamics(const int16_t boat_num)
{
    ITEM_INFO *const boat = &g_Items[boat_num];
    BOAT_INFO *const boat_data = (BOAT_INFO *)boat->data;
    boat->rot.z -= boat_data->tilt_angle;

    XYZ_32 fl_old;
    XYZ_32 bl_old;
    XYZ_32 fr_old;
    XYZ_32 br_old;
    XYZ_32 f_old;
    const int32_t hfl_old =
        Boat_TestWaterHeight(boat, BOAT_FRONT, -BOAT_SIDE, &fl_old);
    const int32_t hfr_old =
        Boat_TestWaterHeight(boat, BOAT_FRONT, BOAT_SIDE, &fr_old);
    const int32_t hbl_old =
        Boat_TestWaterHeight(boat, -BOAT_FRONT, -BOAT_SIDE, &bl_old);
    const int32_t hbr_old =
        Boat_TestWaterHeight(boat, -BOAT_FRONT, BOAT_SIDE, &br_old);
    const int32_t hf_old = Boat_TestWaterHeight(boat, BOAT_TIP, 0, &f_old);
    XYZ_32 old = boat->pos;
    CLAMPG(bl_old.y, hbl_old);
    CLAMPG(br_old.y, hbr_old);
    CLAMPG(fl_old.y, hfl_old);
    CLAMPG(fr_old.y, hfr_old);
    CLAMPG(f_old.y, hf_old);

    boat->rot.y += boat_data->extra_rotation + boat_data->boat_turn;
    boat_data->tilt_angle = boat_data->boat_turn * 6;

    boat->pos.z += (boat->speed * Math_Cos(boat->rot.y)) >> W2V_SHIFT;
    boat->pos.x += (boat->speed * Math_Sin(boat->rot.y)) >> W2V_SHIFT;

    int32_t slip = (Math_Sin(boat->rot.z) * 30) >> W2V_SHIFT;
    if (!slip && boat->rot.z) {
        slip = boat->rot.z > 0 ? 1 : -1;
    }
    boat->pos.z -= (slip * Math_Sin(boat->rot.y)) >> W2V_SHIFT;
    boat->pos.x += (slip * Math_Cos(boat->rot.y)) >> W2V_SHIFT;

    slip = (Math_Sin(boat->rot.x) * 10) >> W2V_SHIFT;
    if (!slip && boat->rot.x) {
        slip = boat->rot.x > 0 ? 1 : -1;
    }

    boat->pos.z -= (slip * Math_Cos(boat->rot.y)) >> W2V_SHIFT;
    boat->pos.x = boat->pos.x - ((slip * Math_Sin(boat->rot.y)) >> W2V_SHIFT);

    XYZ_32 moved = {
        .x = boat->pos.x,
        .y = 0,
        .z = boat->pos.z,
    };
    Boat_DoShift(boat_num);

    int32_t rot = 0;

    XYZ_32 bl;
    const int32_t hbl =
        Boat_TestWaterHeight(boat, -BOAT_FRONT, -BOAT_SIDE, &bl);
    if (hbl < bl_old.y - STEP_L / 2) {
        rot = DoShift(boat, &bl, &bl_old);
    }

    XYZ_32 br;
    const int32_t hbr = Boat_TestWaterHeight(boat, -BOAT_FRONT, BOAT_SIDE, &br);
    if (hbr < br_old.y - STEP_L / 2) {
        rot += DoShift(boat, &br, &br_old);
    }

    XYZ_32 fl;
    const int32_t hfl = Boat_TestWaterHeight(boat, BOAT_FRONT, -BOAT_SIDE, &fl);
    if (hfl < fl_old.y - STEP_L / 2) {
        rot += DoShift(boat, &fl, &fl_old);
    }

    XYZ_32 fr;
    const int32_t hfr = Boat_TestWaterHeight(boat, BOAT_FRONT, BOAT_SIDE, &fr);
    if (hfr < fr_old.y - STEP_L / 2) {
        rot += DoShift(boat, &fr, &fr_old);
    }

    if (!slip) {
        XYZ_32 f;
        const int32_t hf = Boat_TestWaterHeight(boat, BOAT_TIP, 0, &f);
        if (hf < f_old.y - STEP_L / 2) {
            DoShift(boat, &f, &f_old);
        }
    }

    int16_t room_num = boat->room_num;
    const FLOOR_INFO *const floor =
        Room_GetFloor(boat->pos.x, boat->pos.y, boat->pos.z, &room_num);
    int32_t height =
        Room_GetWaterHeight(boat->pos.x, boat->pos.y, boat->pos.z, room_num);
    if (height == NO_HEIGHT) {
        height = Room_GetHeight(floor, boat->pos.x, boat->pos.y, boat->pos.z);
    }
    if (height < boat->pos.y - STEP_L / 2) {
        DoShift(boat, &boat->pos, &old);
    }

    boat_data->extra_rotation = rot;

    const int32_t collide = GetCollisionAnim(boat, &moved);
    if (slip || collide) {
        // clang-format off
        const int32_t new_speed = (
            (boat->pos.z - old.z) * Math_Cos(boat->rot.y) +
            (boat->pos.x - old.x) * Math_Sin(boat->rot.y)
        ) >> W2V_SHIFT;
        // clang-format on

        if (g_Lara.skidoo == boat_num) {
            if (boat->speed > BOAT_MAX_SPEED + BOAT_ACCELERATION
                && new_speed < boat->speed - 10) {
                g_LaraItem->hit_points -= (boat->speed - new_speed) / 2;
                g_LaraItem->hit_status = 1;
                Sound_Effect(31, &g_LaraItem->pos, 0);
            }
        }

        if (slip) {
            if (boat->speed <= BOAT_MAX_SPEED + 10) {
                boat->speed = new_speed;
            }
        } else {
            if (boat->speed > 0 && new_speed < boat->speed) {
                boat->speed = new_speed;
            } else if (boat->speed < 0 && new_speed > boat->speed) {
                boat->speed = new_speed;
            }
        }

        CLAMPL(boat->speed, BOAT_MAX_BACK);
    }

    return collide;
}
