#include "game/lara/lara_control.h"

#include "game/items.h"
#include "game/lara/lara_look.h"
#include "game/lara/lara_misc.h"
#include "game/math.h"
#include "game/music.h"
#include "game/room.h"
#include "game/sound.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/vars.h"
#include "util.h"

void __cdecl Lara_HandleAboveWater(ITEM_INFO *const item, COLL_INFO *const coll)
{
    coll->old.x = item->pos.x;
    coll->old.y = item->pos.y;
    coll->old.z = item->pos.z;
    coll->old_anim_state = item->current_anim_state;
    coll->old_anim_num = item->anim_num;
    coll->old_frame_num = item->frame_num;
    coll->radius = LARA_RADIUS;
    coll->trigger = NULL;

    coll->slopes_are_walls = 0;
    coll->slopes_are_pits = 0;
    coll->lava_is_pit = 0;
    coll->enable_baddie_push = 1;
    coll->enable_spaz = 1;

    if ((g_Input & IN_LOOK) && !g_Lara.extra_anim && g_Lara.look) {
        Lara_LookLeftRight();
    } else {
        Lara_ResetLook();
    }
    g_Lara.look = 1;

    if (g_Lara.skidoo != NO_ITEM) {
        if (g_Items[g_Lara.skidoo].object_num == O_SKIDOO_FAST) {
            if (SkidooControl()) {
                return;
            }
        } else {
            LaraGun();
            return;
        }
    }

    if (g_Lara.extra_anim) {
        g_ExtraControlRoutines[item->current_anim_state](item, coll);
    } else {
        g_LaraControlRoutines[item->current_anim_state](item, coll);
    }

    if (item->rot.z < -LARA_LEAN_UNDO) {
        item->rot.z += LARA_LEAN_UNDO;
    } else if (item->rot.z > LARA_LEAN_UNDO) {
        item->rot.z -= LARA_LEAN_UNDO;
    } else {
        item->rot.z = 0;
    }

    if (g_Lara.turn_rate < -LARA_TURN_UNDO) {
        g_Lara.turn_rate += LARA_TURN_UNDO;
    } else if (g_Lara.turn_rate > LARA_TURN_UNDO) {
        g_Lara.turn_rate -= LARA_TURN_UNDO;
    } else {
        g_Lara.turn_rate = 0;
    }
    item->rot.y += g_Lara.turn_rate;

    Lara_Animate(item);

    if (!g_Lara.extra_anim) {
        Lara_BaddieCollision(item, coll);
        if (g_Lara.skidoo == NO_ITEM) {
            g_LaraCollisionRoutines[item->current_anim_state](item, coll);
        }
    }

    Item_UpdateRoom(item, -LARA_HEIGHT / 2);
    LaraGun();
    Room_TestTriggers(coll->trigger, 0);
}

void __cdecl Lara_HandleSurface(ITEM_INFO *const item, COLL_INFO *const coll)
{
    g_Camera.target_elevation = -22 * PHD_DEGREE;

    coll->old.x = item->pos.x;
    coll->old.y = item->pos.y;
    coll->old.z = item->pos.z;
    coll->radius = LARA_RADIUS;
    coll->trigger = NULL;

    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -STEP_L / 2;
    coll->bad_ceiling = 100;

    coll->slopes_are_walls = 0;
    coll->slopes_are_pits = 0;
    coll->lava_is_pit = 0;
    coll->enable_baddie_push = 0;
    coll->enable_spaz = 0;

    if ((g_Input & IN_LOOK) && g_Lara.look) {
        Lara_LookLeftRight();
    } else {
        Lara_ResetLook();
    }
    g_Lara.look = 1;

    g_LaraControlRoutines[item->current_anim_state](item, coll);

    if (item->rot.z > LARA_LEAN_UNDO_SURF) {
        item->rot.z -= LARA_LEAN_UNDO_SURF;
    } else if (item->rot.z < -LARA_LEAN_UNDO_SURF) {
        item->rot.z += LARA_LEAN_UNDO_SURF;
    } else {
        item->rot.z = 0;
    }

    if (g_Lara.current_active && g_Lara.water_status != LWS_CHEAT) {
        Lara_WaterCurrent(coll);
    }

    Lara_Animate(item);
    item->pos.x +=
        (item->fall_speed * Math_Sin(g_Lara.move_angle)) >> (W2V_SHIFT + 2);
    item->pos.z +=
        (item->fall_speed * Math_Cos(g_Lara.move_angle)) >> (W2V_SHIFT + 2);

    Lara_BaddieCollision(item, coll);

    if (g_Lara.skidoo == NO_ITEM) {
        g_LaraCollisionRoutines[item->current_anim_state](item, coll);
    }

    Item_UpdateRoom(item, 100);
    LaraGun();
    Room_TestTriggers(coll->trigger, 0);
}

