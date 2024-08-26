#include "game/gameflow.h"

#include "decomp/decomp.h"
#include "game/demo.h"
#include "global/funcs.h"
#include "global/vars.h"

#include <stdio.h>

int32_t __cdecl GF_LoadScriptFile(const char *const fname)
{
    g_GF_SunsetEnabled = false;

    if (!S_LoadGameFlow(fname)) {
        return false;
    }

    g_GameFlow.level_complete_track = MX_END_OF_LEVEL;

    g_Inv_Item_Stopwatch.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_STATISTICS];
    g_Inv_Item_Pistols.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PISTOLS];
    g_Inv_Item_Flare.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_FLARE];
    g_Inv_Item_Shotgun.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_SHOTGUN];
    g_Inv_Item_Magnums.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_MAGNUMS];
    g_Inv_Item_Uzis.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_UZIS];
    g_Inv_Item_Harpoon.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_HARPOON];
    g_Inv_Item_M16.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_M16];
    g_Inv_Item_Grenade.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_GRENADE];
    g_Inv_Item_PistolAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_PISTOL_AMMO];
    g_Inv_Item_ShotgunAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_SHOTGUN_AMMO];
    g_Inv_Item_MagnumAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_MAGNUM_AMMO];
    g_Inv_Item_UziAmmo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_UZI_AMMO];
    g_Inv_Item_HarpoonAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_HARPOON_AMMO];
    g_Inv_Item_M16Ammo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_M16_AMMO];
    g_Inv_Item_GrenadeAmmo.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_GRENADE_AMMO];
    g_Inv_Item_SmallMedi.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_SMALL_MEDIPACK];
    g_Inv_Item_LargeMedi.string =
        g_GF_GameStrings[GF_S_GAME_INV_ITEM_LARGE_MEDIPACK];
    g_Inv_Item_Pickup1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PICKUP];
    g_Inv_Item_Pickup2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PICKUP];
    g_Inv_Item_Puzzle1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle3.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Puzzle4.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_PUZZLE];
    g_Inv_Item_Key1.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key2.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key3.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];
    g_Inv_Item_Key4.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_KEY];

    g_Inv_Item_Passport.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_GAME];
    g_Inv_Item_Graphics.string = g_GF_PCStrings[GF_S_PC_DETAIL_LEVELS];
    ;
    g_Inv_Item_Sound.string = g_GF_PCStrings[GF_S_PC_SOUND];
    g_Inv_Item_Controls.string = g_GF_PCStrings[GF_S_PC_CONTROLS];
    g_Inv_Item_Photo.string = g_GF_GameStrings[GF_S_GAME_INV_ITEM_LARA_HOME];

    SetRequesterHeading(
        &g_LoadGameRequester, g_GF_GameStrings[GF_S_GAME_PASSPORT_SELECT_LEVEL],
        0, 0, 0);
    SetRequesterHeading(
        &g_SaveGameRequester, g_GF_GameStrings[GF_S_GAME_PASSPORT_SELECT_LEVEL],
        0, 0, 0);

    return true;
}

int32_t __cdecl GF_DoFrontendSequence(void)
{
    const GAME_FLOW_DIR option =
        GF_InterpretSequence(g_GF_FrontendSequence, GFL_NORMAL, 1);
    return option == GFD_EXIT_GAME;
}

int32_t __cdecl GF_DoLevelSequence(
    const int32_t start_level, const GF_LEVEL_TYPE type)
{
    int32_t current_level = start_level;
    while (true) {
        if (current_level > g_GameFlow.num_levels - 1) {
            g_IsTitleLoaded = 0;
            return GFD_EXIT_TO_TITLE;
        }

        const int16_t *const ptr = g_GF_ScriptTable[current_level];
        const GAME_FLOW_DIR option = GF_InterpretSequence(ptr, type, 0);
        current_level++;

        if (g_GameFlow.single_level >= 0) {
            return option;
        }
        if ((option & ~0xFF) != GFD_LEVEL_COMPLETE) {
            return option;
        }
    }
}

