#include "game/inventory.h"

#include "game/input.h"
#include "game/matrix.h"
#include "game/music.h"
#include "game/output.h"
#include "game/overlay.h"
#include "game/text.h"
#include "global/const.h"
#include "global/funcs.h"
#include "global/types.h"
#include "global/vars.h"

#define TITLE_RING_OBJECTS 3
#define OPTION_RING_OBJECTS 3
#define LOW_LIGHT 5120
#define HIGH_LIGHT 4096

static TEXTSTRING *m_VersionText = NULL;

void __cdecl Inv_Construct(void)
{
    S_SetupAboveWater(0);

    if (g_Inv_Mode != INV_TITLE_MODE) {
        TempVideoAdjust(g_HiRes, 1.0);
    }

    g_PhdWinRight = g_PhdWinMaxX;
    g_PhdWinLeft = 0;
    g_PhdWinTop = 0;
    g_PhdWinBottom = g_PhdWinMaxY;

    g_Inv_IsActive = 1;
    g_Inv_Chosen = 0;

    if (g_Inv_Mode == INV_TITLE_MODE) {
        g_Inv_OptionObjectsCount = TITLE_RING_OBJECTS;
        if (g_GameFlow.gym_enabled) {
            g_Inv_OptionObjectsCount++;
        }
        m_VersionText = Text_Create(-20, -18, 0, g_TR2XVersion);
        Text_AlignRight(m_VersionText, 1);
        Text_AlignBottom(m_VersionText, 1);
        Text_SetScale(m_VersionText, PHD_ONE * 0.5, PHD_ONE * 0.5);
    } else {
        g_Inv_OptionObjectsCount = OPTION_RING_OBJECTS;
        Text_Remove(m_VersionText);
        m_VersionText = NULL;
    }

    for (int32_t i = 0; i < 8; i++) {
        g_Inv_ExtraData[i] = 0;
    }

    for (int32_t i = 0; i < g_Inv_MainObjectsCount; i++) {
        INVENTORY_ITEM *const inv_item = &g_Inv_MainList[i];
        inv_item->meshes_drawn = inv_item->meshes_sel;
        inv_item->current_frame = 0;
        inv_item->goal_frame = 0;
        inv_item->anim_count = 0;
        inv_item->y_rot = 0;
    }

    for (int32_t i = 0; i < g_Inv_OptionObjectsCount; i++) {
        INVENTORY_ITEM *const inv_item = &g_Inv_OptionList[i];
        inv_item->current_frame = 0;
        inv_item->goal_frame = 0;
        inv_item->anim_count = 0;
        inv_item->y_rot = 0;
    }

    g_Inv_MainCurrent = 0;

    if (g_GymInvOpenEnabled && g_Inv_Mode == INV_TITLE_MODE
        && !g_GameFlow.load_save_disabled && g_GameFlow.gym_enabled) {
        g_Inv_OptionCurrent = 3; // TODO: don't hardcode me
        g_SoundOptionLine = 0;
    } else {
        g_Inv_OptionCurrent = 0;
        g_GymInvOpenEnabled = 0;
        g_SoundOptionLine = 0;
    }
}