void __cdecl Lara_HandleUnderwater(ITEM_INFO *const item, COLL_INFO *const coll)
{
    coll->old.x = item->pos.x;
    coll->old.y = item->pos.y;
    coll->old.z = item->pos.z;
    coll->radius = LARA_RADIUS_UW;
    coll->trigger = NULL;

    coll->bad_pos = NO_BAD_POS;
    coll->bad_neg = -LARA_HEIGHT_UW;
    coll->bad_ceiling = LARA_HEIGHT_UW;

    coll->slopes_are_walls = 0;
    coll->slopes_are_pits = 0;
    coll->lava_is_pit = 0;
    coll->enable_baddie_push = 1;
    coll->enable_spaz = 0;

    if ((g_Input & IN_LOOK) && g_Lara.look) {
        Lara_LookLeftRight();
    } else {
        Lara_ResetLook();
    }
    g_Lara.look = 1;

    if (g_Lara.extra_anim) {
        g_ExtraControlRoutines[item->current_anim_state](item, coll);
    } else {
        g_LaraControlRoutines[item->current_anim_state](item, coll);
    }

    if (item->rot.z > LARA_LEAN_UNDO_UW) {
        item->rot.z -= LARA_LEAN_UNDO_UW;
    } else if (item->rot.z < -LARA_LEAN_UNDO_UW) {
        item->rot.z += LARA_LEAN_UNDO_UW;
    } else {
        item->rot.z = 0;
    }

    CLAMP(item->rot.x, -85 * PHD_DEGREE, 85 * PHD_DEGREE);
    CLAMP(item->rot.z, -LARA_LEAN_MAX_UW, LARA_LEAN_MAX_UW);

    if (g_Lara.turn_rate < -LARA_TURN_UNDO) {
        g_Lara.turn_rate += LARA_TURN_UNDO;
    } else if (g_Lara.turn_rate > LARA_TURN_UNDO) {
        g_Lara.turn_rate -= LARA_TURN_UNDO;
    } else {
        g_Lara.turn_rate = 0;
    }

    item->rot.y += g_Lara.turn_rate;
    if (g_Lara.current_active && g_Lara.water_status != LWS_CHEAT) {
        Lara_WaterCurrent(coll);
    }

    Lara_Animate(item);
    item->pos.y -=
        (item->fall_speed * Math_Sin(item->rot.x)) >> (W2V_SHIFT + 2);
    item->pos.x +=
        (Math_Cos(item->rot.x)
         * ((item->fall_speed * Math_Sin(item->rot.y)) >> (W2V_SHIFT + 2)))
        >> W2V_SHIFT;
    item->pos.z +=
        (Math_Cos(item->rot.x)
         * ((item->fall_speed * Math_Cos(item->rot.y)) >> (W2V_SHIFT + 2)))
        >> W2V_SHIFT;

    if (g_Lara.water_status != LWS_CHEAT && !g_Lara.extra_anim) {
        Lara_BaddieCollision(item, coll);
    }

    if (!g_Lara.extra_anim) {
        g_LaraCollisionRoutines[item->current_anim_state](item, coll);
    }

    Item_UpdateRoom(item, 0);
    LaraGun();
    Room_TestTriggers(coll->trigger, 0);
}