int32_t __cdecl GF_InterpretSequence(
    const int16_t *ptr, GF_LEVEL_TYPE type, const int32_t seq_type)
{
    g_GF_NoFloor = false;
    g_GF_DeadlyWater = false;
    g_GF_SunsetEnabled = false;
    g_GF_LaraStartAnim = 0;
    g_GF_Kill2Complete = false;
    g_GF_RemoveAmmo = false;
    g_GF_RemoveWeapons = false;

    for (int32_t i = 0; i < GF_ADD_INV_NUMBER_OF; i++) {
        g_GF_SecretInvItems[i] = 0;
        g_GF_Add2InvItems[i] = 0;
    }

    g_GF_MusicTracks[0] = 2;
    g_CineTargetAngle = PHD_90;
    g_GF_NumSecrets = 3;

    int32_t ntracks = 0;
    GAME_FLOW_DIR option = GFD_EXIT_TO_TITLE;

    while (*ptr != GFE_END_SEQ) {
        switch (*ptr) {
        case GFE_PICTURE:
            ptr += 2;
            break;

        case GFE_LIST_START:
        case GFE_LIST_END:
            ptr++;
            break;

        case GFE_PLAY_FMV:
            if (type != GFL_SAVED) {
                if (ptr[2] == GFE_PLAY_FMV) {
                    if (S_IntroFMV(
                            g_GF_FMVFilenames[ptr[1]],
                            g_GF_FMVFilenames[ptr[3]])) {
                        return GFD_EXIT_GAME;
                    }
                    ptr += 2;
                } else if (S_PlayFMV(g_GF_FMVFilenames[ptr[1]])) {
                    return GFD_EXIT_GAME;
                }
            }
            ptr += 2;
            break;

        case GFE_START_LEVEL:
            if (ptr[1] > g_GameFlow.num_levels) {
                option = GFD_EXIT_TO_TITLE;
            } else if (type != GFL_STORY) {
                if (type == GFL_MIDSTORY) {
                    return GFD_EXIT_TO_TITLE;
                }
                option = StartGame(ptr[1], type);
                g_GF_StartGame = 0;
                if (type == GFL_SAVED) {
                    type = GFL_NORMAL;
                }
                if ((option & ~0xFF) != GFD_LEVEL_COMPLETE) {
                    return option;
                }
            }
            ptr += 2;
            break;

        case GFE_CUTSCENE:
            if (type != GFL_SAVED) {
                const int16_t level = g_CurrentLevel;
                const int32_t result = Game_Cutscene_Start(ptr[1]);
                g_CurrentLevel = level;
                if (result == 2
                    && (type == GFL_STORY || type == GFL_MIDSTORY)) {
                    return GFD_EXIT_TO_TITLE;
                }
                if (result == 3) {
                    return GFD_EXIT_GAME;
                }
            }
            ptr += 2;
            break;

        case GFE_LEVEL_COMPLETE:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                if (LevelStats(g_CurrentLevel)) {
                    return GFD_EXIT_TO_TITLE;
                }
                option = GFD_START_GAME | (g_CurrentLevel + 1);
            }
            ptr++;
            break;

        case GFE_DEMO_PLAY:
            if (type != GFL_SAVED && type != GFL_STORY
                && type != GFL_MIDSTORY) {
                return Demo_Start(ptr[1]);
            }
            ptr += 2;
            break;

        case GFE_JUMP_TO_SEQ:
            ptr += 2;
            break;

        case GFE_SET_TRACK:
            g_GF_MusicTracks[ntracks] = ptr[1];
            Game_SetCutsceneTrack(ptr[1]);
            ntracks++;
            ptr += 2;
            break;

        case GFE_SUNSET:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_SunsetEnabled = true;
            }
            ptr++;
            break;

        case GFE_LOADING_PIC:
            ptr += 2;
            break;

        case GFE_DEADLY_WATER:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_DeadlyWater = true;
            }
            ptr++;
            break;

        case GFE_REMOVE_WEAPONS:
            if (type != GFL_STORY && type != GFL_MIDSTORY
                && type != GFL_SAVED) {
                g_GF_RemoveWeapons = true;
            }
            ptr++;
            break;

        case GFE_GAME_COMPLETE:
            DisplayCredits();
            if (GameStats(g_CurrentLevel)) {
                return GFD_EXIT_TO_TITLE;
            }
            option = GFD_EXIT_TO_TITLE;
            ptr++;
            break;

        case GFE_CUT_ANGLE:
            if (type != GFL_SAVED) {
                g_CineTargetAngle = ptr[1];
            }
            ptr += 2;
            break;

        case GFE_NO_FLOOR:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_NoFloor = ptr[1];
            }
            ptr += 2;
            break;

        case GFE_ADD_TO_INV:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                if (ptr[1] < 1000) {
                    g_GF_SecretInvItems[ptr[1]]++;
                } else if (type != GFL_SAVED) {
                    g_GF_Add2InvItems[ptr[1] - 1000]++;
                }
            }
            ptr += 2;
            break;

        case GFE_START_ANIM:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_LaraStartAnim = ptr[1];
            }
            ptr += 2;
            break;

        case GFE_NUM_SECRETS:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_NumSecrets = ptr[1];
            }
            ptr += 2;
            break;

        case GFE_KILL_TO_COMPLETE:
            if (type != GFL_STORY && type != GFL_MIDSTORY) {
                g_GF_Kill2Complete = true;
            }
            ptr++;
            break;

        case GFE_REMOVE_AMMO:
            if (type != GFL_STORY && type != GFL_MIDSTORY
                && type != GFL_SAVED) {
                g_GF_RemoveAmmo = true;
            }
            ptr++;
            break;

        default:
            return GFD_EXIT_GAME;
        }
    }

    if (type == GFL_STORY || type == GFL_MIDSTORY) {
        return 0;
    }
    return option;
}
