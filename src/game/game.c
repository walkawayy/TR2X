#include "game/game.h"

#include "decomp/decomp.h"
#include "game/camera.h"
#include "game/demo.h"
#include "game/input.h"
#include "game/inventory.h"
#include "game/lara/lara_control.h"
#include "game/music.h"
#include "game/overlay.h"
#include "game/room_draw.h"
#include "game/sound.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <libtrx/utils.h>

static int32_t m_FrameCount = 0;

int32_t __cdecl Game_Control(int32_t nframes, const bool demo_mode)
{
    CLAMPG(nframes, MAX_FRAMES);
    m_FrameCount += nframes;

    if (m_FrameCount <= 0) {
        return 0;
    }

    while (m_FrameCount > 0) {
        // TODO: move me outta here
        if (g_CD_TrackID > 0) {
            Music_Init();
        }

        if (!g_GameFlow.cheat_mode_check_disabled) {
            CheckCheatMode();
        }

        if (g_LevelComplete) {
            return GFD_START_GAME | LV_FIRST;
        }

        Input_Update();
        if (g_IsResetFlag) {
            return GFD_EXIT_TO_TITLE;
        }

        if (demo_mode) {
            if (g_Input) {
                return g_GameFlow.on_demo_interrupt;
            }
            Demo_GetInput();
            if (g_Input == -1) {
                g_Input = 0;
                return g_GameFlow.on_demo_end;
            }
        } else if (g_GameFlow.no_input_timeout) {
            if (g_Input) {
                g_NoInputCounter = 0;
            } else {
                g_NoInputCounter++;
                if (g_NoInputCounter > g_GameFlow.no_input_time) {
                    return GFD_START_DEMO;
                }
            }
        }

        if (g_Lara.death_count > DEATH_WAIT
            || (g_Lara.death_count > DEATH_WAIT_INPUT && g_Input != 0)
            || g_OverlayStatus == 2) {
            if (demo_mode) {
                return g_GameFlow.on_death_demo_mode;
            }
            if (g_CurrentLevel == LV_GYM) {
                return GFD_EXIT_TO_TITLE;
            }
            if (g_GameFlow.on_death_in_game) {
                return g_GameFlow.on_death_in_game;
            }
            if (g_OverlayStatus == 2) {
                g_OverlayStatus = 1;
                const int32_t result = Inv_Display(5);
                if (result) {
                    return result;
                }
            } else {
                g_OverlayStatus = 2;
            }
        }

        if (((g_Input & 0xC00100) != 0 || g_OverlayStatus <= 0)
            && g_Lara.death_count == 0 && !g_Lara.extra_anim) {
            if (g_OverlayStatus > 0) {
                if (g_GameFlow.load_save_disabled) {
                    g_OverlayStatus = 0;
                } else if ((g_Input & 0x800000) != 0) {
                    g_OverlayStatus = -1;
                } else {
                    g_OverlayStatus = (g_Input & 0x400000) != 0 ? -2 : 0;
                }
            } else {
                int32_t result;
                if (g_OverlayStatus == -1) {
                    result = Inv_Display(INV_LOAD_MODE);
                } else if (g_OverlayStatus == -2) {
                    result = Inv_Display(INV_SAVE_MODE);
                } else {
                    result = Inv_Display(INV_GAME_MODE);
                }
                g_OverlayStatus = 1;

                if (result) {
                    if (g_Inv_ExtraData[0] == 1) {
                        if (g_CurrentLevel == LV_GYM) {
                            return GFD_START_GAME | LV_FIRST;
                        }
                        CreateSaveGameInfo();
                        S_SaveGame(
                            &g_SaveGame, sizeof(SAVEGAME_INFO),
                            g_Inv_ExtraData[1]);
                        S_SaveSettings();
                    } else {
                        return result;
                    }
                }
            }
        }

        g_DynamicLightCount = 0;

        {
            int16_t item_num = g_NextItemActive;
            while (item_num != NO_ITEM) {
                ITEM_INFO *const item = &g_Items[item_num];
                const int16_t next = item->next_active;
                const OBJECT_INFO *object = &g_Objects[item->object_num];
                if (object->control) {
                    object->control(item_num);
                }
                item_num = next;
            }
        }

        {
            int16_t fx_num = g_NextEffectActive;
            while (fx_num != NO_ITEM) {
                FX_INFO *const fx = &g_Effects[fx_num];
                const OBJECT_INFO *const object = &g_Objects[fx->object_num];
                const int32_t next = fx->next_active;
                if (object->control) {
                    object->control(fx_num);
                }
                fx_num = next;
            }
        }

        Lara_Control(0);
        HairControl(0);
        Camera_Update();
        Sound_UpdateEffects();

        g_HealthBarTimer--;
        if (g_CurrentLevel || g_IsAssaultTimerActive) {
            g_SaveGame.statistics.timer++;
        }

        m_FrameCount -= 2;
    }

    return 0;
}

int32_t __cdecl Game_Draw(void)
{
    Room_DrawAllRooms(g_Camera.pos.room_num);
    Overlay_DrawGameInfo(true);
    S_OutputPolyList();
    g_Camera.num_frames = S_DumpScreen();
    S_AnimateTextures(g_Camera.num_frames);
    return g_Camera.num_frames;
}

int16_t __cdecl Game_Start(
    const int32_t level_num, const GF_LEVEL_TYPE level_type)
{
    if (level_type == GFL_NORMAL || level_type == GFL_SAVED
        || level_type == GFL_DEMO) {
        g_CurrentLevel = level_num;
    }
    if (level_type != GFL_SAVED) {
        ModifyStartInfo(level_num);
    }
    g_IsTitleLoaded = 0;
    if (level_type != GFL_SAVED) {
        InitialiseLevelFlags();
    }
    if (!Level_Initialise(level_num, level_type)) {
        g_CurrentLevel = 0;
        return GFD_EXIT_GAME;
    }

    GAME_FLOW_DIR option = GameLoop(0);
    if (option == GFD_EXIT_TO_TITLE || option == GFD_START_DEMO) {
        return option;
    }

    if (option == GFD_EXIT_GAME) {
        g_CurrentLevel = 0;
        return option;
    }

    if (g_LevelComplete) {
        if (g_GameFlow.demo_version && g_GameFlow.single_level) {
            return GFD_EXIT_TO_TITLE;
        }

        if (g_CurrentLevel == LV_GYM) {
            S_FadeToBlack();
            return GFD_EXIT_TO_TITLE;
        }

        S_FadeInInventory(1);
        return GFD_LEVEL_COMPLETE | g_CurrentLevel;
    }

    S_FadeToBlack();
    if (!g_Inv_Chosen) {
        return GFD_EXIT_TO_TITLE;
    }

    if (g_Inv_ExtraData[0] == 0) {
        S_LoadGame(&g_SaveGame, sizeof(SAVEGAME_INFO), g_Inv_ExtraData[1]);
        return GFD_START_SAVED_GAME | g_Inv_ExtraData[1];
    }

    if (g_Inv_ExtraData[0] != 1) {
        return GFD_EXIT_TO_TITLE;
    }

    if (g_GameFlow.play_any_level) {
        return g_Inv_ExtraData[1] + 1;
    }

    return GFD_START_GAME | LV_FIRST;
}