void __cdecl Lara_Control(const int16_t item_num)
{
    ITEM_INFO *const item = g_LaraItem;

    const bool room_submerged = g_Rooms[item->room_num].flags & RF_UNDERWATER;
    const int32_t water_depth = Lara_GetWaterDepth(
        item->pos.x, item->pos.y, item->pos.z, item->room_num);
    const int32_t water_height = Room_GetWaterHeight(
        item->pos.x, item->pos.y, item->pos.z, item->room_num);
    const int32_t water_height_diff =
        water_height == NO_HEIGHT ? NO_HEIGHT : item->pos.y - water_height;

    g_Lara.water_surface_dist = -water_height_diff;

    if (g_Lara.skidoo == NO_ITEM && !g_Lara.extra_anim) {
        switch (g_Lara.water_status) {
        case LWS_ABOVE_WATER:
            if (water_height_diff == NO_HEIGHT
                || water_height_diff < LARA_WADE_DEPTH) {
                break;
            }

            if (water_depth <= LARA_SWIM_DEPTH - STEP_L) {
                if (water_height_diff > LARA_WADE_DEPTH) {
                    g_Lara.water_status = LWS_WADE;
                    if (!item->gravity) {
                        item->goal_anim_state = LS_STOP;
                    }
                }
            } else if (room_submerged) {
                g_Lara.air = LARA_MAX_AIR;
                g_Lara.water_status = LWS_UNDERWATER;
                item->gravity = 0;
                item->pos.y += 100;
                Item_UpdateRoom(item, 0);
                Sound_StopEffect(SFX_LARA_FALL);
                if (item->current_anim_state == LS_SWAN_DIVE) {
                    item->rot.x = -45 * PHD_DEGREE;
                    item->goal_anim_state = LS_DIVE;
                    Lara_Animate(item);
                    item->fall_speed *= 2;
                } else if (item->current_anim_state == LS_FAST_DIVE) {
                    item->rot.x = -85 * PHD_DEGREE;
                    item->goal_anim_state = LS_DIVE;
                    Lara_Animate(item);
                    item->fall_speed *= 2;
                } else {
                    item->rot.x = -45 * PHD_DEGREE;
                    item->anim_num = LA_FREEFALL_TO_UNDERWATER;
                    item->frame_num = g_Anims[item->anim_num].frame_base;
                    item->current_anim_state = LS_DIVE;
                    item->goal_anim_state = LS_SWIM;
                    item->fall_speed = item->fall_speed * 3 / 2;
                }
                g_Lara.torso_y_rot = 0;
                g_Lara.torso_x_rot = 0;
                g_Lara.head_y_rot = 0;
                g_Lara.head_x_rot = 0;
                Splash(item);
            }
            break;

        case LWS_UNDERWATER:
            if (room_submerged) {
                break;
            }

            if (water_depth == NO_HEIGHT || ABS(water_height_diff) >= STEP_L) {
                g_Lara.water_status = LWS_ABOVE_WATER;
                item->anim_num = LA_FALL_START;
                item->frame_num = g_Anims[item->anim_num].frame_base;
                item->goal_anim_state = LS_FORWARD_JUMP;
                item->current_anim_state = LS_FORWARD_JUMP;
                item->gravity = 1;
                item->speed = item->fall_speed / 4;
                item->fall_speed = 0;
                item->rot.x = 0;
                item->rot.z = 0;
                g_Lara.torso_y_rot = 0;
                g_Lara.torso_x_rot = 0;
                g_Lara.head_y_rot = 0;
                g_Lara.head_x_rot = 0;
            } else {
                g_Lara.water_status = LWS_SURFACE;
                item->anim_num = LA_UNDERWATER_TO_ONWATER;
                item->frame_num = g_Anims[item->anim_num].frame_base;
                item->goal_anim_state = LS_SURF_TREAD;
                item->current_anim_state = LS_SURF_TREAD;
                item->fall_speed = 0;
                item->pos.y += 1 - water_height_diff;
                item->rot.z = 0;
                item->rot.x = 0;
                g_Lara.dive_count = 11;
                g_Lara.torso_y_rot = 0;
                g_Lara.torso_x_rot = 0;
                g_Lara.head_y_rot = 0;
                g_Lara.head_x_rot = 0;
                Item_UpdateRoom(item, -381);
                Sound_Effect(SFX_LARA_BREATH, &item->pos, SPM_ALWAYS);
            }
            break;

        case LWS_SURFACE:
            if (room_submerged) {
                break;
            }

            if (water_height_diff <= LARA_WADE_DEPTH) {
                g_Lara.water_status = LWS_ABOVE_WATER;
                item->anim_num = LA_FALL_START;
                item->frame_num = g_Anims[item->anim_num].frame_base;
                item->goal_anim_state = LS_FORWARD_JUMP;
                item->current_anim_state = LS_FORWARD_JUMP;
                item->gravity = 1;
                item->speed = item->fall_speed / 4;
            } else {
                g_Lara.water_status = LWS_WADE;
                item->anim_num = LA_STAND_IDLE;
                item->frame_num = g_Anims[item->anim_num].frame_base;
                item->current_anim_state = LS_STOP;
                item->goal_anim_state = LS_WADE;
                Item_Animate(item);
                item->fall_speed = 0;
            }
            item->rot.x = 0;
            item->rot.z = 0;
            g_Lara.torso_y_rot = 0;
            g_Lara.torso_x_rot = 0;
            g_Lara.head_y_rot = 0;
            g_Lara.head_x_rot = 0;
            break;

        case LWS_WADE:
            g_Camera.target_elevation = -22 * PHD_DEGREE;

            if (water_height_diff < LARA_WADE_DEPTH) {
                g_Lara.water_status = LWS_ABOVE_WATER;
                if (item->current_anim_state == LS_WADE) {
                    item->goal_anim_state = LS_RUN;
                }
            } else if (water_height_diff > 730) {
                g_Lara.water_status = LWS_SURFACE;
                item->pos.y += 1 - water_height_diff;

                switch (item->current_anim_state) {
                case LS_BACK:
                    item->goal_anim_state = LS_SURF_BACK;
                    item->anim_num = LA_ONWATER_IDLE_TO_SWIM_BACK;
                    break;

                case LS_STEP_RIGHT:
                    item->goal_anim_state = LS_SURF_RIGHT;
                    item->anim_num = LA_ONWATER_SWIM_RIGHT;
                    break;

                case LS_STEP_LEFT:
                    item->goal_anim_state = LS_SURF_LEFT;
                    item->anim_num = LA_ONWATER_SWIM_LEFT;
                    break;

                default:
                    item->goal_anim_state = LS_SURF_SWIM;
                    item->anim_num = LA_ONWATER_SWIM_FORWARD;
                    break;
                }
                item->current_anim_state = item->goal_anim_state;
                item->frame_num = g_Anims[item->anim_num].frame_base;

                item->rot.z = 0;
                item->rot.x = 0;
                item->gravity = 0;
                item->fall_speed = 0;
                g_Lara.dive_count = 0;
                g_Lara.torso_y_rot = 0;
                g_Lara.torso_x_rot = 0;
                g_Lara.head_y_rot = 0;
                g_Lara.head_x_rot = 0;
                Item_UpdateRoom(item, -LARA_HEIGHT / 2);
            }
            break;

        default:
            break;
        }
    }

    if (item->hit_points <= 0) {
        item->hit_points = -1;
        if (!g_Lara.death_count) {
            Music_Stop();
        }
        g_Lara.death_count++;
        if (item->flags & IF_ONE_SHOT) {
            g_Lara.death_count++;
            return;
        }
    } else if (g_GF_NoFloor && item->pos.y >= g_GF_NoFloor) {
        item->hit_points = -1;
        g_Lara.death_count = 9 * FRAMES_PER_SECOND;
    }

    COLL_INFO coll;
    switch (g_Lara.water_status) {
    case LWS_ABOVE_WATER:
    case LWS_WADE:
        g_Lara.air = LARA_MAX_AIR;
        Lara_HandleAboveWater(item, &coll);
        break;

    case LWS_UNDERWATER:
        if (item->hit_points >= 0) {
            g_Lara.air--;
            if (g_Lara.air < 0) {
                g_Lara.air = -1;
                item->hit_points -= 5;
            }
        }
        Lara_HandleUnderwater(item, &coll);
        break;

    case LWS_SURFACE:
        if (item->hit_points >= 0) {
            g_Lara.air += 10;
            CLAMPG(g_Lara.air, LARA_MAX_AIR);
        }
        Lara_HandleSurface(item, &coll);
        break;

    default:
        break;
    }

    g_SaveGame.statistics.distance += Math_Sqrt(
        SQUARE(item->pos.z - g_Lara.last_pos.z)
        + SQUARE(item->pos.y - g_Lara.last_pos.y)
        + SQUARE(item->pos.x - g_Lara.last_pos.x));

    g_Lara.last_pos = item->pos;
}