int32_t __cdecl Inv_Display(int32_t inventory_mode)
{
    RING_INFO ring = { 0 };
    IMOTION_INFO imo = { 0 };

    bool demo_needed = false;
    int32_t v35 = 0;
    bool pass_open = false;
    if (inventory_mode == INV_KEYS_MODE && !g_Inv_KeyObjectsCount) {
        g_Inv_Chosen = NO_OBJECT;
        return 0;
    }

    Text_Remove(g_AmmoTextInfo);
    g_AmmoTextInfo = NULL;

    Output_AlterFOV(80 * PHD_DEGREE);
    g_Inv_Mode = inventory_mode;
    g_Inv_NFrames = 2;

    Inv_Construct();
    if (inventory_mode == INV_TITLE_MODE) {
        S_FadeInInventory(0);
    } else {
        S_FadeInInventory(1);
    }

    Sound_Shutdown();
    if (inventory_mode != INV_TITLE_MODE) {
        Music_SetVolume(0);
    }

    switch (inventory_mode) {
    case INV_TITLE_MODE:
    case INV_SAVE_MODE:
    case INV_LOAD_MODE:
    case INV_DEATH_MODE:
        Inv_Ring_Init(
            &ring, 1, &g_Inv_OptionList, g_Inv_OptionObjectsCount,
            g_Inv_OptionCurrent, &imo);
        break;

    case INV_KEYS_MODE:
        Inv_Ring_Init(
            &ring, 2, &g_Inv_KeysList, g_Inv_KeyObjectsCount, g_Inv_MainCurrent,
            &imo);
        break;

    default:
        if (g_Inv_MainObjectsCount) {
            Inv_Ring_Init(
                &ring, 0, &g_Inv_MainList, g_Inv_MainObjectsCount,
                g_Inv_MainCurrent, &imo);
        } else {
            Inv_Ring_Init(
                &ring, 1, &g_Inv_OptionList, g_Inv_OptionObjectsCount,
                g_Inv_OptionCurrent, &imo);
        }
        break;
    }

    Sound_Effect(111, 0, SPM_ALWAYS);
    g_Inv_NFrames = 2;

    do {
        if (inventory_mode == INV_TITLE_MODE && g_CD_TrackID > 0) {
            Music_Init();
        }
        Inv_Ring_CalcAdders(&ring, 24);
        Input_Update();

        if (g_Inv_DemoMode) {
            if (g_Input != 0) {
                return g_GameFlow.on_demo_interrupt;
            }
            Demo_GetInput();
            if (g_Input == -1) {
                return g_GameFlow.on_demo_end;
            }
        } else if (g_Input != 0) {
            g_NoInputCounter = 0;
        }

        g_InputDB = GetDebouncedInput(g_Input);
        if (g_Inv_Mode != INV_TITLE_MODE || g_Input != 0 || g_InputDB) {
            g_NoInputCounter = 0;
            g_IsResetFlag = 0;
        } else if (g_GameFlow.num_demos || g_GameFlow.no_input_timeout) {
            g_NoInputCounter++;
            if (g_NoInputCounter > g_GameFlow.no_input_time) {
                demo_needed = true;
                g_IsResetFlag = 1;
            }
        }

        if (g_StopInventory) {
            return GFD_EXIT_TO_TITLE;
        }

        if ((g_Inv_Mode == INV_SAVE_MODE || g_Inv_Mode == INV_LOAD_MODE
             || g_Inv_Mode == INV_DEATH_MODE)
            && !pass_open) {
            g_InputDB = IN_SELECT;
            g_Input = 0;
        }

        for (int32_t frame = 0; frame < g_Inv_NFrames; frame++) {
            if (g_Inv_IsOptionsDelay) {
                if (g_Inv_OptionsDelayCounter) {
                    g_Inv_OptionsDelayCounter--;
                } else {
                    g_Inv_IsOptionsDelay = 0;
                }
            }
            Inv_Ring_DoMotions(&ring);
        }
        ring.camera.pos.z = ring.radius + 598;

        S_InitialisePolyList(0);
        if (g_Inv_Mode == INV_TITLE_MODE) {
            Inv_DoInventoryPicture();
        } else {
            Inv_DoInventoryBackground();
        }
        S_AnimateTextures(g_Inv_NFrames);

        PHD_3DPOS viewpos;
        Inv_Ring_GetView(&ring, &viewpos);
        Matrix_GenerateW2V(&viewpos);
        Inv_Ring_Light(&ring);

        Matrix_Push();
        Matrix_TranslateAbs(
            ring.ringpos.pos.x, ring.ringpos.pos.y, ring.ringpos.pos.z);
        Matrix_RotYXZ(
            ring.ringpos.rot.y, ring.ringpos.rot.x, ring.ringpos.rot.z);

        int32_t angle = 0;
        for (int32_t j = 0; j < ring.number_of_objects; j++) {
            INVENTORY_ITEM *inv_item = ring.list[j];
            if (j != ring.current_object) {
                g_LsAdder = LOW_LIGHT;
                for (int32_t k = 0; k < g_Inv_NFrames; k++) {
                    if (inv_item->y_rot < 0) {
                        inv_item->y_rot += 256;
                    } else if (inv_item->y_rot > 0) {
                        inv_item->y_rot -= 256;
                    }
                }
            } else {
                for (int32_t k = 0; k < g_Inv_NFrames; k++) {
                    if (ring.rotating) {
                        g_LsAdder = LOW_LIGHT;
                        if (inv_item->y_rot > 0) {
                            inv_item->y_rot -= 512;
                        } else if (inv_item->y_rot < 0) {
                            inv_item->y_rot += 512;
                        }
                    } else if (
                        imo.status == RNG_SELECTED
                        || imo.status == RNG_DESELECTING
                        || imo.status == RNG_SELECTING
                        || imo.status == RNG_DESELECT
                        || imo.status == RNG_CLOSING_ITEM) {
                        g_LsAdder = HIGH_LIGHT;
                        const int32_t delta =
                            inv_item->y_rot_sel - inv_item->y_rot;
                        if (delta != 0) {
                            if (delta > 0 && delta < 0x8000) {
                                inv_item->y_rot += 1024;
                            } else {
                                inv_item->y_rot -= 1024;
                            }
                            inv_item->y_rot &= 0xFC00;
                        }
                    } else if (
                        ring.number_of_objects == 1
                        || (!(g_Input & IN_RIGHT) && !(g_Input & IN_LEFT))) {
                        g_LsAdder = HIGH_LIGHT;
                        inv_item->y_rot += 256;
                    }
                }

                if ((imo.status == RNG_OPEN || imo.status == RNG_SELECTING
                     || imo.status == RNG_SELECTED
                     || imo.status == RNG_DESELECTING
                     || imo.status == RNG_DESELECT
                     || imo.status == RNG_CLOSING_ITEM)
                    && !ring.rotating && !(g_Input & IN_LEFT)
                    && !(g_Input & IN_RIGHT)) {
                    RingNotActive(inv_item);
                }
            }

            if (imo.status == RNG_OPEN || imo.status == RNG_SELECTING
                || imo.status == RNG_SELECTED || imo.status == RNG_DESELECTING
                || imo.status == RNG_DESELECT
                || imo.status == RNG_CLOSING_ITEM) {
                RingIsOpen(&ring);
            } else {
                RingIsNotOpen(&ring);
            }

            if (imo.status == RNG_OPENING || imo.status == RNG_CLOSING
                || imo.status == RNG_MAIN2OPTION
                || imo.status == RNG_OPTION2MAIN
                || imo.status == RNG_EXITING_INVENTORY || imo.status == RNG_DONE
                || ring.rotating) {
                RingActive();
            }

            Matrix_Push();
            Matrix_RotYXZ(angle, 0, 0);
            Matrix_TranslateRel(ring.radius, 0, 0);
            Matrix_RotYXZ(0x4000, inv_item->x_rot_pt, 0);
            Inv_DrawInventoryItem(inv_item);
            Matrix_Pop();
            angle += ring.angle_adder;
        }

        Matrix_Pop();
        Overlay_DrawModeInfo();
        Text_Draw();
        S_OutputPolyList();
        Sound_UpdateContinued();

        const int32_t frames = S_DumpScreen();
        g_Inv_NFrames = frames;
        g_Camera.num_frames = frames;
        if (g_CurrentLevel) {
            g_SaveGame.statistics.timer += frames / 2;
        }

        if (!ring.rotating) {
            switch (imo.status) {
            case RNG_OPEN:
                if ((g_Input & IN_RIGHT) != 0 && ring.number_of_objects > 1) {
                    Inv_Ring_RotateLeft(&ring);
                    Sound_Effect(108, 0, SPM_ALWAYS);
                    break;
                }

                if ((g_Input & IN_LEFT) != 0 && ring.number_of_objects > 1) {
                    Inv_Ring_RotateRight(&ring);
                    Sound_Effect(108, 0, SPM_ALWAYS);
                    break;
                }

                if (g_IsResetFlag
                    || (((g_InputDB & IN_OPTION) || (g_InputDB & IN_DESELECT))
                        && g_Inv_Mode != INV_TITLE_MODE)) {
                    Sound_Effect(112, 0, SPM_ALWAYS);
                    g_Inv_Chosen = NO_OBJECT;
                    if (ring.type != RT_MAIN) {
                        g_Inv_OptionCurrent = ring.current_object;
                    } else {
                        g_Inv_MainCurrent = ring.current_object;
                    }
                    if (inventory_mode == INV_TITLE_MODE) {
                        S_FadeOutInventory(0);
                    } else {
                        S_FadeOutInventory(1);
                    }
                    Inv_Ring_MotionSetup(&ring, RNG_CLOSING, RNG_DONE, 32);
                    Inv_Ring_MotionRadius(&ring, 0);
                    Inv_Ring_MotionCameraPos(&ring, -1536);
                    Inv_Ring_MotionRotation(
                        &ring, 0x8000, ring.ringpos.rot.y + 0x8000);
                    g_Input = 0;
                    g_InputDB = 0;
                }

                if ((g_InputDB & IN_SELECT) != 0) {
                    if ((g_Inv_Mode == INV_SAVE_MODE
                         || g_Inv_Mode == INV_LOAD_MODE
                         || g_Inv_Mode == INV_DEATH_MODE)
                        && !pass_open) {
                        pass_open = true;
                    }

                    g_SoundOptionLine = 0;
                    INVENTORY_ITEM *inv_item;
                    if (ring.type == RT_MAIN) {
                        g_Inv_MainCurrent = ring.current_object;
                        inv_item = *(&g_Inv_MainList + ring.current_object);
                    } else if (ring.type == RT_OPTION) {
                        g_Inv_OptionCurrent = ring.current_object;
                        inv_item = *(&g_Inv_OptionList + ring.current_object);
                    } else {
                        g_Inv_KeysCurrent = ring.current_object;
                        inv_item = *(&g_Inv_KeysList + ring.current_object);
                    }

                    inv_item->goal_frame = inv_item->open_frame;
                    inv_item->anim_direction = 1;
                    Inv_Ring_MotionSetup(
                        &ring, RNG_SELECTING, RNG_SELECTED, 16);
                    Inv_Ring_MotionRotation(
                        &ring, 0,
                        -16384 - ring.angle_adder * ring.current_object);
                    Inv_Ring_MotionItemSelect(&ring, inv_item);
                    g_Input = 0;
                    g_InputDB = 0;

                    switch (inv_item->obj_num) {
                    case O_COMPASS_OPTION:
                        Sound_Effect(113, 0, SPM_ALWAYS);
                        break;

                    case O_PHOTO_OPTION:
                        Sound_Effect(109, 0, SPM_ALWAYS);
                        break;

                    case O_PISTOL_OPTION:
                    case O_SHOTGUN_OPTION:
                    case O_MAGNUM_OPTION:
                    case O_UZI_OPTION:
                    case O_HARPOON_OPTION:
                    case O_M16_OPTION:
                    case O_GRENADE_OPTION:
                        Sound_Effect(114, 0, SPM_ALWAYS);
                        break;

                    default:
                        Sound_Effect(111, 0, SPM_ALWAYS);
                        break;
                    }
                }

                if ((g_InputDB & IN_FORWARD) != 0
                    && inventory_mode != INV_TITLE_MODE
                    && inventory_mode != INV_KEYS_MODE) {
                    if (ring.type == RT_OPTION) {
                        if (g_Inv_MainObjectsCount > 0) {
                            Inv_Ring_MotionSetup(
                                &ring, RNG_CLOSING, RNG_OPTION2MAIN, 24);
                            Inv_Ring_MotionRadius(&ring, 0);
                            Inv_Ring_MotionRotation(
                                &ring, 0x8000, ring.ringpos.rot.y + 0x8000);
                            Inv_Ring_MotionCameraPitch(&ring, 0x2000);
                            imo.misc = 0x2000;
                        }
                        g_InputDB = 0;
                    } else if (ring.type == RT_MAIN) {
                        if (g_Inv_KeyObjectsCount > 0) {
                            Inv_Ring_MotionSetup(
                                &ring, RNG_CLOSING, RNG_MAIN2KEYS, 24);
                            Inv_Ring_MotionRadius(&ring, 0);
                            Inv_Ring_MotionRotation(
                                &ring, 0x8000, ring.ringpos.rot.y + 0x8000);
                            Inv_Ring_MotionCameraPitch(&ring, 0x2000);
                            imo.misc = 0x2000;
                        }
                        g_Input = 0;
                        g_InputDB = 0;
                    }
                } else if (
                    (g_InputDB & IN_BACK) != 0
                    && inventory_mode != INV_TITLE_MODE
                    && inventory_mode != INV_KEYS_MODE) {
                    if (ring.type == RT_KEYS) {
                        if (g_Inv_MainObjectsCount > 0) {
                            Inv_Ring_MotionSetup(
                                &ring, RNG_CLOSING, RNG_KEYS2MAIN, 24);
                            Inv_Ring_MotionRadius(&ring, 0);
                            Inv_Ring_MotionRotation(
                                &ring, 0x8000, ring.ringpos.rot.y + 0x8000);
                            Inv_Ring_MotionCameraPitch(&ring, -0x2000);
                            imo.misc = -0x2000;
                        }
                        g_Input = 0;
                        g_InputDB = 0;
                    } else if (ring.type == RT_MAIN) {
                        if (g_Inv_OptionObjectsCount > 0
                            && !g_GameFlow.lockout_option_ring) {
                            Inv_Ring_MotionSetup(
                                &ring, RNG_CLOSING, RNG_MAIN2OPTION, 24);
                            Inv_Ring_MotionRadius(&ring, 0);
                            Inv_Ring_MotionRotation(
                                &ring, 0x8000, ring.ringpos.rot.y + 0x8000);
                            Inv_Ring_MotionCameraPitch(&ring, -0x2000);
                            imo.misc = -0x2000;
                        }
                        g_InputDB = 0;
                    }
                }
                break;

            case RNG_MAIN2OPTION:
                Inv_Ring_MotionSetup(&ring, RNG_OPENING, RNG_OPEN, 24);
                Inv_Ring_MotionRadius(&ring, 688);
                ring.camera_pitch = -(int16_t)(imo.misc);
                imo.camera_pitch_rate = imo.misc / 24;
                imo.camera_pitch_target = 0;
                ring.list = &g_Inv_OptionList;
                ring.type = RT_OPTION;
                g_Inv_MainCurrent = ring.current_object;
                g_Inv_MainObjectsCount = ring.number_of_objects;
                ring.number_of_objects = g_Inv_OptionObjectsCount;
                ring.current_object = g_Inv_OptionCurrent;
                Inv_Ring_CalcAdders(&ring, 24);
                Inv_Ring_MotionRotation(
                    &ring, 0x8000,
                    -16384 - ring.angle_adder * ring.current_object);
                ring.ringpos.rot.y = imo.rotate_target + 0x8000;
                break;

            case RNG_MAIN2KEYS:
                Inv_Ring_MotionSetup(&ring, RNG_OPENING, RNG_OPEN, 24);
                Inv_Ring_MotionRadius(&ring, 688);
                imo.camera_pitch_target = 0;
                ring.camera_pitch = -(int16_t)(imo.misc);
                imo.camera_pitch_rate = imo.misc / 24;
                g_Inv_MainCurrent = ring.current_object;
                g_Inv_MainObjectsCount = ring.number_of_objects;
                ring.list = &g_Inv_KeysList;
                ring.type = RT_KEYS;
                ring.number_of_objects = g_Inv_KeyObjectsCount;
                ring.current_object = g_Inv_KeysCurrent;
                Inv_Ring_CalcAdders(&ring, 24);
                Inv_Ring_MotionRotation(
                    &ring, 0x8000,
                    -16384 - ring.angle_adder * ring.current_object);
                ring.ringpos.rot.y = imo.rotate_target + 0x8000;
                break;

            case RNG_KEYS2MAIN:
                Inv_Ring_MotionSetup(&ring, RNG_OPENING, RNG_OPEN, 24);
                Inv_Ring_MotionRadius(&ring, 688);
                ring.camera_pitch = -(int16_t)(imo.misc);
                imo.camera_pitch_rate = imo.misc / 24;
                imo.camera_pitch_target = 0;
                ring.list = &g_Inv_MainList;
                ring.type = RT_MAIN;
                g_Inv_KeysCurrent = ring.current_object;
                ring.number_of_objects = g_Inv_MainObjectsCount;
                ring.current_object = g_Inv_MainCurrent;
                Inv_Ring_CalcAdders(&ring, 24);
                Inv_Ring_MotionRotation(
                    &ring, 0x8000,
                    -16384 - ring.angle_adder * ring.current_object);
                ring.ringpos.rot.y = imo.rotate_target + 0x8000;
                break;

            case RNG_OPTION2MAIN:
                Inv_Ring_MotionSetup(&ring, RNG_OPENING, RNG_OPEN, 24);
                Inv_Ring_MotionRadius(&ring, 688);
                ring.camera_pitch = -(int16_t)(imo.misc);
                imo.camera_pitch_rate = imo.misc / 24;
                g_Inv_OptionCurrent = ring.current_object;
                g_Inv_OptionObjectsCount = ring.number_of_objects;
                imo.camera_pitch_target = 0;
                ring.list = &g_Inv_MainList;
                ring.type = RT_MAIN;
                ring.number_of_objects = g_Inv_MainObjectsCount;
                ring.current_object = g_Inv_MainCurrent;
                Inv_Ring_CalcAdders(&ring, 24);
                Inv_Ring_MotionRotation(
                    &ring, 0x8000,
                    -16384 - ring.angle_adder * ring.current_object);
                ring.ringpos.rot.y = imo.rotate_target + 0x8000;
                break;

            case RNG_SELECTED: {
                INVENTORY_ITEM *inv_item = ring.list[ring.current_object];
                if (inv_item->obj_num == O_PASSPORT_CLOSED) {
                    inv_item->obj_num = O_PASSPORT_OPTION;
                }

                bool busy = false;
                for (int32_t frame = 0; frame < g_Inv_NFrames; frame++) {
                    busy = false;
                    if (inv_item->y_rot == inv_item->y_rot_sel) {
                        busy = Inv_AnimateInventoryItem(inv_item);
                    }
                }

                if (!busy && !g_Inv_IsOptionsDelay) {
                    do_inventory_options(inv_item);
                    if (g_InputDB & IN_DESELECT) {
                        inv_item->sprites = 0;
                        Inv_Ring_MotionSetup(
                            &ring, RNG_CLOSING_ITEM, RNG_DESELECT, 0);
                        g_Input = 0;
                        g_InputDB = 0;
                        if (g_Inv_Mode == INV_LOAD_MODE
                            || g_Inv_Mode == INV_SAVE_MODE) {
                            Inv_Ring_MotionSetup(
                                &ring, RNG_CLOSING_ITEM, RNG_EXITING_INVENTORY,
                                0);
                            g_InputDB = 0;
                            g_Input = 0;
                        }
                    }

                    if ((g_InputDB & IN_SELECT) != 0) {
                        inv_item->sprites = 0;
                        g_Inv_Chosen = inv_item->obj_num;
                        if (ring.type != RT_MAIN) {
                            g_Inv_OptionCurrent = ring.current_object;
                        } else {
                            g_Inv_MainCurrent = ring.current_object;
                        }
                        if (g_Inv_Mode == INV_TITLE_MODE
                            && (inv_item->obj_num == O_DETAIL_OPTION
                                || inv_item->obj_num == O_SOUND_OPTION
                                || inv_item->obj_num == O_CONTROL_OPTION
                                || inv_item->obj_num == O_GAMMA_OPTION)) {
                            Inv_Ring_MotionSetup(
                                &ring, RNG_CLOSING_ITEM, RNG_DESELECT, 0);
                        } else {
                            Inv_Ring_MotionSetup(
                                &ring, RNG_CLOSING_ITEM, RNG_EXITING_INVENTORY,
                                0);
                        }
                        g_Input = 0;
                        g_InputDB = 0;
                    }
                }
                break;
            }

            case RNG_DESELECT:
                Sound_Effect(112, 0, SPM_ALWAYS);
                Inv_Ring_MotionSetup(&ring, RNG_DESELECTING, RNG_OPEN, 16);
                Inv_Ring_MotionRotation(
                    &ring, 0, -16384 - ring.angle_adder * ring.current_object);
                g_Input = 0;
                g_InputDB = 0;
                break;

            case RNG_CLOSING_ITEM: {
                int32_t frame = 0;
                INVENTORY_ITEM *inv_item = ring.list[ring.current_object];
                if (g_Inv_NFrames > 0) {
                    while (Inv_AnimateInventoryItem(inv_item)) {
                        frame++;
                        if (frame >= g_Inv_NFrames) {
                            break;
                        }
                    }

                    if (inv_item->obj_num == O_PASSPORT_OPTION) {
                        inv_item->obj_num = O_PASSPORT_CLOSED;
                        inv_item->current_frame = 0;
                    }

                    imo.count = 16;
                    imo.status = imo.status_target;
                    Inv_Ring_MotionItemDeselect(&ring, inv_item);
                }
                break;
            }

            case RNG_EXITING_INVENTORY:
                if (!imo.count) {
                    if (inventory_mode == INV_TITLE_MODE) {
                        S_FadeOutInventory(0);
                    } else {
                        S_FadeOutInventory(1);
                    }
                    Inv_Ring_MotionSetup(&ring, RNG_CLOSING, RNG_DONE, 32);
                    Inv_Ring_MotionRadius(&ring, 0);
                    Inv_Ring_MotionCameraPos(&ring, -1536);
                    Inv_Ring_MotionRotation(
                        &ring, 0x8000, ring.ringpos.rot.y + 0x8000);
                }
                break;

            default:
                break;
            }
        }
    } while (imo.status != RNG_DONE);

    RemoveInventoryText();
    S_FinishInventory();
    g_Inv_IsActive = 0;

    if (g_IsResetFlag) {
        return GFD_EXIT_TO_TITLE;
    }

    if (demo_needed) {
        return GFD_START_DEMO;
    }

    if (g_Inv_Chosen == NO_OBJECT) {
        if (inventory_mode != INV_TITLE_MODE && g_OptionMusicVolume != 0) {
            Music_SetVolume(25 * g_OptionMusicVolume + 5);
        }
        return 0;
    }

    switch (g_Inv_Chosen) {
    case O_PASSPORT_OPTION:
        if (g_Inv_ExtraData[0] == 1 && g_OptionMusicVolume != 0) {
            Music_SetVolume(25 * g_OptionMusicVolume + 5);
        }
        return 1;

    case O_PHOTO_OPTION:
        if (g_GameFlow.gym_enabled) {
            g_Inv_ExtraData[1] = 0;
            return 1;
        }
        break;

    case O_PISTOL_OPTION:
    case O_SHOTGUN_OPTION:
    case O_MAGNUM_OPTION:
    case O_UZI_OPTION:
    case O_HARPOON_OPTION:
    case O_M16_OPTION:
    case O_GRENADE_OPTION:
    case O_SMALL_MEDIPACK_OPTION:
    case O_LARGE_MEDIPACK_OPTION:
    case O_FLARES_OPTION:
        Lara_UseItem(g_Inv_Chosen);
        break;

    default:
        break;
    }

    if (inventory_mode != INV_TITLE_MODE && g_OptionMusicVolume != 0) {
        Music_SetVolume(25 * g_OptionMusicVolume + 5);
    }
    return 0;
}