void __cdecl Lara_ControlExtra(const int16_t item_num)
{
    Item_Animate(&g_Items[item_num]);
}

void __cdecl Lara_Animate(ITEM_INFO *const item)
{
    item->frame_num++;

    const ANIM_STRUCT *anim = &g_Anims[item->anim_num];
    if (anim->num_changes > 0 && Item_GetAnimChange(item, anim)) {
        anim = &g_Anims[item->anim_num];
        item->current_anim_state = anim->current_anim_state;
    }

    if (item->frame_num > anim->frame_end) {
        if (anim->num_commands > 0) {
            const int16_t *cmd_ptr = &g_AnimCommands[anim->command_idx];
            for (int32_t i = 0; i < anim->num_commands; i++) {
                const int16_t cmd = *cmd_ptr++;

                switch (cmd) {
                case AC_MOVE_ORIGIN:
                    Item_Translate(item, cmd_ptr[0], cmd_ptr[1], cmd_ptr[2]);
                    cmd_ptr += 3;
                    break;

                case AC_JUMP_VELOCITY:
                    item->fall_speed = *cmd_ptr++;
                    item->speed = *cmd_ptr++;
                    item->gravity = 1;

                    if (g_Lara.calc_fallspeed) {
                        item->fall_speed = g_Lara.calc_fallspeed;
                        g_Lara.calc_fallspeed = 0;
                    }
                    break;

                case AC_ATTACK_READY:
                    if (g_Lara.gun_status != LGS_SPECIAL) {
                        g_Lara.gun_status = LGS_ARMLESS;
                    }
                    break;

                case AC_SOUND_FX:
                case AC_EFFECT:
                    cmd_ptr += 2;
                    break;

                default:
                    break;
                }
            }
        }

        item->anim_num = anim->jump_anim_num;
        item->frame_num = anim->jump_frame_num;
        anim = &g_Anims[item->anim_num];
        item->current_anim_state = anim->current_anim_state;
    }

    if (anim->num_commands > 0) {
        const int16_t *cmd_ptr = &g_AnimCommands[anim->command_idx];
        for (int32_t i = 0; i < anim->num_commands; i++) {
            const int16_t cmd = *cmd_ptr++;

            switch (cmd) {
            case AC_MOVE_ORIGIN:
                cmd_ptr += 3;
                break;

            case AC_JUMP_VELOCITY:
                cmd_ptr += 2;
                break;

            case AC_SOUND_FX: {
                const int32_t frame = cmd_ptr[0];
                const SOUND_EFFECT_ID sound_id = cmd_ptr[1] & 0x3FFF;
                const ANIM_COMMAND_ENVIRONMENT type =
                    (cmd_ptr[1] & 0xC000) >> 14;
                cmd_ptr += 2;

                if (item->frame_num != frame) {
                    break;
                }

                if (type == ACE_ALL
                    || (type == ACE_LAND
                        && (g_Lara.water_surface_dist >= 0
                            || g_Lara.water_surface_dist == NO_HEIGHT))
                    || (type == ACE_WATER && g_Lara.water_surface_dist < 0
                        && g_Lara.water_surface_dist != NO_HEIGHT)) {
                    Sound_Effect(sound_id, &item->pos, SPM_ALWAYS);
                }
                break;
            }

            case AC_EFFECT:
                const int32_t frame = cmd_ptr[0];
                const ANIM_COMMAND_ENVIRONMENT type =
                    (cmd_ptr[1] & 0xC000) >> 14;
                const int32_t fx_func_idx = cmd_ptr[1] & 0x3FFF;
                cmd_ptr += 2;

                if (item->frame_num != *cmd_ptr) {
                    break;
                }

                if (type == ACE_ALL
                    || (type == ACE_LAND
                        && (g_Lara.water_surface_dist >= 0
                            || g_Lara.water_surface_dist == NO_HEIGHT))
                    || (type == ACE_WATER && g_Lara.water_surface_dist < 0)) {
                    g_EffectRoutines[fx_func_idx](item);
                }
                break;

            default:
                break;
            }
        }
    }

    if (item->gravity) {
        const int32_t speed = anim->velocity
            + anim->acceleration * (item->frame_num - anim->frame_base - 1);
        item->speed -= speed >> 16;
        item->speed += (anim->acceleration + speed) >> 16;
        item->fall_speed += (item->fall_speed >= FAST_FALL_SPEED ? 1 : GRAVITY);
        item->pos.y += item->fall_speed;
    } else {
        const int32_t speed = anim->velocity
            + anim->acceleration * (item->frame_num - anim->frame_base);
        item->speed = speed >> 16;
    }

    item->pos.x += (item->speed * Math_Sin(g_Lara.move_angle)) >> W2V_SHIFT;
    item->pos.z += (item->speed * Math_Cos(g_Lara.move_angle)) >> W2V_SHIFT;
}

void __cdecl Lara_UseItem(const int16_t object_num)
{
    ITEM_INFO *const item = g_LaraItem;

    switch (object_num) {
    case O_PISTOL_ITEM:
    case O_PISTOL_OPTION:
        g_Lara.request_gun_type = LGT_PISTOLS;
        break;

    case O_SHOTGUN_ITEM:
    case O_SHOTGUN_OPTION:
        g_Lara.request_gun_type = LGT_SHOTGUN;
        break;

    case O_MAGNUM_ITEM:
    case O_MAGNUM_OPTION:
        g_Lara.request_gun_type = LGT_MAGNUMS;
        break;

    case O_UZI_ITEM:
    case O_UZI_OPTION:
        g_Lara.request_gun_type = LGT_UZIS;
        break;

    case O_HARPOON_ITEM:
    case O_HARPOON_OPTION:
        g_Lara.request_gun_type = LGT_HARPOON;
        break;

    case O_M16_ITEM:
    case O_M16_OPTION:
        g_Lara.request_gun_type = LGT_M16;
        break;

    case O_GRENADE_ITEM:
    case O_GRENADE_OPTION:
        g_Lara.request_gun_type = LGT_ROCKET;
        break;

    case O_SMALL_MEDIPACK_ITEM:
    case O_SMALL_MEDIPACK_OPTION:
        if (item->hit_points > 0 && item->hit_points < LARA_MAX_HITPOINTS) {
            item->hit_points += LARA_MAX_HITPOINTS / 2;
            CLAMPG(item->hit_points, LARA_MAX_HITPOINTS);
            Inv_RemoveItem(O_SMALL_MEDIPACK_ITEM);
            Sound_Effect(SFX_MENU_MEDI, NULL, SPM_ALWAYS);
            g_SaveGame.statistics.medipacks++;
        }
        break;

    case O_LARGE_MEDIPACK_ITEM:
    case O_LARGE_MEDIPACK_OPTION:
        if (item->hit_points > 0 && item->hit_points < LARA_MAX_HITPOINTS) {
            item->hit_points = LARA_MAX_HITPOINTS;
            Inv_RemoveItem(O_LARGE_MEDIPACK_ITEM);
            Sound_Effect(SFX_MENU_MEDI, NULL, SPM_ALWAYS);
            g_SaveGame.statistics.medipacks += 2;
        }
        break;

    case O_FLARES_ITEM:
    case O_FLARES_OPTION:
        g_Lara.request_gun_type = LGT_FLARE;
        break;

    default:
        break;
    }
}

void __cdecl Lara_CheatGetStuff(void)
{
    Inv_AddItem(O_HARPOON_ITEM);
    Inv_AddItem(O_M16_ITEM);
    Inv_AddItem(O_GRENADE_ITEM);
    Inv_AddItem(O_SHOTGUN_ITEM);
    Inv_AddItem(O_UZI_ITEM);
    Inv_AddItem(O_MAGNUM_ITEM);
    Inv_AddItem(O_PISTOL_ITEM);

    for (int32_t i = 0; i < 10; i++) {
        Inv_AddItem(O_FLARES_ITEM);
        Inv_AddItem(O_SMALL_MEDIPACK_ITEM);
        Inv_AddItem(O_LARGE_MEDIPACK_ITEM);
    }

    g_Lara.magnum_ammo.ammo = 1000;
    g_Lara.uzi_ammo.ammo = 2000;
    g_Lara.shotgun_ammo.ammo = 300;
    g_Lara.harpoon_ammo.ammo = 300;
    g_Lara.m16_ammo.ammo = 300;
    g_Lara.grenade_ammo.ammo = 300;
}

void __cdecl Lara_InitialiseLoad(const int16_t item_num)
{
    g_Lara.item_num = item_num;
    g_LaraItem = &g_Items[item_num];
}
